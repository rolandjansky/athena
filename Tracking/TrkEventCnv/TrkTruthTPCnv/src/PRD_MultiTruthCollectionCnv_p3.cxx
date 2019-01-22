/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// T/P converter for PRD_MultiTruthCollection.
// Andrei Gaponenko <agaponenko@lbl.gov>, 2007
// Olivier Arnaez <olivier.arnaez@cern.ch>, 2015

#include "TrkTruthTPCnv/PRD_MultiTruthCollectionCnv_p3.h"

#include "TrkTruthTPCnv/PRD_MultiTruthCollection_p3.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"

#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p2.h"

namespace {
  HepMcParticleLinkCnv_p2 particleLinkConverter;
}


void PRD_MultiTruthCollectionCnv_p3::persToTrans( const Trk::PRD_MultiTruthCollection_p3* pers,
                                                  PRD_MultiTruthCollection* trans,
                                                  MsgStream& msg )
{
  msg<<MSG::DEBUG<<"PRD_MultiTruthCollectionCnv_p3::persToTrans()"<<endmsg;

  for(Trk::PRD_MultiTruthCollection_p3::CollectionType::const_iterator i=pers->m_entries.begin(); i!=pers->m_entries.end(); i++) {
    HepMcParticleLink link;
    particleLinkConverter.persToTrans(&i->particle, &link, msg);
    trans->insert(trans->end(), std::make_pair(Identifier(i->id), link) );
  }

  msg<<MSG::DEBUG<<"PRD_MultiTruthCollectionCnv_p3::persToTrans() DONE"<<endmsg;
}

void PRD_MultiTruthCollectionCnv_p3::transToPers( const PRD_MultiTruthCollection* trans,
                                                  Trk::PRD_MultiTruthCollection_p3* pers,
                                                  MsgStream& msg )
{
  msg<<MSG::DEBUG<<"PRD_MultiTruthCollectionCnv_p3::transToPers()"<<endmsg;

  pers->m_entries.reserve(trans->size());

  for(PRD_MultiTruthCollection::const_iterator i=trans->begin(); i!=trans->end(); i++) {
    HepMcParticleLink_p2 link;
    particleLinkConverter.transToPers(&i->second, &link, msg);
    pers->m_entries.push_back(Trk::PRD_MultiTruthCollection_p3::Entry(i->first.get_compact(), link));
  }

  msg<<MSG::DEBUG<<"PRD_MultiTruthCollectionCnv_p3::transToPers() DONE"<<endmsg;
}
