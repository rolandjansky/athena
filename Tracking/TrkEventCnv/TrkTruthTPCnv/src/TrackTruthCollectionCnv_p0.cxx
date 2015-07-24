/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// T/P converter for TrackTruthCollection.
// Andrei Gaponenko <agaponenko@lbl.gov>, 2007


#include "TrkTruthTPCnv/TrackTruthCollectionCnv_p0.h"

#include "TrkTruthTPCnv/TrackTruthCollection_p0.h"
#include "TrkTruthData/TrackTruthCollection.h"

#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p1.h"

#include "DataModel/DataLink.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrackTruthCollectionAccessor.h"

namespace {
  HepMcParticleLinkCnv_p1 particleLinkConverter;
}

void TrackTruthCollectionCnv_p0::persToTrans( const Trk::TrackTruthCollection_p0* pers,
					      TrackTruthCollection* trans, 
					      MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TrackTruthCollectionCnv_p0::persToTrans(): PERS size = "<<pers->size()<<endreq;

  if(pers->empty()) {
    msg<<MSG::WARNING<<"persToTrans(): can't find out the corresponding TrackCollection for the empty TrackTruthCollection_p0 (old-format)"<<endreq;
  }
  else {
    TrackTruthCollectionAccessor::trackCollectionLink(trans) = (*pers)[0].first.getStorableObjectPointer();

    for(Trk::TrackTruthCollection_p0::const_iterator i=pers->begin(); i!=pers->end(); i++) {
      trans->insert(trans->end(), std::make_pair(i->first, i->second));
    }
  }

  msg<<MSG::DEBUG<<"TrackTruthCollectionCnv_p0::persToTrans() DONE"<<endreq;
}

void TrackTruthCollectionCnv_p0::transToPers( const TrackTruthCollection* /*trans*/, 
					      Trk::TrackTruthCollection_p0* /*pers*/, 
					      MsgStream& /* msg */  ) 
{
  throw std::runtime_error("TrackTruthCollectionCnv_p0::transToPers() is not implemented, _p1 or later is meant to be used.");
}
