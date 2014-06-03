///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileMuContainerCnv.h 
// Header file for class TileMuContainerCnv
// Author: Aranzazu Ruiz <aranzazu.ruiz.martinez@cern.ch>
// Date:   July 2008
/////////////////////////////////////////////////////////////////// 
#ifndef TILEEVENTATHENAPOOL_TILEMUCONTAINERCNV_H 
#define TILEEVENTATHENAPOOL_TILEMUCONTAINERCNV_H 

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// TileTPCnv includes
#include "TileTPCnv/TileMuContainer_p1.h"

// TileEvent includes
#include "TileEvent/TileMuContainer.h"

typedef TileMuContainer_p1  TileMuContainer_PERS;

class TileMuContainerCnv: public T_AthenaPoolCustomCnv<
                                      TileMuContainer, 
                                      TileMuContainer_PERS 
                                   > 

{

  // make the factory for this converter our friend
  friend class CnvFactory<TileMuContainerCnv>;

 protected:

  /** Create the converter from the service locator
   */
  TileMuContainerCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual TileMuContainer_PERS*
    createPersistent( TileMuContainer* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual TileMuContainer* createTransient();

};

inline TileMuContainerCnv::TileMuContainerCnv( ISvcLocator* svcLocator ) :
  T_AthenaPoolCustomCnv<TileMuContainer, TileMuContainer_PERS>(svcLocator)
{}

#endif //> TILEEVENTATHENAPOOL_TILEMUCONTAINERCNV_H
