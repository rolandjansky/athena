/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// T/P converter for DetailedTrackTruthCollection.
// 
// Andrei Gaponenko <agaponenko@lbl.gov>, 2007

#include "TrkTruthData/DetailedTrackTruthCollection.h"

#include "TrkTruthTPCnv/DetailedTrackTruthCollectionCnv_p1.h"
#include "TrkTruthTPCnv/DetailedTrackTruthCollection_p1.h"

#include "DataModelAthenaPool/DataLinkCnv_p1.h"
#include "TrkTruthTPCnv/DetailedTrackTruthCnv_p1.h"

#include "StoreGate/StoreGateSvc.h"

#include "TrackTruthCollectionAccessor.h"

namespace {
  DataLinkCnv_p1<DataLink<TrackCollection> > dataLinkConverter;
  DetailedTrackTruthCnv_p1 detailedTruthConverter;
}

void DetailedTrackTruthCollectionCnv_p1::persToTrans( const Trk::DetailedTrackTruthCollection_p1* pers,
						      DetailedTrackTruthCollection* trans, 
						      MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"DetailedTrackTruthCollectionCnv_p1::persToTrans(): PERS size = "<<pers->m_entries.size()<<endreq;
  
  trans->clear();

  dataLinkConverter.persToTrans(pers->m_trackCollectionLink, TrackTruthCollectionAccessor::trackCollectionLink(trans), msg);

  for(Trk::DetailedTrackTruthCollection_p1::CollectionType::const_iterator i=pers->m_entries.begin(); i!=pers->m_entries.end(); i++) {
    DetailedTrackTruth dt;
    detailedTruthConverter.persToTrans(&i->detailedTruth, &dt, msg);
    trans->insert(trans->end(), std::make_pair(i->key.m_index, dt) );
  }

  msg<<MSG::DEBUG<<"DetailedTrackTruthCollectionCnv_p1::persToTrans() DONE"<<endreq;
}

void DetailedTrackTruthCollectionCnv_p1::transToPers( const DetailedTrackTruthCollection*,
						      Trk::DetailedTrackTruthCollection_p1*,
						      MsgStream& msg ) 
{
  const char* info = "DetailedTrackTruthCollectionCnv_p1::transToPers()";
  msg<<MSG::FATAL<<info<<endreq;
  throw std::runtime_error(info);
}
