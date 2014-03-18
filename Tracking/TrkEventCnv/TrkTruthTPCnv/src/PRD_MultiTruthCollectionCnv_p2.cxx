/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// T/P converter for PRD_MultiTruthCollection.
// Andrei Gaponenko <agaponenko@lbl.gov>, 2007

#include "TrkTruthTPCnv/PRD_MultiTruthCollectionCnv_p2.h"

#include "TrkTruthTPCnv/PRD_MultiTruthCollection_p2.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"

#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p1.h"

namespace {
  HepMcParticleLinkCnv_p1 particleLinkConverter;
}


void PRD_MultiTruthCollectionCnv_p2::persToTrans( const Trk::PRD_MultiTruthCollection_p2* pers,
						  PRD_MultiTruthCollection* trans, 
						  MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"PRD_MultiTruthCollectionCnv_p2::persToTrans()"<<endreq;
  
  for(Trk::PRD_MultiTruthCollection_p2::CollectionType::const_iterator i=pers->m_entries.begin(); i!=pers->m_entries.end(); i++) {
    HepMcParticleLink link;
    particleLinkConverter.persToTrans(&i->particle, &link, msg);
    trans->insert(trans->end(), std::make_pair(Identifier(i->id), link) );
  }

  msg<<MSG::DEBUG<<"PRD_MultiTruthCollectionCnv_p2::persToTrans() DONE"<<endreq;
}

void PRD_MultiTruthCollectionCnv_p2::transToPers( const PRD_MultiTruthCollection* trans, 
						  Trk::PRD_MultiTruthCollection_p2* pers, 
						  MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"PRD_MultiTruthCollectionCnv_p2::transToPers()"<<endreq;

  pers->m_entries.reserve(trans->size());

  for(PRD_MultiTruthCollection::const_iterator i=trans->begin(); i!=trans->end(); i++) {
    HepMcParticleLink_p1 link;
    particleLinkConverter.transToPers(&i->second, &link, msg);
    pers->m_entries.push_back(Trk::PRD_MultiTruthCollection_p2::Entry(i->first.get_compact(), link));
  }

  msg<<MSG::DEBUG<<"PRD_MultiTruthCollectionCnv_p2::transToPers() DONE"<<endreq;
}
