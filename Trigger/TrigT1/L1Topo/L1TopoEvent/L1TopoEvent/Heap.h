//  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#ifndef L1TOPOEVENT_HEAP
#define L1TOPOEVENT_HEAP

#include <vector>
#include <exception>
#include <mutex>
#include <pthread.h>
#include <unordered_map>


namespace TCS {

   template<class T>
   class Heap {
   public:
      Heap(const std::string & name, size_t capacity = 120) :
         m_name(name),
         m_originalCapacity(capacity)
      {}

      ~Heap() {
         for( auto & x : m_threadHeapMap ) {
            destroy(x.second);
         }
         m_threadHeapMap.clear();
      }

      /* @brief provide a clean heap for the current thread
       * either clears the existing heap or provides a new one
       */
      void
      clear() {
         pthread_t threadId = pthread_self();
         typename std::unordered_map<pthread_t, HeapStructure>::iterator x;
         {
            auto lock = std::scoped_lock{m_map_lock};
            x = m_threadHeapMap.find(threadId);
         }
         if(x != m_threadHeapMap.end()) {
            clear(x->second);
         } else {
            initHeap(threadId);
         }
      }

      /** @brief create an object on the heap for the current thread
       */
      T* create(const T & obj) {
         auto & h = getHeap(pthread_self());
         if(  (int)(h.pos - (T*)h.heap) == (int)m_originalCapacity ) {
            extend(h);
         }
         T* newObject = new(h.pos++) T(obj);
         return newObject;
      }

      size_t size() const {
         const auto & h = getHeap(pthread_self());
         return (h.pos - (T*)h.heap) + h.heapCollection.size() * m_originalCapacity;
      }

      size_t capacity() const {
         const auto & h = getHeap(pthread_self());
         return h.capacity;
      }

   private:

      class HeapStructure {
      public:
         void *                heap {nullptr};
         T *                   pos {nullptr};
         std::vector<void*>    heapCollection;
         std::vector<size_t>   heapSizes;
         size_t                capacity{0};                // allocated space in byte
      };

      std::unordered_map<pthread_t, HeapStructure> m_threadHeapMap;
      mutable std::mutex m_map_lock;
      std::string m_name{};
      size_t m_originalCapacity{0};

      HeapStructure &
      getHeap(pthread_t threadId) {
         auto lock = std::scoped_lock{m_map_lock};
         return m_threadHeapMap.at(threadId);
      }

      const HeapStructure &
      getHeap(pthread_t threadId) const {
         auto lock = std::scoped_lock{m_map_lock};
         return m_threadHeapMap.at(threadId);
      }

      void
      destroy(HeapStructure & h) {
         clear(h);
         free(h.heap);
         h.heap = nullptr;
         h.pos = nullptr;
         h.capacity = 0;
      }

      void
      extend(HeapStructure & h) {
         h.heapCollection.push_back(h.heap); // park the old heap which has grown too small
         h.heap = malloc( m_originalCapacity * sizeof(T) ); // create a new heap the same size as the original one
         h.pos = (T*)h.heap; // make current position point to it
         h.capacity += m_originalCapacity; // adjust the capacity
      }


      void
      initHeap(pthread_t threadId)
      {
         HeapStructure h;
         h.capacity = m_originalCapacity;
         h.heap = malloc( h.capacity * sizeof(T) );
         h.pos = (T*)h.heap;
         auto lock = std::scoped_lock{m_map_lock};
         m_threadHeapMap[threadId] = h;
      }

      void
      clear(HeapStructure & h) {
         // destroy objects on the heap structure
         T * p = (T*)h.heap;
         while(p != h.pos) {
            (p++)->~T();
         }
         // destroy object on previous
         for(void * heap : h.heapCollection) {
            T * p = (T*)heap;
            for(unsigned int t=0; t<m_originalCapacity; ++t)
               (p++)->~T();
            free(heap); // destroy these
         }
         h.heapCollection.clear();
         h.pos = (T*)h.heap;
         h.capacity = m_originalCapacity;
      }
   };
}

#endif
