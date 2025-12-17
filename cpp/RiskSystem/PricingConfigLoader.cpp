#include "PricingConfigLoader.h"
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <regex>

std::string PricingConfigLoader::getConfigFile() const {
    return configFile_;
}

void PricingConfigLoader::setConfigFile(const std::string& file) {
    configFile_ = file;
}

/*------regex pattern to find attribute value------*/
std::string PricingConfigLoader::extractAttributeValue(const std::string& element, const std::string& attribute){
    std::regex pattern(attribute + "=\"([^\"]+)\"");
    std::smatch match;

    if (std::regex_search(element, match, pattern)){
        return match[1].str();
    }
    throw std::runtime_error("Attribute '" + attribute + "'not found in element");
}

/*------find element from Engine------*/
PricingEngineConfig PricingConfigLoader::parseXml(const std::string& content) {
    PricingEngineConfig config;

    std::regex enginePattern("<Engine[^>]*tradeType=\"([^\"]+)\"[^>]*assembly=\"([^\"]+)\"[^>]*pricingEngine=\"([^\"]+)\"[^>]*/?>");
    std::smatch match;

    std::string::const_iterator searchStart(content.cbegin());
    while (std::regex_search(searchStart, content.cend(), match, enginePattern)){
        PricingEngineConfigItem item;
        item.setTradeType(match[1].str());
        item.setAssembly(match[2].str());
        item.setTypeName(match[3].str());

        config.push_back(item);

        searchStart = match.suffix().first;
    }
    return config;
}

PricingEngineConfig PricingConfigLoader::loadConfig() {
    if (configFile_.empty()){
        throw std::invalid_argument("Config file not set");
    }

    std::ifstream file(configFile_);
    if (!file.is_open()){
        throw std::runtime_error("Cannot open config file: " + configFile_);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    return parseXml(content);
}
