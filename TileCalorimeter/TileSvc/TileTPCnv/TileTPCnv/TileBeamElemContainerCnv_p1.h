///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileBeamElemContainerCnv_p1.h 
// Header file for class TileBeamElemContainerCnv_p1
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILEBEAMELEMCONTAINERCNV_P1_H
#define TILETPCNV_TILEBEAMELEMCONTAINERCNV_P1_H

// TileTPCnv includes
#define private public
#include "TileTPCnv/TileBeamElemContainer_p1.h"
#undef private 
#include "TileTPCnv/TileBeamElemCnv_p1.h"
#include "TileTPCnv/T_TilePoolContainerCnv.h"

// Tile includes
#include "TileEvent/TileBeamElemContainer.h"

typedef T_TilePoolContainerCnv<
            TileBeamElemContainer,
            TileBeamElemContainer_p1,
            TileBeamElemCnv_p1
       > TileBeamElemContainerCnv_p1;

#endif //> TILETPCNV_TILEBEAMELEMCONTAINERCNV_P1_H

