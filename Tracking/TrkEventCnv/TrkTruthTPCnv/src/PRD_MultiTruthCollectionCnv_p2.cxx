/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// T/P converter for PRD_MultiTruthCollection.
// Andrei Gaponenko <agaponenko@lbl.gov>, 2007

#include "TrkTruthTPCnv/PRD_MultiTruthCollectionCnv_p2.h"

#include "TrkTruthTPCnv/PRD_MultiTruthCollection_p2.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"

#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p1.h"

namespace {
  const HepMcParticleLinkCnv_p1 particleLinkConverter;
}


void PRD_MultiTruthCollectionCnv_p2::persToTrans( const Trk::PRD_MultiTruthCollection_p2* pers,
						  PRD_MultiTruthCollection* trans, 
						  MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"PRD_MultiTruthCollectionCnv_p2::persToTrans()"<<endmsg;

  for (const Trk::PRD_MultiTruthCollection_p2::Entry& ent : pers->m_entries) {
    HepMcParticleLink link;
    particleLinkConverter.persToTrans(&ent.particle, &link, msg);
    trans->insert(trans->end(), std::make_pair(Identifier(ent.id), link) );
  }

  msg<<MSG::DEBUG<<"PRD_MultiTruthCollectionCnv_p2::persToTrans() DONE"<<endmsg;
}

void PRD_MultiTruthCollectionCnv_p2::transToPers( const PRD_MultiTruthCollection* trans, 
						  Trk::PRD_MultiTruthCollection_p2* pers, 
						  MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"PRD_MultiTruthCollectionCnv_p2::transToPers()"<<endmsg;

  pers->m_entries.reserve(trans->size());

  for (const auto& p : *trans) {
    HepMcParticleLink_p1 link;
    particleLinkConverter.transToPers(&p.second, &link, msg);
    pers->m_entries.push_back(Trk::PRD_MultiTruthCollection_p2::Entry(p.first.get_compact(), link));
  }

  msg<<MSG::DEBUG<<"PRD_MultiTruthCollectionCnv_p2::transToPers() DONE"<<endmsg;
}
