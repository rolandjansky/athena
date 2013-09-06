/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        MuonCaloEnergyContainer_tlp1.h
// package:     Reconstruction/RecTPCnv
// authors:     Ketevi A. Assamagan
// date:        2008-04-17
//-----------------------------------------------------------------------------

#ifndef RECTPCNV_MUONCALOENERGY_CONTAINER_TLP1_H
#define RECTPCNV_MUONCALOENERGY_CONTAINER_TLP1_H

#include <vector>
#include "RecTPCnv/CaloEnergy_p2.h"
#include "RecTPCnv/MuonCaloEnergyContainer_p1.h"

class MuonCaloEnergyContainer_tlp1
{
    public:

      std::vector<MuonCaloEnergyContainer_p1>  m_muonCaloEnergyContainer;
      std::vector<CaloEnergy_p2>               m_caloEnergies;
};

#endif

