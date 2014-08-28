///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SelectedParticlesCnv_p1.h 
// Header file for class SelectedParticlesCnv_p1
// Author: S.Protopopescu serban@bnl.gov
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTTPCNV_SELECTEDPARTICLESCNV_P1_H 
#define PARTICLEEVENTTPCNV_SELECTEDPARTICLESCNV_P1_H 

// STL includes

// Gaudi includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/SelectedParticles_p1.h"

// Forward declaration
class MsgStream;
class SelectedParticles;

class SelectedParticlesCnv_p1 : public T_AthenaPoolTPCnvBase<
                                          SelectedParticles, 
                                          SelectedParticles_p1
                                       >  
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  SelectedParticlesCnv_p1();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c SelectedParticles
   *  from its persistent representation @c SelectedParticles_p1
   */
  virtual void persToTrans( const SelectedParticles_p1* pers, 
                            SelectedParticles* trans, 
                            MsgStream& msg ) ;

  /** Method creating the persistent representation @c SelectedParticles_p1
   *  from its transient representation @c SelectedParticles
   */
  virtual void transToPers( const SelectedParticles* trans, 
                            SelectedParticles_p1* pers, 
                            MsgStream& msg ) ;

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline SelectedParticlesCnv_p1::SelectedParticlesCnv_p1()
{}


#endif //> PARTICLEEVENTTPCNV_SELECTEDPARTICLESCNV_P1_H
