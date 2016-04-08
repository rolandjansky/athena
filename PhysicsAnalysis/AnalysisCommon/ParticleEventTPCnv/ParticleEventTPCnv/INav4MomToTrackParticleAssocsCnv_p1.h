///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// INav4MomToTrackParticleAssocsCnv_p1.h 
// Header file for class INav4MomToTrackParticleAssocsCnv_p1
// Author: Karsten Koeneke
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTTPCNV_INAV4MOMTOTRACKPARTICLEASSOCSCNV_p1_H 
#define PARTICLEEVENTTPCNV_INAV4MOMTOTRACKPARTICLEASSOCSCNV_p1_H

// STL includes

// HepMC / CLHEP includes

// Gaudi includes

#include "AthLinks/ElementLink.h"

// DataModelAthenaPool includes
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "DataModelAthenaPool/DataLinkCnv_p2.h"

// TrackParticleIncludes
#include "Particle/TrackParticleContainer.h"
#include "ParticleEvent/INav4MomToTrackParticleAssocs.h"

// EventCommonTPCnv includes
#include "ParticleEventTPCnv/INav4MomToTrackParticleAssocs_p1.h"

// Forward declaration
class MsgStream;

class INav4MomToTrackParticleAssocsCnv_p1 
  : public T_AthenaPoolTPCnvBase< INav4MomToTrackParticleAssocs, 
                                  INav4MomToTrackParticleAssocs_p1
                                  >  
{

  /////////////////////////////////////////////////////////////////// 
  // Public typedefs: 
  /////////////////////////////////////////////////////////////////// 
public: 

  /// converter for @c ElementLink to @c INavigable4MomentumCollection
  typedef ElementLinkCnv_p3< ElementLink<INavigable4MomentumCollection> > INav4MomCnv_t;

  /// converter for @c ElementLink to @c TrackParticleContainer
  typedef ElementLinkCnv_p3< ElementLink< Rec::TrackParticleContainer> > TrackParticleCnv_t;

  typedef DataLinkCnv_p2<DataLink<INav4MomToTrackParticleAssocs> > IAssocStoresCnv_t;


  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 

  /** Default constructor: 
   */
  INav4MomToTrackParticleAssocsCnv_p1();

  /** Destructor: 
   */
  virtual ~INav4MomToTrackParticleAssocsCnv_p1();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c INav4MomToTrackParticleAssocs
   *  from its persistent representation @c INav4MomToTrackParticleAssocs_p1
   */
  virtual void persToTrans( const INav4MomToTrackParticleAssocs_p1* pers, 
                            INav4MomToTrackParticleAssocs* trans, 
                            MsgStream& msg );

  /** Method creating the persistent representation @c INav4MomToTrackParticleAssocs_p1
   *  from its transient representation @c INav4MomToTrackParticleAssocs
   */
  virtual void transToPers( const INav4MomToTrackParticleAssocs* trans, 
                            INav4MomToTrackParticleAssocs_p1* pers, 
                            MsgStream& msg );

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
protected: 

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
protected: 

  /// converter for @c ElementLink to @c INavigable4MomentumCollection
  INav4MomCnv_t m_inav4MomLinkCnv;

  /// converter for @c ElementLink to @c TrackParticleCollection
  TrackParticleCnv_t m_TrackParticleLinkCnv;

  /// converter for @c DataLink to @c INav4MomAssocs
  IAssocStoresCnv_t m_assocStoresCnv;
};



/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline INav4MomToTrackParticleAssocsCnv_p1::INav4MomToTrackParticleAssocsCnv_p1()
{}

inline INav4MomToTrackParticleAssocsCnv_p1::~INav4MomToTrackParticleAssocsCnv_p1()
{}

#endif //> PARTICLEEVENTTPCNV_INAV4MOMTOTRACKPARTICLEASSOCSCNV_p1_H
