/*
    Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"

#include "xAODTrigL1Calo/TriggerTowerContainer.h"

#include "L1CaloFEXSim/jTower.h"
#include "L1CaloFEXSim/jTowerBuilder.h"

#include "L1CaloFEXSim/jTowerContainer.h"

#include "TROOT.h"
#include "TH1.h"
#include "TH1F.h"
#include "TPad.h"
#include "TCanvas.h"

// TOWER IS A COLLECTION OF SUPER CELLS
// IT SHOULD HAVE A UNIQUE ID
// IT SHOULD BE ABLE TO RETURN LIST OF SUPER CELLS BELONGING TO IT

// THIS IS A CLASS DESIGNED TO BUILD AN JTOWER USING THE JTOWER CLASS AND THEN PRINT THE RELEVANT INFORMATION TO THE SCREEN USING FUNCTION CALLS FROM THE JTOWER CLASS

namespace LVL1 {

jTowerBuilder::jTowerBuilder(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent)
{
  declareInterface<IjTowerBuilder>(this);
}


void jTowerBuilder::init(std::unique_ptr<jTowerContainer> & jTowerContainerRaw) 
{

  execute(jTowerContainerRaw);

}


void jTowerBuilder::reset() 
{

}


void jTowerBuilder::execute(std::unique_ptr<jTowerContainer> & jTowerContainerRaw) 
{
  BuildAllTowers(jTowerContainerRaw);
}
 
  // TOWER IDs FOR CLARITY
  // Left Barrel IETower = 100000 + X
  // Right Barrel IETower = 200000 + X
  // Left Transition ID Tower = 300000 + X;
  // Right Transition ID Tower = 400000 + X;
  // Left Endcap ID Tower = 500000 + X
  // Right Endcap ID Tower = 600000 + X
  // Left Hadronic Endcap ID Tower = 11100000 + X --> These are just Layer 5 of Endcap Towers.  They will never be generated as standalone jTowers.
  // Right Haronic Endcap ID Tower = 22200000 + X --> These are just Layer 5 of Endcap Towers.  They will never be generated as standalone jTowers.

 void jTowerBuilder::BuildEMBjTowers(std::unique_ptr<jTowerContainer> & jTowerContainerRaw) const 
{
  // Regions 0 only.  Region 1 is 'transition region'.
  for (int ieta = 0; ieta < 14; ++ieta) { // loop over 14 eta steps (ignoring last step as it is transition region)
    for (int iphi = 0; iphi < 64; ++iphi){ // loop over 64 phi steps
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, 100000, -1);
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, 200000, 1);
    }
  }

}

void jTowerBuilder::BuildTRANSjTowers(std::unique_ptr<jTowerContainer> & jTowerContainerRaw) const
{

  int TRANS_MODIFIER = 14;
  int tmpVal = TRANS_MODIFIER;

  for (int ieta = tmpVal; ieta < tmpVal + 1; ieta++){ // loop over eta steps
    for (int iphi = 0; iphi < 64; ++iphi){ // loop over 64 phi steps
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, 300000, -1);
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, 400000, 1);
    }
  }

}

  void jTowerBuilder::BuildEMEjTowers(std::unique_ptr<jTowerContainer> & jTowerContainerRaw) const
{
  // Region 1
  int EME_MODIFIER = 15;
  int tmpVal = EME_MODIFIER;

  for (int ieta = tmpVal; ieta < tmpVal + 3; ++ieta){ // loop over eta steps
    for (int iphi = 0; iphi < 64; ++iphi){ // loop over 64 phi steps
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, 500000, -1);
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, 600000, 1);
    }
    EME_MODIFIER++;
  }

  // Region 2
  tmpVal = EME_MODIFIER;
  for (int ieta = tmpVal; ieta < tmpVal + 2; ++ieta){ // loop over eta steps
    for (int iphi = 0; iphi < 64; ++iphi){ // loop over 64 phi steps
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, 500000, -1);
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, 600000, 1);
    }
    EME_MODIFIER++;
  }

  // Region 3
  tmpVal = EME_MODIFIER;
  for (int ieta = tmpVal; ieta < tmpVal + 4; ++ieta){ // loop over eta steps
    for (int iphi = 0; iphi < 64; ++iphi){ // loop over 64 phi steps
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, 500000, -1);
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, 600000, 1);
    }
    EME_MODIFIER++;
  }

  // Region 4
  tmpVal = EME_MODIFIER;
  for (int ieta = tmpVal; ieta < tmpVal + 1; ++ieta){ // loop over eta steps
    for (int iphi = 0; iphi < 64; ++iphi){ // loop over 64 phi steps
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, 500000, -1);
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, 600000, 1);
    }
    EME_MODIFIER++;
  }


}

  void jTowerBuilder::BuildEMIEjTowers(std::unique_ptr<jTowerContainer> & jTowerContainerRaw) const
  {
    int EMEI_MODIFIER = 25;
    int tmpVal = EMEI_MODIFIER;

    for (int ieta = tmpVal; ieta < tmpVal + 25; ++ieta){ // loop over eta steps
      for (int iphi = 0; iphi < 64; ++iphi){ // loop over 64 phi steps
	BuildSingleTower(jTowerContainerRaw, ieta, iphi, 700000, -1);
	BuildSingleTower(jTowerContainerRaw, ieta, iphi, 800000, 1);
      }
      EMEI_MODIFIER++;
    }

  }

// REDUNDANT AND NOT USED, BUT MAY BE REQUIRED IN FUTURE ==========================================================================================================
  void jTowerBuilder::BuildHECjTowers(std::unique_ptr<jTowerContainer> & jTowerContainerRaw) const
{

  // Region 0
  int HEC_MODIFIER = 29;
  int tmpVal = HEC_MODIFIER;
  for (int ieta = tmpVal; ieta < tmpVal + 10; ++ieta){ // loop over eta steps
    for (int iphi = 0; iphi < 64; ++iphi){ // loop over 64 phi steps
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, 11100000, -1);
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, 22200000, 1);
    }
    HEC_MODIFIER++;
  }

  // Region 1
  tmpVal = HEC_MODIFIER;
  for (int ieta = tmpVal; ieta < tmpVal + 4; ++ieta){ // loop over eta steps
    for (int iphi = 0; iphi < 32; ++iphi){ // loop over 64 phi steps
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, 11100000, -1);
      BuildSingleTower(jTowerContainerRaw, ieta, iphi, 22200000, 1);
    }
    HEC_MODIFIER++;
  }

}

void jTowerBuilder::BuildSingleTower(std::unique_ptr<jTowerContainer> & jTowerContainerRaw,float eta, float phi, float keybase, int posneg)  const
{

  jTowerContainerRaw->push_back(eta, phi, keybase, posneg);

}

void jTowerBuilder::BuildAllTowers(std::unique_ptr<jTowerContainer> & jTowerContainerRaw) const
{
  BuildEMBjTowers(jTowerContainerRaw);
  BuildTRANSjTowers(jTowerContainerRaw);
  BuildEMEjTowers(jTowerContainerRaw);
  BuildEMIEjTowers(jTowerContainerRaw);

}

} // end of LVL1 namespace
