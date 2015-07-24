/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// T/P converter for PRD_MultiTruthCollection.
// Andrei Gaponenko <agaponenko@lbl.gov>, 2007

#include "TrkTruthTPCnv/PRD_MultiTruthCollectionCnv_p1.h"

#include "TrkTruthTPCnv/PRD_MultiTruthCollection_p1.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"

#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p1.h"


namespace {
  HepMcParticleLinkCnv_p1 particleLinkConverter;
}



void PRD_MultiTruthCollectionCnv_p1::persToTrans( const Trk::PRD_MultiTruthCollection_p1* pers,
						  PRD_MultiTruthCollection* trans, 
						  MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"PRD_MultiTruthCollectionCnv_p1::persToTrans()"<<endreq;
  
  for(Trk::PRD_MultiTruthCollection_p1::CollectionType::const_iterator i=pers->m_entries.begin(); i!=pers->m_entries.end(); i++) {
    HepMcParticleLink link;
    particleLinkConverter.persToTrans(&i->particle, &link, msg);
    //trans->insert(trans->end(), std::make_pair(Identifier(i->id), link) );

    if(!m_isInitialized) {
      if (this->initialize(msg) != StatusCode::SUCCESS) {
	msg << MSG::FATAL << "Could not initialize PRD_MultiTruthCollectionCnv_p1 " << endreq;
      }
    }

    Identifier chanId;
    if (m_pixId->is_shortened_pixel_id(i->id)) {
     chanId = m_pixId->pixel_id_from_shortened(i->id);
    }
    else {
      chanId = i->id;
    }
    trans->insert(trans->end(), std::make_pair(chanId, link) );
  }

  msg<<MSG::DEBUG<<"PRD_MultiTruthCollectionCnv_p1::persToTrans() DONE"<<endreq;
}

void PRD_MultiTruthCollectionCnv_p1::transToPers( const PRD_MultiTruthCollection* trans, 
						  Trk::PRD_MultiTruthCollection_p1* pers, 
						  MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"PRD_MultiTruthCollectionCnv_p1::transToPers()"<<endreq;

  pers->m_entries.reserve(trans->size());

  for(PRD_MultiTruthCollection::const_iterator i=trans->begin(); i!=trans->end(); i++) {
    HepMcParticleLink_p1 link;
    particleLinkConverter.transToPers(&i->second, &link, msg);
    pers->m_entries.push_back(Trk::PRD_MultiTruthCollection_p1::Entry(i->first.get_compact(), link));
  }

  msg<<MSG::DEBUG<<"PRD_MultiTruthCollectionCnv_p1::transToPers() DONE"<<endreq;
}

StatusCode PRD_MultiTruthCollectionCnv_p1::initialize(MsgStream &log) {
   // Do not initialize again:
   m_isInitialized=true;

   // Get Storegate, ID helpers, and so on
   ISvcLocator* svcLocator = Gaudi::svcLocator();
   // get StoreGate service
   StatusCode sc = svcLocator->service("StoreGateSvc", m_storeGate);
   if (sc.isFailure()) {
      log << MSG::FATAL << "StoreGate service not found !" << endreq;
      return StatusCode::FAILURE;
   }

   // get DetectorStore service
   StoreGateSvc *detStore;
   sc = svcLocator->service("DetectorStore", detStore);
   if (sc.isFailure()) {
      log << MSG::FATAL << "DetectorStore service not found !" << endreq;
      return StatusCode::FAILURE;
   } 
   //   else {
   //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found DetectorStore." << endreq;
   //   }

   // Get the sct helper from the detector store
   sc = detStore->retrieve(m_pixId, "PixelID");
   if (sc.isFailure()) {
      log << MSG::FATAL << "Could not get PixelID helper !" << endreq;
      return StatusCode::FAILURE;
   } 


   //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converter initialized." << endreq;
   return StatusCode::SUCCESS;
}
