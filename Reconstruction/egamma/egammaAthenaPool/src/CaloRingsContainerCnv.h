///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CaloRingsContainerCnv.h 
// Header file for class CaloRingsContainerCnv
// Author: D.E.Ferreira de Lima<dferreir@mail.cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef RECATHENAPOOL_CALORINGSCONTAINERCNV_H 
#define RECATHENAPOOL_CALORINGSCONTAINERCNV_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// RecTPCnv includes
#include "egammaEventTPCnv/CaloRingsContainer_p1.h"

// egammaEvent includes
#include "egammaEvent/CaloRingsContainer.h"


// Forward declaration

// the latest persistent representation type of DataCollection:
typedef CaloRingsContainer_p1  CaloRingsContainer_PERS;

class CaloRingsContainerCnv: public T_AthenaPoolCustomCnv<
                                      CaloRingsContainer, 
                                      CaloRingsContainer_PERS 
                                   > 

{

  // make the factory for this converter our friend
  friend class CnvFactory<CaloRingsContainerCnv>;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Create the converter from the service locator
   */
  CaloRingsContainerCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual CaloRingsContainer_PERS*
    createPersistent( CaloRingsContainer* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual CaloRingsContainer* createTransient();

};

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline CaloRingsContainerCnv::CaloRingsContainerCnv( ISvcLocator* svcLocator ) :
  T_AthenaPoolCustomCnv<CaloRingsContainer, CaloRingsContainer_PERS>(svcLocator)
{}

#endif //> RECATHENAPOOL_CALORINGSCONTAINERCNV_H
