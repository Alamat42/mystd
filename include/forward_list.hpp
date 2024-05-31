#pragma once 

#include <iterator>
#include <memory>

#include "forward_list_node.hpp"
#include "forward_list_iterator.hpp"
#include "concepts.hpp"

namespace mystd {

template <typename T, typename Allocator = std::allocator<T>>
class ForwardList {
    using NodeBase = detail::FwdListNodeBase<T>;
    using Node = detail::FwdListNode<T>;
    using Traits = typename std::allocator_traits<Allocator>;
    using NodeAlloc = typename Traits::template rebind_alloc<Node>;
    using NodeTraits = typename std::allocator_traits<NodeAlloc>;

public:
    using value_type = T;
    using allocator_type = Allocator;

    using reference = value_type &;
    using const_reference = const value_type &;

    using pointer = Traits::pointer;
    using const_pointer = Traits::const_pointer;

    using size_type = Traits::size_type;
    using difference_type = Traits::difference_type;

    using iterator = detail::FwdListIterator<false, T>;
    using const_iterator = detail::FwdListIterator<true, T>;

    ForwardList();

    explicit ForwardList(const Allocator &alloc);

    ForwardList(const ForwardList &other);

    ForwardList(const ForwardList &other, const Allocator &alloc);

    ForwardList(ForwardList &&other);

    ForwardList(ForwardList &&other, const Allocator &alloc) ;

    explicit ForwardList(size_type count, const Allocator &alloc = Allocator{});

    ForwardList(size_type count, 
            const T &value, 
            const Allocator &alloc = Allocator{});

    ForwardList(std::input_iterator auto first, 
            std::input_iterator auto last, 
            const Allocator &alloc = Allocator{});

    ForwardList(std::initializer_list<T> ilist, 
            const Allocator &alloc = Allocator{});

    ~ForwardList();

    ForwardList &operator = (const ForwardList &other);
    ForwardList &operator = (ForwardList &&other);
    ForwardList &operator = (std::initializer_list<T> ilist);

    Allocator get_allocator() const;

    reference front() { return head_.next()->value(); }
    const_reference front() const { return head_.next()->value; }

    iterator before_begin() { return iterator(&head_); }
    iterator begin() { return iterator(head_.next()); }
    iterator end() { return iterator(nullptr); }

    const_iterator before_begin() const { return const_iterator(&head_); }
    const_iterator begin() const { return const_iterator(head_.next()); }
    const_iterator end() const { return const_iterator(nullptr); }

    iterator cbefore_begin() const { return const_iterator(&head_); }
    const_iterator cbegin() const { return const_iterator(head_.next()); }
    const_iterator cend() const { return const_iterator(nullptr); }

    bool empty() const { return head_.next() == nullptr; }
    size_type max_size() const { 
        return std::numeric_limits<difference_type>::max();
    }

    iterator insert_after(const_iterator pos, const T& value);
    iterator insert_after(const_iterator pos, T&& value);
    iterator insert_after(const_iterator pos, size_type count, const T& value);
    iterator insert_after(const_iterator pos, std::initializer_list<T> ilist);
    iterator insert_after(const_iterator pos, 
            std::input_iterator auto first, 
            std::input_iterator auto last);

    template<typename ...Args>
    iterator emplace_after(const_iterator pos, Args &&...args);

    void clear();

    iterator erase_after(const_iterator pos);    
    iterator erase_after(const_iterator first, const_iterator last);    

    void push_front(const T &value);
    void push_front(T &&value);

    template<class... Args>
    reference emplace_front(Args &&...args);

    void pop_front() { destroy_next_node(before_begin()); };

    void resize(size_type count);
    void resize(size_type count, const value_type& value);

    void swap(ForwardList &other);

    void merge(ForwardList &other);
    void merge(ForwardList &&other);
    // detail::compare is a concept for comparator
    void merge(ForwardList &other, detail::compare<T> auto comp);
    void merge(ForwardList &&other, detail::compare<T> auto comp);

    void splice_after(const_iterator pos, ForwardList &other);

    void splice_after(const_iterator pos, ForwardList &other, 
            const_iterator it);

    void splice_after(const_iterator pos, ForwardList& other, 
            const_iterator first, const_iterator last);

