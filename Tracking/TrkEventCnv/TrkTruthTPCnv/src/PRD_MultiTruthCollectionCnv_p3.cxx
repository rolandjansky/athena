/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// T/P converter for PRD_MultiTruthCollection.
// Andrei Gaponenko <agaponenko@lbl.gov>, 2007
// Olivier Arnaez <olivier.arnaez@cern.ch>, 2015

#include "TrkTruthTPCnv/PRD_MultiTruthCollectionCnv_p3.h"

#include "TrkTruthTPCnv/PRD_MultiTruthCollection_p3.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"

#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p2.h"

namespace {
  const HepMcParticleLinkCnv_p2 particleLinkConverter;
}


void PRD_MultiTruthCollectionCnv_p3::persToTrans( const Trk::PRD_MultiTruthCollection_p3* pers,
                                                  PRD_MultiTruthCollection* trans,
                                                  MsgStream& msg )
{
  msg<<MSG::DEBUG<<"PRD_MultiTruthCollectionCnv_p3::persToTrans()"<<endmsg;

  for (const Trk::PRD_MultiTruthCollection_p3::Entry& ent : pers->m_entries) {
    HepMcParticleLink link;
    particleLinkConverter.persToTrans(&ent.particle, &link, msg);
    trans->insert(trans->end(), std::make_pair(Identifier(ent.id), link) );
  }

  msg<<MSG::DEBUG<<"PRD_MultiTruthCollectionCnv_p3::persToTrans() DONE"<<endmsg;
}

void PRD_MultiTruthCollectionCnv_p3::transToPers( const PRD_MultiTruthCollection* trans,
                                                  Trk::PRD_MultiTruthCollection_p3* pers,
                                                  MsgStream& msg )
{
  msg<<MSG::DEBUG<<"PRD_MultiTruthCollectionCnv_p3::transToPers()"<<endmsg;

  pers->m_entries.reserve(trans->size());

  for (const auto& p : *trans) {
    HepMcParticleLink_p2 link;
    particleLinkConverter.transToPers(&p.second, &link, msg);
    pers->m_entries.push_back(Trk::PRD_MultiTruthCollection_p3::Entry(p.first.get_compact(), link));
  }

  msg<<MSG::DEBUG<<"PRD_MultiTruthCollectionCnv_p3::transToPers() DONE"<<endmsg;
}
