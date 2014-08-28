///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ParticleShallowCloneContainerCnv_p1.h 
// Header file for class ParticleShallowCloneContainerCnv_p1
// Author: Karsten Koeneke - February 2013
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTTPCNV_PARTICLESHALLOWCLONECONTAINERCNV_P1_H 
#define PARTICLEEVENTTPCNV_PARTICLESHALLOWCLONECONTAINERCNV_P1_H 

// STL includes

// Gaudi includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// ParticleEvent includes
#include "ParticleEvent/ParticleShallowCloneContainer.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/ParticleShallowCloneContainer_p1.h"

// forward declares

class ParticleShallowCloneContainerCnv_p1 
  : public T_AthenaPoolTPCnvBase< ParticleShallowCloneContainer, 
                                  ParticleShallowCloneContainer_p1
                                  >  
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  ParticleShallowCloneContainerCnv_p1();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c ParticleShallowCloneContainer
   *  from its persistent representation @c ParticleShallowCloneContainer_p1
   */
  virtual void persToTrans( const ParticleShallowCloneContainer_p1* pers, 
                            ParticleShallowCloneContainer* trans, 
                            MsgStream& msg ) ;

  /** Method creating the persistent representation @c ParticleShallowCloneContainer_p1
   *  from its transient representation @c ParticleShallowCloneContainer
   */
  virtual void transToPers( const ParticleShallowCloneContainer* trans, 
                            ParticleShallowCloneContainer_p1* pers, 
                            MsgStream& msg ) ;

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 
}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline ParticleShallowCloneContainerCnv_p1::ParticleShallowCloneContainerCnv_p1()
{}

#endif //> PARTICLEEVENTTPCNV_PARTICLESHALLOWCLONECONTAINERCNV_P1_H
