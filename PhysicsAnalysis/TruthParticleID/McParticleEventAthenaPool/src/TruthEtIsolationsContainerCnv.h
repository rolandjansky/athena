///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TruthEtIsolationsContainerCnv.h 
// Header file for class TruthEtIsolationsContainerCnv
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEEVENTATHENAPOOL_TRUTHETISOLATIONSCONTAINERCNV_H 
#define MCPARTICLEEVENTATHENAPOOL_TRUTHETISOLATIONSCONTAINERCNV_H 

// STL includes


// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// McParticleEvent includes
#include "McParticleEvent/TruthEtIsolationsContainer.h"

// McParticleEventTPCnv includes
#include "McParticleEventTPCnv/TruthEtIsolationsContainer_p1.h"

// Forward declaration

// the latest persistent representation type of DataCollection:
typedef TruthEtIsolationsContainer_p1  TruthEtIsolationsContainer_PERS;

class TruthEtIsolationsContainerCnv: public T_AthenaPoolCustomCnv<
                                          TruthEtIsolationsContainer, 
			                  TruthEtIsolationsContainer_PERS 
			                > 

{

  // make the factory for this converter our friend
  friend class CnvFactory<TruthEtIsolationsContainerCnv>;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Create the converter from the service locator
   */
  TruthEtIsolationsContainerCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual TruthEtIsolationsContainer_PERS*
  createPersistent( TruthEtIsolationsContainer* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual TruthEtIsolationsContainer* createTransient();

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected:

};

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> MCPARTICLEEVENTATHENAPOOL_TRUTHETISOLATIONSCONTAINERCNV_H

