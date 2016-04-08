///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrackParticleAssocsCnv_p1.h 
// Header file for class TrackParticleAssocsCnv_p1
// Author: C.Anastopoulos
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTTPCNV_TRACKPARTICLEASSOCSCNV_p1_H 
#define PARTICLEEVENTTPCNV_TRACKPARTICLEASSOCSCNV_p1_H

// STL includes

// HepMC / CLHEP includes

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

class TrackParticleAssocsCnv_p1 : public T_AthenaPoolTPCnvBase<
                                              TrackParticleAssocs, 
                                              TrackParticleAssocs_p1
                                            >  
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public typedefs: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// converter for @c ElementLink to @c TrackParticlecollection
  typedef ElementLinkCnv_p3< ElementLink< Rec::TrackParticleContainer> >
          TrackParticleCnv_t;
  typedef DataLinkCnv_p2<DataLink<TrackParticleAssocs> > IAssocStoresCnv_t;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  TrackParticleAssocsCnv_p1();

  /** Destructor: 
   */
  virtual ~TrackParticleAssocsCnv_p1();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c INav4MomAssocs
   *  from its persistent representation @c INav4MomAssocs_p3
   */
  virtual void persToTrans( const TrackParticleAssocs_p1* pers, 
                            TrackParticleAssocs* trans, 
                            MsgStream& msg );

  /** Method creating the persistent representation @c INav4MomAssocs_p3
   *  from its transient representation @c INav4MomAssocs
   */
  virtual void transToPers( const TrackParticleAssocs* trans, 
                            TrackParticleAssocs_p1* pers, 
                            MsgStream& msg );

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// converter for @c ElementLink to @c TrackParticleCollection
  TrackParticleCnv_t m_TrackParticleLinkCnv;

  /// converter for @c DataLink to @c INav4MomAssocs
  IAssocStoresCnv_t m_assocStoresCnv;
}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline TrackParticleAssocsCnv_p1::TrackParticleAssocsCnv_p1()
{}

inline TrackParticleAssocsCnv_p1::~TrackParticleAssocsCnv_p1()
{}

#endif //> PARTICLEEVENTTPCNV_TRACKPARTICLEASSOCSCNV_p1_H
