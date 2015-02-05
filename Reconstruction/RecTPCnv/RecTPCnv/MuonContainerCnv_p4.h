///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonContainerCnv_p4.h 
// Header file for class MuonContainerCnv_p4
// Author: Ketevi A. Assamagan <ketevi@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_MUONCONTAINERCNV_P4_H 
#define RECTPCNV_MUONCONTAINERCNV_P4_H 

// STL includes

// Gaudi includes

// muonEvent includes
#include "muonEvent/MuonContainer.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// RecTPCnv includes
#include "RecTPCnv/MuonContainer_p4.h"
#include "RecTPCnv/MuonCnv_p4.h"

typedef T_AthenaPoolTPCnvVector< 
            Analysis::MuonContainer, 
            MuonContainer_p4, 
            MuonCnv_p4 
       > MuonContainerCnv_p4;


#endif //> RECTPCNV_MUONCONTAINERCNV_P4_H
