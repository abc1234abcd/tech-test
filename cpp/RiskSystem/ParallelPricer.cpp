#include "ParallelPricer.h"
#include "../Pricers/GovBondPricingEngine.h"
#include "../Pricers/CorpBondPricingEngine.h"
#include "../Pricers/FxPricingEngine.h"
#include <stdexcept>
#include <vector>
#include <thread>

/*------resultReceiver wrapper------*/
class ThreadSafeResultReceiver : public IScalarResultReceiver {
public:
    ThreadSafeResultReceiver(IScalarResultReceiver* inner, std::mutex& mutex)
        : inner_(inner), mutex_(mutex) {}
    
    void addResult(const std::string& tradeId, double result) override {
        std::lock_guard<std::mutex> lock(mutex_);
        inner_->addResult(tradeId, result);
    }
    
    void addError(const std::string& tradeId, const std::string& error) override {
        std::lock_guard<std::mutex> lock(mutex_);
        inner_->addError(tradeId, error);
    }

private:
    IScalarResultReceiver* inner_;
    std::mutex& mutex_;
};

ParallelPricer::~ParallelPricer() {
    /*------clearnups------*/
    pricerTypes_.clear();
}

void ParallelPricer::loadPricers() {
    PricingConfigLoader pricingConfigLoader;
    pricingConfigLoader.setConfigFile("./PricingConfig/PricingEngines.xml");
    PricingEngineConfig pricerConfig = pricingConfigLoader.loadConfig();
    
    for (const auto& configItem : pricerConfig) {
        /*------store the type name so we can instantiate engines per-thread safely------*/
        pricerTypes_[configItem.getTradeType()] = configItem.getTypeName();
    }
}

void ParallelPricer::price(const std::vector<std::vector<ITrade*>>& tradeContainers, 
                           IScalarResultReceiver* resultReceiver) {
    loadPricers();

    ThreadSafeResultReceiver threadSafeReceiver(resultReceiver, resultMutex_);

    std::vector<std::thread> threads;

    for (const auto& tradeContainer : tradeContainers) {
        for (ITrade* trade : tradeContainer) {
            threads.emplace_back([this, trade, &threadSafeReceiver]() {
                std::string tradeType = trade->getTradeType();
                auto it = pricerTypes_.find(tradeType);
                if (it == pricerTypes_.end()) {
                    threadSafeReceiver.addError(trade->getTradeId(), "No Pricing Engines available for this trade type");
                    return;
                }
                IPricingEngine* engine = nullptr;
                const std::string& typeName = it->second;
                if (typeName == "HmxLabs.TechTest.Pricers.GovBondPricingEngine") {
                    engine = new GovBondPricingEngine();
                } else if (typeName == "HmxLabs.TechTest.Pricers.CorpBondPricingEngine") {
                    engine = new CorpBondPricingEngine();
                } else if (typeName == "HmxLabs.TechTest.Pricers.FxPricingEngine") {
                    engine = new FxPricingEngine();
                } else {
                    threadSafeReceiver.addError(trade->getTradeId(), "Unknown pricing engine type: " + typeName);
                    return;
                }
                engine->price(trade, &threadSafeReceiver);
                delete engine;
            });
        }
    }
    for (auto& thread : threads) {
        thread.join();
    }
}