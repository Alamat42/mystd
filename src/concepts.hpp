#pragma once 

#include <concepts>

namespace mystd {
namespace detail {

template<typename Compare, typename T>
concept compare = requires (const T &lhs, const T &rhs, Compare comp) {
    { comp(lhs, rhs) } -> std::convertible_to<bool>;
};

} //end namespace mystd
} //end namespace detail
