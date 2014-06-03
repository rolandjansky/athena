///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileTTL1ContainerCnv.h 
// Header file for class TileTTL1ContainerCnv
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef TILEEVENTATHENAPOOL_TILETTL1CONTAINERCNV_H 
#define TILEEVENTATHENAPOOL_TILETTL1CONTAINERCNV_H 

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// TileTPCnv includes
#include "TileTPCnv/TileTTL1Container_p1.h"

// TileEvent includes
#include "TileEvent/TileTTL1Container.h"

typedef TileTTL1Container_p1  TileTTL1Container_PERS;

class TileTTL1ContainerCnv: public T_AthenaPoolCustomCnv<
                                      TileTTL1Container, 
                                      TileTTL1Container_PERS 
                                   > 

{

  // make the factory for this converter our friend
  friend class CnvFactory<TileTTL1ContainerCnv>;

 protected:

  /** Create the converter from the service locator
   */
  TileTTL1ContainerCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual TileTTL1Container_PERS*
    createPersistent( TileTTL1Container* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual TileTTL1Container* createTransient();

};

inline TileTTL1ContainerCnv::TileTTL1ContainerCnv( ISvcLocator* svcLocator ) :
  T_AthenaPoolCustomCnv<TileTTL1Container, TileTTL1Container_PERS>(svcLocator)
{}

#endif //> TILEEVENTATHENAPOOL_TILETTL1CONTAINERCNV_H
