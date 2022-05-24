/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// T/P converter for PRD_MultiTruthCollection.
// Andrei Gaponenko <agaponenko@lbl.gov>, 2007

#include "TrkTruthTPCnv/PRD_MultiTruthCollectionCnv_p1.h"

#include "TrkTruthTPCnv/PRD_MultiTruthCollection_p1.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"

#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p1.h"


namespace {
  const HepMcParticleLinkCnv_p1 particleLinkConverter;
}



void PRD_MultiTruthCollectionCnv_p1::persToTrans( const Trk::PRD_MultiTruthCollection_p1* pers,
						  PRD_MultiTruthCollection* trans, 
						  MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"PRD_MultiTruthCollectionCnv_p1::persToTrans()"<<endmsg;
  
  for (const Trk::PRD_MultiTruthCollection_p1::Entry& ent : pers->m_entries) {
    HepMcParticleLink link;
    particleLinkConverter.persToTrans(&ent.particle, &link, msg);

    if(!m_isInitialized) {
      if (this->initialize(msg) != StatusCode::SUCCESS) {
	msg << MSG::FATAL << "Could not initialize PRD_MultiTruthCollectionCnv_p1 " << endmsg;
      }
    }

    Identifier chanId;
    if (m_pixId->is_shortened_pixel_id(ent.id)) {
     chanId = m_pixId->pixel_id_from_shortened(ent.id);
    }
    else {
      chanId = ent.id;
    }
    trans->insert(trans->end(), std::make_pair(chanId, link) );
  }

  msg<<MSG::DEBUG<<"PRD_MultiTruthCollectionCnv_p1::persToTrans() DONE"<<endmsg;
}

void PRD_MultiTruthCollectionCnv_p1::transToPers( const PRD_MultiTruthCollection* trans, 
						  Trk::PRD_MultiTruthCollection_p1* pers, 
						  MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"PRD_MultiTruthCollectionCnv_p1::transToPers()"<<endmsg;

  pers->m_entries.reserve(trans->size());

  for (const auto& p : *trans) {
    HepMcParticleLink_p1 link;
    particleLinkConverter.transToPers(&p.second, &link, msg);
    pers->m_entries.push_back(Trk::PRD_MultiTruthCollection_p1::Entry(p.first.get_compact(), link));
  }

  msg<<MSG::DEBUG<<"PRD_MultiTruthCollectionCnv_p1::transToPers() DONE"<<endmsg;
}

StatusCode PRD_MultiTruthCollectionCnv_p1::initialize(MsgStream &log) {
   // Do not initialize again:
   m_isInitialized=true;

   // Get Storegate, ID helpers, and so on
   ISvcLocator* svcLocator = Gaudi::svcLocator();
   // get StoreGate service
   StatusCode sc = svcLocator->service("StoreGateSvc", m_storeGate);
   if (sc.isFailure()) {
      log << MSG::FATAL << "StoreGate service not found !" << endmsg;
      return StatusCode::FAILURE;
   }

   // get DetectorStore service
   StoreGateSvc *detStore;
   sc = svcLocator->service("DetectorStore", detStore);
   if (sc.isFailure()) {
      log << MSG::FATAL << "DetectorStore service not found !" << endmsg;
      return StatusCode::FAILURE;
   } 
   //   else {
   //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found DetectorStore." << endmsg;
   //   }

   // Get the sct helper from the detector store
   sc = detStore->retrieve(m_pixId, "PixelID");
   if (sc.isFailure()) {
      log << MSG::FATAL << "Could not get PixelID helper !" << endmsg;
      return StatusCode::FAILURE;
   } 


   //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converter initialized." << endmsg;
   return StatusCode::SUCCESS;
}
