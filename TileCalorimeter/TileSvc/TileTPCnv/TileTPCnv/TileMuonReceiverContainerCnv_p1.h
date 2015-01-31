///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileMuonReceiverContainerCnv_p1.h 
// Header file for class TileMuonReceiverContainerCnv_p1
// Author: Joao Gentil Saraiva <joao.gentil.saraiva@cern.ch>
// Date:   March 2014
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILEMUONRECEIVERCONTAINERCNV_P1_H
#define TILETPCNV_TILEMUONRECEIVERCONTAINERCNV_P1_H

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// TileTPCnv includes
#include "TileTPCnv/TileMuonReceiverContainer_p1.h"
#include "TileTPCnv/TileMuonReceiverObjCnv_p1.h"

// TileEvent includes
#include "TileEvent/TileContainer.h"

typedef T_AthenaPoolTPCnvVector<
            TileMuonReceiverContainer,
            TileMuonReceiverContainer_p1,
            TileMuonReceiverObjCnv_p1
       > TileMuonReceiverContainerCnv_p1;

#endif
