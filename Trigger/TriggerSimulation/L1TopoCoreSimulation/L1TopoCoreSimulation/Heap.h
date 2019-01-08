/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  Heap.h
//  L1TopoCoreSimulation
//
//  Created by Joerg Stelzer on 11/23/12.
//

#ifndef __L1TopoCoreSimulation__Heap__
#define __L1TopoCoreSimulation__Heap__

#include <iostream>
#include <cstdlib>

namespace TCS {

   template<class T>
   class Heap {
   public:
      Heap(size_t capacity = 10) :
         m_heap(0)
         , m_pos(0)
         , m_capacity(0)
      {
         initialize(capacity);
      }

      void
      initialize(size_t capacity = 10) {
         if(m_heap==0) {
            m_heap = malloc(capacity*sizeof(T));
            m_pos = (T*)m_heap;
            m_capacity = capacity;
         }
      }

      size_t
      extend(size_t extendCapacity) {
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
         m_heap = m_pos = m_capacity = 0;
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

#endif /* defined(__L1TopoCoreSimulation__Heap__) */
