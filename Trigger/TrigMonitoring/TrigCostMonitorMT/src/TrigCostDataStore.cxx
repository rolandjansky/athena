/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/ExtendedEventContext.h"

#include "TrigCostMonitorMT/TrigCostDataStore.h"

StatusCode TrigCostDataStore::insert(const std::string& caller, const EventContext& context, const TrigTimeStamp& stamp, MsgStream& msg) {
  m_store.grow_to_at_least( context.slot() + 1, AITimerMapTBB() ); // Ensure we have one map per processing slot
  AITimerMapTBB& mapReference = m_store.at( context.slot() );
  AITimerMapTBB::accessor a;
  const AlgorithmIdentifier ai(caller, context.getExtension<Atlas::ExtendedEventContext>()->proxy());
  if (mapReference.insert(a, std::move(ai))) {
    // Obtains lock on the key value 'name' until 'a' goes out of scope or calls release()
    a->second = stamp;
  } else if (msg.level() <= MSG::WARNING) {
    msg << MSG::WARNING << "Key caller:'" << ai.m_caller << "' store:'" << ai.m_store << "' already in the TrigCostDataStore" << endmsg; 
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigCostDataStore::retrieve(const std::string& caller, const EventContext& context, MsgStream& msg, TrigTimeStamp& result) const {
  if (m_store.size() <= context.slot()) {
    msg << MSG::ERROR << "Nothing yet stored for slot #" << context.slot() << endmsg;
    return StatusCode::FAILURE;
  }
  const AITimerMapTBB& mapReference = m_store.at( context.slot() );
  const AlgorithmIdentifier ai(caller, context.getExtension<Atlas::ExtendedEventContext>()->proxy());
  AITimerMapTBB::const_accessor ca; // Does not lock other const_accessor
  if (!mapReference.find(ca, ai)) {
    msg << MSG::ERROR << "Cannot access key caller:'" << ai.m_caller << "' store:'" << ai.m_store << "' from the the TrigCostDataStore" << endmsg;
    return StatusCode::FAILURE;
  }
  result = ca->second;
  return StatusCode::SUCCESS; 
}

StatusCode TrigCostDataStore::clear(const EventContext& context, MsgStream& msg) {
  m_store.grow_to_at_least( context.slot() + 1, AITimerMapTBB() ); // Ensure we have one map per processing slot
  AITimerMapTBB& mapReference = m_store.at( context.slot() );
  mapReference.clear();
  if (msg.level() <= MSG::DEBUG) msg << MSG::DEBUG << "End of event in slot " << context.slot() << endmsg;
  return StatusCode::SUCCESS;  
}
