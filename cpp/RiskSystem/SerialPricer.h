#ifndef SERIALPRICER_H
#define SERIALPRICER_H

#include "../Models/IPricingEngine.h"
#include "../Models/ITrade.h"
#include "../Models/IScalarResultReceiver.h"
#include "PricingConfigLoader.h"
#include <map>
#include <vector>
#include <string>
#include <memory>

class SerialPricer {
private:
    std::map<std::string, std::unique_ptr<IPricingEngine>> pricers_;
    void loadPricers();
    
public:
    ~SerialPricer();
    void price(const std::vector<std::vector<ITrade*>>& tradeContainers, 
               IScalarResultReceiver* resultReceiver);
};

#endif // SERIALPRICER_H
