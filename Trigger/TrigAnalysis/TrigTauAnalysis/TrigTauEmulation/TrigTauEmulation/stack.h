/**
Copyright 2010-2015 Bernard van Gastel, bvgastel@bitpowder.com.
This file is part of Bit Powder Libraries.

Bit Powder Libraries is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Bit Powder Libraries is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Bit Powder Libraries.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _STACK_H_
#define _STACK_H_

#include <assert.h>
#include "iterators.h"
//#include "lib-common.h"

#define NO_COPY(Class)\
Class(const Class&) = delete;\
Class& operator=(const Class&) = delete 

namespace bitpowder {
namespace lib {

template <class T, class PtrContainer, T PtrContainer::*ptr> class Queue;

template <class T, class PtrContainer = typename std::remove_pointer<T>::type, T PtrContainer::*ptr = &PtrContainer::next>
class Stack {
    NO_COPY(Stack);
  public:
    typedef T value_type;
    typedef typename std::remove_pointer<T>::type X;
    typedef Stack Container;
  private:
    mutable T value;
  public:
    inline Stack(std::initializer_list<T> l) : value(nullptr) {
        for (auto e : l)
            push(e);
    }
    inline Stack() : value(nullptr) {
    }
    inline ~Stack() {
        clear();
    }
    inline Stack(Stack&& c) : value(std::move(c.value)) {
        c.value = nullptr;
    }
    Stack& operator=(Stack&& s) {
        value = std::move(s.value);
        s.value = nullptr;
        return *this;
    }
    void clear() {
        value = nullptr;
    }

    inline void push(const T& e) {
        push_back(e);
    }

    inline T pop() {
        return pop_back();
    }

    inline void unpop(const T& e) {
        push_back(e);
    }

    inline void push_back(const T& e) {
        if (e) {
            assert(e->*ptr == nullptr);
            e->*ptr = std::move(value);
            value = e;
        }
    }

    inline void push_back(T&& e) {
        if (e) {
            assert(e->*ptr == nullptr);
            e->*ptr = std::move(value);
            value = std::move(e);
        }
    }

    inline T pop_back() {
        T e = std::move(value);
        if (e) {
            value = std::move((T)e->*ptr);
            e->*ptr = nullptr;
        }
        return e;
    }

    inline Stack pop_all() {
        return std::move(*this);
    }

    inline T back() {
        return value;
    }

    inline const T back() const {
        return value;
    }
    /*
        inline bool push_back(Stack& c) {
            if (&c == this || c.empty())
                return false;
            if (!empty()) {
                T last = c.value;
                while (last && last->*ptr) {
                    last = last->*ptr;
                }
                if (last)
                    last->*ptr = value;
            }
            value = std::move(c.value);
            c.value = nullptr;
            return true;
        }
    */
    inline bool push_back(T&& first, T&& last) {
        if (value == first || !first || !last)
            return false;
        last->*ptr = std::move(value);
        value = std::move(first);
        return true;
    }
    /*
    inline bool push_back(const T& first, const T& last) {
        if (value == first || !first || !last)
            return false;
        last->*ptr = std::move(value);
        value = first;
        return true;
    }
    */
    inline void push_back(Queue<T,PtrContainer,ptr>&& c) {
        push_back(std::move(c.head), std::move(c.tail));
        c.head = nullptr;
        c.tail = nullptr;
    }

    inline void erase(const T& e) {
        if (!e || empty())
            return;
        if (value == e) {
            pop();
            return;
        }
        // it is not the first item, and there are items
        T current = value;
        while (current->*ptr && current->*ptr != e) {
            current = current->*ptr;
        }
        if (current->*ptr) { // therefore we know current->*ptr == e, but this check is cheaper
            current->*ptr = std::move(e->*ptr);
            e->*ptr = nullptr;
        }
    }

    inline bool empty() const {
        return !value;
    }

    class const_iterator : public std::iterator<std::forward_iterator_tag, T, unsigned int> {
      protected:
        T* value;
      public:
        const_iterator(T* value) : value(value) {
        }
        const_iterator(const const_iterator& a) : value(a.value) {
        }
        const_iterator(const_iterator&& a) : value(nullptr) {
            std::swap(a.value, value);
        }
        const_iterator& operator=(const_iterator& a) {
            a.value = value;
            return *this;
        }
        const_iterator& operator=(const_iterator&& a) {
            std::swap(a.value, value);
            return *this;
        }
        ~const_iterator() {
        }
        inline T operator*() const {
            return value ? *value : nullptr;
        }
        inline T operator->() const {
            return value ? *value : nullptr;
        }
        inline void operator++() {
            value = value && *value ? &((*value)->*ptr) : nullptr;
        }
        inline const_iterator operator+(int i) {
            const_iterator retval = *this;
            while (retval && i-- > 0)
                ++retval;
            return retval;
        }
        inline bool operator!=(const const_iterator& b) const {
            return !(*this == b);
        }
        inline bool operator==(const const_iterator& b) const {
            if (value == b.value)
                return true;
            X* thisPointsTo = value && *value ? &** value : nullptr;
            X* bPointsTo = b.value && *b.value ? &** b.value : nullptr;
            return thisPointsTo == bPointsTo;
        }
        inline operator bool() const {
            return value && *value;
        }
    };

    class iterator : public const_iterator {
      public:
        iterator(T* value) : const_iterator(value) {
        }
        iterator(const_iterator&& value) : const_iterator(std::move(value)) {
        }
        inline T erase() {
            T retval = std::move(*this->value);
            *this->value = std::move(bool(retval) ? retval->*ptr : nullptr);
            return retval;
        }
        inline void insert(const T& a) {
            if (!this->value || !*this->value)
                return;
            a->*ptr = std::move(*this->value);
            *this->value = a;
        }
        inline iterator operator+(int i) {
            return {const_iterator::operator+(i)};
        }
    };


    inline iterator begin() {
        return {&value};
    }
    inline iterator end() {
        return {nullptr};
    }

    inline const_iterator begin() const {
        return {&value};
    }
    inline const_iterator end() const {
        return {nullptr};
    }

    template <class P>
    auto select(P&& func) const -> select_container<typename std::result_of<P(T)>::type::second_type, const Container, P> {
        return {*this, std::move(func)};
    }

    template <class P>
    auto select(P&& func) -> select_container<typename std::result_of<P(T)>::type::second_type, Container, P> {
        return {*this, std::move(func)};
    }

    template <class P>
    inline auto sbegin(P&& func) const -> select_iterator<typename std::result_of<P(T)>::type::second_type,P,decltype(this->begin())> {
        return {begin(), std::move(func)};
    }
    template <class P>
    inline auto sbegin(P&& func) -> select_iterator<typename std::result_of<P(T)>::type::second_type,P,decltype(this->begin())> {
        return {begin(), std::move(func)};
    }

    template <class P>
    inline int erase_if(P func) {
        int retval = 0;
        for (auto it = sbegin([&func](T e) {
        return std::make_pair(func(e), e);
        }); it; it.erase(), ++retval);
        return retval;
    }

    template <class P>
    inline auto first(P func) const -> decltype(*this->template select(std::ref(func)).begin()) {
        for (auto e : select(std::ref(func)))
            return e;
        return nullptr;
    }
};

}
}

namespace std {
    template <class T, class PtrContainer, T PtrContainer::*nextPtr>
    void swap(bitpowder::lib::Stack<T, PtrContainer, nextPtr>& lhs, bitpowder::lib::Stack<T, PtrContainer, nextPtr>& rhs) {
        std::swap(lhs.value, rhs.value);
    }
}
#endif


