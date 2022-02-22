/*
    Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//    gTowerBuilder - Builds gTowers (readout elements of gFEX)
//                              -------------------
//     begin                : 01 04 2021
//     email                : cecilia.tosciri@cern.ch
//***************************************************************************

#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "L1CaloFEXSim/gTowerBuilder.h"



// TOWER IS A COLLECTION OF SUPER CELLS
// IT SHOULD HAVE A UNIQUE ID
// IT SHOULD BE ABLE TO RETURN LIST OF SUPER CELLS BELONGING TO IT

// THIS IS A CLASS DESIGNED TO BUILD A GTOWER USING THE GTOWER CLASS AND THEN PRINT THE RELEVANT INFORMATION TO THE SCREEN USING FUNCTION CALLS FROM THE GTOWER CLASS

namespace LVL1 {

gTowerBuilder::gTowerBuilder(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent)
{
  declareInterface<IgTowerBuilder>(this);
}


void gTowerBuilder::init(std::unique_ptr<gTowerContainer> & gTowerContainerRaw)
{

  execute(gTowerContainerRaw);
}


void gTowerBuilder::reset()
{
}


void gTowerBuilder::execute(std::unique_ptr<gTowerContainer> & gTowerContainerRaw)
{
  BuildAllTowers(gTowerContainerRaw);
}

  // TOWER IDs FOR CLARITY (updated Jan 21, 2022)
  // EM Calorimeter
  // EMB region (gFEX Eta [0.0, 1.4])= 100000 + X (left); 200000 + X (right)
  // EMB/EMEC region (gFEX Eta [1.4, 1.6])= 300000 + X (left); 400000 + X (right)
  // EMEC region (gFEX Eta [1.6, 3.2])= 500000 + X (left); 600000 + X (right) (no overlap with FCAL)
  // FCAL region (gFEX Eta [3.2, 4.9])= 900000 + X (left); 1000000 + X (right)
  // HAD Calorimeter
  // Tile region (gFEX Eta [0.0, 1.4])= 100000 + X (left); 200000 + X (right)
  // Tile/HEC region (gFEX Eta [1.4, 1.6])= 300000 + X (left); 400000 + X (right)
  // HEC region (gFEX Eta [1.6, 3.2])= 500000 + X (left); 600000 + X (right) (32 phi bins)
 


 void gTowerBuilder::BuildEMBgTowers(std::unique_ptr<gTowerContainer> & gTowerContainerRaw) const
{
  int nphi = 32;
  // This is pure EMB region (for gFEX: Eta Range 0.0-1.4)
  for (int ieta = 0; ieta < 7; ++ieta) { // loop over 7 eta steps
    for (int iphi = 0; iphi < nphi; ++iphi){ // loop over 32 phi steps
      BuildSingleTower(gTowerContainerRaw, ieta, iphi, nphi, 100000, -1);
      BuildSingleTower(gTowerContainerRaw, ieta, iphi, nphi, 200000, 1);
    }
  }

}

void gTowerBuilder::BuildTransEMBgTowers(std::unique_ptr<gTowerContainer> & gTowerContainerRaw) const
{

  int TRANSEMB_MODIFIER = 7;
  int tmpVal = TRANSEMB_MODIFIER;
  int nphi = 32;
  //Transition region is where EMB and EMEC overlap (for gFEX: Eta Range 1.4-1.6)
  for (int ieta = tmpVal; ieta < tmpVal + 1; ieta++){ // loop over eta steps
    for (int iphi = 0; iphi < nphi; ++iphi){ // loop over 32 phi steps
      BuildSingleTower(gTowerContainerRaw, ieta, iphi, nphi, 300000, -1);
      BuildSingleTower(gTowerContainerRaw, ieta, iphi, nphi, 400000, 1);
    }
    TRANSEMB_MODIFIER++;
  }

}

void gTowerBuilder::BuildEMEgTowers(std::unique_ptr<gTowerContainer> & gTowerContainerRaw) const
{
  // This is pure EMEC region (for gFEX: Eta Range 1.6-3.3)
  int EME_MODIFIER = 8;
  int tmpVal = EME_MODIFIER;
  int nphi = 32;
  for (int ieta = tmpVal; ieta < tmpVal + 4; ++ieta){ // loop over eta steps
    for (int iphi = 0; iphi < nphi; ++iphi){ // loop over 32 phi steps
      BuildSingleTower(gTowerContainerRaw, ieta, iphi, nphi, 500000, -1);
      BuildSingleTower(gTowerContainerRaw, ieta, iphi, nphi, 600000, 1);
    }
    EME_MODIFIER++;
  }

}

void gTowerBuilder::BuildFwdEMECgTowers(std::unique_ptr<gTowerContainer> & gTowerContainerRaw) const
{
// Transition region between EMEC and FCAL (for gFEX: Eta Range 2.8-3.2)
  int TRANSFCAL_MODIFIER = 12;
  int tmpVal = TRANSFCAL_MODIFIER;
  int nphi = 32;
  for (int ieta = tmpVal; ieta < tmpVal + 4; ++ieta){ // loop over eta steps
    for (int iphi = 0; iphi < nphi; ++iphi){ // loop over 32 phi steps
      BuildSingleTower(gTowerContainerRaw, ieta, iphi, nphi, 700000, -1);
      BuildSingleTower(gTowerContainerRaw, ieta, iphi, nphi, 800000, 1);
    }
    TRANSFCAL_MODIFIER++;
  }

}


void gTowerBuilder::BuildFCALgTowers(std::unique_ptr<gTowerContainer> & gTowerContainerRaw) const
{
  // This is pure EMEC region (for gFEX: Eta Range 3.2-4.9)
  int FCAL_MODIFIER = 16;
  int tmpVal = FCAL_MODIFIER;
  int nphi = 16;
  for (int ieta = tmpVal; ieta < tmpVal + 4; ++ieta){ // loop over eta steps
    for (int iphi = 0; iphi < nphi; ++iphi){ // loop over 16 phi steps
      BuildSingleTower(gTowerContainerRaw, ieta, iphi, nphi, 900000, -1);
      BuildSingleTower(gTowerContainerRaw, ieta, iphi, nphi, 1000000, 1);
    }
    FCAL_MODIFIER++;
  }

}



  void gTowerBuilder::BuildSingleTower(std::unique_ptr<gTowerContainer> & gTowerContainerRaw, int ieta, int iphi, int nphi, int keybase, int posneg) const
{

  gTowerContainerRaw->push_back(ieta, iphi, nphi, keybase, posneg);


}

  void gTowerBuilder::BuildAllTowers(std::unique_ptr<gTowerContainer> & gTowerContainerRaw) const
{
  BuildEMBgTowers(gTowerContainerRaw);
  BuildTransEMBgTowers(gTowerContainerRaw);
  BuildEMEgTowers(gTowerContainerRaw);
  BuildFwdEMECgTowers(gTowerContainerRaw);
  BuildFCALgTowers(gTowerContainerRaw);

}

} // end of LVL1 namespace