    size_type remove(const T& value);
    size_type remove_if(std::predicate<T> auto pred);

    void reverse();

    size_type unique();
    size_type unique(detail::compare<T> auto pred);

    void sort();
    void sort(detail::compare<T> auto comp);

    void assign(std::input_iterator auto first, std::input_iterator auto last);

private:
    void insert_empty_after(const_iterator pos, size_type n);

    Node *new_empty_node(NodeBase *next_ = nullptr);
    Node *new_node(NodeBase *next_, const T &value);
    Node *new_node(NodeBase *next_, T &&value);
    
    template <typename ...Args>
    Node *new_emplace_node(NodeBase *next, Args &&...args);

    size_type crop(size_type count);
    void destroy_next_node(const_iterator pos);
    void sort_impl(ForwardList &list, 
            size_type size, 
            detail::compare<T> auto comp);

    NodeBase head_;
    NodeAlloc alloc_;
};

template <typename T, typename Alloc>
ForwardList<T, Alloc>::ForwardList() 
    : head_(nullptr), alloc_(Alloc{}) {}

template <typename T, typename Alloc>
ForwardList<T, Alloc>::ForwardList(const Alloc &alloc) 
    : head_(nullptr), alloc_(alloc) {}

template <typename T, typename Alloc>
ForwardList<T, Alloc>::ForwardList(const ForwardList &other) 
    : ForwardList(other, 
        Traits::select_on_container_copy_construction(other.alloc_)) {}

template <typename T, typename Alloc>
ForwardList<T, Alloc>::ForwardList(const ForwardList &other,
    const Alloc &alloc) 
    : ForwardList(alloc) 
{
    assign(other.cbegin(), other.cend());
}

template <typename T, typename Alloc>
ForwardList<T, Alloc>::ForwardList(ForwardList &&other) 
    : ForwardList(std::move(other), Alloc()) {}

template <typename T, typename Alloc>
ForwardList<T, Alloc>::ForwardList(ForwardList &&other, 
    const Alloc &alloc) 
    : head_(other.head_), alloc_(alloc)
{
    other.head_ = nullptr;
}

template <typename T, typename Alloc>
ForwardList<T, Alloc>::ForwardList(size_type count, const Alloc &alloc) 
    : ForwardList(alloc)
{
    insert_empty_after(before_begin(), count);
}

template <typename T, typename Alloc>
ForwardList<T, Alloc>::ForwardList(size_type count, 
    const T &value, const Alloc &alloc) 
    : ForwardList(alloc)
{
    insert_after(before_begin(), count, value);
}

template <typename T, typename Alloc>
ForwardList<T, Alloc>::ForwardList(
    std::input_iterator auto first, std::input_iterator auto last, 
    const Alloc &alloc) 
    : ForwardList(alloc)
{
    assign(first, last);
}

template <typename T, typename Alloc>
ForwardList<T, Alloc>::ForwardList(
    std::initializer_list<T> init, const Alloc &alloc) 
    : ForwardList(alloc)
{
    assign(std::move_iterator(init.begin()), std::move_iterator(init.end()));
}
template <typename T, typename Alloc>
ForwardList<T, Alloc>::~ForwardList() {
    clear();
}

template <typename T, typename Alloc>
ForwardList<T, Alloc> &ForwardList<T, Alloc>::operator = (
        const ForwardList &other) 
{
    if(std::addressof(*this) == std::addressof(other)) {
        return *this;
    }

    clear();

    if(NodeTraits::propagate_on_container_copy_assignment::value) {
        alloc_ = other.alloc_;
    }

    assign(other.cbegin(), other.cend());
    return *this;
}

template <typename T, typename Alloc>
ForwardList<T, Alloc> &ForwardList<T, Alloc>::operator = (
        ForwardList &&other) 
{
    if(NodeTraits::propagate_on_container_move_assignment::value) {
        clear();
        alloc_ = std::move(other.alloc_);
        head_ = other.head_;
        other.head_.setNext(nullptr);
    } else if(NodeTraits::is_always_equal::value || alloc_ == other.alloc_) {
        clear();
        head_ = other.head_;
        other.head_.setNext(nullptr);
    } else {
        assign(std::move_iterator(other.begin()), 
               std::move_iterator(other.end()));
    }

    return *this;
}

template <typename T, typename Alloc>
ForwardList<T, Alloc> &ForwardList<T, Alloc>::operator = (
        std::initializer_list<T> ilist) 
{
    assign(std::move_iterator(ilist.begin()), std::move_iterator(ilist.end()));
    return *this;
}
template <typename T, typename Alloc>
ForwardList<T, Alloc>::iterator 
ForwardList<T, Alloc>::insert_after(const_iterator pos, const T& value) {
    pos.setNext(new_node(pos.next(), value));
    return iterator(pos.next());
}

template <typename T, typename Alloc>
ForwardList<T, Alloc>::iterator 
ForwardList<T, Alloc>::insert_after(const_iterator pos, T&& value) {
    pos.setNext(new_node(pos.next(), std::move(value)));
    return iterator(pos.next());
}

template <typename T, typename Alloc>
ForwardList<T, Alloc>::iterator 
ForwardList<T, Alloc>::insert_after(
        const_iterator pos, 
        size_type count, 
        const T& value)
{
    for(size_type i = 0; i < count; ++i) {
        pos = insert_after(pos, value);
    }
    
    return iterator(pos.node());
}

template <typename T, typename Alloc>
ForwardList<T, Alloc>::iterator 
ForwardList<T, Alloc>::insert_after( 
        const_iterator pos, 
        std::initializer_list<T> ilist)
{
    for(auto &&value: ilist) {
        pos = insert_after(pos, std::move(value));
    }

    return iterator(pos.node());
}

template <typename T, typename Alloc>
ForwardList<T, Alloc>::iterator 
ForwardList<T, Alloc>::insert_after( 
        const_iterator pos, 
        std::input_iterator auto first, 
        std::input_iterator auto last)
{
    for(; first != last; ++first) {
        pos = insert_after(pos, *first);
    }

    return iterator(pos.node());
}

template <typename T, typename Alloc>
template<typename ...Args>
ForwardList<T, Alloc>::iterator 
ForwardList<T, Alloc>::emplace_after(const_iterator pos, Args &&...args) {
    pos.setNext(new_emplace_node(pos.next(), std::forward<Args>(args)...));
    return iterator(pos.node());
}

template <typename T, typename Alloc>
void ForwardList<T, Alloc>::clear() {
    erase_after(before_begin(), end());
}


template <typename T, typename Alloc>
typename ForwardList<T, Alloc>::iterator 
ForwardList<T, Alloc>::erase_after(const_iterator pos) {
    if(!pos.next()) {
        return iterator(pos.node());
    }
    
    destroy_next_node(pos);
    return iterator(std::next(pos).node());
}

template <typename T, typename Alloc>
typename ForwardList<T, Alloc>::iterator 
ForwardList<T, Alloc>::erase_after(const_iterator first, const_iterator last) {
    if(first == last || std::next(first) == last) {
        return iterator(last.node());
    }

    while(std::next(first) != last) {
        destroy_next_node(first);
    }

    return iterator(last.node());
}

template <typename T, typename Alloc>
void ForwardList<T, Alloc>::assign(
        std::input_iterator auto first, 
        std::input_iterator auto last)
{
    auto dest = before_begin();

    for(auto source = first; source != last; ++source, ++dest) {
        if(dest.next() != nullptr) {
            dest.next()->value() = *source;
        } else {
            dest.setNext(new_node(nullptr, *source));
        }
    }

    erase_after(dest, end());
}

template <typename T, typename Alloc>
void ForwardList<T, Alloc>::push_front(const T &value) {
    head_.setNext(new_node(head_.next(), value));
}

template <typename T, typename Alloc>
void ForwardList<T, Alloc>::push_front(T &&value) {
    head_.setNext(new_node(head_.next(), std::move(value)));
}

template <typename T, typename Alloc>
template<typename ...Args>
ForwardList<T, Alloc>::reference 
ForwardList<T, Alloc>::emplace_front(Args &&...args) {
    head_.setNext(new_emplace_node(head_.next(), std::forward<Args>(args)...));
    return front();
}

template <typename T, typename Alloc>
void ForwardList<T, Alloc>::resize(size_type count) {
    size_type old_size = crop(count);

    if(old_size >= count) {
        return;
    }

    auto it = before_begin();
    std::advance(it, old_size);
    insert_empty_after(it, count - old_size);
}

template <typename T, typename Alloc>
void ForwardList<T, Alloc>::resize(size_type count, const value_type& value) {
    size_type old_size = crop(count);

    if(old_size >= count) {
        return;
    }

    auto it = before_begin();
    std::advance(it, old_size);
    insert_after(it, count - old_size, value);
}

template <typename T, typename Alloc>
void ForwardList<T, Alloc>::swap(ForwardList &other) {
    std::swap(*this, other);
}

template <typename T, typename Alloc>
void ForwardList<T, Alloc>::merge(ForwardList &other) {
    merge(std::move(other), std::less<T>{});
}

template <typename T, typename Alloc>
void ForwardList<T, Alloc>::merge(ForwardList &&other) {
    merge(std::move(other), std::less<T>{});
}

template <typename T, typename Alloc>
void ForwardList<T, Alloc>::merge(
        ForwardList &other, 
        detail::compare<T> auto comp) 
{
    merge(std::move(other), comp);
}

template <typename T, typename Alloc>
void ForwardList<T, Alloc>::merge(
        ForwardList &&other, 
        detail::compare<T> auto comp) 
{
    if(addressof(*this) == addressof(other)) {
        return;
    }

    auto dst_prev = before_begin();
    auto src_prev = other.before_begin();
    auto dst = begin();
    auto src = other.begin();

    for(auto last = end(); src != last && dst != last; ++dst_prev, ++dst) {
        if(comp(*dst, *src)) {
            splice_after(dst_prev, other, src_prev);
            dst = std::next(dst_prev);
            src = std::next(src_prev);
        } 
    }

    if(dst != end()) {
        return;
    }

    dst_prev.setNext(src.node());
    src_prev.setNext(nullptr);
}

template <typename T, typename Alloc>
void ForwardList<T, Alloc>::splice_after(
        const_iterator pos, 
        ForwardList &other) 
{
    splice_after(pos, other, other.before_begin(), other.end());
}


template <typename T, typename Alloc>
void ForwardList<T, Alloc>::splice_after( 
        const_iterator pos, 
        ForwardList &other, 
        const_iterator it)
{
    auto temp = pos.next();
    pos.setNext(it.next());
    it.setNext(it.next()->next());
    pos.next()->setNext(temp);
}

template <typename T, typename Alloc>
void ForwardList<T, Alloc>::splice_after( 
        const_iterator pos, 
        ForwardList& other, 
        const_iterator first, 
        const_iterator last) 
{
    auto temp = pos.next();
    pos.setNext(first.next());

    for(auto next = std::next(pos); next != last; ++pos, ++next);

    pos.setNext(temp);
    first.setNext(last.node());
}

template <typename T, typename Alloc>
ForwardList<T, Alloc>::size_type 
ForwardList<T, Alloc>::remove(const T& value) {
    return remove_if([&value](const T& element) {
            return element == value;
            });
}

template <typename T, typename Alloc>
ForwardList<T, Alloc>::size_type 
ForwardList<T, Alloc>::remove_if(std::predicate<T> auto pred) {
    auto curr = begin();
    auto prev = before_begin();
    auto stop = end();
    size_type count = 0;

    while(curr != stop) {
        if(pred(*curr)) {
            destroy_next_node(prev);
            curr = std::next(prev);
            ++count;
            continue;
        } else { 
            ++prev; 
            ++curr;
        }
    }

    return count;
}

template <typename T, typename Alloc>
void ForwardList<T, Alloc>::reverse() {
    Node *prev = nullptr;
    Node *curr = begin().node();
    
    while(curr != nullptr) {
        Node *temp = curr->next();
        curr->setNext(prev);
        prev = curr;
        curr = temp;
    }

    head_.setNext(prev);
}

template <typename T, typename Alloc>
ForwardList<T, Alloc>::size_type ForwardList<T, Alloc>::unique() {
    return unique(std::equal_to<T>{});
}

template <typename T, typename Alloc>
ForwardList<T, Alloc>::size_type 
ForwardList<T, Alloc>::unique(detail::compare<T> auto pred) {
    if(this->empty()) {
        return 0;
    }

    auto prev = begin();
    auto curr = std::next(prev);
    auto last = end();
    size_type count = 0;

    while(curr != last) {
        if(pred(*prev, *curr)) {
            destroy_next_node(prev);
            curr = std::next(prev);
            ++count;
        } else {
            ++prev;
            ++curr;
        }
    }

    return count;
}

template <typename T, typename Alloc>
void ForwardList<T, Alloc>::sort() {
    sort(std::less<T>{});
}

template <typename T, typename Alloc>
void ForwardList<T, Alloc>::sort(detail::compare<T> auto comp) {
    size_type size = std::distance(begin(), end());
    sort_impl(*this, size, comp);
}

//implementation of merge sort
template <typename T, typename Alloc>
void ForwardList<T, Alloc>::sort_impl(
        ForwardList &list,
        size_type size,
        detail::compare<T> auto comp) 
{
    if(size <= 1) {
        return;
    }

    size_type left_sz = size / 2;
    size_type right_sz = size - left_sz;
    auto pos = std::next(list.before_begin(), left_sz);

    ForwardList right;
    right.head_.setNext(pos.next());
    pos.setNext(nullptr);
    sort_impl(list, left_sz, comp);
    sort_impl(right, right_sz, comp);
    list.merge(right, comp);
}

template <typename T, typename Alloc>
void ForwardList<T, Alloc>::insert_empty_after(
        const_iterator pos, 
        size_type count) 
{
    for(size_type i = 0; i < count; ++i) {
        pos.setNext(new_empty_node(pos.next()));        
    }
}

template <typename T, typename Alloc>
typename ForwardList<T, Alloc>::Node *
ForwardList<T, Alloc>::new_empty_node(NodeBase *next) {
    Node *node = NodeTraits::allocate(alloc_, 1);
    NodeTraits::construct(alloc_, node, next);
    return node;
}

template <typename T, typename Alloc>
typename ForwardList<T, Alloc>::Node *
ForwardList<T, Alloc>::new_node(NodeBase *next, const T &value) {
    Node *node = NodeTraits::allocate(alloc_, 1);
    NodeTraits::construct(alloc_, node, next, value);
    return node;
}

template <typename T, typename Alloc>
typename ForwardList<T, Alloc>::Node *
ForwardList<T, Alloc>::new_node(NodeBase *next, T &&value) {
    Node *node = NodeTraits::allocate(alloc_, 1);
    NodeTraits::construct(alloc_, node, next, std::move(value));
    return node;
}

template <typename T, typename Alloc>
template<typename ...Args>
typename ForwardList<T, Alloc>::Node *
ForwardList<T, Alloc>::new_emplace_node(NodeBase *next, Args &&...args) {
    Node *node = NodeTraits::allocate(alloc_, 1);
    NodeTraits::construct(alloc_, node, next, std::forward<Args>(args)...);
    return node;
}

template <typename T, typename Alloc>
ForwardList<T, Alloc>::size_type 
ForwardList<T, Alloc>::crop(size_type count) {
    size_type size = std::distance(begin(), end());

    if(size > count) {
        auto it = before_begin();
        std::advance(it, count);
        erase_after(it, end());
    }

    return size;
}

template <typename T, typename Alloc>
void ForwardList<T, Alloc>::destroy_next_node(const_iterator pos) {
    Node *node = pos.next();
    pos.setNext(node->next());
    NodeTraits::destroy(alloc_, node);
    NodeTraits::deallocate(alloc_, node, 1);
}

template <typename T, typename Alloc>
bool operator == (
        const ForwardList<T, Alloc> &lhs,
        const ForwardList<T, Alloc> &rhs)
{
    if(addressof(lhs) == addressof(rhs)) {
        return true;
    }

    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename T, typename Alloc>
bool operator != (
        const ForwardList<T, Alloc> &lhs,
        const ForwardList<T, Alloc> &rhs)
{
    return !(lhs == rhs);
}

template <typename T, typename Alloc>
auto operator <=> (
        const ForwardList<T, Alloc> &lhs,
        const ForwardList<T, Alloc> &rhs)
{
    return std::lexicographical_compare_three_way(
            lhs.begin(), lhs.end(), rhs.begin, rhs.end()
        );
}

} //end namespace mystd
