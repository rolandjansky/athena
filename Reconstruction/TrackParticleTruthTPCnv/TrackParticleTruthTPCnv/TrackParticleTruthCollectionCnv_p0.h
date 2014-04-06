/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLETRUTHCOLLECTIONCNV_P0_H 
#define TRACKPARTICLETRUTHCOLLECTIONCNV_P0_H 

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class StoreGateSvc;
class MsgStream;
class TrackParticleTruthCollection;
class TrackParticleTruthVector;

namespace Rec {
  typedef TrackParticleTruthVector  TrackParticleTruthCollection_p0;
}

class TrackParticleTruthCollectionCnv_p0 
  : public T_AthenaPoolTPCnvBase<TrackParticleTruthCollection,
                                 Rec::TrackParticleTruthCollection_p0>
{ 
public: 
  /** Method creating the transient representation of @c Analysis::TrackParticleTruthCollection
   *  from its persistent representation @c TrackParticleTruthCollection_p0
   */
  virtual void persToTrans( const Rec::TrackParticleTruthCollection_p0* persObj, 
                            TrackParticleTruthCollection* transObj, 
                            MsgStream& msg );
  
  /** Method creating the persistent representation @c TrackParticleTruthCollection_p0
   *  from its transient representation @c Analysis::TrackParticleTruthCollection
   */
  virtual void transToPers( const TrackParticleTruthCollection* transObj,
                            Rec::TrackParticleTruthCollection_p0* persObj, 
                            MsgStream& msg );

}; 

#endif/*TRACKPARTICLETRUTHCOLLECTIONCNV_P0_H*/
