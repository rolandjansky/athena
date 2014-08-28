///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CompositeParticleContainerCnv_p1.h 
// Header file for class CompositeParticleContainerCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTTPCNV_COMPOSITEPARTICLECONTAINERCNV_P1_H 
#define PARTICLEEVENTTPCNV_COMPOSITEPARTICLECONTAINERCNV_P1_H 

// STL includes

// Gaudi includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// ParticleEvent includes
#include "ParticleEvent/CompositeParticleContainer.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/CompositeParticleContainer_p1.h"

// forward declares

class CompositeParticleContainerCnv_p1 
  : public T_AthenaPoolTPCnvBase<
               CompositeParticleContainer, 
               CompositeParticleContainer_p1
               >  
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  CompositeParticleContainerCnv_p1();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c CompositeParticleContainer
   *  from its persistent representation @c CompositeParticleContainer_p1
   */
  virtual void persToTrans( const CompositeParticleContainer_p1* pers, 
                            CompositeParticleContainer* trans, 
                            MsgStream& msg ) ;

  /** Method creating the persistent representation @c CompositeParticleContainer_p1
   *  from its transient representation @c CompositeParticleContainer
   */
  virtual void transToPers( const CompositeParticleContainer* trans, 
                            CompositeParticleContainer_p1* pers, 
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

inline CompositeParticleContainerCnv_p1::CompositeParticleContainerCnv_p1()
{}

#endif //> PARTICLEEVENTTPCNV_COMPOSITEPARTICLECONTAINERCNV_P1_H
