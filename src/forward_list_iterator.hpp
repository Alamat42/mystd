#pragma once 

#include <cstddef>
#include <iterator>

#include "forward_list_node.hpp"

namespace mystd {

template <typename T, typename Allocator> 
class ForwardList;

namespace detail {


template <typename T>
struct FwdListNode : public FwdListNodeBase<T> {
private:
    using Base = FwdListNodeBase<T>;

public:
    FwdListNode() : Base(), value_(T{}) {}

    FwdListNode(Base *next, const T &value) 
        : Base(next), value_(value) {}

    FwdListNode(Base *next, T &&value = T{}) 
        : Base(next), value_(std::move(value)) {}

    template <typename ...Args>
    FwdListNode(Base *next, Args &&...args)
        : Base(next), value_(std::forward<Args>(args)...) {}
    
    T &value() { return value_; }
    const T &value() const { return value_; }

private:
    T value_;
};

template <bool IsConst, typename T>
class FwdListIterator {
    template<typename, typename Allocator>
    friend class mystd::ForwardList;

    friend class FwdListIterator<true, T>;

    using conditional = std::conditional_t<IsConst, const T, T>;
    using NodeBase = FwdListNodeBase<T>;
    using Node = FwdListNode<T>;

public:
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = conditional *;
    using reference = conditional &;
    using iterator_category = typename std::forward_iterator_tag;

    FwdListIterator(const FwdListIterator<false, T> &iterator) 
        : node_(iterator.node()) {}

    FwdListIterator &operator ++ ();
    FwdListIterator operator ++ (int);
    reference operator * () const;
    pointer operator -> () const;

    bool operator == (const FwdListIterator &rhs) const = default;

private:
    explicit FwdListIterator(NodeBase *node) : node_(node) {}

    Node *node() const { return static_cast<Node *>(node_); }
    void setNext(NodeBase *next) { node_->setNext(next); }
    Node *next() const { return node_->next(); }

    NodeBase *node_;
};

template <bool IsConst, typename T>
FwdListIterator<IsConst, T> &FwdListIterator<IsConst, T>::operator ++ () {
    node_ = node_->next();
    return *this;
}

template <bool IsConst, typename T>
FwdListIterator<IsConst, T> FwdListIterator<IsConst, T>::operator ++ (int) {
    auto copy = *this;
    node_ = node_->next();
    return copy;
}

template <bool IsConst, typename T>
typename FwdListIterator<IsConst, T>::reference 
    FwdListIterator<IsConst, T>::operator * () const
{
    return static_cast<Node *>(node_)->value();
}

template <bool IsConst, typename T>
typename FwdListIterator<IsConst, T>::pointer
    FwdListIterator<IsConst, T>::operator -> () const
{
    return &(static_cast<Node *>(node_)->value());
}

template <bool IsConst, typename T>
bool operator == (FwdListIterator<true, T> lhs, FwdListIterator<true, T> rhs) {
    return lhs == rhs;
}

template <bool IsConst, typename T>
bool operator != (FwdListIterator<true, T> lhs, FwdListIterator<true, T> rhs) {
    return !(lhs == rhs);
}

} //end namespace mystd
} //end namespace detail
