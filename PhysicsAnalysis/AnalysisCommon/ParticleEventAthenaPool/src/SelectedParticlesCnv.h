///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SelectedParticlesCnv.h 
// Header file for class SelectedParticlesCnv
// Author: S.Protopopescu serban@bnl.gov
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTATHENAPOOL_SELECTEDPARTICLESCNV_H
#define PARTICLEEVENTATHENAPOOL_SELECTEDPARTICLESCNV_H

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IAlgTool.h"
#include "StoreGate/StoreGateSvc.h"

// ParticleEvent includes
#include "ParticleEvent/SelectedParticles.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/SelectedParticlesCnv_p1.h"

// Forward declaration

// the latest persistent representation type of SelectedParticles:
typedef SelectedParticles_p1  SelectedParticles_PERS;

class SelectedParticlesCnv: public T_AthenaPoolCustomCnv<
                                      SelectedParticles, 
                                      SelectedParticles_PERS 
                                   > 

{

  // make the factory for this converter our friend
  friend class CnvFactory<SelectedParticlesCnv>;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Create the converter from the service locator
   */
  SelectedParticlesCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual SelectedParticles_PERS*
    createPersistent( SelectedParticles* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual SelectedParticles* createTransient();

};

#endif //>PARTICLEEVENTATHENAPOOL_SELECTEDPARTICLESCNV_H 
