//  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#ifndef L1TOPOEVENT_HEAP
#define L1TOPOEVENT_HEAP

#include <vector>
#include <cstdlib>
#include <string>

namespace TCS {

   template<class T>
   class Heap {
   public:
      Heap(const std::string & name, size_t capacity = 120) :
         m_name(name),
         m_originalCapacity(capacity)
      {
         m_heap.heap = allocate(m_originalCapacity);
         m_heap.pos = (T*)m_heap.heap;
      }

      ~Heap() {
         clear();
         deallocate(m_heap.heap);
         m_heap.heap = nullptr;
         m_heap.pos = nullptr;
      }

      /* @brief provide a clean heap of original size*/
      void
      clear() {
         // destruct objects on the currently active memory block
         T * p = (T*)m_heap.heap;
         while(p != m_heap.pos) {
            (p++)->~T();
         }
         m_heap.pos = (T*)m_heap.heap; // reset the position to the beginning of the active memory block
         // destroy objects on all other memory blocks
         for(void * heap : m_heap.heapCollection) {
            T * p = (T*)heap;
            for(unsigned int t=0; t<m_originalCapacity; ++t)
               (p++)->~T();
            deallocate(heap); // destroy other blocks
         }
         m_heap.heapCollection.clear(); // clear the collection of memory blocks
      }

      /** @brief create an object on the heap */
      T* create(const T & obj) {
         if(  (int)(m_heap.pos - (T*)m_heap.heap) == (int)m_originalCapacity ) {
            extend();
         }
         T* newObject = new(m_heap.pos++) T(obj);
         return newObject;
      }

      size_t size() const {
         return (m_heap.pos - (T*)m_heap.heap) + m_heap.heapCollection.size() * m_originalCapacity;
      }

      size_t capacity() const {
         return (m_heap.heapCollection.size() + 1) * m_originalCapacity;
      }

   private:

      class HeapStructure {
      public:
         void *                heap {nullptr};             // starting point of the currently active memory block
         T *                   pos {nullptr};              // position in the currently active memory block
         std::vector<void*>    heapCollection;             // vector of pointers to already allocated memory block
      };

      HeapStructure m_heap;
      std::string m_name{};
      size_t m_originalCapacity{0};

      inline void *
      allocate(size_t size) {
         return ::operator new ( size * sizeof(T) );
      }

      inline void
      deallocate(void* & mem) {
         ::operator delete ( mem );
         mem = nullptr;
      }

      void
      extend() {
         m_heap.heapCollection.push_back(m_heap.heap); // park the old heap which has grown too small
         m_heap.heap = allocate(m_originalCapacity); // create a new heap the same size as the original one
         m_heap.pos = (T*)m_heap.heap; // make current position point to it
      }
   };
}

#endif
