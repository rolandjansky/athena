/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CopyTruthInfo.h"

#include "xAODJet/JetAuxContainer.h"
#include "xAODJet/JetContainer.h"

#include <iostream>
#include <typeinfo>

//================================================================
CopyTruthInfo::CopyTruthInfo(const std::string &name, ISvcLocator *pSvcLocator) :
  OverlayAlgBase(name, pSvcLocator) {}

//================================================================
StatusCode CopyTruthInfo::overlayInitialize()
{
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode CopyTruthInfo::overlayFinalize()
{
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode CopyTruthInfo::overlayExecute() {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "CopyTruthInfo::execute() begin"<< endreq;

  typedef std::vector<std::string> KeyList;
  KeyList keys = m_storeGateData->keys<xAOD::JetContainer>();
  for(KeyList::const_iterator k=keys.begin(); k!=keys.end(); ++k) {
    std::auto_ptr<xAOD::JetContainer> ap(m_storeGateData->retrievePrivateCopy<xAOD::JetContainer>(*k));
    log << MSG::DEBUG << "Working on p="<<ap.get()<<", key="<<*k << endreq;
    if(!m_storeGateOutput->record(ap, *k).isSuccess()) {
      log << MSG::WARNING << "Problem recording object p="<<ap.get()<<", key="<<*k << endreq;
    }
  }

  keys = m_storeGateData->keys<xAOD::JetAuxContainer>();
  for(KeyList::const_iterator k=keys.begin(); k!=keys.end(); ++k) {
    std::auto_ptr<xAOD::JetAuxContainer> ap(m_storeGateData->retrievePrivateCopy<xAOD::JetAuxContainer>(*k));
    log << MSG::DEBUG << "Working on p="<<ap.get()<<", key="<<*k << endreq;
    if(!m_storeGateOutput->record(ap, *k).isSuccess()) {
      log << MSG::WARNING << "Problem recording object p="<<ap.get()<<", key="<<*k << endreq;
    }
  }

  log << MSG::DEBUG << "CopyObjects::execute() end"<< endreq;
  return StatusCode::SUCCESS;
}

//================================================================
//EOF
