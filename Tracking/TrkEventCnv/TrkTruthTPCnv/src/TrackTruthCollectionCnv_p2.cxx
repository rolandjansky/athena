/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// T/P converter for TrackTruthCollection.
// Andrei Gaponenko <agaponenko@lbl.gov>, 2007
// Olivier Arnaez <olivier.arnaez@cern.ch>, 2015

#include "TrkTruthTPCnv/TrackTruthCollectionCnv_p2.h"

#include "TrkTruthTPCnv/TrackTruthCollection_p2.h"
#include "TrkTruthData/TrackTruthCollection.h"

#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p2.h"
#include "DataModelAthenaPool/DataLinkCnv_p1.h"

#include "StoreGate/StoreGateSvc.h"

#include "TrackTruthCollectionAccessor.h"

namespace {
  HepMcParticleLinkCnv_p2 particleLinkConverter;
  DataLinkCnv_p1<DataLink<TrackCollection> > dataLinkConverter;
}

void TrackTruthCollectionCnv_p2::persToTrans( const Trk::TrackTruthCollection_p2* pers,
                                              TrackTruthCollection* trans,
                                              MsgStream& msg )
{
  msg<<MSG::DEBUG<<"TrackTruthCollectionCnv_p2::persToTrans(): PERS size = "<<pers->m_entries.size()<<endmsg;

  dataLinkConverter.persToTrans(pers->m_trackCollectionLink, TrackTruthCollectionAccessor::trackCollectionLink(trans), msg);

  for(Trk::TrackTruthCollection_p2::CollectionType::const_iterator i=pers->m_entries.begin(); i!=pers->m_entries.end(); i++) {
    HepMcParticleLink link;
    particleLinkConverter.persToTrans(&i->particle, &link, msg);
    trans->insert(trans->end(), std::make_pair(i->index, TrackTruth(link, i->probability, 0)) );
  }

  msg<<MSG::DEBUG<<"TrackTruthCollectionCnv_p2::persToTrans() DONE"<<endmsg;
}

void TrackTruthCollectionCnv_p2::transToPers( const TrackTruthCollection*,
                                              Trk::TrackTruthCollection_p2*,
                                              MsgStream& /*msg*/ )
{
  throw std::runtime_error("TrackTruthCollectionCnv_p2::transToPers is not supported in this release!");
}
