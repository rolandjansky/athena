/*
    Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOFEXSIM_JetAlg_H
#define TRIGT1CALOFEXSIM_JetAlg_H 

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "StoreGate/WriteHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthMessaging.h"

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
  static std::map<TString, std::vector<std::shared_ptr<L1Jet>>> m_JetMap;
  static std::map<TString, std::shared_ptr<Seed>> m_SeedMap;

  static StatusCode SeedGrid(const xAOD::JGTowerContainer*towers, TString seedname, bool &m_dumpSeedsEtaPhi);
  static StatusCode SeedFinding(const xAOD::JGTowerContainer*towers, TString seedname, float seed_size, float range, std::vector<float> noise, float seed_tower_noise_multiplier, float seed_total_noise_multiplier, float seed_min_ET_MeV, bool m_debug = false);

  static StatusCode BuildFatJet(const xAOD::JGTowerContainer towers, TString jetname, float jet_r, std::vector<float> noise, float jet_tower_noise_multiplier, float jet_total_noise_multiplier, float jet_min_ET_MeV); 
  static StatusCode BuildJet(const xAOD::JGTowerContainer*towers, TString seedname, TString jetname, float jet_size, std::vector<float> noise, float jet_tower_noise_multiplier, float jet_total_noise_multiplier, float jet_min_ET_MeV, bool m_debug = false, bool m_saveSeeds = false);
  static StatusCode BuildRoundJet(const xAOD::JGTowerContainer*towers, TString seedname, TString jetname, float jet_size, std::vector<float> noise, float jet_tower_noise_multiplier, float jet_total_noise_multiplier, float jet_min_ET_MeV, bool m_debug = false, bool m_saveSeeds = false);

};
#endif
