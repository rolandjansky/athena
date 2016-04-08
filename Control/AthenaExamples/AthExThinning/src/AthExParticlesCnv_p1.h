///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthExParticlesCnv_p1.h 
// Header file for class AthExParticlesCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXTHINNING_AthExPARTICLESCNV_P1_H 
#define ATHEXTHINNING_AthExPARTICLESCNV_P1_H 

// STL includes

// HepMC / CLHEP includes

// Gaudi includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// DataModel converters
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"

// AthExThinning includes
#include "AthExThinning/AthExParticles.h"
#include "AthExThinning/AthExParticles_p1.h"


// Forward declaration
class MsgStream;

class AthExParticlesCnv_p1 : public T_AthenaPoolTPCnvBase<
                                              AthExParticles, 
				              AthExParticles_p1
                                              >  
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  AthExParticlesCnv_p1();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c Particles
   *  from its persistent representation @c Particles_p1
   */
  virtual void persToTrans( const AthExParticles_p1* persObj, 
			    AthExParticles* transObj, 
			    MsgStream &msg );

  /** Method creating the persistent representation @c Particles_p1
   *  from its transient representation @c Particles
   */
  virtual void transToPers( const AthExParticles* transObj, 
			    AthExParticles_p1* persObj, 
			    MsgStream &msg );

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

inline AthExParticlesCnv_p1::AthExParticlesCnv_p1()
{}

#endif //> ATHEXTHINNING_AthExPARTICLESCNV_P1_H
