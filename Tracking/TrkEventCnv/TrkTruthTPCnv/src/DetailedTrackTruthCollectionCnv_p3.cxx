/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// T/P converter for DetailedTrackTruthCollection.
//
// Andrei Gaponenko <agaponenko@lbl.gov>, 2008
// Olivier.Arnaez@cern.ch, 2015

#include "TrkTruthData/DetailedTrackTruthCollection.h"

#include "TrkTruthTPCnv/DetailedTrackTruthCollectionCnv_p3.h"
#include "TrkTruthTPCnv/DetailedTrackTruthCollection_p3.h"

#include "DataModelAthenaPool/DataLinkCnv_p1.h"
#include "TrkTruthTPCnv/DetailedTrackTruthCnv_p3.h"

#include "StoreGate/StoreGateSvc.h"

#include "TrackTruthCollectionAccessor.h"

namespace {
  DataLinkCnv_p1<DataLink<TrackCollection> > dataLinkConverter;
  DetailedTrackTruthCnv_p3 detailedTruthConverter;
}

void DetailedTrackTruthCollectionCnv_p3::persToTrans( const Trk::DetailedTrackTruthCollection_p3* pers,
                                                      DetailedTrackTruthCollection* trans,
                                                      MsgStream& msg )
{
  msg<<MSG::DEBUG<<"DetailedTrackTruthCollectionCnv_p3::persToTrans(): PERS size = "<<pers->m_entries.size()<<endmsg;

  trans->clear();

  dataLinkConverter.persToTrans(pers->m_trackCollectionLink, TrackTruthCollectionAccessor::trackCollectionLink(trans), msg);

  for(Trk::DetailedTrackTruthCollection_p3::CollectionType::const_iterator i=pers->m_entries.begin(); i!=pers->m_entries.end(); i++) {
    DetailedTrackTruth dt;
    detailedTruthConverter.persToTrans(&i->detailedTruth, &dt, msg);
    trans->insert(trans->end(), std::make_pair(i->key.m_index, dt) );
  }

  msg<<MSG::DEBUG<<"DetailedTrackTruthCollectionCnv_p3::persToTrans() DONE"<<endmsg;
}

void DetailedTrackTruthCollectionCnv_p3::transToPers( const DetailedTrackTruthCollection* trans,
                                                      Trk::DetailedTrackTruthCollection_p3* pers,
                                                      MsgStream& msg )
{
  msg<<MSG::DEBUG<<"DetailedTrackTruthCollectionCnv_p3::transToPers()"<<endmsg;
  msg<<MSG::DEBUG<<"DetailedTrackTruthCollectionCnv_p3::transToPers(): input size = "<<trans->size()<<endmsg;

  dataLinkConverter.transToPers(trans->trackCollectionLink(), pers->m_trackCollectionLink, msg);

  pers->m_entries.resize(trans->size());
  Trk::DetailedTrackTruthCollection_p3::CollectionType::size_type ipers(0);

  for(DetailedTrackTruthCollection::const_iterator itrans=trans->begin(); itrans!=trans->end(); itrans++, ipers++) {
    Trk::DetailedTrackTruthCollection_p3::Entry& current = pers->m_entries[ipers];
    current.key.m_index = itrans->first.index(); // extract index from ElementLink
    detailedTruthConverter.transToPers(&itrans->second, &current.detailedTruth, msg);
  }

  msg<<MSG::DEBUG<<"DetailedTrackTruthCollectionCnv_p3::transToPers() DONE"<<endmsg;
}
