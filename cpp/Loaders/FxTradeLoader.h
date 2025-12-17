#ifndef FXTRADELOADER_H
#define FXTRADELOADER_H

#include "ITradeLoader.h"
#include "../Models/FxTrade.h"
#include <string>
#include <vector>

class FxTradeLoader : public ITradeLoader {
private:
    std::string dataFile_;

public:
    /*------add 'splitLine' and 'loadTradesFromFile' methods, and FxTrade obj------*/
    std::vector<std::string> splitLine(const std::string& line);
    FxTrade* createTradeFromLine(const std::string& line);
    void loadTradesFromFile(const std::string& filename, std::vector<ITrade*>& trades);
    
    std::vector<ITrade*> loadTrades() override;
    std::string getDataFile() const override;
    void setDataFile(const std::string& file) override;
};

#endif // FXTRADELOADER_H
