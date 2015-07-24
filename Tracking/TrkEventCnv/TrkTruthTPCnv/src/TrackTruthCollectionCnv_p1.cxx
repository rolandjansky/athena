/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// T/P converter for TrackTruthCollection.
// Andrei Gaponenko <agaponenko@lbl.gov>, 2007

#include "TrkTruthTPCnv/TrackTruthCollectionCnv_p1.h"

#include "TrkTruthTPCnv/TrackTruthCollection_p1.h"
#include "TrkTruthData/TrackTruthCollection.h"

#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p1.h"
#include "DataModelAthenaPool/DataLinkCnv_p1.h"

#include "StoreGate/StoreGateSvc.h"

#include "TrackTruthCollectionAccessor.h"

namespace {
  HepMcParticleLinkCnv_p1 particleLinkConverter;
  DataLinkCnv_p1<DataLink<TrackCollection> > dataLinkConverter;
}

void TrackTruthCollectionCnv_p1::persToTrans( const Trk::TrackTruthCollection_p1* pers,
					      TrackTruthCollection* trans, 
					      MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TrackTruthCollectionCnv_p1::persToTrans(): PERS size = "<<pers->m_entries.size()<<endreq;

  dataLinkConverter.persToTrans(pers->m_trackCollectionLink, TrackTruthCollectionAccessor::trackCollectionLink(trans), msg);

  for(Trk::TrackTruthCollection_p1::CollectionType::const_iterator i=pers->m_entries.begin(); i!=pers->m_entries.end(); i++) {
    HepMcParticleLink link;
    particleLinkConverter.persToTrans(&i->particle, &link, msg);
    trans->insert(trans->end(), std::make_pair(i->index, TrackTruth(link, i->probability, 0)) );
  }

  msg<<MSG::DEBUG<<"TrackTruthCollectionCnv_p1::persToTrans() DONE"<<endreq;
}

void TrackTruthCollectionCnv_p1::transToPers( const TrackTruthCollection* trans, 
					      Trk::TrackTruthCollection_p1* pers, 
					      MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TrackTruthCollectionCnv_p1::transToPers()"<<endreq;
  msg<<MSG::DEBUG<<"TrackTruthCollectionCnv_p1::transToPers(): input size = "<<trans->size()<<endreq;
  
  dataLinkConverter.transToPers(trans->trackCollectionLink(), pers->m_trackCollectionLink, msg);

  pers->m_entries.resize(trans->size());
  Trk::TrackTruthCollection_p1::CollectionType::size_type ipers(0);
  for(TrackTruthCollection::const_iterator itrans=trans->begin(); itrans!=trans->end(); itrans++, ipers++) {
    Trk::TrackTruthCollection_p1::Entry& current = pers->m_entries[ipers];
    const TrackTruth& truth = itrans->second;
    current.index = itrans->first.index(); // extract index from ElementLink
    current.probability = truth.probability();
    particleLinkConverter.transToPers(&itrans->second.particleLink(), &current.particle, msg);
  }

  msg<<MSG::DEBUG<<"TrackTruthCollectionCnv_p1::transToPers() DONE"<<endreq;
}
