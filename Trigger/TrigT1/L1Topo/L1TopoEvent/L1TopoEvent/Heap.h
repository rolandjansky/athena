//  Heap.h
//  L1TopoEvent
//  Created by Joerg Stelzer on 11/23/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#ifndef __L1TopoEvent__Heap__
#define __L1TopoEvent__Heap__

#include <iostream>
#include <cstdlib>

namespace TCS {

   template<class T>
   class Heap {
   public:
      Heap(size_t capacity = 100) :
         m_heap(0)
         , m_pos(0)
         , m_capacity(capacity)
      {
         m_heap = malloc( m_capacity * sizeof(T) );
         m_pos = (T*)m_heap;
      }

      ~Heap() {
         destroy();
      }

      size_t
      extend(size_t extendCapacity = 0) {
         if(extendCapacity==0)
            extendCapacity = m_capacity;
         void * newHeap = malloc( (m_capacity+extendCapacity) * sizeof(T) );
         size_t oldSize = size();
         memcpy(newHeap, m_heap, (m_pos-m_heap)  * sizeof(T) );
         free(m_heap);
         m_heap = newHeap;
         m_pos = (T*)newHeap;
         m_pos += oldSize;
         return m_capacity;
      }

      void
      clear() {
         T * p = (T*)m_heap;
         while(p != m_pos)
            (p++)->~T();
         m_pos = (T*)m_heap;
      }

      void
      destroy() {
         clear();
         free(m_heap);
         m_heap = 0;
         m_pos = (T*)0;
         m_capacity = 0;
      }

      T* create(const T & obj) {
         return new(m_pos++) T(obj);
      }

      size_t size() const { return m_pos-(T*)m_heap; }
      size_t capacity() const { return m_capacity; }

   private:
      void * m_heap;
      T * m_pos;

      size_t m_capacity; // allocated space in byte
   };

} // end of namespace TCS

#endif /* defined(__L1TopoEvent__Heap__) */
