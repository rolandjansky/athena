/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "AthViews/View.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "AthenaBaseComps/AthCheckMacros.h"

#include "AlgorithmIdentifier.h"

/////////////////////////////////////////////////////////////////////////////

AlgorithmIdentifier::AlgorithmIdentifier() :
  m_context(nullptr),
  m_msg(nullptr),
  m_caller(""), 
  m_store(""),
  m_viewID(0),
  m_hash(0)
  {}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

AlgorithmIdentifier::AlgorithmIdentifier(const EventContext& context, const std::string& caller, const std::string& storeName, MsgStream& msg, const int16_t viewID) :
  m_context(&context),
  m_msg(&msg),
  m_caller(caller), 
  m_store(storeName),
  m_viewID(viewID),
  m_hash(std::hash<std::string>{}(m_caller + m_store))
  {}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

TrigConf::HLTHash AlgorithmIdentifier::callerHash() const {
  return TrigConf::HLTUtils::string2hash(m_caller, "ALG");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

TrigConf::HLTHash AlgorithmIdentifier::storeHash() const {
  return TrigConf::HLTUtils::string2hash(m_store, "STORE");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode AlgorithmIdentifier::isValid() const {
  if (m_context == nullptr || m_msg == nullptr) return StatusCode::FAILURE;
  if (m_hash == 0) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

AlgorithmIdentifier AlgorithmIdentifierMaker::make(const EventContext& context, const std::string& caller, MsgStream& msg) {
  const SG::View* view = nullptr;
  const IProxyDict* proxy = nullptr;
  if (context.hasExtension<Atlas::ExtendedEventContext>()) {
    proxy = context.getExtension<Atlas::ExtendedEventContext>().proxy();
    if (proxy) view = dynamic_cast<const SG::View*>(proxy);
  }
  if (!proxy) {
    msg << MSG::ERROR << "Unable to obtain IProxyDict from Atlas::ExtendedEventContext" << endmsg;
    return AlgorithmIdentifier(); // Return a default constructed, invalid, identifier
  }
  const int16_t viewID = (view == nullptr ? AlgorithmIdentifier::s_noView : view->viewID());
  return AlgorithmIdentifier(context, caller, proxy->name(), msg, viewID);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
