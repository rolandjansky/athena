///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonCaloEnergyContainerCnv_p1.h 
// Header file for class MuonCaloEnergyContainerCnv_p1
// Author: Ketevi A. Assamagan<ketevi@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_MUON_CALOENERGY_CONTAINERCNV_P1_H 
#define RECTPCNV_MUON_CALOENERGY_CONTAINERCNV_P1_H 

// STL includes

// Gaudi includes

// muonEvent includes
#include "muonEvent/MuonCaloEnergyContainer.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// RecTPCnv includes
#include "RecTPCnv/MuonCaloEnergyContainer_p1.h"
#include "RecTPCnv/CaloEnergyCnv_p2.h"

typedef T_AthenaPoolTPCnvVector< 
            MuonCaloEnergyContainer, 
            MuonCaloEnergyContainer_p1, 
            CaloEnergyCnv_p2 
       > MuonCaloEnergyContainerCnv_p1;


#endif //> RECTPCNV_MUON_CSLOENERGY_CONTAINERCNV_P1_H
