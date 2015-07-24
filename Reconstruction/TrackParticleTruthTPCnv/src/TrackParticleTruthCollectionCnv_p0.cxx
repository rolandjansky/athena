/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionCnv_p0.h"

#include "TrackParticleTruthTPCnv/TrackParticleTruthCollection_p0.h"
#include "ParticleTruth/TrackParticleTruthCollection.h"

#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p1.h"

#include "DataModel/DataLink.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionAccessor.h"

namespace {
  HepMcParticleLinkCnv_p1 particleLinkConverter;
}

void TrackParticleTruthCollectionCnv_p0::persToTrans( const Rec::TrackParticleTruthCollection_p0* pers,
                                              TrackParticleTruthCollection* trans, 
                                              MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TrackParticleTruthCollectionCnv_p0::persToTrans(): PERS size = "<<pers->size()<<endreq;

  if(pers->empty()) {
    msg<<MSG::WARNING<<"persToTrans(): can't find out the corresponding TrackCollection for the empty TrackParticleTruthCollection_p0 (old-format)"<<endreq;
  }
  else {
    //static (const) DataLink<Rec::TrackParticleContainer>                 = 
    TrackParticleTruthCollectionAccessor::trackParticleContainerLink(trans) = dynamic_cast<const Rec::TrackParticleContainer*>((*pers)[0].first.getStorableObjectPointer());

    for(Rec::TrackParticleTruthCollection_p0::const_iterator i=pers->begin(); i!=pers->end(); i++) {
      trans->insert(trans->end(), std::make_pair(i->first, i->second));
    }
  }

  msg<<MSG::DEBUG<<"TrackParticleTruthCollectionCnv_p0::persToTrans() DONE"<<endreq;
}

void TrackParticleTruthCollectionCnv_p0::transToPers( const TrackParticleTruthCollection*, 
                                              Rec::TrackParticleTruthCollection_p0* , 
                                              MsgStream& msg ) 
{
  msg<<MSG::DEBUG<<"TrackParticleTruthCollectionCnv_p0::transToPers()"<<endreq;
  throw std::runtime_error("TrackParticleTruthCollectionCnv_p0::transToPers() is not implemented, _p1 or later is meant to be used.");
  //never reached msg<<MSG::DEBUG<<"TrackParticleTruthCollectionCnv_p0::transToPers() DONE"<<endreq;
}
