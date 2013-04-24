///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// eflowContainerCnv.h 
// Header file for class eflowObjectContainerCnv
// Author: M.Hodgkinson
// Based on JetCollectionCnv.h
/////////////////////////////////////////////////////////////////// 
#ifndef RECATHENAPOOL_EFLOWOBJECTCONTAINERCNV_H 
#define RECATHENAPOOL_EFLOWOBJECTCONTAINERCNV_H 

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "eflowEventTPCnv/eflowObjectContainer_p5.h"
#include "eflowEvent/eflowObjectContainer.h"

class eflowObjectContainerCnv: public T_AthenaPoolCustomCnv<eflowObjectContainer, eflowObjectContainer_p5>
{

  // make the factory for this converter our friend
  friend class CnvFactory<eflowObjectContainerCnv>;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Create the converter from the service locator
   */
  eflowObjectContainerCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual eflowObjectContainer_p5* createPersistent( eflowObjectContainer* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual eflowObjectContainer* createTransient();

};

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline eflowObjectContainerCnv::eflowObjectContainerCnv( ISvcLocator* svcLocator ) :
  T_AthenaPoolCustomCnv<eflowObjectContainer, eflowObjectContainer_p5>(svcLocator)
{}

#endif
