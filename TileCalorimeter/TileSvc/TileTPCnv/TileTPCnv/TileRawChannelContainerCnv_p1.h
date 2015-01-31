///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileRawChannelContainerCnv_p1.h 
// Header file for class TileRawChannelContainerCnv_p1
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILERAWCHANNELCONTAINERCNV_P1_H
#define TILETPCNV_TILERAWCHANNELCONTAINERCNV_P1_H

// TileTPCnv includes
#define private public
#include "TileTPCnv/TileRawChannelContainer_p1.h"
#undef private 
#include "TileTPCnv/TileRawChannelCnv_p1.h"
#include "TileTPCnv/T_TilePoolContainerCnv.h"

// Tile includes
#include "TileEvent/TileRawChannelContainer.h"

typedef T_TilePoolContainerCnv<
            TileRawChannelContainer,
            TileRawChannelContainer_p1,
            TileRawChannelCnv_p1
       > TileRawChannelContainerCnv_p1;

#endif //> TILETPCNV_TILERAWCHANNELCONTAINERCNV_P1_H

