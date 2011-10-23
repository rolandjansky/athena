///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PhotonContainerCnv.h 
// Header file for class PhotonContainerCnv
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAATHENAPOOL_PHOTONCONTAINERCNV_H 
#define EGAMMAATHENAPOOL_PHOTONCONTAINERCNV_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/PhotonContainer_p1.h"
#include "egammaEventTPCnv/PhotonContainer_p2.h"
#include "egammaEventTPCnv/PhotonContainer_p3.h"
#include "egammaEventTPCnv/PhotonContainer_p4.h"
#include "egammaEventTPCnv/PhotonContainer_p5.h"

// egammaEvent includes
#include "egammaEvent/PhotonContainer.h"


// Forward declaration

// the latest persistent representation type of DataCollection:
typedef PhotonContainer_p5  PhotonContainer_PERS;

class PhotonContainerCnv: public T_AthenaPoolCustomCnv<
                                      PhotonContainer, 
                                      PhotonContainer_PERS 
                                   > 

{

  // make the factory for this converter our friend
  friend class CnvFactory<PhotonContainerCnv>;

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected:

  /** Create the converter from the service locator
   */
  PhotonContainerCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual PhotonContainer_PERS*
    createPersistent( PhotonContainer* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual PhotonContainer* createTransient();

};

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline PhotonContainerCnv::PhotonContainerCnv( ISvcLocator* svcLocator ) :
  T_AthenaPoolCustomCnv<PhotonContainer, PhotonContainer_PERS>(svcLocator)
{}

#endif //> EGAMMAATHENAPOOL_PHOTONCONTAINERCNV_H
