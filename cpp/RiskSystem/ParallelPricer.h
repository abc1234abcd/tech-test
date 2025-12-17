#ifndef PARALLELPRICER_H
#define PARALLELPRICER_H

#include "../Models/IPricingEngine.h"
#include "../Models/ITrade.h"
#include "../Models/IScalarResultReceiver.h"
#include "PricingConfigLoader.h"
#include <map>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <future>

class ParallelPricer {
private:
    /*------map tradeType -> pricing engine type name (factory info)------*/
    std::map<std::string, std::string> pricerTypes_;
    std::mutex resultMutex_;
    
    void loadPricers();
    
public:
    ~ParallelPricer();
    
    void price(const std::vector<std::vector<ITrade*>>& tradeContainers, 
               IScalarResultReceiver* resultReceiver);
};

#endif // PARALLELPRICER_H
