/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/ExtendedEventContext.h"

#include "AthenaBaseComps/AthCheckMacros.h"

#include "TrigCostDataStore.h"

/////////////////////////////////////////////////////////////////////////////

StatusCode TrigCostDataStore::initialize(size_t nSlots) {
  m_store.resize( nSlots, AITimerMapTBB() );
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostDataStore::insert(const EventContext& context, const std::string& caller, MsgStream& msg, const TrigTimeStamp& payload) {
  ATH_CHECK( checkSlot(context.slot(), msg) );
  const IProxyDict* proxy = context.getExtension<Atlas::ExtendedEventContext>()->proxy();
  if (proxy == nullptr) return StatusCode::FAILURE;
  const SG::View* view = dynamic_cast<const SG::View*>(proxy);
  const int16_t viewID = (view == nullptr ? -1 : view->viewID());
  const AlgorithmIdentifier ai(caller, proxy->name(), viewID);
  AITimerMapTBB& mapReference = m_store.at( context.slot() );
  AITimerMapTBB::accessor a;
  if (mapReference.insert(a, std::move(ai))) {
    // Obtains lock on the key value 'name' until 'a' goes out of scope or calls release()
    a->second = payload;
  } else if (msg.level() <= MSG::WARNING) {
    msg << MSG::WARNING << "Key caller:'" << ai.m_caller << "' store:'" << ai.m_store << "' already in the TrigCostDataStore" << endmsg; 
  }
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostDataStore::retrieve(const EventContext& context, const std::string& caller, MsgStream& msg, TrigTimeStamp& payload) const {
  // Use proxy from given context to get the store (the name of the IProxyDict)
  const IProxyDict* proxy = context.getExtension<Atlas::ExtendedEventContext>()->proxy();
  if (proxy == nullptr) return StatusCode::FAILURE;
  return retrieve(context, proxy->name(), caller, msg, payload);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostDataStore::retrieve(const EventContext& context, const std::string& storeName, const std::string& caller, MsgStream& msg, TrigTimeStamp& payload) const {
  // Bundle supplied store name & caller into an AlgorithmIdentifier, rather than using the store from the context
  const AlgorithmIdentifier ai(caller, storeName);
  return retrieve(context, ai, msg, payload);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostDataStore::retrieve(const EventContext& context, const AlgorithmIdentifier& ai, MsgStream& msg, TrigTimeStamp& payload) const {
  ATH_CHECK( checkSlot(context.slot(), msg) );
  const AITimerMapTBB& mapReference = m_store.at( context.slot() );
  AITimerMapTBB::const_accessor ca; // Does not lock other const_accessor
  if (!mapReference.find(ca, ai)) {
    if (msg.level() <= MSG::DEBUG) {
      msg << MSG::DEBUG << "Cannot access key caller:'" << ai.m_caller << "' store:'" << ai.m_store << "' from the the TrigCostDataStore" << endmsg;
    }
    return StatusCode::FAILURE;
  }
  payload = ca->second;
  return StatusCode::SUCCESS; 
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostDataStore::clear(const EventContext& context, MsgStream& msg) {
  ATH_CHECK( checkSlot(context.slot(), msg) );
  AITimerMapTBB& mapReference = m_store.at( context.slot() );
  mapReference.clear();
  if (msg.level() <= MSG::DEBUG) msg << MSG::DEBUG << "Clearing slot " << context.slot() << endmsg;
  return StatusCode::SUCCESS;  
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostDataStore::getIterators(const EventContext& context, MsgStream& msg, AITimerMapTBB::const_iterator& begin, AITimerMapTBB::const_iterator& end) {
  ATH_CHECK( checkSlot(context.slot(), msg) );
  const AITimerMapTBB& mapReference = m_store.at( context.slot() );
  begin = mapReference.begin();
  end = mapReference.end();
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode TrigCostDataStore::checkSlot(const size_t slot, MsgStream& msg) const {
  if (m_store.size() <= slot) {
    msg << MSG::FATAL << "Requesting slot " << slot << " but we only reserved for " << m_store.size() << " slots. Make sure setSlots() was called." << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

