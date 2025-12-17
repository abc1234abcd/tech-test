#ifndef SCALARRESULTS_H
#define SCALARRESULTS_H

#include "IScalarResultReceiver.h"
#include "ScalarResult.h"
#include <map>
#include <vector>
#include <optional>
#include <string>
#include <iterator>

class ScalarResults : public IScalarResultReceiver {
public:
    virtual ~ScalarResults();
    std::optional<ScalarResult> operator[](const std::string& tradeId) const;

    bool containsTrade(const std::string& tradeId) const;

    virtual void addResult(const std::string& tradeId, double result) override;

    virtual void addError(const std::string& tradeId, const std::string& error) override;

    /*------define getTradeIds------*/
    const std::vector<std::string>& getTradeIds() const;

    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = ScalarResult;
        using difference_type = std::ptrdiff_t;
        using pointer = ScalarResult*;
        using reference = ScalarResult&;

        Iterator() = default;
        Iterator(const ScalarResults* parent, const std::vector<std::string>* tradeIds, size_t index)
            : parent_(parent), tradeIds_(tradeIds), index_(index) {}

        Iterator& operator++(){
            if (index_ < tradeIds_->size()){
                index_++;
            }
            return *this;
        };
        ScalarResult operator*() const{
            if(index_ >= tradeIds_->size()){
                throw std::out_of_range("Iterator out of range");
            }
            const std::string& tradeId = (*tradeIds_)[index_];
            return parent_->operator[](tradeId).value();
        }
        bool operator!=(const Iterator& other) const{
            return index_!= other.index_;
        }
        private:
            const ScalarResults* parent_ = nullptr;
            const std::vector<std::string>* tradeIds_ = nullptr;
            size_t index_ = 0;
    };

    Iterator begin() const;
    Iterator end() const;

private:
    std::map<std::string, double> results_;
    std::map<std::string, std::string> errors_;
    mutable std::vector<std::string> tradeIds_;
    mutable bool tradeIdsCached_ = false;
};

#endif // SCALARRESULTS_H
