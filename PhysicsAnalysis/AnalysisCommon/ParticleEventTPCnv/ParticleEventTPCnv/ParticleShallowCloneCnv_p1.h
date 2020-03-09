///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ParticleShallowCloneCnv_p1.h 
// Header file for class ParticleShallowCloneCnv_p1
// Author: Karsten Koeneke - February 2013
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTTPCNV_PARTICLESHALLOWCLONECNV_P1_H 
#define PARTICLEEVENTTPCNV_PARTICLESHALLOWCLONECNV_P1_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/ParticleShallowClone_p1.h"

// Forward declaration
class MsgStream;
namespace Analysis {
  class ParticleShallowClone;
}

class ParticleShallowCloneCnv_p1 
  : public T_AthenaPoolTPCnvConstBase<Analysis::ParticleShallowClone, ParticleShallowClone_p1>
{ 
 public: 
  using base_class::transToPers;
  using base_class::persToTrans;


  /** Default constructor: 
   */
  ParticleShallowCloneCnv_p1();


  /** Method creating the transient representation of @c ParticleShallowClone
   *  from its persistent representation @c ParticleShallowClone_p1
   */
  virtual void persToTrans( const ParticleShallowClone_p1* persObj, 
                            Analysis::ParticleShallowClone* transObj, 
                            MsgStream& msg ) const override;

  /** Method creating the persistent representation @c ParticleShallowClone_p1
   *  from its transient representation @c ParticleShallowClone
   */
  virtual void transToPers( const Analysis::ParticleShallowClone* transObj, 
                            ParticleShallowClone_p1* persObj, 
                            MsgStream& msg ) const override;
}; 


inline ParticleShallowCloneCnv_p1::ParticleShallowCloneCnv_p1()
{}

#endif //> PARTICLEEVENTTPCNV_COMPOSITEPARTICLECNV_P1_H
