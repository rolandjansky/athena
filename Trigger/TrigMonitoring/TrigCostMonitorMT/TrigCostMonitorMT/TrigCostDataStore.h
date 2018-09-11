/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTMONITORMT_TRIGCOSTDATASTORE_H
#define TRIGCOSTMONITORMT_TRIGCOSTDATASTORE_H

#include <tbb/concurrent_hash_map.h>
#include <tbb/concurrent_vector.h>

#include "TrigTimeAlgs/TrigTimeStamp.h"

#include "GaudiKernel/EventContext.h"
#include "AthenaKernel/IProxyDict.h"
#include "AthViews/View.h"

class TrigCostDataStore {
  public:

  TrigCostDataStore() : m_store() {}
  ~TrigCostDataStore() {}

  StatusCode insert(const std::string& caller, const EventContext& context, const TrigTimeStamp& stamp, MsgStream& msg);
  StatusCode retrieve(const std::string& caller, const EventContext& context, MsgStream& msg, TrigTimeStamp& result) const;
  StatusCode clear(const EventContext& context, MsgStream& msg);

  private:

  struct AlgorithmIdentifier {
    AlgorithmIdentifier(const std::string& caller, const IProxyDict* proxy) :
      m_caller(caller), 
      m_store(proxy->name()),
      m_isView(dynamic_cast<const SG::View*>(proxy) != nullptr),
      m_hash(std::hash<std::string>{}(m_caller + m_store))
      {}

    const std::string m_caller; //!< Name of the algorithm
    const std::string m_store; //!< Name of the algorithm's store. '0_StoreGateSvc_Impl' for main store.
    const bool m_isView; //!< True if the algorithm's store is an EventView
    const size_t m_hash; //!< Hash of algorithm + store. Event-unique quantity.
  };

  struct AlgorithmIdentifierHashCompare {
    static size_t hash(const AlgorithmIdentifier& ai) {
      return ai.m_hash;
    }
    static bool equal(const AlgorithmIdentifier& x, const AlgorithmIdentifier& y) {
      return (x.m_caller == y.m_caller && x.m_store == y.m_store);
    }
  };

  typedef tbb::concurrent_hash_map< AlgorithmIdentifier, TrigTimeStamp, AlgorithmIdentifierHashCompare> AITimerMapTBB;
  typedef tbb::concurrent_vector< AITimerMapTBB > VectorAITimerMapTBB;

  // Underlying store
  VectorAITimerMapTBB m_store;
};

#endif // TRIGCOSTMONITORMT_TRIGCOSTDATASTORE_H

