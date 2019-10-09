/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackParticleTruthCollectionContainerCnv.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionContainerCnv_tlp1.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionContainerCnv_tlp2.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionContainerCnv_tlp3.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollection_p1.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollection_p2.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollection_p3.h"

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"


TrackParticleTruthCollectionContainerCnv::TrackParticleTruthCollectionContainerCnv(ISvcLocator* svcLoc) : 
  TrackParticleTruthCollectionContainerCnvBase(svcLoc),
  m_converter_p1(new TrackParticleTruthCollectionContainerCnv_tlp1),
  m_converter_p2(new TrackParticleTruthCollectionContainerCnv_tlp2),
  m_converter_p3(new TrackParticleTruthCollectionContainerCnv_tlp3)
{;}


TrackParticleTruthCollectionContainerCnv::~TrackParticleTruthCollectionContainerCnv(){
  delete m_converter_p1;
  delete m_converter_p2;
  delete m_converter_p3;
}

TrackParticleTruthCollectionContainerPERS* TrackParticleTruthCollectionContainerCnv::createPersistent(TrackParticleTruthCollectionContainer* trans) {
  MsgStream log(messageService(), "TrackParticleTruthCollectionContainerCnv");
  log<<MSG::DEBUG<<"Writing TrackParticleTruthCollectionContainer_tlp3"<<endreq;

  TrackParticleTruthCollectionContainerPERS* p_cont =  m_converter_p3->createPersistent( trans, log );
  return p_cont;
}


TrackParticleTruthCollectionContainer* TrackParticleTruthCollectionContainerCnv::createTransient() {

  pool::Guid p1_guid("9F47124C-0033-4556-B14A-D7F28E4249EC");
  pool::Guid p2_guid("9F47124C-0033-4556-B14A-D7F28E4249ED");
  pool::Guid p3_guid("52DD075D-FAAA-49CD-A882-6D822318EA1D");


  MsgStream log(messageService(), "TrackParticleTruthCollectionContainerCnv" );
  TrackParticleTruthCollectionContainer *p_collection = nullptr;
  if( compareClassGuid( p3_guid ) ){
      poolReadObject< TrackParticleTruthCollectionContainerPERS >(*m_converter_p3);
      p_collection = m_converter_p3->createTransient( log );
  }
  else if( compareClassGuid( p2_guid ) ){
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
