#include "SerialTradeLoader.h"
#include "../Loaders/BondTradeLoader.h"
#include "../Loaders/FxTradeLoader.h"
#include <memory>

std::vector<std::unique_ptr<ITradeLoader>> SerialTradeLoader::getTradeLoaders() {
    std::vector<std::unique_ptr<ITradeLoader>> loaders;
    
    auto bondLoader = std::make_unique<BondTradeLoader>();
    bondLoader->setDataFile("TradeData/BondTrades.dat");
    loaders.push_back(std::move(bondLoader));
    
    auto fxLoader = std::make_unique<FxTradeLoader>();
    fxLoader->setDataFile("TradeData/FxTrades.dat");
    loaders.push_back(std::move(fxLoader));
    
    return loaders;
}

std::vector<std::vector<ITrade*>> SerialTradeLoader::loadTrades() {
    auto loaders = getTradeLoaders();
    std::vector<std::vector<ITrade*>> result;
    
    for (auto& loader : loaders) {
        result.push_back(loader->loadTrades());
    }
    
    return result;
}

