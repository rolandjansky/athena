/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTMONITORMT_TRIGCOSTDATASTORE_H
#define TRIGCOSTMONITORMT_TRIGCOSTDATASTORE_H 1

#include <tbb/concurrent_hash_map.h>
#include <tbb/concurrent_vector.h>

#include "TrigTimeAlgs/TrigTimeStamp.h"

#include "GaudiKernel/EventContext.h"
#include "AthViews/View.h"

  /** 
   * @class TrigCostDataStore
   * @brief Thread safe and multi-slot hash-map to cache TrigTimeStamp objects during event execution
   *
   * The TrigTimeStamp objects are later used to construct a trigger cost summary for the event.
   * Data are partitioned based on the slot of the event context and both the name and store of the 
   * audited algorithm, these three pieces of data uniquely identify an algorithm execution.
   */
class TrigCostDataStore {
  public:

  /** 
   * @class AlgorithmIdentifier
   * @brief Small structure to hold an algorithm's name and store, plus some details on its EventView.
   */
  struct AlgorithmIdentifier {
    AlgorithmIdentifier(const std::string& caller, const std::string& storeName, const int16_t viewID = -1) :
      m_caller(caller), 
      m_store(storeName),
      m_viewID(viewID),
      m_hash(std::hash<std::string>{}(m_caller + m_store))
      {}

    const std::string m_caller; //!< Name of the algorithm
    const std::string m_store; //!< Name of the algorithm's store. '0_StoreGateSvc_Impl' for main store.
    const int16_t m_viewID; //!< If within an event view, then the ViewID. -1 if the main store.
    const size_t m_hash; //!< Hash of algorithm + store. Event-unique quantity.
  };

  /** 
   * @class AlgorithmIdentifierHashCompare
   * @brief Static hash and equal members as required by tbb::concurrent_hash_map
   */
  struct AlgorithmIdentifierHashCompare {
    /** 
     * @brief Hash function for AlgorithmIdentifier to assign bucket in hash map
     * @param[in] ai AlgorithmIdentifier to calculate hash for
     * @returns Cached std::hash<std::string> of algorithm name combined with algorithm store
     */
    static size_t hash(const AlgorithmIdentifier& ai) {
      return ai.m_hash;
    }

    /** 
     * @brief Equality function for AlgorithmIdentifier to check for uniqueness within hash bucket
     * @param[in] x First AlgorithmIdentifier to check equality of
     * @param[in] y Second AlgorithmIdentifier to check equality of
     * @returns True if both the algorithm name and algorithm store strings are identical
     */
    static bool equal(const AlgorithmIdentifier& x, const AlgorithmIdentifier& y) {
      return (x.m_caller == y.m_caller && x.m_store == y.m_store);
    }
  };

  typedef tbb::concurrent_hash_map< AlgorithmIdentifier, TrigTimeStamp, AlgorithmIdentifierHashCompare> AITimerMapTBB;
  typedef std::vector< AITimerMapTBB > VectorAITimerMapTBB;

  /**
   * @brief Default constructor
   */
  TrigCostDataStore() : m_store() {}

  /**
   * @brief Destructor. Currently empty.
   */
  ~TrigCostDataStore() {}

  /**
   * @brief Initialise internal storage
   * @param[in] nSlots. Number of possible concurrent slots to reserve space for.
   */
  StatusCode initialize(size_t nSlots);

  /**
   * @brief Inserts the payload into the map
   * @param[in] context The event context of the algorithm
   * @param[in] caller The name of the algorithm to monitor
   * @param[in] msg Message service reference
   * @param[in] payload The payload to record
   * @returns Success unless and out-of-range slot, then Failure
   */
  StatusCode insert(const EventContext& context, const std::string& caller,  MsgStream& msg, const TrigTimeStamp& payload);

  /**
   * @brief Retrieve a payload from the map given an algorithm name and event context
   * @param[in] context The event context of the algorithm
   * @param[in] caller The name of the algorithm to retrieve
   * @param[in] msg Message service reference
   * @param[out] payload Reference to payload to return
   * @returns Success if the payload was located, else Failure
   */
  StatusCode retrieve(const EventContext& context, const std::string& caller, MsgStream& msg, TrigTimeStamp& payload) const;

  /**
   * @brief Retrieve a payload from the map given an algorithm name and store name. Note, this overrides the store in the context
   * @param[in] context The event context of the algorithm
   * @param[in] storeName The name of the algorithm's store. Overrides what is in the supplied context
   * @param[in] caller The name of the algorithm to retrieve
   * @param[in] msg Message service reference
   * @param[out] payload Reference to payload to return
   * @returns Success if the payload was located, else Failure
   */
  StatusCode retrieve(const EventContext& context, const std::string& storeName, const std::string& caller, MsgStream& msg, TrigTimeStamp& payload) const;

  /**
   * @brief Retrieve a payload from the map given an AlgorithmIdentifier
   * @param[in] context The event context of the algorithm
   * @param[in] ai The AlgorithmIdentifier to fetch
   * @param[in] msg Message service reference
   * @param[out] payload Reference to payload to return
   * @returns Success if the payload was located, else Failure
   */
  StatusCode retrieve(const EventContext& context, const AlgorithmIdentifier& ai, MsgStream& msg, TrigTimeStamp& payload) const;

  /**
   * @brief Clears all data stored in an event slot
   * @param[in] context The event context of the slot to clear
   * @param[in] msg Message service reference
   * @returns Success if the slot was cleared
   */
  StatusCode clear(const EventContext& context, MsgStream& msg);

  /**
   * @brief Fetches Begin and End const iterators to iterate over the data store
   * @param[in] context The event context of the slot to return iterators over
   * @param[in] msg Message service reference
   * @param[out] begin Return of iterator to start of map
   * @param[out] end Return of iterator to end of map
   * @returns Success if the slot is populated, else Failure
   */
  StatusCode getIterators(const EventContext& context, MsgStream& msg, AITimerMapTBB::const_iterator& begin, AITimerMapTBB::const_iterator& end);

  private:

  /**
   * @brief Internal check that the supplied slot number is in range
   * @param[in] slot Event context slot ID
   * @param[in] msg Message service reference
   * @returns Success if the class has been initialised with setSlots(nSlots) where nSlots > slot 
   */
  StatusCode checkSlot(const size_t slot, MsgStream& msg) const;

  VectorAITimerMapTBB m_store; //!< Underlying thread-safe data store for all slots: vector< hashmap< AlgorithmIdentifier, payload > >
};

#endif // TRIGCOSTMONITORMT_TRIGCOSTDATASTORE_H

