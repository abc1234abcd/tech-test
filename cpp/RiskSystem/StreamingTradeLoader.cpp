#include "StreamingTradeLoader.h"
#include "../Loaders/BondTradeLoader.h"
#include "../Loaders/FxTradeLoader.h"
#include "PricingConfigLoader.h"
#include "../Pricers/GovBondPricingEngine.h"
#include "../Pricers/CorpBondPricingEngine.h"
#include "../Pricers/FxPricingEngine.h"
#include <stdexcept>
#include <memory>


std::vector<ITradeLoader*> StreamingTradeLoader::getTradeLoaders() {
    std::vector<ITradeLoader*> loaders;
    
    BondTradeLoader* bondLoader = new BondTradeLoader();
    bondLoader->setDataFile("TradeData/BondTrades.dat");
    loaders.push_back(bondLoader);
    
    FxTradeLoader* fxLoader = new FxTradeLoader();
    fxLoader->setDataFile("TradeData/FxTrades.dat");
    loaders.push_back(fxLoader);
    
    return loaders;
}

void StreamingTradeLoader::loadPricers() {
    PricingConfigLoader pricingConfigLoader;
    pricingConfigLoader.setConfigFile("./PricingConfig/PricingEngines.xml");
    PricingEngineConfig pricerConfig = pricingConfigLoader.loadConfig();
    
    for (const auto& configItem : pricerConfig) {
        IPricingEngine* engine = nullptr;
        
        /*------pricing engine instance as in factory------*/
        if (configItem.getTypeName() == "HmxLabs.TechTest.Pricers.GovBondPricingEngine") {
            engine = new GovBondPricingEngine();
        } else if (configItem.getTypeName() == "HmxLabs.TechTest.Pricers.CorpBondPricingEngine") {
            engine = new CorpBondPricingEngine();
        } else if (configItem.getTypeName() == "HmxLabs.TechTest.Pricers.FxPricingEngine") {
            engine = new FxPricingEngine();
        } else {
            throw std::runtime_error("Unknown pricing engine type: " + configItem.getTypeName());
        }
        
        pricers_[configItem.getTradeType()] = engine;
    }
}

StreamingTradeLoader::~StreamingTradeLoader() {
    /*------cleanups------*/
    for (auto& pair : pricers_){
        delete pair.second;
    }
    pricers_.clear();
}

void StreamingTradeLoader::loadAndPrice(IScalarResultReceiver* resultReceiver) {
    if (resultReceiver == nullptr){
        throw std::invalid_argument("resultReceiver cannot be null");
    }

    loadPricers();

    /*------load trades and pricing concurrently------*/
    auto loaders = getTradeLoaders();

    for (auto loader : loaders){
        auto trades = loader->loadTrades();
        for (ITrade* trade : trades){
            std::string tradeType = trade->getTradeType();
            if (pricers_.find(tradeType) ==pricers_.end()){
                resultReceiver->addError(trade->getTradeId(), "No pricing engine is available for this trade type");
            } else {
                IPricingEngine* pricer = pricers_[tradeType];
                pricer->price(trade, resultReceiver); 
            }
            delete trade;
        }
        delete loader;
    }
}
