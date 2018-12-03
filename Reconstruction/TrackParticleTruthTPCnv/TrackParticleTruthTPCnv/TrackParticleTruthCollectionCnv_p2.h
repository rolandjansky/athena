/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLETRUTHCOLLECTIONCNV_P2_H 
#define TRACKPARTICLETRUTHCOLLECTIONCNV_P2_H 

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class StoreGateSvc;
class MsgStream;
class TrackParticleTruthCollection;

namespace Rec { class TrackParticleTruthCollection_p2; }


class TrackParticleTruthCollectionCnv_p2 
  : public T_AthenaPoolTPCnvBase<TrackParticleTruthCollection,
                                 Rec::TrackParticleTruthCollection_p2>
{ 
public: 
  /** Method creating the transient representation of @c Analysis::TrackParticleTruthCollection
   *  from its persistent representation @c TrackParticleTruthCollection_p2
   */
  virtual void persToTrans( const Rec::TrackParticleTruthCollection_p2* persObj, 
                            TrackParticleTruthCollection* transObj, 
                            MsgStream& msg );
  
  /** Method creating the persistent representation @c TrackParticleTruthCollection_p2
   *  from its transient representation @c Analysis::TrackParticleTruthCollection
   */
  virtual void transToPers( const TrackParticleTruthCollection* transObj, 
                            Rec::TrackParticleTruthCollection_p2* persObj, 
                            MsgStream& msg );

}; 

template<>
class T_TPCnv<TrackParticleTruthCollection, Rec::TrackParticleTruthCollection_p2>
  : public TrackParticleTruthCollectionCnv_p2
{
public:
};

#endif/*TRACKPARTICLETRUTHCOLLECTIONCNV_P2_H*/

