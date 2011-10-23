///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// egDetailContainerCnv.h 
// Header file for class egDetailContainerCnv
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAATHENAPOOL_EGDETAILCONTAINERCNV_H 
#define EGAMMAATHENAPOOL_EGDETAILCONTAINERCNV_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egDetailContainer_p1.h"
#include "egammaEventTPCnv/egDetailContainer_p2.h"

// egammaEvent includes
#include "egammaEvent/egDetailContainer.h"


// Forward declaration

// the latest persistent representation type of DataCollection:
typedef egDetailContainer_p2  egDetailContainer_PERS;

class egDetailContainerCnv: public T_AthenaPoolCustomCnv<
                                      egDetailContainer, 
                                      egDetailContainer_PERS 
                                   > 

{

  // make the factory for this converter our friend
  friend class CnvFactory<egDetailContainerCnv>;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Create the converter from the service locator
   */
  egDetailContainerCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual egDetailContainer_PERS*
    createPersistent( egDetailContainer* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual egDetailContainer* createTransient();

};

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline egDetailContainerCnv::egDetailContainerCnv( ISvcLocator* svcLocator ) :
  T_AthenaPoolCustomCnv<egDetailContainer, egDetailContainer_PERS>(svcLocator)
{}

#endif //> EGAMMAATHENAPOOL_EGDETAILCONTAINERCNV_H
