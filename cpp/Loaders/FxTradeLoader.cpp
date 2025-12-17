#include "FxTradeLoader.h"
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <chrono>

// NOTE: These methods are only here to allow the solution to compile prior to the test being completed.


/*------return items from flie line------*/
std::vector<std::string> FxTradeLoader::splitLine(const std::string& line){
    std::vector<std::string> items;
    std::string current;

    auto trim = [](const std::string& s) -> std::string {
        std::string result = s;
        if (result.size() >= 3 && (unsigned char)result[0] == 0xEF && (unsigned char)result[1] == 0xBB && (unsigned char)result[2] == 0xBF) {
            result.erase(0,3);
        }
        while (!result.empty() && (result.back() == '\r' || result.back() == '\n' || isspace((unsigned char)result.back()))) result.pop_back();
        size_t pos = 0;
        while (pos < result.size() && isspace((unsigned char)result[pos])) pos++;
        if (pos > 0) result.erase(0, pos);
        return result;
    };

    size_t i = 0;
    while (i < line.size()) {
        if (i + 1 < line.size() && (unsigned char)line[i] == 0xC2 && (unsigned char)line[i+1] == 0xAC) {
            std::string token = trim(current);
            items.push_back(token);
            current.clear();
            i += 2;
        } else {
            current += line[i];
            ++i;
        }
    }

    std::string last = trim(current);
    if (!last.empty() || !items.empty()) items.push_back(last);

    return items;
}

/*------parse items to 'trade'------*/
FxTrade* FxTradeLoader::createTradeFromLine(const std::string& line){
    std::vector<std::string> items = splitLine(line);
    if (items.size() < 9){
        throw std::runtime_error("Invalid FX trade line format");
    }

    std::string tradeType = FxTrade::FxSpotTradeType;
    if (items[0] =="FxFwd"){
        tradeType = FxTrade::FxForwardTradeType;
    }

    FxTrade* trade = new FxTrade(items[8], tradeType);

    std::tm tm ={};
    std::istringstream dateStream(items[1]);
    dateStream >> std::get_time(&tm, "%Y-%m-%d");
    auto timePoint = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    trade ->setTradeDate(timePoint);

    std::string instrument = items[2] + items[3];
    trade->setInstrument(instrument);

    trade->setCounterparty(items[7]);
    trade->setNotional(std::stod(items[4]));
    trade->setRate(std::stod(items[5]));

    std::tm valueTm = {};
    std::istringstream valueDateStream(items[6]);
    valueDateStream >> std::get_time(&valueTm, "%Y-%m-%d");
    auto valueTimePoint = std::chrono::system_clock::from_time_t(std::mktime(&valueTm));
    trade->setValueDate(valueTimePoint);

    return trade;
}


/*------load trade from file line by line------*/
void FxTradeLoader::loadTradesFromFile(const std::string& filename, std::vector<ITrade*>& trades){
    if (filename.empty()){
        throw std::invalid_argument("Filename cannot be empty");
    }

    std::ifstream stream(filename);
    if (!stream.is_open()){
        throw std::runtime_error("Cannot open file:" + filename);
    }

    std::string line;
    int lineCount = 0;
    while (std::getline(stream, line)){
        if (lineCount == 0 || lineCount == 1){
            lineCount++;
            continue;
        }

        if (line.find("END") == 0){
            break;
        }

        if (!line.empty()){
            trades.push_back(createTradeFromLine(line));
        }
        lineCount++;
    }
}


std::vector<ITrade*> FxTradeLoader::loadTrades() {
    std::vector<ITrade*> trades;
    loadTradesFromFile(dataFile_, trades);
    return trades;
}

std::string FxTradeLoader::getDataFile() const {
    return dataFile_;
}

void FxTradeLoader::setDataFile(const std::string& file) {
    dataFile_= file;
}
