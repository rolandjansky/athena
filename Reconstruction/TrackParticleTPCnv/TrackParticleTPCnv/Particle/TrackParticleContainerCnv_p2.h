/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLECONTAINER_CNV_P2_REC_H
#define TRACKPARTICLECONTAINER_CNV_P2_REC_H


#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TrkParticleBase/TrackParticleBase.h"

#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"
#include "TrackParticleTPCnv/Particle/TrackParticleContainer_p2.h"


class TrackParticleContainerCnv_p2 : public T_AthenaPoolTPCnvBase< Rec::TrackParticleContainer, Rec::TrackParticleContainer_p2 >
{
public:
  TrackParticleContainerCnv_p2() {}
 
 // these implementations have to stay
 virtual void transToPers(const Rec::TrackParticleContainer* transVect, Rec::TrackParticleContainer_p2* persVect, MsgStream &log) {
     persVect->clear();
     persVect->reserve( transVect->size() );
     // convert vector entries one by one
     for( Rec::TrackParticleContainer::const_iterator it = transVect->begin();it != transVect->end();  ++it ) {
         persVect->push_back( toPersistent( (ITPConverterFor<Rec::TrackParticle>**)0, *it, log ) );
     } 
  }
  
  
 virtual void persToTrans(const Rec::TrackParticleContainer_p2* persVect, Rec::TrackParticleContainer* transVect, MsgStream &log)
 {
     transVect->clear();
     transVect->reserve( persVect->size() );
     // convert vector entries one by one
     for( Rec::TrackParticleContainer_p2::const_iterator it  = persVect->begin();it != persVect->end();  ++it )
     {
       transVect->push_back( dynamic_cast<Rec::TrackParticle*>( createTransFromPStore( (ITPConverterFor<Rec::TrackParticle>**)0, *it, log ) ) );
     }
 }

};

#endif 

