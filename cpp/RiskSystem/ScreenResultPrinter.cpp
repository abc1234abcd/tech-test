#include "ScreenResultPrinter.h"
#include <iostream>
#include <iomanip>
#include <sstream>

void ScreenResultPrinter::printResults(ScalarResults& results) {
    for (const auto& result : results) {
        // Write code here to print out the results such that we have:
        // TradeID : Result : Error
        // If there is no result then the output should be:
        // TradeID : Error
        // If there is no error the output should be:
        // TradeID : Result
        const std::string tradeId = result.getTradeId().empty() ? "<unknown>" : result.getTradeId();

        bool hasResult = result.getResult().has_value();
        bool hasError = result.getError().has_value();

        std::cout << tradeId;
        /*------Both result and error: TradeID : Result : Error------*/
        if (hasResult && hasError) {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(3) << result.getResult().value();
            std::cout << " : " << oss.str() << " : " << result.getError().value();
        } else if (hasResult) {
            /*------Only result: TradeID : Result------*/
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(3) << result.getResult().value();
            std::cout << " : " << oss.str();
        } else if (hasError) {
            /*------Only error: TradeID : Error------*/
            std::cout << " : " << result.getError().value();
        }

        std::cout << std::endl;
    }
}
