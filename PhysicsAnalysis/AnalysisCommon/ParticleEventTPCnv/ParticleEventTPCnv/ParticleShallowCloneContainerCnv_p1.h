///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
  : public T_AthenaPoolTPCnvConstBase< ParticleShallowCloneContainer, 
                                       ParticleShallowCloneContainer_p1
                                       >  
{ 
 public: 
  using base_class::transToPers;
  using base_class::persToTrans;


  /** Default constructor: 
   */
  ParticleShallowCloneContainerCnv_p1();


  /** Method creating the transient representation of @c ParticleShallowCloneContainer
   *  from its persistent representation @c ParticleShallowCloneContainer_p1
   */
  virtual void persToTrans( const ParticleShallowCloneContainer_p1* pers, 
                            ParticleShallowCloneContainer* trans, 
                            MsgStream& msg ) const override;

  /** Method creating the persistent representation @c ParticleShallowCloneContainer_p1
   *  from its transient representation @c ParticleShallowCloneContainer
   */
  virtual void transToPers( const ParticleShallowCloneContainer* trans, 
                            ParticleShallowCloneContainer_p1* pers, 
                            MsgStream& msg ) const override;
}; 


inline ParticleShallowCloneContainerCnv_p1::ParticleShallowCloneContainerCnv_p1()
{}

#endif //> PARTICLEEVENTTPCNV_PARTICLESHALLOWCLONECONTAINERCNV_P1_H
