/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackParticleTruthCollectionContainerCnv.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionContainerCnv_tlp1.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionContainerCnv_tlp2.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollection_p1.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollection_p2.h"
#include "ParticleTruth/TrackParticleTruthVector.h"

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"


TrackParticleTruthCollectionContainerCnv::TrackParticleTruthCollectionContainerCnv(ISvcLocator* svcLoc) : 
  TrackParticleTruthCollectionContainerCnvBase(svcLoc),
  m_converter_p1(new TrackParticleTruthCollectionContainerCnv_tlp1),
  m_converter_p2(new TrackParticleTruthCollectionContainerCnv_tlp2)
{;}


TrackParticleTruthCollectionContainerCnv::~TrackParticleTruthCollectionContainerCnv(){
  delete m_converter_p1;
  delete m_converter_p2;
}

TrackParticleTruthCollectionContainerPERS* TrackParticleTruthCollectionContainerCnv::createPersistent(TrackParticleTruthCollectionContainer* trans) {
  MsgStream log(msgSvc(), "TrackParticleTruthCollectionContainerCnv");
  log<<MSG::DEBUG<<"Writing TrackParticleTruthCollectionContainer_tlp2"<<endmsg;

  TrackParticleTruthCollectionContainerPERS* p_cont =  m_converter_p2->createPersistent( trans, log );
  return p_cont;
}


TrackParticleTruthCollectionContainer* TrackParticleTruthCollectionContainerCnv::createTransient() {

  pool::Guid p1_guid("9F47124C-0033-4556-B14A-D7F28E4249EC");
  pool::Guid p2_guid("9F47124C-0033-4556-B14A-D7F28E4249ED");


  MsgStream log(msgSvc(), "TrackParticleTruthCollectionContainerCnv" );
  TrackParticleTruthCollectionContainer *p_collection = 0;
  if( compareClassGuid( p2_guid ) ){
      poolReadObject< TrackParticleTruthCollectionContainerPERS >(*m_converter_p2);
      p_collection = m_converter_p2->createTransient( log );
  }
  else if( compareClassGuid( p1_guid ) ){
      poolReadObject< TrackParticleTruthCollectionContainerPERS >(*m_converter_p1);
      p_collection = m_converter_p1->createTransient( log );
  }
  else {
    throw std::runtime_error( "Unsupported persistent version of TrigTrackCountsCollection" );
  }

  return p_collection;
}
