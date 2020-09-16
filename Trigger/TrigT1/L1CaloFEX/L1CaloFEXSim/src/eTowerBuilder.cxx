/*
    Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"

#include "xAODTrigL1Calo/TriggerTowerContainer.h"

#include "L1CaloFEXSim/eTower.h"
#include "L1CaloFEXSim/eTowerBuilder.h"

#include "L1CaloFEXSim/eTowerContainer.h"

#include "TROOT.h"
#include "TH1.h"
#include "TH1F.h"
#include "TPad.h"
#include "TCanvas.h"

// TOWER IS A COLLECTION OF SUPER CELLS
// IT SHOULD HAVE A UNIQUE ID
// IT SHOULD BE ABLE TO RETURN LIST OF SUPER CELLS BELONGING TO IT

// THIS IS A CLASS DESIGNED TO BUILD AN ETOWER USING THE ETOWER CLASS AND THEN PRINT THE RELEVANT INFORMATION TO THE SCREEN USING FUNCTION CALLS FROM THE ETOWER CLASS

namespace LVL1 {

eTowerBuilder::eTowerBuilder(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent)
{
  declareInterface<IeTowerBuilder>(this);
}

eTowerBuilder::~eTowerBuilder()
{
}


void eTowerBuilder::init(std::unique_ptr<eTowerContainer> & eTowerContainerRaw) 
{

  execute(eTowerContainerRaw);
  return;
}


void eTowerBuilder::reset() 
{
  return;
}


void eTowerBuilder::execute(std::unique_ptr<eTowerContainer> & eTowerContainerRaw) 
{
  BuildAllTowers(eTowerContainerRaw);
  return;
}
 
  // TOWER IDs FOR CLARITY
  // Left Barrel IETower = 100000 + X
  // Right Barrel IETower = 200000 + X
  // Left Transition ID Tower = 300000 + X;
  // Right Transition ID Tower = 400000 + X;
  // Left Endcap ID Tower = 500000 + X
  // Right Endcap ID Tower = 600000 + X
  // Left Hadronic Endcap ID Tower = 11100000 + X --> These are just Layer 5 of Endcap Towers.  They will never be generated as standalone eTowers.
  // Right Haronic Endcap ID Tower = 22200000 + X --> These are just Layer 5 of Endcap Towers.  They will never be generated as standalone eTowers.

 void eTowerBuilder::BuildEMBeTowers(std::unique_ptr<eTowerContainer> & eTowerContainerRaw) 
{
  // Regions 0 only.  Region 1 is 'transition region'.
  for (int ieta = 0; ieta < 14; ++ieta) { // loop over 14 eta steps (ignoring last step as it is transition region)
    for (int iphi = 0; iphi < 64; ++iphi){ // loop over 64 phi steps
      BuildSingleTower(eTowerContainerRaw, ieta, iphi, 100000, -1);
      BuildSingleTower(eTowerContainerRaw, ieta, iphi, 200000, 1);
    }
  }

  return;
}

void eTowerBuilder::BuildTRANSeTowers(std::unique_ptr<eTowerContainer> & eTowerContainerRaw) 
{

  int TRANS_MODIFIER = 14;
  int tmpVal = TRANS_MODIFIER;

  for (int ieta = tmpVal; ieta < tmpVal + 1; ieta++){ // loop over eta steps
    for (int iphi = 0; iphi < 64; ++iphi){ // loop over 64 phi steps
      BuildSingleTower(eTowerContainerRaw, ieta, iphi, 300000, -1);
      BuildSingleTower(eTowerContainerRaw, ieta, iphi, 400000, 1);
    }
  }

  return;
}

  void eTowerBuilder::BuildEMEeTowers(std::unique_ptr<eTowerContainer> & eTowerContainerRaw) 
{
  // Region 1
  int EME_MODIFIER = 15;
  int tmpVal = EME_MODIFIER;

  for (int ieta = tmpVal; ieta < tmpVal + 3; ++ieta){ // loop over eta steps
    for (int iphi = 0; iphi < 64; ++iphi){ // loop over 64 phi steps
      BuildSingleTower(eTowerContainerRaw, ieta, iphi, 500000, -1);
      BuildSingleTower(eTowerContainerRaw, ieta, iphi, 600000, 1);
    }
    EME_MODIFIER++;
  }

  // Region 2
  tmpVal = EME_MODIFIER;
  for (int ieta = tmpVal; ieta < tmpVal + 2; ++ieta){ // loop over eta steps
    for (int iphi = 0; iphi < 64; ++iphi){ // loop over 64 phi steps
      BuildSingleTower(eTowerContainerRaw, ieta, iphi, 500000, -1);
      BuildSingleTower(eTowerContainerRaw, ieta, iphi, 600000, 1);
    }
    EME_MODIFIER++;
  }

  // Region 3
  tmpVal = EME_MODIFIER;
  for (int ieta = tmpVal; ieta < tmpVal + 4; ++ieta){ // loop over eta steps
    for (int iphi = 0; iphi < 64; ++iphi){ // loop over 64 phi steps
      BuildSingleTower(eTowerContainerRaw, ieta, iphi, 500000, -1);
      BuildSingleTower(eTowerContainerRaw, ieta, iphi, 600000, 1);
    }
    EME_MODIFIER++;
  }

  // Region 4
  tmpVal = EME_MODIFIER;
  for (int ieta = tmpVal; ieta < tmpVal + 1; ++ieta){ // loop over eta steps
    for (int iphi = 0; iphi < 64; ++iphi){ // loop over 64 phi steps
      BuildSingleTower(eTowerContainerRaw, ieta, iphi, 500000, -1);
      BuildSingleTower(eTowerContainerRaw, ieta, iphi, 600000, 1);
    }
    EME_MODIFIER++;
  }

  return;

}

// REDUNDANT AND NOT USED==========================================================================================================
  void eTowerBuilder::BuildHECeTowers(std::unique_ptr<eTowerContainer> & eTowerContainerRaw) 
{

  // Region 0
  int HEC_MODIFIER = 29;
  int tmpVal = HEC_MODIFIER;
  for (int ieta = tmpVal; ieta < tmpVal + 10; ++ieta){ // loop over eta steps
    for (int iphi = 0; iphi < 64; ++iphi){ // loop over 64 phi steps
      BuildSingleTower(eTowerContainerRaw, ieta, iphi, 11100000, -1);
      BuildSingleTower(eTowerContainerRaw, ieta, iphi, 22200000, 1);
    }
    HEC_MODIFIER++;
  }

  // Region 1
  tmpVal = HEC_MODIFIER;
  for (int ieta = tmpVal; ieta < tmpVal + 4; ++ieta){ // loop over eta steps
    for (int iphi = 0; iphi < 32; ++iphi){ // loop over 64 phi steps
      BuildSingleTower(eTowerContainerRaw, ieta, iphi, 11100000, -1);
      BuildSingleTower(eTowerContainerRaw, ieta, iphi, 22200000, 1);
    }
    HEC_MODIFIER++;
  }

  return;

}

void eTowerBuilder::BuildSingleTower(std::unique_ptr<eTowerContainer> & eTowerContainerRaw,float eta, float phi, float keybase, int posneg) 
{

  eTowerContainerRaw->push_back(eta, phi, keybase, posneg);

  return;

}

void eTowerBuilder::BuildAllTowers(std::unique_ptr<eTowerContainer> & eTowerContainerRaw) 
{
  BuildEMBeTowers(eTowerContainerRaw);
  BuildTRANSeTowers(eTowerContainerRaw);
  BuildEMEeTowers(eTowerContainerRaw);
  return;

}

} // end of LVL1 namespace
