#ifndef BONDTRADELOADER_H
#define BONDTRADELOADER_H

#include "ITradeLoader.h"
#include "../Models/BondTrade.h"
#include <string>
#include <vector>
#include <memory>

class BondTradeLoader : public ITradeLoader {
private:
    static constexpr char separator = ',';
    std::string dataFile_;
    
    BondTrade* createTradeFromLine(const std::string& line);
    // changed: loadTradesFromFile now fills a vector of raw ITrade* (ownership transferred to caller)
    void loadTradesFromFile(const std::string& filename, std::vector<ITrade*>& trades);
    
public:
    std::vector<ITrade*> loadTrades() override;
    std::string getDataFile() const override;
    void setDataFile(const std::string& file) override;
};

#endif // BONDTRADELOADER_H
