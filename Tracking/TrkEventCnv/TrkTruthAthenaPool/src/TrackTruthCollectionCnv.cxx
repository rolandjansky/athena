/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackTruthCollectionCnv.h"

#include "TrkTruthData/TrackTruthCollection.h"
#include "TrkTruthTPCnv/TrackTruthCollection_p0.h"
#include "TrkTruthTPCnv/TrackTruthCollection_p1.h"
#include "TrkTruthTPCnv/TrackTruthCollectionCnv_p1.h"

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"

// Athena
#include "StoreGate/StoreGateSvc.h"

// This is release 12 guid for TrackTruthVector.
pool::Guid TrackTruthCollectionCnv::s_p0_guid("8BC86D69-DBC4-4B34-8273-513D9BE771D5");

pool::Guid TrackTruthCollectionCnv::s_p1_guid("ED2B4B64-7CF6-48B3-8C40-29F8501A6090");


//================================================================
TrackTruthCollectionCnv::TrackTruthCollectionCnv(ISvcLocator* svcLoc) : 
  TrackTruthCollectionCnvBase(svcLoc)
{}

//================================================================
TrackTruthCollectionPERS* TrackTruthCollectionCnv::createPersistent(TrackTruthCollection* trans) {
  MsgStream log(msgSvc(), "TrackTruthCollectionCnv");
  log<<MSG::DEBUG<<"Writing TrackTruthCollection_p1"<<endmsg;
  TrackTruthCollectionPERS* pers=new TrackTruthCollectionPERS();
  m_converter_p1.transToPers(trans,pers,log); 
  return pers;
}

//================================================================
TrackTruthCollection* TrackTruthCollectionCnv::createTransient() {
  MsgStream log(msgSvc(), "TrackTruthCollectionCnv" );
  std::auto_ptr<TrackTruthCollection> trans(new TrackTruthCollection());
  
  if (compareClassGuid(s_p1_guid)) {
    log<<MSG::DEBUG<<"Read TrackTruthCollection_p1. GUID="<<m_classID.toString()<<endmsg;
    Trk::TrackTruthCollection_p1* pers=poolReadObject<Trk::TrackTruthCollection_p1>();
    m_converter_p1.persToTrans(pers, trans.get(), log);
    delete pers;
  }
  else if (compareClassGuid(s_p0_guid)) {
    log<<MSG::DEBUG<<"Read version p0 of TrackTruthCollection. GUID="<<m_classID.toString()<<endmsg;
    TrackTruthVector *pers = poolReadObject<TrackTruthVector>();
    m_converter_p0.persToTrans(pers, trans.get(), log);
    delete pers;
  }
  else {
    log<<MSG::ERROR<<"Unsupported persistent version of TrackTruthCollection. GUID="
       <<m_classID.toString()<<endmsg;
    throw std::runtime_error("Unsupported persistent version of Data Collection");
  }
  
  return trans.release();
}

//================================================================
