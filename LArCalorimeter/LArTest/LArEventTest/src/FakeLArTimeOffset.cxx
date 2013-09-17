/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArEventTest/FakeLArTimeOffset.h"
#include "LArRawConditions/LArFEBTimeOffset.h"
#include "LArRawConditions/LArGlobalTimeOffset.h"

FakeLArTimeOffset::FakeLArTimeOffset(const std::string & name, ISvcLocator * pSvcLocator)
  : Algorithm(name, pSvcLocator),
    m_globalOffset(0)
{
  declareProperty("FEBids",m_FEBids);
  declareProperty("FEbTimeOffsets",m_FEBtimeOffsets);
  declareProperty("GlobalTimeOffset",m_globalTimeOffset=0);
  declareProperty("FebTimeOffsetKey",m_keyFebOffset="FebTimeOffset");
  declareProperty("GlobalTimeOffsetKey",m_keyGlobalOffset="GlobalTimeOffset");
}

FakeLArTimeOffset::~FakeLArTimeOffset() {}

StatusCode FakeLArTimeOffset::initialize() {
  MsgStream logstr(msgSvc(), name());
  StatusCode sc;
  StoreGateSvc* detStore;
  logstr << MSG::DEBUG << "Initialize..." << endreq;
  sc= service("DetectorStore",detStore);
  if(sc.isFailure()) {
    logstr << MSG::ERROR << "DetectorStore service not found" << endreq;
    return StatusCode::FAILURE;
  }
  //Check Consistency
  if (m_FEBtimeOffsets.size()!=m_FEBids.size()) {
    logstr << MSG::ERROR << "Problem with jobOpts: Have " << m_FEBtimeOffsets.size() << " time offset values for " 
	   << m_FEBids.size() << " FEBs." << endreq;
    return StatusCode::FAILURE;
  }
  LArGlobalTimeOffset* globalTimeOffset=new LArGlobalTimeOffset();
  globalTimeOffset->setTimeOffset(m_globalTimeOffset);
  sc=detStore->record(globalTimeOffset,m_keyGlobalOffset);
  if(sc.isFailure()) {
    logstr << MSG::ERROR << "Can't record LArGlobalTimeOffset to DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }
  const ILArGlobalTimeOffset* ilarGobalTimeOffset=globalTimeOffset;
  sc=detStore->symLink(globalTimeOffset,ilarGobalTimeOffset);
  if(sc.isFailure()) {
    logstr << MSG::ERROR << "Can't symlink LArGlobalTimeOffset to abstract interface in  DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }


  LArFEBTimeOffset* febTimeOffset=new LArFEBTimeOffset();
  for (unsigned i=0;i<m_FEBids.size();i++) {
    const HWIdentifier id(m_FEBids[i]);
    febTimeOffset->setTimeOffset(id,m_FEBtimeOffsets[i]);
    logstr << MSG::DEBUG << "FEB 0x" << MSG::hex << id << " Offset=" << m_FEBtimeOffsets[i] << endreq;
  }
  sc=detStore->record(febTimeOffset,m_keyFebOffset);
  if(sc.isFailure()) {
    logstr << MSG::ERROR << "Can't record LArFEBTimeOffset to DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }
  const ILArFEBTimeOffset* ilarFEBTimeOffset=febTimeOffset;
  sc=detStore->symLink(febTimeOffset,ilarFEBTimeOffset);
  if(sc.isFailure()) {
    logstr << MSG::ERROR << "Can't symlink LArFEBTimeOffset to abstract interface in  DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}



StatusCode FakeLArTimeOffset::execute()
{return StatusCode::SUCCESS;}

StatusCode FakeLArTimeOffset::finalize()
{return StatusCode::SUCCESS;}
