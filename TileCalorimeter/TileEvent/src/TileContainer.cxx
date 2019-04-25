/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileContainer.cxx
//  Author   : Alexandre Solodkov
//  Created  : Mar, 2003
//
//  DESCRIPTION:
//     methods for all container classes
//
//  HISTORY:
//    12Mar03: Created
//
//  BUGS:
//    
//    
//
//*****************************************************************************

#include "TileEvent/TileContainer.h"
#include "TileEvent/TileHitContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileCellIDC.h"
#include "TileEvent/TileBeamElemContainer.h"
#include "AthenaKernel/BaseInfo.h"
 
SG_ADD_BASE (TileContainer<TileCell>, DataVector<TileCell>);
