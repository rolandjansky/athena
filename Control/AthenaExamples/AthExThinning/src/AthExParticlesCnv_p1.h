///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// AthExParticlesCnv_p1.h 
// Header file for class AthExParticlesCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXTHINNING_AthExPARTICLESCNV_P1_H 
#define ATHEXTHINNING_AthExPARTICLESCNV_P1_H 

// STL includes


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

class AthExParticlesCnv_p1 : public T_AthenaPoolTPCnvWithKeyBase<
                                              AthExParticles, 
				              AthExParticles_p1
                                              >  
{ 
 public: 
  using base_class::transToPersWithKey;
  using base_class::persToTransWithKey;


  /** Default constructor: 
   */
  AthExParticlesCnv_p1();


  /** Method creating the transient representation of @c Particles
   *  from its persistent representation @c Particles_p1
   */
  virtual void persToTransWithKey( const AthExParticles_p1* persObj, 
                                   AthExParticles* transObj,
                                   const std::string& key,
                                   MsgStream &msg ) const override;

  /** Method creating the persistent representation @c Particles_p1
   *  from its transient representation @c Particles
   */
  virtual void transToPersWithKey( const AthExParticles* transObj, 
                                   AthExParticles_p1* persObj, 
                                   const std::string& key,
                                   MsgStream &msg ) const override;
}; 


inline AthExParticlesCnv_p1::AthExParticlesCnv_p1()
{}

#endif //> ATHEXTHINNING_AthExPARTICLESCNV_P1_H
