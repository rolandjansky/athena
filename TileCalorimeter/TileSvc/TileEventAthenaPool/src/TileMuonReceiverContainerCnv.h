///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileMuonReceiverContainerCnv.h 
// Header file for class TileMuonReceiverContainerCnv
// Author: Joao Gentil Saraiva <joao.gentil.saraiva@cern.ch>
// Date:   March 2014
/////////////////////////////////////////////////////////////////// 
#ifndef TILEEVENTATHENAPOOL_TILEMUONRECEIVERCONTAINERCNV_H 
#define TILEEVENTATHENAPOOL_TILEMUONRECEIVERCONTAINERCNV_H 

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// TileTPCnv includes
#include "TileTPCnv/TileMuonReceiverContainer_p1.h"

// TileEvent includes
#include "TileEvent/TileMuonReceiverContainer.h"

typedef TileMuonReceiverContainer_p1  TileMuonReceiverContainer_PERS;

class TileMuonReceiverContainerCnv: public T_AthenaPoolCustomCnv<
                                      TileMuonReceiverContainer, 
                                      TileMuonReceiverContainer_PERS 
                                   > 
{

  // make the factory for this converter our friend
  friend class CnvFactory<TileMuonReceiverContainerCnv>;

 protected:

  /** Create the converter from the service locator
   */
  TileMuonReceiverContainerCnv(ISvcLocator* svcloc);

  /** Build the persistent representation from the transient one.
   */
  virtual TileMuonReceiverContainer_PERS*
    createPersistent( TileMuonReceiverContainer* transCont );
  
  /** Build the transient representation from a persistent one
   */
  virtual TileMuonReceiverContainer* createTransient();

};

inline TileMuonReceiverContainerCnv::TileMuonReceiverContainerCnv( ISvcLocator* svcLocator ) :
  T_AthenaPoolCustomCnv<TileMuonReceiverContainer, TileMuonReceiverContainer_PERS>(svcLocator)
{}

#endif
