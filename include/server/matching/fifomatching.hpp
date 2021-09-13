#ifndef FIFO_MATCHING_HPP
#define FIFO_MATCHING_HPP

#include <map>
#include <functional>
#include <optional>

#include "matchresult.hpp"
#include "level.hpp"
#include "util.hpp"

namespace server {
namespace matching {
using price = uint64_t;
using order_id = uint64_t;
using Level = server::tradeorder::Level;
using Order = ::tradeorder::Order;
using Limit = server::tradeorder::Limit;
using askbook = std::map<price, Level>;
using bidbook = std::map<price, Level, std::greater<price>>;
using limitbook = std::unordered_map<order_id, Limit>;

bool noMatchingLevel(bidbook&, uint64_t order_price, uint64_t bid_price);
bool noMatchingLevel(askbook&, uint64_t order_price, uint64_t ask_price);
void addFills(MatchResult& match_result, Order& order, Limit* book_lim, uint32_t fill_qty);
void popLimitFromQueue(Limit*& book_lim, Level& book_lvl, limitbook& limitbook);

// inline this so we dont have to capture these references every time we loop
// a level.. hopefully
template<typename Book, typename BookItr>
inline bool orderFullyMatchedInLevel(Limit*& book_lim, Order& order_to_match, MatchResult& match_result, 
Level& book_lvl, limitbook& limitbook, Book& book, BookItr& book_itr) {
    while (book_lim != nullptr) {
        uint32_t fill_qty = std::min(book_lim->order.getCurrQty(), order_to_match.getCurrQty());
        order_to_match.decreaseQty(fill_qty);
        book_lim->order.decreaseQty(fill_qty);
        addFills(match_result, order_to_match, book_lim, fill_qty);
        if (order_to_match.getCurrQty() == 0) { //order doesnt get added
            if (book_lim->order.getCurrQty() == 0) {
                popLimitFromQueue(book_lim, book_lvl, limitbook);
            }
            if (book_lvl.getLevelOrderCount() == 0) {
                book.erase(book_itr++);
            }
            match_result.setOrderFilled();
            return true;
        }
        popLimitFromQueue(book_lim, book_lvl, limitbook);
    }
    return false;
}

template <typename T>
std::optional<MatchResult> FIFOMatch(Order& order_to_match, T& book, limitbook& limitbook);

}
}

#endif
