#pragma once

namespace mystd {
namespace detail {

template <typename T> 
struct FwdListNode;

template <typename T>
struct FwdListNodeBase {
    FwdListNodeBase(FwdListNodeBase *next = nullptr) : next_(next) {}
    
    void setNext(FwdListNodeBase *next) { next_ = next; }

    FwdListNode<T> *next() const { 
        return static_cast<FwdListNode<T> *>(next_); 
    }

private:
    FwdListNodeBase<T> *next_;
};

} //end namespace mystd
} //end namespace detail
