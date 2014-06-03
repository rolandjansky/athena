///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileL2ContainerCnv.h 
// Header file for class TileL2ContainerCnv
// Author: Aranzazu Ruiz <aranzazu.ruiz.martinez@cern.ch>
// Date:   July 2008
/////////////////////////////////////////////////////////////////// 
#ifndef TILEEVENTATHENAPOOL_TILEL2CONTAINERCNV_H 
#define TILEEVENTATHENAPOOL_TILEL2CONTAINERCNV_H 

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// TileTPCnv includes
#include "TileTPCnv/TileL2Container_p1.h"
#include "TileTPCnv/TileL2Container_p2.h"

// TileEvent includes
#include "TileEvent/TileL2Container.h"

typedef TileL2Container_p2  TileL2Container_PERS;

class TileL2ContainerCnv: public T_AthenaPoolCustomCnv<
                                      TileL2Container, 
                                      TileL2Container_PERS 
                                   > 

{

  // make the factory for this converter our friend
  friend class CnvFactory<TileL2ContainerCnv>;

 protected:

  /** Create the converter from the service locator
   */
  TileL2ContainerCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual TileL2Container_PERS*
    createPersistent( TileL2Container* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual TileL2Container* createTransient();

};

inline TileL2ContainerCnv::TileL2ContainerCnv( ISvcLocator* svcLocator ) :
  T_AthenaPoolCustomCnv<TileL2Container, TileL2Container_PERS>(svcLocator)
{}

#endif //> TILEEVENTATHENAPOOL_TILEL2CONTAINERCNV_H
