///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonContainer_p1.h 
// Header file for class MuonCaloEnergyContainer_p1
// Author: Ketevi A. Assamagan
//         February 2008
/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_MUON_CALOENERGY_CONTAINER_P1_H 
#define RECTPCNV_MUON_CALOENERGY_CONTAINER_P1_H 

// STL includes
#include <vector>

// RecTPCnv includes
#include "RecTPCnv/CaloEnergy_p2.h"

class MuonCaloEnergyContainer_p1 : public std::vector<CaloEnergy_p2>
{};

#endif //> RECTPCNV_MUONCONTAINER_P2_H
