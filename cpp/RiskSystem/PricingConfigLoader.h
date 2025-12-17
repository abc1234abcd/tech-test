#ifndef PRICINGCONFIGLOADER_H
#define PRICINGCONFIGLOADER_H

#include "PricingEngineConfig.h"
#include <string>

class PricingConfigLoader {
private:
    std::string configFile_;

    /*------add extractAttributeValue method------*/
    std::string extractAttributeValue(const std::string& element, const std::string& attribute);
    PricingEngineConfig parseXml(const std::string& content);
    
public:
    std::string getConfigFile() const;
    void setConfigFile(const std::string& file);
    PricingEngineConfig loadConfig();
};

#endif // PRICINGCONFIGLOADER_H
