/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLETRUTHCOLLECTIONCNV_P3_H 
#define TRACKPARTICLETRUTHCOLLECTIONCNV_P3_H 

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class StoreGateSvc;
class MsgStream;
class TrackParticleTruthCollection;

namespace Rec { class TrackParticleTruthCollection_p3; }


class TrackParticleTruthCollectionCnv_p3 
  : public T_AthenaPoolTPCnvBase<TrackParticleTruthCollection,
                                 Rec::TrackParticleTruthCollection_p3>
{ 
public: 
  /** Method creating the transient representation of @c Analysis::TrackParticleTruthCollection
   *  from its persistent representation @c TrackParticleTruthCollection_p3
   */
  virtual void persToTrans( const Rec::TrackParticleTruthCollection_p3* persObj, 
                            TrackParticleTruthCollection* transObj, 
                            MsgStream& msg );
  
  /** Method creating the persistent representation @c TrackParticleTruthCollection_p3
   *  from its transient representation @c Analysis::TrackParticleTruthCollection
   */
  virtual void transToPers( const TrackParticleTruthCollection* transObj, 
                            Rec::TrackParticleTruthCollection_p3* persObj, 
                            MsgStream& msg );

}; 

template<>
class T_TPCnv<TrackParticleTruthCollection, Rec::TrackParticleTruthCollection_p3>
  : public TrackParticleTruthCollectionCnv_p3
{
public:
};

#endif/*TRACKPARTICLETRUTHCOLLECTIONCNV_P3_H*/

