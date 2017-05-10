/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS__G4UA_THREADACTIONHOLDER_H
#define G4ATLASTOOLS__G4UA_THREADACTIONHOLDER_H

// System includes
#include <thread>
#include <memory>
#include <utility>

// Other includes
#include "tbb/concurrent_unordered_map.h"

namespace G4UA
{

  /// @brief A thread-local storage wrapper for the user actions.
  ///
  /// This container is implemented as a wrapper for a concurrent map
  /// keyed by std thread ID. It is thus fully thread-safe (in theory).
  ///
  /// The thread-local storage is cleared in the destructor for now, at least
  /// until TBB's concurrent containers support move semantics and unique_ptr.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  template<class ActionType>
  class ThreadActionHolder
  {

    public:

      using ThreadMapKey_t = std::thread::id;
      using ThreadMapVal_t = ActionType*;
      //using ThreadMapVal_t = std::unique_ptr<ActionType>; // not supported
      using ThreadMapHash_t = std::hash<ThreadMapKey_t>;

      using ThreadMap_t = tbb::concurrent_unordered_map
                          < ThreadMapKey_t, ThreadMapVal_t, ThreadMapHash_t >;

      using const_iterator = typename ThreadMap_t::const_iterator;

      /// @brief Destructor will clean up the thread-local storage.
      /// Would prefer to do this automatically with unique_ptr.
      ~ThreadActionHolder() {
        for(auto mapPair : m_threadMap){
          delete mapPair.second;
        }
        m_threadMap.clear();
      }

      /// Get the object of the current thread.
      ActionType* get() {
        auto mapPair = m_threadMap.find( std::this_thread::get_id() );
        if(mapPair == m_threadMap.end()) return nullptr;
        return mapPair->second;
      }

      /// Assign the object of the current thread.
      /// Memory management must be handled manually for now.
      void set(std::unique_ptr<ActionType> action) {
        const auto tid = std::this_thread::get_id();
        m_threadMap.insert( std::make_pair(tid, action.release()) );
      }

      /// Constant-access iteration over the action map
      const_iterator begin() const {
        return m_threadMap.begin();
      }

      /// Constant-access iteration over the action map
      const_iterator end() const {
        return m_threadMap.end();
      }

    private:

      /// The wrapped thread-local storage container
      ThreadMap_t m_threadMap;

  }; // class ThreadActionHolder

}

#endif
