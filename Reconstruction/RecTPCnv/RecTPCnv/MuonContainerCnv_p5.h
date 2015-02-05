///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonContainerCnv_p5.h 
// Header file for class MuonContainerCnv_p5
// Author: Ketevi A. Assamagan <ketevi@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_MUONCONTAINERCNV_P5_H 
#define RECTPCNV_MUONCONTAINERCNV_P5_H 1

// STL includes

// Gaudi includes

// muonEvent includes
#include "muonEvent/MuonContainer.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// RecTPCnv includes
#include "RecTPCnv/MuonContainer_p5.h"
#include "RecTPCnv/MuonCnv_p5.h"

typedef T_AthenaPoolTPCnvVector< 
            Analysis::MuonContainer, 
            MuonContainer_p5, 
            MuonCnv_p5 
       > MuonContainerCnv_p5;


#endif //> RECTPCNV_MUONCONTAINERCNV_P5_H
