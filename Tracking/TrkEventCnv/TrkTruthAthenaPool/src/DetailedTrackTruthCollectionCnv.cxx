/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DetailedTrackTruthCollectionCnv.h"

#include "TrkTruthData/DetailedTrackTruthCollection.h"
#include "TrkTruthTPCnv/DetailedTrackTruthCollection_p0.h"
#include "TrkTruthTPCnv/DetailedTrackTruthCollection_p1.h"
#include "TrkTruthTPCnv/DetailedTrackTruthCollection_p2.h"
#include "TrkTruthTPCnv/DetailedTrackTruthCollection_p3.h"

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"

// Athena
#include "StoreGate/StoreGateSvc.h"

// This is release 12 guid for DetailedTrackTruthVector.
pool::Guid DetailedTrackTruthCollectionCnv::p0_guid("8F573396-F26F-47FD-B05C-549E9D5C77E1");
pool::Guid DetailedTrackTruthCollectionCnv::p1_guid("1BA26DEE-BACE-43D6-8F19-E9490CDE8BB8");
pool::Guid DetailedTrackTruthCollectionCnv::p2_guid("B30AE3A0-F34E-45B6-A8A1-B9CB5E669EB2");
pool::Guid DetailedTrackTruthCollectionCnv::p3_guid("F5A1DC75-C3FB-4575-8178-905223CF1277");

//================================================================
DetailedTrackTruthCollectionCnv::DetailedTrackTruthCollectionCnv(ISvcLocator* svcLoc) : 
  DetailedTrackTruthCollectionCnvBase(svcLoc)
{}

//================================================================
DetailedTrackTruthCollectionPERS* DetailedTrackTruthCollectionCnv::createPersistent(DetailedTrackTruthCollection* trans) {
  MsgStream log(messageService(), "DetailedTrackTruthCollectionCnv");
  log<<MSG::DEBUG<<"Writing DetailedTrackTruthCollection_p2"<<endreq;

  //orig: return new DetailedTrackTruthCollectionPERS( *reinterpret_cast<DetailedTrackTruthCollectionPERS*>(trans) );

  DetailedTrackTruthCollectionPERS* pers=new DetailedTrackTruthCollectionPERS();
  m_converter_p2.transToPers(trans,pers,log); 
  return pers;
}

//================================================================
DetailedTrackTruthCollection* DetailedTrackTruthCollectionCnv::createTransient() {
  MsgStream log(messageService(), "DetailedTrackTruthCollectionCnv" );
  DetailedTrackTruthCollection *trans(0);

  if (compareClassGuid(p3_guid)) {
    trans = new DetailedTrackTruthCollection();
    log<<MSG::DEBUG<<"Read DetailedTrackTruthCollection_p3. GUID="<<m_classID.toString()<<endreq;
    Trk::DetailedTrackTruthCollection_p3* pers=poolReadObject<Trk::DetailedTrackTruthCollection_p3>();
    m_converter_p3.persToTrans(pers, trans, log);
    delete pers;
  }
  else if (compareClassGuid(p2_guid)) {
    trans = new DetailedTrackTruthCollection();
    log<<MSG::DEBUG<<"Read DetailedTrackTruthCollection_p2. GUID="<<m_classID.toString()<<endreq;
    Trk::DetailedTrackTruthCollection_p2* pers=poolReadObject<Trk::DetailedTrackTruthCollection_p2>();
    m_converter_p2.persToTrans(pers, trans, log);
    delete pers;
  }
  else if (compareClassGuid(p1_guid)) {
    trans = new DetailedTrackTruthCollection();
    log<<MSG::DEBUG<<"Read DetailedTrackTruthCollection_p1. GUID="<<m_classID.toString()<<endreq;
    Trk::DetailedTrackTruthCollection_p1* pers=poolReadObject<Trk::DetailedTrackTruthCollection_p1>();
    m_converter_p1.persToTrans(pers, trans, log);
    delete pers;
  }
  else {
    std::string info("Unsupported persistent version of DetailedTrackTruthCollection. GUID=" + m_classID.toString());
    log<<MSG::FATAL<<info<<endreq;
    throw std::runtime_error(info);
  }
  
  return trans;
}

//================================================================
