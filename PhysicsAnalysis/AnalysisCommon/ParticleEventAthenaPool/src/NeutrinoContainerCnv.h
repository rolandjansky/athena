///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// NeutrinoContainerCnv.h 
// Header file for class NeutrinoContainerCnv
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTATHENAPOOL_NEUTRINOCONTAINERCNV_H 
#define PARTICLEEVENTATHENAPOOL_NEUTRINOCONTAINERCNV_H 1

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// ParticleEvent includes
#include "ParticleEvent/NeutrinoContainer.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/NeutrinoContainer_p2.h"

// Forward declaration

// the latest persistent representation type of DataCollection:
typedef NeutrinoContainer_p2  NeutrinoContainer_PERS;

class NeutrinoContainerCnv: public T_AthenaPoolCustomCnv<
                                      NeutrinoContainer, 
                                      NeutrinoContainer_PERS 
                                   > 

{

  // make the factory for this converter our friend
  friend class CnvFactory<NeutrinoContainerCnv>;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Create the converter from the service locator
   */
  NeutrinoContainerCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual NeutrinoContainer_PERS*
    createPersistent( NeutrinoContainer* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual NeutrinoContainer* createTransient();

};

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline NeutrinoContainerCnv::NeutrinoContainerCnv( ISvcLocator* svcLocator ) :
  T_AthenaPoolCustomCnv<NeutrinoContainer, NeutrinoContainer_PERS>(svcLocator)
{}

#endif //> PARTICLEEVENTATHENAPOOL_NEUTRINOCONTAINERCNV_H
