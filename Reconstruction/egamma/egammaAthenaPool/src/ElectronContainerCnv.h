///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ElectronContainerCnv.h 
// Header file for class ElectronContainerCnv
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAATHENAPOOL_ELECTRONCONTAINERCNV_H 
#define EGAMMAATHENAPOOL_ELECTRONCONTAINERCNV_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/ElectronContainer_p1.h"
#include "egammaEventTPCnv/ElectronContainer_p2.h"
#include "egammaEventTPCnv/ElectronContainer_p3.h"
#include "egammaEventTPCnv/ElectronContainer_p4.h"
#include "egammaEventTPCnv/ElectronContainer_p5.h"

// egammaEvent includes
#include "egammaEvent/ElectronContainer.h"


// Forward declaration

// the latest persistent representation type of DataCollection:
typedef ElectronContainer_p5  ElectronContainer_PERS;

class ElectronContainerCnv: public T_AthenaPoolCustomCnv<
                                      ElectronContainer, 
                                      ElectronContainer_PERS 
                                   > 

{

  // make the factory for this converter our friend
  friend class CnvFactory<ElectronContainerCnv>;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Create the converter from the service locator
   */
  ElectronContainerCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual ElectronContainer_PERS*
    createPersistent( ElectronContainer* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual ElectronContainer* createTransient();

};

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline ElectronContainerCnv::ElectronContainerCnv( ISvcLocator* svcLocator ) :
  T_AthenaPoolCustomCnv<ElectronContainer, ElectronContainer_PERS>(svcLocator)
{}

#endif //> EGAMMAATHENAPOOL_ELECTRONCONTAINERCNV_H
