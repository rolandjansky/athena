///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonContainerCnv_p3.h 
// Header file for class MuonContainerCnv_p3
// Author: Ketevi A. Assamagan <ketevi@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_MUONCONTAINERCNV_P3_H 
#define RECTPCNV_MUONCONTAINERCNV_P3_H 

// STL includes

// Gaudi includes

// muonEvent includes
#include "muonEvent/MuonContainer.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// RecTPCnv includes
#include "RecTPCnv/MuonContainer_p3.h"
#include "RecTPCnv/MuonCnv_p3.h"

typedef T_AthenaPoolTPCnvVector< 
            Analysis::MuonContainer, 
            MuonContainer_p3, 
            MuonCnv_p3 
       > MuonContainerCnv_p3;


#endif //> RECTPCNV_MUONCONTAINERCNV_P3_H
