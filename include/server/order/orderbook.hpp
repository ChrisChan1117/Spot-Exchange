#ifndef ORDER_BOOK_HPP
#define ORDER_BOOK_HPP

#include <cstdint>
#include <map>
#include "order.hpp"
#include "level.hpp"
#include <boost/any.hpp>

namespace server {
namespace tradeorder {
using price = uint64_t;
using order_id = uint64_t;
using askbook = std::map<price, Level>;
using bidbook = std::map<price, Level, std::greater<price>>;
class OrderBook {
public:
    OrderBook() {
        
    }
    void addOrder(::tradeorder::Order&& order) {
        Level* level = nullptr;
        if (order.getSide() == 'B')
            level = &getSideLevel(order.getPrice(), bids_);
        else
            level = &getSideLevel(order.getPrice(), asks_);
        auto limitr = limitorders_.emplace(order.getOrderID(), Limit(order, *level));
        if (!limitr.second)
            return;
        Limit& limit = limitr.first->second;
        if (level->head == nullptr) {
            level->head = &limit;
            level->tail = &limit;
        }
        else {
            Limit* limit_temp = level->tail;
            level->tail = &limit;
            limit.prev_limit = limit_temp;
            limit_temp->next_limit = &limit;
        }
    }
    void modifyOrder() {
        
    }
private:
    template<typename T>
    Level& getSideLevel(const uint64_t price, T sidebook) {
        auto lvlitr = sidebook.find(price);
        if (lvlitr == sidebook.end()) {
            auto ret = sidebook.emplace(price, Level());
            if (!ret.second) {
                // error
            }
            lvlitr = ret.first;
        }
        return lvlitr->second;
    }
    uint64_t ticker_;
    askbook asks_;
    bidbook bids_;
    std::unordered_map<order_id, Limit> limitorders_;
};
}
}
#endif
