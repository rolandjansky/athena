/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArEventTest/FakeLArOFCs.h"
#include "LArRawConditions/LArOFCComplete.h"
#include "LArIdentifier/LArOnlineID.h"


FakeLArOFCs::FakeLArOFCs(const std::string & name, ISvcLocator * pSvcLocator): Algorithm(name, pSvcLocator) {
  declareProperty("MaxSample",m_maxSample=2);
  declareProperty("NSample",m_NSample=5);
  declareProperty("OFCKey",m_keyOFC="LArOFC");
}

FakeLArOFCs::~FakeLArOFCs() {}

                     
StatusCode FakeLArOFCs::initialize() {
  MsgStream log(msgSvc(), name());
  StatusCode sc;
  StoreGateSvc* detStore;
  log << MSG::DEBUG << "Initialize..." << endreq;
  sc= service("DetectorStore",detStore);
  if(sc.isFailure()) {
    log << MSG::ERROR << "DetectorStore service not found" << endreq;
    return StatusCode::FAILURE;
  }

  const LArOnlineID* onlineHelper = 0;
  sc = detStore->retrieve(onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    log << MSG::ERROR << "Could not get LArOnlineID helper !" << endreq;
    return StatusCode::FAILURE;
  }

  std::vector<std::vector<float> > aVec(25);
  for (int i=0;i<m_NSample;i++)
    if (i==m_maxSample)
      aVec[0].push_back(1);
    else
      aVec[0].push_back(0);

  std::vector< std::vector<float> > bVec(25); //Stays zero-size;
  LArOFCComplete* larOFCComplete=new LArOFCComplete();
  for(int igain=0; igain<3; igain++) {
    std::vector<HWIdentifier>::const_iterator itOnId = onlineHelper->channel_begin();
    std::vector<HWIdentifier>::const_iterator itOnId_e = onlineHelper->channel_end();
    for (;itOnId!=itOnId_e;itOnId++) {
      const HWIdentifier ch_id= *itOnId;
      larOFCComplete->set(ch_id,(int)igain,aVec,bVec);
    }
  }//End loop over gains.
  detStore->record(larOFCComplete,m_keyOFC);
  const ILArOFC* iLArOFC=larOFCComplete;
  detStore->symLink(larOFCComplete,iLArOFC);
  return StatusCode::SUCCESS;
}

StatusCode FakeLArOFCs::execute()
{return StatusCode::SUCCESS;}
  
StatusCode FakeLArOFCs::finalize()
{return StatusCode::SUCCESS;}
