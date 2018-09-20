/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/ExtendedEventContext.h"

#include "TrigCostMonitorMT/TrigCostDataStore.h"

StatusCode TrigCostDataStore::insert(const EventContext& context, const std::string& caller, MsgStream& msg, const TrigTimeStamp& stamp) {
  m_store.grow_to_at_least( context.slot() + 1, AITimerMapTBB() ); // Ensure we have one map per processing slot
  const IProxyDict* proxy = context.getExtension<Atlas::ExtendedEventContext>()->proxy();
  if (proxy == nullptr) return StatusCode::FAILURE;
  const bool isView = (dynamic_cast<const SG::View*>(proxy) != nullptr);
  const AlgorithmIdentifier ai(caller, proxy->name(), isView);
  AITimerMapTBB& mapReference = m_store.at( context.slot() );
  AITimerMapTBB::accessor a;
  if (mapReference.insert(a, std::move(ai))) {
    // Obtains lock on the key value 'name' until 'a' goes out of scope or calls release()
    a->second = stamp;
  } else if (msg.level() <= MSG::WARNING) {
    msg << MSG::WARNING << "Key caller:'" << ai.m_caller << "' store:'" << ai.m_store << "' already in the TrigCostDataStore" << endmsg; 
  }
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostDataStore::retrieve(const EventContext& context, const std::string& caller, MsgStream& msg, TrigTimeStamp& result) const {
  // Use proxy from given context to get store
  const IProxyDict* proxy = context.getExtension<Atlas::ExtendedEventContext>()->proxy();
  if (proxy == nullptr) return StatusCode::FAILURE;
  return retrieve(context, proxy->name(), caller, msg, result);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostDataStore::retrieve(const EventContext& context, const std::string& storeName, const std::string& caller, MsgStream& msg, TrigTimeStamp& result) const {
  // Bundle supplied store name & caller into an AlgorithmIdentifier rather than using store from context
  const AlgorithmIdentifier ai(caller, storeName);
  return retrieve(context, ai, msg, result);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostDataStore::retrieve(const EventContext& context, const AlgorithmIdentifier& ai, MsgStream& msg, TrigTimeStamp& result) const {
  if (m_store.size() <= context.slot()) {
    msg << MSG::ERROR << "Nothing yet stored for slot #" << context.slot() << endmsg;
    return StatusCode::FAILURE;
  }
  const AITimerMapTBB& mapReference = m_store.at( context.slot() );
  AITimerMapTBB::const_accessor ca; // Does not lock other const_accessor
  if (!mapReference.find(ca, ai)) {
    if (msg.level() <= MSG::INFO) {
      msg << MSG::INFO << "Cannot access key caller:'" << ai.m_caller << "' store:'" << ai.m_store << "' from the the TrigCostDataStore" << endmsg;
    }
    return StatusCode::FAILURE;
  }
  result = ca->second;
  return StatusCode::SUCCESS; 
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostDataStore::clear(const EventContext& context, MsgStream& msg) {
  m_store.grow_to_at_least( context.slot() + 1, AITimerMapTBB() ); // Ensure we have one map per processing slot
  AITimerMapTBB& mapReference = m_store.at( context.slot() );
  mapReference.clear();
  if (msg.level() <= MSG::DEBUG) msg << MSG::DEBUG << "Clearing slot " << context.slot() << endmsg;
  return StatusCode::SUCCESS;  
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostDataStore::getIterators(const EventContext& context, MsgStream& msg, AITimerMapTBB::const_iterator& begin, AITimerMapTBB::const_iterator& end) {
  if (m_store.size() <= context.slot()) {
    msg << MSG::ERROR << "Nothing yet stored for slot #" << context.slot() << endmsg;
    return StatusCode::FAILURE;
  }
  const AITimerMapTBB& mapReference = m_store.at( context.slot() );
  begin = mapReference.begin();
  end = mapReference.end();
  return StatusCode::SUCCESS;
}
