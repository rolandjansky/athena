///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileCosmicMuonContainerCnv.h 
// Header file for class TileCosmicMuonContainerCnv
// Author: Aranzazu Ruiz <aranzazu.ruiz.martinez@cern.ch>
// Date:   July 2008
// Revised:   July 2009 maneira@lip.pt
/////////////////////////////////////////////////////////////////// 
#ifndef TILEEVENTATHENAPOOL_TILECOSMICMUONCONTAINERCNV_H 
#define TILEEVENTATHENAPOOL_TILECOSMICMUONCONTAINERCNV_H 

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// TileTPCnv includes
#include "TileTPCnv/TileCosmicMuonContainer_p1.h"
#include "TileTPCnv/TileCosmicMuonContainer_p2.h"

// TileEvent includes
#include "TileEvent/TileCosmicMuonContainer.h"

typedef TileCosmicMuonContainer_p2  TileCosmicMuonContainer_PERS;

class TileCosmicMuonContainerCnv: public T_AthenaPoolCustomCnv<
                                      TileCosmicMuonContainer, 
                                      TileCosmicMuonContainer_PERS 
                                   > 

{

  // make the factory for this converter our friend
  friend class CnvFactory<TileCosmicMuonContainerCnv>;

 protected:

  /** Create the converter from the service locator
   */
  TileCosmicMuonContainerCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual TileCosmicMuonContainer_PERS*
    createPersistent( TileCosmicMuonContainer* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual TileCosmicMuonContainer* createTransient();

};

inline TileCosmicMuonContainerCnv::TileCosmicMuonContainerCnv( ISvcLocator* svcLocator ) :
  T_AthenaPoolCustomCnv<TileCosmicMuonContainer, TileCosmicMuonContainer_PERS>(svcLocator)
{}

#endif //> TILEEVENTATHENAPOOL_TILECOSMICMUONCONTAINERCNV_H
