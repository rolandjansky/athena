/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AthViews/View.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "AthenaBaseComps/AthCheckMacros.h"

#include "TrigCostMonitor/AlgorithmIdentifier.h"

/////////////////////////////////////////////////////////////////////////////

AlgorithmIdentifier::AlgorithmIdentifier() :
  m_realSlot(),
  m_slotToSaveInto(),
  m_caller(""), 
  m_store(""),
  m_viewID(0),
  m_hash(0)
  {}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

AlgorithmIdentifier::AlgorithmIdentifier(const size_t realSlot, const size_t saveSlot, const std::string& caller, const std::string& storeName, const int16_t viewID) :
  m_realSlot(realSlot),
  m_slotToSaveInto(saveSlot),
  m_caller(caller), 
  m_store(storeName),
  m_viewID(viewID),
  m_hash(std::hash<std::string>{}(m_caller + m_store + std::to_string(realSlot)))
  {}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

TrigConf::HLTHash AlgorithmIdentifier::callerHash (MsgStream& msg) const {
  try {
    return TrigConf::HLTUtils::string2hash(m_caller, "ALG");
  } catch (const std::exception& ex) {
    msg << MSG::DEBUG << "Caught " << typeid(ex).name() << ": " << ex.what() << endmsg;
  }
  return 0;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

TrigConf::HLTHash AlgorithmIdentifier::storeHash (MsgStream& msg) const {
  try {
    return TrigConf::HLTUtils::string2hash(m_store, "STORE");
  } catch (const std::exception& ex) {
    msg << MSG::DEBUG << "Caught " << typeid(ex).name() << ": " << ex.what() << endmsg;
  }
  return 0;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode AlgorithmIdentifier::isValid() const {
  if (m_caller == "" || m_store == "" || !m_hash) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void AlgorithmIdentifier::dump (MsgStream& msg) {
  TrigConf::HLTHash theCallerHash = callerHash(msg);
  TrigConf::HLTHash theStoreHash = storeHash(msg);

  msg << MSG::INFO << "----------" << endmsg;
  msg << "Valid? " << (isValid() ? "YES" : "NO") << endmsg;
  msg << "Real Slot:" << m_realSlot << endmsg;
  msg << "Slot Save Overrride:" << m_slotToSaveInto << endmsg;
  msg << "Caller:'" << m_caller << "'" << endmsg;
  msg << "Caller Hash:" << theCallerHash << endmsg;
  msg << "Store:'" << m_store << "'" << endmsg;
  msg << "Store Hash:" << theStoreHash << endmsg;
  msg << "View ID:" << m_viewID << endmsg;
  msg << "AI Hash:" << m_hash << endmsg;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

AlgorithmIdentifier AlgorithmIdentifierMaker::make(const EventContext& context, const std::string& caller, MsgStream& msg, const int16_t slotOverride) {
  const SG::View* view = nullptr;
  const IProxyDict* proxy = nullptr;
  if (Atlas::hasExtendedEventContext(context)) {
    proxy = Atlas::getExtendedEventContext(context).proxy();
    if (proxy) view = dynamic_cast<const SG::View*>(proxy);
  }
  if (!proxy) {
    msg << MSG::ERROR << "Unable to obtain IProxyDict from Atlas::ExtendedEventContext" << endmsg;
    return AlgorithmIdentifier(); // Return a default constructed, invalid, identifier
  }
  const int16_t viewID = (view == nullptr ? AlgorithmIdentifier::s_noView : view->viewID());
  const size_t saveSlot = (slotOverride >= 0 ? slotOverride : context.slot());
  return AlgorithmIdentifier(context.slot(), saveSlot, caller, proxy->name(), viewID);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
