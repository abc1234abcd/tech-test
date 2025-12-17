#include "ScalarResults.h"
#include <stdexcept>

ScalarResults::~ScalarResults() = default;

std::optional<ScalarResult> ScalarResults::operator[](const std::string& tradeId) const {
    if (!containsTrade(tradeId)) {
        return std::nullopt;
    }

    std::optional<double> priceResult = std::nullopt;
    std::optional<std::string> error = std::nullopt;

    auto resultIt = results_.find(tradeId);
    if (resultIt != results_.end()) {
        priceResult = resultIt->second;
    }

    auto errorIt = errors_.find(tradeId);
    if (errorIt != errors_.end()) {
        error = errorIt->second;
    }

    return ScalarResult(tradeId, priceResult, error);
}

bool ScalarResults::containsTrade(const std::string& tradeId) const {
    return results_.find(tradeId) != results_.end() || errors_.find(tradeId) != errors_.end();
}

/*------invalidate cache------*/
void ScalarResults::addResult(const std::string& tradeId, double result) {
    results_[tradeId] = result;
    tradeIdsCached_ = false;
}

void ScalarResults::addError(const std::string& tradeId, const std::string& error) {
    errors_[tradeId] = error;
    tradeIdsCached_ = false;
}

/*------get all unique tradeIds------*/
const std::vector<std::string>& ScalarResults::getTradeIds() const {
    if (!tradeIdsCached_) {
        tradeIds_.clear();
      
        for (const auto& pair : results_) {
            tradeIds_.push_back(pair.first);
        }
        for (const auto& pair : errors_) {
            if (std::find(tradeIds_.begin(), tradeIds_.end(), pair.first) == tradeIds_.end()) {
                tradeIds_.push_back(pair.first);
            }
        }
        tradeIdsCached_ = true;
    }
    return tradeIds_;
}

ScalarResults::Iterator ScalarResults::begin() const {
    return Iterator(this, &getTradeIds(), 0);
}

ScalarResults::Iterator ScalarResults::end() const {
    return Iterator(this, &getTradeIds(), getTradeIds().size());
}
