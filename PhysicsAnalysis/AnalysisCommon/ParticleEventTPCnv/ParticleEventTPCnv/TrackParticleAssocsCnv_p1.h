///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// TrackParticleAssocsCnv_p1.h 
// Header file for class TrackParticleAssocsCnv_p1
// Author: C.Anastopoulos
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTTPCNV_TRACKPARTICLEASSOCSCNV_p1_H 
#define PARTICLEEVENTTPCNV_TRACKPARTICLEASSOCSCNV_p1_H

// STL includes

// Gaudi includes

#include "AthLinks/ElementLink.h"

// DataModelAthenaPool includes
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "DataModelAthenaPool/DataLinkCnv_p2.h"

// TrackParticleIncludes
#include "Particle/TrackParticleContainer.h"
#include "ParticleEvent/TrackParticleAssocs.h"

// EventCommonTPCnv includes
#include "ParticleEventTPCnv/TrackParticleAssocs_p1.h"

// Forward declaration
class MsgStream;

class TrackParticleAssocsCnv_p1 : public T_AthenaPoolTPCnvConstBase<
                                              TrackParticleAssocs, 
                                              TrackParticleAssocs_p1
                                            >  
{ 
 public: 
  using base_class::transToPers;
  using base_class::persToTrans;


  /// converter for @c ElementLink to @c TrackParticlecollection
  typedef ElementLinkCnv_p3< ElementLink< Rec::TrackParticleContainer> >
          TrackParticleCnv_t;
  typedef DataLinkCnv_p2<DataLink<TrackParticleAssocs> > IAssocStoresCnv_t;


  /** Default constructor: 
   */
  TrackParticleAssocsCnv_p1();

  /** Destructor: 
   */
  virtual ~TrackParticleAssocsCnv_p1();


  /** Method creating the transient representation of @c INav4MomAssocs
   *  from its persistent representation @c INav4MomAssocs_p3
   */
  virtual void persToTrans( const TrackParticleAssocs_p1* pers, 
                            TrackParticleAssocs* trans, 
                            MsgStream& msg ) const override;

  /** Method creating the persistent representation @c INav4MomAssocs_p3
   *  from its transient representation @c INav4MomAssocs
   */
  virtual void transToPers( const TrackParticleAssocs* trans, 
                            TrackParticleAssocs_p1* pers, 
                            MsgStream& msg ) const override;

protected: 

  /// converter for @c ElementLink to @c TrackParticleCollection
  TrackParticleCnv_t m_TrackParticleLinkCnv;

  /// converter for @c DataLink to @c INav4MomAssocs
  IAssocStoresCnv_t m_assocStoresCnv;
}; 


inline TrackParticleAssocsCnv_p1::TrackParticleAssocsCnv_p1()
{}

inline TrackParticleAssocsCnv_p1::~TrackParticleAssocsCnv_p1()
{}

#endif //> PARTICLEEVENTTPCNV_TRACKPARTICLEASSOCSCNV_p1_H
