#include "../RiskSystem/SerialTradeLoader.h"
#include "../RiskSystem/StreamingTradeLoader.h"
#include "../Models/ScalarResults.h"
#include "../RiskSystem/SerialPricer.h"
#include "../RiskSystem/ParallelPricer.h"
#include "../RiskSystem/ScreenResultPrinter.h"
#include <iostream>
#include <string>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

int _getch() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif

int main(int argc, char* argv[]) {
    SerialTradeLoader tradeLoader;
    auto allTrades = tradeLoader.loadTrades();
    
    ScalarResults results;
    ParallelPricer pricer;
    pricer.price(allTrades, &results);
    
    ScreenResultPrinter screenPrinter;
    screenPrinter.printResults(results);
    // cleanup: delete all trades to avoid memory leaks. Ownership of trades is transferred from loaders
    for (auto& tradeContainer : allTrades) {
        for (ITrade* trade : tradeContainer) {
            delete trade;
        }
    }
    allTrades.clear();

    //cleanup
    for (auto& tradeContainer : allTrades){
        for (auto trade : tradeContainer){
            delete trade;
        }
    }
    
    std::cout << "Press any key to exit.." << std::endl;
    _getch();
    
    return 0;
}
