/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOFEXSIM_JetAlg_H
#define TRIGT1CALOFEXSIM_JetAlg_H 

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "StoreGate/WriteHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloTriggerTool/JTowerSCMap.h"
#include "CaloTriggerTool/GTowerSCMap.h"
#include "CaloIdentifier/TTOnlineID.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/JTower_ID.h"
#include "CaloIdentifier/GTower_ID.h"
#include "CaloEvent/CaloCellContainer.h"
#include "xAODTrigL1Calo/JGTower.h"
#include "xAODTrigL1Calo/JGTowerContainer.h"
#include "xAODTrigL1Calo/JGTowerAuxContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetInterface/IJetUpdateJvt.h"
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"
#include "Identifier/IdentifierHash.h"
#include "TH1.h"
#include "TH2.h"

class JetAlg{ 

 public:
  struct L1Jet{

    float eta;
    float phi;
    float et;
  // constructor for convenience
    L1Jet(float eta_j, float phi_j, float et_j){
      eta = eta_j;
      phi = phi_j;
      et  = et_j;
    };
  };

  struct Seed{

    std::vector<float> eta;
    std::vector<std::vector<float>> phi;
    std::vector<std::vector<float>> et;
    std::vector<std::vector<bool>> local_max;
  };

  static StatusCode SeedGrid(const xAOD::JGTowerContainer*towers, JetAlg::Seed*seeds);
  static StatusCode SeedFinding(const xAOD::JGTowerContainer*towers, JetAlg::Seed*seeds, float seed_size,float range, std::vector<float> noise);

  static StatusCode BuildJet(const xAOD::JGTowerContainer*towers,std::vector<float> noise, JetAlg::Seed*seeds, std::vector<JetAlg::L1Jet> &js, float jet_size);

};
#endif
