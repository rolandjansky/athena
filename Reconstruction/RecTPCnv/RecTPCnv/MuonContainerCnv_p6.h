///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonContainerCnv_p6.h 
// Header file for class MuonContainerCnv_p6
// Author: Ketevi A. Assamagan <ketevi@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_MUONCONTAINERCNV_P6_H 
#define RECTPCNV_MUONCONTAINERCNV_P6_H 1

// STL includes

// Gaudi includes

// muonEvent includes
#include "muonEvent/MuonContainer.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// RecTPCnv includes
#include "RecTPCnv/MuonContainer_p6.h"
#include "RecTPCnv/MuonCnv_p6.h"

typedef T_AthenaPoolTPCnvVector< 
            Analysis::MuonContainer, 
            MuonContainer_p6, 
            MuonCnv_p6 
       > MuonContainerCnv_p6;


#endif //> RECTPCNV_MUONCONTAINERCNV_P6_H
