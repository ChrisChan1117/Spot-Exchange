#ifndef FIFO_MATCHING_HPP
#define FIFO_MATCHING_HPP

#include <map>
#include <functional>

#include "matchresult.hpp"
#include "level.hpp"

namespace server {
namespace matching {
using price = uint64_t;
using Level = server::tradeorder::Level;
using Limit = server::tradeorder::Limit;
using askbook = std::map<price, Level>;
using bidbook = std::map<price, Level, std::greater<price>>;

bool comparePrices(std::map<price, Level, std::greater<price>>&, uint64_t order_price, uint64_t bid_price) {
    return order_price > bid_price;
}

bool comparePrices(std::map<price, Level>&, uint64_t order_price, uint64_t ask_price) {
    return order_price < ask_price;
}

template <typename T>
MatchResult FIFOMatch(Limit& order_to_match, T& book) {
    MatchResult match_result;
    if (book.empty())
        return match_result;
    auto book_itr = book.begin();
    Level& book_lvl = book_itr->second;
    uint64_t closest_book_price = book_itr->first;
    uint64_t order_price = order_to_match.order.getPrice();
    while (book_itr != book.end()) {
        if (comparePrices(T, order_price, closest_book_price)) // no matching to do
            return match_result;
        Limit* book_lim = book_lvl.head;
        while (book_lim != nullptr) {
            uint16_t book_qty = book_lim->order.getCurrQty();
            uint16_t fill_qty = std::min(book_qty, order_to_match.order.getCurrQty());
            order_to_match.order.decreaseQty(fill_qty);
            book_lim->order.decreaseQty(fill_qty);
            if (order_to_match.order.getCurrQty() == 0) { //order doesnt get added
                return match_result;
            }
            Limit* temp_lim = book_lim->next_limit;
            book_lvl.head = temp_lim;
            temp_lim->prev_limit = nullptr;
            book_lim = book_lim->next_limit;
        }
        ++book_itr;
        book_lvl = book_itr->second;
    }
    return match_result;
}

template MatchResult FIFOMatch<bidbook>(Limit&, bidbook&);
template MatchResult FIFOMatch<askbook>(Limit&, askbook&);

}
}

#endif
