///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TruthParticleContainerCnv.h 
// Header file for class TruthParticleContainerCnv
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEEVENTATHENAPOOL_TRUTHPARTICLECONTAINERCNV_H 
#define MCPARTICLEEVENTATHENAPOOL_TRUTHPARTICLECONTAINERCNV_H 

// STL includes


// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// McParticleEvent includes
#include "McParticleEvent/TruthParticleContainer.h"

// McParticleEventTPCnv includes
#include "McParticleEventTPCnv/TruthParticleContainer_p6.h"

// Forward declaration
class StoreGateSvc;
class ITruthParticleCnvTool;

// the latest persistent representation type of DataCollection:
typedef TruthParticleContainer_p6  TruthParticleContainer_PERS;

class TruthParticleContainerCnv: public T_AthenaPoolCustomCnv<
                                          TruthParticleContainer, 
			                  TruthParticleContainer_PERS 
			                > 

{

  // make the factory for this converter our friend
  friend class CnvFactory<TruthParticleContainerCnv>;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Create the converter from the service locator
   */
  TruthParticleContainerCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual TruthParticleContainer_PERS*
  createPersistent( TruthParticleContainer* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual TruthParticleContainer* createTransient();

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Pointer to the StoreGateSvc
   */
  StoreGateSvc* m_storeGate;

  /** Pointer to the McEventCollection -> TruthParticleContainer converter
   *  tool.
   */
  ITruthParticleCnvTool* m_cnvTool;

};

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> MCPARTICLEEVENTATHENAPOOL_TRUTHPARTICLECONTAINERCNV_H

