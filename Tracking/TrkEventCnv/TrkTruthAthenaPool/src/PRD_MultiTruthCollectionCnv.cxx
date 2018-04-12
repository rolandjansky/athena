/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PRD_MultiTruthCollectionCnv.h"
#include "TrkTruthTPCnv/PRD_MultiTruthCollection_p1.h"
#include "TrkTruthTPCnv/PRD_MultiTruthCollection_p2.h"
#include "TrkTruthTPCnv/PRD_MultiTruthCollection_p3.h"
#include "TrkTruthTPCnv/PRD_MultiTruthCollectionCnv_p1.h"
#include "TrkTruthTPCnv/PRD_MultiTruthCollectionCnv_p2.h"
#include "TrkTruthTPCnv/PRD_MultiTruthCollectionCnv_p3.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"

pool::Guid PRD_MultiTruthCollectionCnv::p0_guid("30794FF9-F003-44A6-8553-ED61E2039882");
pool::Guid PRD_MultiTruthCollectionCnv::p1_guid("6649A9D1-719F-4954-A385-D01BCA8E41EF");
pool::Guid PRD_MultiTruthCollectionCnv::p2_guid("714F2E4A-419D-4BDB-8080-BEEB6CDBA0DA");
pool::Guid PRD_MultiTruthCollectionCnv::p3_guid("68703476-9D09-4504-B492-E5E4DC933E71");

//================================================================
PRD_MultiTruthCollectionCnv::PRD_MultiTruthCollectionCnv(ISvcLocator* svcLoc) : 
  PRD_MultiTruthCollectionCnvBase(svcLoc)
{}

//================================================================
PRD_MultiTruthCollectionPERS* PRD_MultiTruthCollectionCnv::createPersistent(PRD_MultiTruthCollection* trans) {
  MsgStream log(messageService(), "PRD_MultiTruthCollectionCnv");
  log<<MSG::DEBUG<<"Writing PRD_MultiTruthCollection_p2"<<endreq;
  PRD_MultiTruthCollectionPERS* pers=new PRD_MultiTruthCollectionPERS();
  m_converter_p2.transToPers(trans,pers,log);
  return pers;
}

//================================================================
PRD_MultiTruthCollection* PRD_MultiTruthCollectionCnv::createTransient() {
  MsgStream log(messageService(), "PRD_MultiTruthCollectionCnv" );
  std::auto_ptr<PRD_MultiTruthCollection> trans(new PRD_MultiTruthCollection());

  if (compareClassGuid(p3_guid)) {
    log<<MSG::DEBUG<<"Read PRD_MultiTruthCollection_p3. GUID="<<m_classID.toString()<<endreq;
    Trk::PRD_MultiTruthCollection_p3* pers=poolReadObject<Trk::PRD_MultiTruthCollection_p3>();
    m_converter_p3.persToTrans(pers, trans.get(), log);
    delete pers;
  }
  else if (compareClassGuid(p2_guid)) {
    log<<MSG::DEBUG<<"Read PRD_MultiTruthCollection_p2. GUID="<<m_classID.toString()<<endreq;
    Trk::PRD_MultiTruthCollection_p2* pers=poolReadObject<Trk::PRD_MultiTruthCollection_p2>();
    m_converter_p2.persToTrans(pers, trans.get(), log);
    delete pers;
  }
  else if (compareClassGuid(p1_guid)) {
    log<<MSG::DEBUG<<"Read PRD_MultiTruthCollection_p1. GUID="<<m_classID.toString()<<endreq;
    Trk::PRD_MultiTruthCollection_p1* pers=poolReadObject<Trk::PRD_MultiTruthCollection_p1>();
    m_converter_p1.persToTrans(pers, trans.get(), log);
    delete pers;
  }
  else if (compareClassGuid(p0_guid)) {
    log<<MSG::DEBUG<<"Read version p0 of PRD_MultiTruthCollection. GUID="<<m_classID.toString()<<endreq;
    trans.reset(poolReadObject<PRD_MultiTruthCollection>());
  }
  else {
    log<<MSG::ERROR<<"Unsupported persistent version of PRD_MultiTruthCollection. GUID="
       <<m_classID.toString()<<endreq;
    throw std::runtime_error("Unsupported persistent version of Data Collection");
  }
  
  return trans.release();
}

//================================================================
