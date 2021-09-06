#ifndef ORDER_MANAGER_HPP
#define ORDER_MANAGER_HPP

#include <unordered_map>
#include <vector>

#include "order.hpp"
#include "orderbook.hpp"

namespace server {
namespace tradeorder {
class OrderManager {
public:
    void addOrder(::tradeorder::Order order);
    void modifyOrder();
    void cancelOrder();
private:
    using order_id = uint64_t; using ticker = uint64_t;
    using book_index = uint64_t;
    std::unordered_map<ticker, OrderBook> orderbooks_;
};
}
}
#endif
