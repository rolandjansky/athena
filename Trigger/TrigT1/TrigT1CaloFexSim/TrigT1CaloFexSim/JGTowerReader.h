/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOFEXSIM_JGTOWERREADER_H
#define TRIGT1CALOFEXSIM_JGTOWERREADER_H 

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "StoreGate/WriteHandle.h"
#include "GaudiKernel/ITHistSvc.h"
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
#include "TrigT1CaloFexSim/Objects.h"
#include "TrigT1CaloFexSim/JetAlg.h"
#include "TrigT1CaloFexSim/METAlg.h"
#include "TrigT1CaloFexSim/JGTower.h"
#include "string.h"
std::vector<std::string> splitString(std::string parentString, std::string sep, bool stripEmpty=false);
class JGTowerReader: public ::AthAlgorithm { 
 public: 
  JGTowerReader( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~JGTowerReader(); 
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();
  virtual StatusCode  beginInputFile();
  virtual StatusCode  ProcessObjects();
  virtual StatusCode  HistBookFill(const TString name, Int_t nbinsx, Double_t xbin_down, Double_t xbin_up, float xvalue, float wei);
  virtual StatusCode  HistBookFill(const TString name, Int_t nbinsx, const Double_t* xbins, float xvalue,float wei);
  static StatusCode BuildBlocksFromTowers(std::vector<TowerObject::Block>& blocks, const xAOD::JGTowerContainer towers, const int blockRows, const int blockCols, const bool useNegTowers);
 private: 
  bool m_vetoBCID;
  bool m_outputNoise;
  std::string m_scType; 
  bool m_debugJetAlg;
  bool m_dumpTowerInfo;
  bool m_dumpSeedsEtaPhi;
  bool m_makeRoundJetsPUsub;

  bool  m_makeSquareJets;
  bool  m_buildgBlockJets;
  float m_jJet_thr;
  float m_jJet_seed_size;
  float m_jJet_max_r;
  float m_jJet_r;
  float m_jJet_seed_tower_noise_multiplier;
  float m_jJet_seed_total_noise_multiplier;
  float m_jJet_seed_min_ET_MeV;
  float m_jJet_jet_tower_noise_multiplier;
  float m_jJet_jet_total_noise_multiplier;
  float m_jJet_jet_min_ET_MeV;

  bool  m_makeRoundJets;
  float m_jJetRound_seed_size;
  float m_jJetRound_max_r;
  float m_jJetRound_r;
  float m_jJetRound_seed_tower_noise_multiplier;
  float m_jJetRound_seed_total_noise_multiplier;
  float m_jJetRound_seed_min_ET_MeV;
  float m_jJetRound_jet_tower_noise_multiplier;
  float m_jJetRound_jet_total_noise_multiplier;
  float m_jJetRound_jet_min_ET_MeV;

  bool  m_makeRoundLargeRJets;
  float m_jJetRound_LargeR_seed_size;
  float m_jJetRound_LargeR_max_r;
  float m_jJetRound_LargeR_r;
  float m_jJetRound_LargeR_seed_min_ET_MeV;
  float m_jJetRound_LargeR_jet_min_ET_MeV;

  bool m_makeJetsFromMap;
  std::string m_towerMap;
  float m_map_seed_tower_noise_multiplier;
  float m_map_seed_total_noise_multiplier;
  float m_map_seed_min_ET_MeV;
  float m_map_jet_tower_noise_multiplier;
  float m_map_jet_total_noise_multiplier;
  float m_map_jet_min_ET_MeV;

  bool m_plotSeeds;
  bool m_saveSeeds;

  float m_gJet_r;
  float m_gJet_block_tower_noise_multiplier; 
  float m_gJet_block_min_ET_MeV;
  float m_gJet_tower_noise_multiplier;
  float m_gJet_jet_min_ET_MeV;
  float m_gFEX_pTcone_cut;
  bool  m_gFEX_OnlyPosRho; 
  bool  m_gFEX_useNegTowers;
  bool  m_gFEX_Rho_useNegTowers; 
  //job options for gFEX MET algorithms                                          
  bool m_useRMS;
  bool m_useMedian;

  std::string m_noise_file;
  
  std::string m_gXERHOLUT_file;
  std::string m_jXERHO_correction_file;
  float  m_jXERHO_fixed_noise_cut;  
  float  m_jXERHO_rho_up_threshold;
  float  m_jXERHO_min_noise_cut;
 
  const CaloCell_SuperCell_ID* m_scid;
  const JTower_ID* m_jTowerId;
  const GTower_ID* m_gTowerId;
 
  virtual StatusCode ReadTowerMap();
  virtual StatusCode CheckTowerMap(const xAOD::JGTowerContainer*jTs);
  virtual StatusCode DumpTowerInfo(const xAOD::JGTowerContainer*jTs, const CaloCellContainer* scells);
  virtual StatusCode JFexAlg(const xAOD::JGTowerContainer*jTs);
  virtual StatusCode GFexAlg(const xAOD::JGTowerContainer*gTs); 
  virtual StatusCode BuildJetsFromMap(const xAOD::JGTowerContainer*jTs);

  std::vector<float> jT_noise;
  std::vector<float> jTowerArea;
  std::vector < std::vector < int > > jFEX_bins;
  std::vector < std::vector < int > > jFEX_bins_core;
  bool buildbins=false;
  std::vector<float> jJet_thr;
  std::vector<float> jJet_jet_thr;
  std::vector<float> gT_noise;
  std::vector<float> gJet_thr;
  ServiceHandle<ITHistSvc> histSvc;
  std::vector<TString> hists;

  // tower map entries
  int totalTowers = 0;
  int towersInMap = 0;
  std::vector<float> towerMap_towerNumber;
  std::vector<float> towerMap_towerEta;
  std::vector<float> towerMap_towerPhi;
  std::vector<int> towerMap_towerSampling;
  std::vector< std::vector<int> > towerMap_towerLayers;

  int totalSeeds = 0;
  int seedsInMap = 0;
  std::vector<float> towerMap_seedEta;
  std::vector<float> towerMap_seedPhi;
  std::vector< std::vector<int> > towerMap_seedTowers;
  std::vector< std::vector<int> > towerMap_seedLocalMaxSeeds;

  int totalJets = 0;
  int jetsInMap = 0;
  std::vector<float> towerMap_jetEta;
  std::vector<float> towerMap_jetPhi;
  std::vector<int> towerMap_jetSeed;
  std::vector< std::vector<int> > towerMap_jetTowers;

  std::vector<int> towerMap_AODtowersIndices;

  std::vector<float> towerMapSeed_ET;
  std::vector<bool> towerMapSeed_isLocalMax;

  JetAlg::Seed*   jSeeds=new JetAlg::Seed;
  JetAlg::Seed*   jJetSeeds = new JetAlg::Seed;
  JetAlg::Seed*   gSeeds=new JetAlg::Seed;

  std::vector<JetAlg::L1Jet>  jL1Jets;
  std::vector<JetAlg::L1Jet>  jJet_L1Jets;
  std::vector<JetAlg::L1Jet>  gL1Jets;
  std::map<TString, TH1*> m_hName;
  std::map<TString, TH2*> m_h2Name;
  int m_jTowerHashMax;
  int m_gTowerHashMax;
  std::shared_ptr<TowerHelper> jT_helper;
  std::shared_ptr<TowerHelper> gT_helper;

  unsigned int m_eventCount = 0;
  SG::AuxElement::Accessor<float>* acc_rho_barrel = new SG::AuxElement::Accessor<float>("Rho_barrel");
  SG::AuxElement::Accessor<float>* acc_rhoA = new SG::AuxElement::Accessor<float>("RhoA");
  SG::AuxElement::Accessor<float>* acc_rhoB = new SG::AuxElement::Accessor<float>("RhoB"); 
  SG::AuxElement::Accessor<float>* acc_rhoC = new SG::AuxElement::Accessor<float>("RhoC");

  SG::AuxElement::Accessor<float>* acc_posRhoA = new SG::AuxElement::Accessor<float>("RhoA_positive_gTowers");
  SG::AuxElement::Accessor<float>* acc_posRhoB = new SG::AuxElement::Accessor<float>("RhoB_positive_gTowers"); 
  SG::AuxElement::Accessor<float>* acc_posRhoC = new SG::AuxElement::Accessor<float>("RhoC_positive_gTowers");
  
  SG::AuxElement::Accessor<float>* acc_threshA = new SG::AuxElement::Accessor<float>("ThreshA");
  SG::AuxElement::Accessor<float>* acc_threshB = new SG::AuxElement::Accessor<float>("ThreshB");  
  SG::AuxElement::Accessor<float>* acc_threshC = new SG::AuxElement::Accessor<float>("ThreshC");
  
  SG::AuxElement::Accessor<float>* acc_mht = new SG::AuxElement::Accessor<float>("MHT");
  SG::AuxElement::Accessor<float>* acc_mst = new SG::AuxElement::Accessor<float>("MST");
}; 

#endif //> !TRIGT1CALOFEXSIM_JGTOWERREADER_H
