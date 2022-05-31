/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLETRUTHCOLLECTIONCNV_P1_H 
#define TRACKPARTICLETRUTHCOLLECTIONCNV_P1_H 

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class StoreGateSvc;
class MsgStream;
class TrackParticleTruthCollection;

namespace Rec { class TrackParticleTruthCollection_p1; }


class TrackParticleTruthCollectionCnv_p1 
  : public T_AthenaPoolTPCnvConstBase<TrackParticleTruthCollection,
                                      Rec::TrackParticleTruthCollection_p1>
{ 
public:
  using base_class::persToTrans;
  using base_class::transToPers;

  /** Method creating the transient representation of @c Analysis::TrackParticleTruthCollection
   *  from its persistent representation @c TrackParticleTruthCollection_p1
   */
  virtual void persToTrans( const Rec::TrackParticleTruthCollection_p1* persObj, 
                            TrackParticleTruthCollection* transObj, 
                            MsgStream& msg ) const override;
  
  /** Method creating the persistent representation @c TrackParticleTruthCollection_p1
   *  from its transient representation @c Analysis::TrackParticleTruthCollection
   */
  virtual void transToPers( const TrackParticleTruthCollection* transObj, 
                            Rec::TrackParticleTruthCollection_p1* persObj, 
                            MsgStream& msg ) const override;

}; 

template<>
class T_TPCnv<TrackParticleTruthCollection, Rec::TrackParticleTruthCollection_p1>
  : public TrackParticleTruthCollectionCnv_p1
{
public:
};

#endif/*TRACKPARTICLETRUTHCOLLECTIONCNV_P1_H*/

