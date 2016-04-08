///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthExParticlesCnv.h 
// Header file for class AthExParticlesCnv
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXTHINNING_AthExPARTICLESCNV_H 
#define ATHEXTHINNING_AthExPARTICLESCNV_H 

// STL includes


// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// AthExThinning includes
#include "AthExThinning/AthExParticles.h"
#include "AthExThinning/AthExParticles_p1.h"

// Forward declaration

// the latest persistent representation type of DataCollection:
typedef AthExParticles_p1  AthExParticles_PERS;

class AthExParticlesCnv: public T_AthenaPoolCustomCnv<
                                          AthExParticles, 
			                  AthExParticles_PERS 
			                  > 

{

  // make the factory for this converter our friend
  friend class CnvFactory<AthExParticlesCnv>;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Create the converter from the service locator
   */
  AthExParticlesCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual AthExParticles_PERS*
  createPersistent( AthExParticles* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual AthExParticles* createTransient();

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected:
};

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> ATHEXTHINNING_AthExPARTICLESCNV_H

