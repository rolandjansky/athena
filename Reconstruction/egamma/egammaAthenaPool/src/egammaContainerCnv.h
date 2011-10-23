///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// egammaContainerCnv.h 
// Header file for class egammaContainerCnv
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAATHENAPOOL_EGAMMACONTAINERCNV_H 
#define EGAMMAATHENAPOOL_EGAMMACONTAINERCNV_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egammaContainer_p1.h"
#include "egammaEventTPCnv/egammaContainer_p2.h"
#include "egammaEventTPCnv/egammaContainer_p3.h"
#include "egammaEventTPCnv/egammaContainer_p4.h"
#include "egammaEventTPCnv/egammaContainer_p5.h"

// egammaEvent includes
#include "egammaEvent/egammaContainer.h"


// Forward declaration

// the latest persistent representation type of DataCollection:
typedef egammaContainer_p5  egammaContainer_PERS;

class egammaContainerCnv: public T_AthenaPoolCustomCnv<
                                      egammaContainer, 
			              egammaContainer_PERS
                                   > 

{

  // make the factory for this converter our friend
  friend class CnvFactory<egammaContainerCnv>;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Create the converter from the service locator
   */
  egammaContainerCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual egammaContainer_PERS*
    createPersistent( egammaContainer* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual egammaContainer* createTransient();

};

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline egammaContainerCnv::egammaContainerCnv( ISvcLocator* svcLocator ) :
  T_AthenaPoolCustomCnv<egammaContainer, egammaContainer_PERS>(svcLocator)
{}

#endif //> EGAMMAATHENAPOOL_EGAMMACONTAINERCNV_H
