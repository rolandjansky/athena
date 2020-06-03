/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  Heap.h
//  L1TopoEvent
//  Created by Joerg Stelzer on 11/23/12.

#ifndef __L1TopoEvent__Heap__
#define __L1TopoEvent__Heap__

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>

namespace TCS {

   template<class T>
   class Heap {
   public:
      Heap(const std::string & heapname, size_t capacity = 120) :
         m_heapname(heapname)
         , m_heap(0)
         , m_pos(0)
         , m_capacity(capacity)
         , m_originalCapacity(capacity)
      {
         m_heap = malloc( m_capacity * sizeof(T) );
         m_pos = (T*)m_heap;
      }

      ~Heap() {
         destroy();
      }

      void
      extend() {
         m_heapCollection.push_back(m_heap); // park the old heap which has grown too small

         m_heap = malloc( m_originalCapacity * sizeof(T) ); // create a new heap the same size as the original one

         m_pos = (T*)m_heap; // make current position point to it

         m_capacity += m_originalCapacity; // adjust the capacity
      }


      void
      clear() {
         // destroy objects on current heap
         T * p = (T*)m_heap;
         while(p != m_pos) {
            (p++)->~T();
         }
         // destroy object on previous 
         for(void * heap : m_heapCollection) {
            T * p = (T*)heap;
            for(unsigned int t=0; t<m_originalCapacity; ++t)
               (p++)->~T();
            free(heap); // destroy these
         }
         m_heapCollection.clear();
            
         m_pos = (T*)m_heap;

         m_capacity = m_originalCapacity;
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
         //std::cout << "about to create size = " << size() << "  capacity = " << capacity() << " of heap " << m_heapname << std::endl;
        if(  (int)(m_pos - (T*)m_heap) == (int)m_originalCapacity ) {
            extend();
         }
         //std::cout << "Creating TOB on " << m_heapname << ", heap size " << size() << ", heap capacity " << capacity() << std::endl;
         T* newObject = new(m_pos++) T(obj);
         //std::cout << "create " << newObject << std::endl;
         return newObject;
      }

      size_t size() const { return (m_pos - (T*)m_heap) + m_heapCollection.size() * m_originalCapacity; }

      size_t capacity() const { return m_capacity; }

   private:

      std::string m_heapname;
      void * m_heap;
      T * m_pos;
      std::vector<void*> m_heapCollection;
      std::vector<size_t> m_heapSizes;

      size_t m_capacity; // allocated space in byte
      size_t m_originalCapacity;
   };

} // end of namespace TCS

#endif /* defined(__L1TopoEvent__Heap__) */
