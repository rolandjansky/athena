///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileTTL1ContainerCnv_p1.h 
// Header file for class TileTTL1ContainerCnv_p1
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILETTL1CONTAINERCNV_P1_H
#define TILETPCNV_TILETTL1CONTAINERCNV_P1_H

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// TileTPCnv includes
#include "TileTPCnv/TileTTL1Container_p1.h"
#include "TileTPCnv/TileTTL1Cnv_p1.h"

// TileEvent includes
#include "TileEvent/TileTTL1Container.h"

typedef T_AthenaPoolTPCnvVector<
            TileTTL1Container,
            TileTTL1Container_p1,
            TileTTL1Cnv_p1
       > TileTTL1ContainerCnv_p1;

#endif //> TILETPCNV_TILETTL1CONTAINERCNV_P1_H
