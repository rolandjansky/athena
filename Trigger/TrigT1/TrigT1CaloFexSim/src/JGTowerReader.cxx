/* *   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// TrigT1CaloFexSim includes
#include "TrigT1CaloFexSim/JGTowerReader.h"
#include "TrigT1CaloFexSim/JGTower.h"
#include "TrigT1CaloFexSim/JetAlg.h"
#include "TrigT1CaloFexSim/Objects.h"
#include "TSystem.h"
#include "xAODEventInfo/EventInfo.h"
#include "TTree.h"
#include "TH1D.h"
#include "iostream"
#include <sstream>
#include <fstream>
#include <TMath.h>
#include "StoreGate/ReadHandle.h"
#include "xAODEventInfo/EventInfo.h"
#include "TrigT1CaloFexSim/JGTowerMaker.h"
#include "LArElecCalib/LArCalibErrorCode.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloTriggerTool/LArTTCell.h"
#include "CaloTriggerTool/GTowerSCMap.h"
#include "CaloTriggerTool/JTowerSCMap.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "xAODTrigL1Calo/JGTowerContainer.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODTrigger/JetRoIAuxContainer.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODTrigger/JetRoI.h"
#include "xAODTrigger/EnergySumRoI.h"
#include "xAODTrigger/EnergySumRoIAuxInfo.h"
#include "TFile.h"
#include "PathResolver/PathResolver.h"
#include "AthContainers/DataVector.h"
#include "AthContainers/ConstDataVector.h"

#include "CaloTriggerTool/GTowerSCMap.h"
#include "CaloTriggerTool/JTowerSCMap.h"


JGTowerReader::JGTowerReader( const std::string& name, ISvcLocator* pSvcLocator ) : 
AthAlgorithm( name, pSvcLocator ),
histSvc("THistSvc",name){

  declareProperty("outputNoise",m_outputNoise=false);
  declareProperty("debugJetAlg", m_debugJetAlg=false);
  declareProperty("SuperCellType",m_scType="SCell");
  declareProperty("dumpTowerInfo", m_dumpTowerInfo=false);
  declareProperty("dumpSeedsEtaPhi", m_dumpSeedsEtaPhi=false);
  declareProperty("noise_file", m_noise_file="Run3L1CaloSimulation/Noise/noise_r10684.root");
  declareProperty("makeRoundJetsPUsub", m_makeRoundJetsPUsub=false);

  declareProperty("makeSquareJets", m_makeSquareJets = true);
  declareProperty("jJet_seed_size", m_jJet_seed_size=0.3);
  declareProperty("jJet_max_r", m_jJet_max_r=0.4);
  declareProperty("jJet_r", m_jJet_r=0.4);
  declareProperty("jJet_seed_tower_noise_multiplier", m_jJet_seed_tower_noise_multiplier = 1.0);
  declareProperty("jJet_seed_total_noise_multiplier", m_jJet_seed_total_noise_multiplier = 1.0);
  declareProperty("jJet_seed_min_ET_MeV", m_jJet_seed_min_ET_MeV = 2000.0);
  declareProperty("jJet_jet_tower_noise_multiplier", m_jJet_jet_tower_noise_multiplier = 1.0);
  declareProperty("jJet_jet_total_noise_multiplier", m_jJet_jet_total_noise_multiplier = 0.0);
  declareProperty("jJet_jet_min_ET_MeV", m_jJet_jet_min_ET_MeV = 2000.0);

  declareProperty("makeRoundJets", m_makeRoundJets = false); 
  declareProperty("jJetRound_seed_size", m_jJetRound_seed_size=0.3);
  declareProperty("jJetRound_max_r", m_jJetRound_max_r=0.4);
  declareProperty("jJetRound_r", m_jJetRound_r=0.4);
  declareProperty("jJetRound_seed_tower_noise_multiplier", m_jJetRound_seed_tower_noise_multiplier = 1.0);
  declareProperty("jJetRound_seed_total_noise_multiplier", m_jJetRound_seed_total_noise_multiplier = 1.0);
  declareProperty("jJetRound_seed_min_ET_MeV", m_jJetRound_seed_min_ET_MeV = 2000.0);
  declareProperty("jJetRound_jet_tower_noise_multiplier", m_jJetRound_jet_tower_noise_multiplier = 1.0);
  declareProperty("jJetRound_jet_total_noise_multiplier", m_jJetRound_jet_total_noise_multiplier = 0.0);
  declareProperty("jJetRound_jet_min_ET_MeV", m_jJetRound_jet_min_ET_MeV = 2000.0);

  declareProperty("makeRoundLargeRJets", m_makeRoundLargeRJets = false); 
  declareProperty("jJetRound_LargeR_seed_size", m_jJetRound_LargeR_seed_size=0.3);
  declareProperty("jJetRound_LargeR_max_r", m_jJetRound_LargeR_max_r=0.4);
  declareProperty("jJetRound_LargeR_r", m_jJetRound_LargeR_r=0.8);
  declareProperty("jJetRound_LargeR_seed_min_ET_MeV", m_jJetRound_LargeR_seed_min_ET_MeV = 2000.0);
  declareProperty("jJetRound_LargeR_jet_min_ET_MeV",  m_jJetRound_LargeR_jet_min_ET_MeV = 2000.0);
  
  declareProperty("makeJetsFromMap", m_makeJetsFromMap = false);
  declareProperty("towerMap", m_towerMap = "");
  declareProperty("map_seed_tower_noise_multiplier", m_map_seed_tower_noise_multiplier = 1.0);
  declareProperty("map_seed_total_noise_multiplier", m_map_seed_total_noise_multiplier = 1.0);
  declareProperty("map_seed_min_ET_MeV", m_map_seed_min_ET_MeV = 2000.0);
  declareProperty("map_jet_tower_noise_multiplier", m_map_jet_tower_noise_multiplier = 1.0);
  declareProperty("map_jet_total_noise_multiplier", m_map_jet_total_noise_multiplier = 0.0);
  declareProperty("map_jet_min_ET_MeV", m_map_jet_min_ET_MeV = 2000.0);

  declareProperty("plotSeeds", m_plotSeeds = false);
  declareProperty("saveSeeds", m_saveSeeds = false);

  declareProperty("buildgBlockJets", m_buildgBlockJets=false);

  declareProperty("gJet_r", m_gJet_r=1.0);
  declareProperty("gJet_block_tower_noise_multiplier", m_gJet_block_tower_noise_multiplier = 0.0);
  declareProperty("gJet_block_min_ET_MeV", m_gJet_block_min_ET_MeV = 0.0);
  declareProperty("gJet_tower_noise_multiplier", m_gJet_tower_noise_multiplier = 0.0);
  declareProperty("gJet_jet_min_ET_MeV", m_gJet_jet_min_ET_MeV = 2000.0);

  declareProperty("useRMS", m_useRMS=true);
  declareProperty("useMedian", m_useMedian=false);
  declareProperty("gFEX_useNegTowers", m_gFEX_useNegTowers=true);
  declareProperty("gFEX_Rho_useNegTowers",m_gFEX_Rho_useNegTowers=true);
  declareProperty("gFEX_OnlyPosRho", m_gFEX_OnlyPosRho=true); 
  declareProperty("gFEX_pTcone_cut", m_gFEX_pTcone_cut=25);  //cone threshold for Jets without Jets: declared in GeV
  declareProperty("gXERHOLUT_file", m_gXERHOLUT_file="Run3L1CaloSimulation/Noise/gTowerNoisevsRho.20210105.MiddleTrainNoNoiseCut.r11881.root");

  declareProperty("jXERHO_correction_file"  , m_jXERHO_correction_file="Run3L1CaloSimulation/Noise/jTowerCorrection.20200510.r11881.root");  //correction file for jXERHO
  declareProperty("jXERHO_fixed_noise_cut"  , m_jXERHO_fixed_noise_cut=0.0);  
  declareProperty("jXERHO_rho_up_threshold" , m_jXERHO_rho_up_threshold=1000.0);
  declareProperty("jXERHO_min_noise_cut"    , m_jXERHO_min_noise_cut=100.0);  
}


JGTowerReader::~JGTowerReader() {
  delete jSeeds;
  delete jJetSeeds;
  delete gSeeds;
  delete acc_rhoA; 
  delete acc_rhoB;
  delete acc_rhoC;
  delete acc_rho_barrel; 
  delete acc_threshA;
  delete acc_threshB;
  delete acc_threshC;
  
  delete acc_mht;
  delete acc_mst;

  METAlg::m_METMap.clear();
  jL1Jets.clear();
  jJet_L1Jets.clear();
  gL1Jets.clear();
  
}


StatusCode JGTowerReader::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");

  CHECK( histSvc.retrieve() );
  CHECK(detStore()->retrieve(m_jTowerId));
  CHECK(detStore()->retrieve(m_gTowerId));

  std::string fullPathToNoise = PathResolverFindCalibFile(m_noise_file);
  std::ifstream noise_exist(fullPathToNoise.c_str());
  if(noise_exist){
    ATH_MSG_INFO ("Noise is set with root file:" << fullPathToNoise << "...");
    TFile*noise = new TFile(fullPathToNoise.c_str());
    TH1F*jh_noise = (TH1F*)noise->Get("jT_noise");
    TH1F*gh_noise = (TH1F*)noise->Get("gT_noise");
  
    for(int i=0;i<jh_noise->GetSize();i++){
       jT_noise.push_back( jh_noise->GetBinContent(i+1) );
    }
  
    for(int i=0;i<gh_noise->GetSize();i++){
       gT_noise.push_back( gh_noise->GetBinContent(i+1) );
    } 
  }
  
  std::string fullPathTo_gXERHOLUT_file = PathResolverFindCalibFile(m_gXERHOLUT_file);
  std::ifstream gXERHOLUT_exist(fullPathTo_gXERHOLUT_file.c_str());

  if(gXERHOLUT_exist) m_gXERHOLUT_file = fullPathTo_gXERHOLUT_file;

  std::string fullPathTo_jXERHO_correction_file = PathResolverFindCalibFile(m_jXERHO_correction_file);
  std::ifstream jXERHO_correction_exist(fullPathTo_jXERHO_correction_file.c_str());
  jTowerArea.clear();
  jTowerArea.resize(7712,1.0);
  if(jXERHO_correction_exist){
      ATH_MSG_INFO ("jXERHO_correction_file is set with root file:" << fullPathTo_jXERHO_correction_file << "...");
      TFile *correctionfile = new TFile(fullPathTo_jXERHO_correction_file.c_str());
      TH1F *jTowerArea_final_hist = (TH1F*)correctionfile->Get("jTowerArea_final_hist");
      for (size_t i=0; i < 7712; i++) {
          jTowerArea[i]=jTowerArea_final_hist->GetBinContent(i+1);
          if (jTowerArea[i] == 0) {
                ATH_MSG_DEBUG ("jTowerArea should never be zero, this concerns tower "<<i);
          }
      }

      float renormfactor=1.0/jTowerArea[0];

      for (unsigned int i=0; i<jTowerArea.size(); i ++) {
          jTowerArea[i]= jTowerArea[i]*renormfactor;
      }
  } else {
      ATH_MSG_WARNING ("jXERHO_correction_file is NOT set to "<< m_jXERHO_correction_file << " full path: "<< fullPathTo_jXERHO_correction_file << "...");
  }
  
  // read in the tower map
  if(m_makeJetsFromMap) {
    CHECK( ReadTowerMap() );
  }

  acc_rhoA = new SG::AuxElement::Accessor<float>("RhoA");
  acc_rhoB = new SG::AuxElement::Accessor<float>("RhoB");
  acc_rhoC = new SG::AuxElement::Accessor<float>("RhoC");
  acc_rho_barrel = new SG::AuxElement::Accessor<float>("Rho_barrel");
  
  acc_threshA = new SG::AuxElement::Accessor<float>("ThreshA");
  acc_threshB = new SG::AuxElement::Accessor<float>("ThreshB");
  acc_threshC = new SG::AuxElement::Accessor<float>("ThreshC");

  acc_mht = new SG::AuxElement::Accessor<float>("MHT");
  acc_mst = new SG::AuxElement::Accessor<float>("MST");

  //Here is to set up the tower helper which returns the tower eta/phi indices
  //Define the histograms for each region
  std::vector<TH2F*> hs_jT;
  TH2F* j_h1=new TH2F("j_h1","j_h1",1,-3.2,-3.1,32,-TMath::Pi(),TMath::Pi());
  TH2F* j_h2=new TH2F("j_h2","j_h2",3,-3.1,-2.5,32,-TMath::Pi(),TMath::Pi());
  TH2F* j_h3=new TH2F("j_h3","j_h3",50,-2.5, 2.5,64,-TMath::Pi(),TMath::Pi());
  TH2F* j_h4=new TH2F("j_h4","j_h4",3,2.5,3.1,32,-TMath::Pi(),TMath::Pi());
  TH2F* j_h5=new TH2F("j_h5","j_h5",1,3.1,3.2,32,-TMath::Pi(),TMath::Pi());
  hs_jT.push_back(j_h1);
  hs_jT.push_back(j_h2);
  hs_jT.push_back(j_h3);
  hs_jT.push_back(j_h4);
  hs_jT.push_back(j_h5);
  jT_helper = std::make_shared<TowerHelper>(hs_jT);


  std::vector<TH2F*> hs_gT;
  double fgT_Eta_p[5] = {3.2, 3.5, 4.0, 4.45,4.9};
  double fgT_Eta_n[5] = {-4.9, -4.45, -4.0, -3.5,-3.2};
  double gT_barrel[35]={-3.2,-3.1,-2.9,-2.7,-2.5,-2.4,-2.2,-2.0,-1.8,-1.6,-1.4,-1.2,-1.0,-0.8,-0.6,-0.4,-0.2,0.0,
                  0.2,0.4,0.6,0.8,1.0,1.2,1.4,1.6,1.8,2.0,2.2,2.4,2.5,2.7,2.9,3.1,3.2};
  TH2F* g_h1=new TH2F("g_h1","g_h1",4 , fgT_Eta_p,17,-TMath::Pi(),TMath::Pi());  
  TH2F* g_h2=new TH2F("g_h2","g_h2",34, gT_barrel,32,-TMath::Pi(),TMath::Pi());
  TH2F* g_h3=new TH2F("g_h3","g_h3",4 , fgT_Eta_n,17,-TMath::Pi(),TMath::Pi());
  hs_gT.push_back(g_h1);
  hs_gT.push_back(g_h2);
  hs_gT.push_back(g_h3);
  gT_helper = std::make_shared<TowerHelper>(hs_gT);

  return StatusCode::SUCCESS;
}

StatusCode JGTowerReader::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode JGTowerReader::execute() {  
  
  if(m_eventCount % 100 == 0){
    ATH_MSG_INFO ("Executing " << name() << " on event " << m_eventCount);
  }
  else if(m_eventCount % 10 == 0){
    ATH_MSG_INFO ("    Executing " << name() << " on event " << m_eventCount);
  }
  else {
    ATH_MSG_DEBUG ("Executing " << name() << " on event " << m_eventCount);
  }
  m_eventCount += 1;

  setFilterPassed(false); //optional: start with algorithm not passed

  const xAOD::EventInfo* eventInfo = 0;
  CHECK( evtStore()->retrieve( eventInfo, "EventInfo" ) );

  ToolHandle<Trig::IBunchCrossingTool> m_bcTool("Trig::MCBunchCrossingTool/BunchCrossingTool");
  //int distFrontBunchTrain = m_bcTool->distanceFromFront(eventInfo->bcid(), Trig::IBunchCrossingTool::BunchCrossings);
  //CHECK(HistBookFill("distFrontBunchTrain",100,0,100, distFrontBunchTrain, 1.));

  const CaloCellContainer* scells = 0;
  CHECK( evtStore()->retrieve( scells, m_scType) );

  const xAOD::JGTowerContainer* jTowers =0;
  CHECK( evtStore()->retrieve( jTowers,"JTower"));
  //when noise file is not available, set the noise as constant for EM, Had, and FCal respectively
  if(jT_noise.size()==0){
    ATH_MSG_INFO (" Failed to find noise file, setting jTower noise manually ");
    for( const auto &jT : *jTowers){
       if(jT->sampling()==0) jT_noise.push_back(430);
       else if(jT->sampling()==1) jT_noise.push_back(2400);
       else jT_noise.push_back(2000);
    }
  }

  const xAOD::JGTowerContainer* gTowers =0;
  CHECK( evtStore()->retrieve( gTowers,"GTower"));

  //when noise file is not available, set the noise as constant for EM, Had, and FCal respectively
  if(gT_noise.size()==0){
    ATH_MSG_INFO (" Failed to find noise file, setting gTower noise manually ");
    for(unsigned int i = 0; i < gTowers->size(); i++) {
      gT_noise.push_back(1500);
    }
  }


  ATH_MSG_DEBUG ("Successfully retrieved cells("<< scells->size() << "), jTowers("<< jTowers->size() <<") and gTowers (" << gTowers->size() << ")");

  // make and check tower mapping
  if(m_eventCount==1 && m_dumpTowerInfo) {
    CHECK( DumpTowerInfo(jTowers, scells) );
  }
  if(m_eventCount==1 && m_makeJetsFromMap) {
    CHECK( CheckTowerMap(jTowers) );
  }

  if(m_outputNoise){

    for(const auto &jT : *jTowers){
       //===============Test Block================
       int ieta = jT_helper->iEta(jT->eta());
       int iphi = jT_helper->iPhi(jT->eta(), jT->phi());
       ATH_MSG_DEBUG(jT->eta()<<"("<<ieta<<")"<<" "<<jT->phi()<<"("<<iphi<<")");
       if(fabs(jT->et())<0.1) continue;
       CHECK(HistBookFill(Form("jTNoise/jT%d",jT->Id()), 5000, -1500, 3500, jT->et(),1));
       if(jT->sampling()==0){
         if(fabs(jT->eta())<2.5)  CHECK(HistBookFill("jTNoise_regions/region0_em", 5000, -1500, 3500, jT->et(),1));
         else if(fabs(jT->eta())<3.1)  CHECK(HistBookFill("jTNoise_regions/region1_em", 5000, -1500, 3500, jT->et(),1));
         else if(fabs(jT->eta())<3.2)  CHECK(HistBookFill("jTNoise_regions/region2_em", 5000, -1500, 3500, jT->et(),1));
       }
       else if(jT->sampling()==1){
         if(fabs(jT->eta())<2.5)  CHECK(HistBookFill("jTNoise_regions/region0_had", 5000, -1500, 3500, jT->et(),1));
         else if(fabs(jT->eta())<3.1)  CHECK(HistBookFill("jTNoise_regions/region1_had", 5000, -1500, 3500, jT->et(),1));
         else if(fabs(jT->eta())<3.2)  CHECK(HistBookFill("jTNoise_regions/region2_had", 5000, -1500, 3500, jT->et(),1));
       }

       else CHECK(HistBookFill("jTNoise_regions/region3", 5000, -1500, 3500, jT->et(),1));
    }

    for(const auto &gT : *gTowers){
       if(fabs(gT->et())<0.1) continue;
       CHECK(HistBookFill(Form("gTNoise/gT%d",gT->Id()), 7000, -1500, 5500, gT->et(),1));
       if(gT->sampling()==0){
         if(fabs(gT->eta())<2.4)  CHECK(HistBookFill("gTNoise_regions/region0_em", 7000, -1500, 5500, gT->et(),1));
         else if(fabs(gT->eta())<2.5)  CHECK(HistBookFill("gTNoise_regions/region1_em", 7000, -1500, 5500, gT->et(),1));
         else if(fabs(gT->eta())<3.1)  CHECK(HistBookFill("gTNoise_regions/region2_em", 7000, -1500, 5500, gT->et(),1));
         else if(fabs(gT->eta())<3.2)  CHECK(HistBookFill("gTNoise_regions/region3_em", 7000, -1500, 5500, gT->et(),1));
       }
       else if(gT->sampling()==1){
         if(fabs(gT->eta())<2.4)  CHECK(HistBookFill("gTNoise_regions/region0_had", 7000, -1500, 5500, gT->et(),1));
         else if(fabs(gT->eta())<2.5)  CHECK(HistBookFill("gTNoise_regions/region1_had", 7000, -1500, 5500, gT->et(),1));
         else if(fabs(gT->eta())<3.1)  CHECK(HistBookFill("gTNoise_regions/region2_had", 7000, -1500, 5500, gT->et(),1));
         else if(fabs(gT->eta())<3.2)  CHECK(HistBookFill("gTNoise_regions/region3_had", 7000, -1500, 5500, gT->et(),1));
       }
       else CHECK(HistBookFill("gTNoise_regions/region4", 7000, -1500, 5500, gT->et(),1));
    }

  }
  ATH_MSG_DEBUG ("About to call JFexAlg");
  CHECK(JFexAlg(jTowers)); // all the functions for JFex shall be executed here
  ATH_MSG_DEBUG ("About to call GFexAlg");
  CHECK(GFexAlg(gTowers)); // all the functions for GFex shall be executed here

  ATH_MSG_DEBUG ("About to call ProcessObject()");
  CHECK(ProcessObjects());  // this is the function to make output as well as memory cleaning

 
  ATH_MSG_DEBUG ("Algorithm passed");

  setFilterPassed(true); //if got here, assume that means algorithm passed
  return StatusCode::SUCCESS;
}

StatusCode JGTowerReader::beginInputFile() { 
  //
  //This method is called at the start of each input file, even if
  //the input file contains no events. Accumulate metadata information here
  //

  //example of retrieval of CutBookkeepers: (remember you will need to include the necessary header files and use statements in requirements file)
  // const xAOD::CutBookkeeperContainer* bks = 0;
  // CHECK( inputMetaStore()->retrieve(bks, "CutBookkeepers") );

  //example of IOVMetaData retrieval (see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AthAnalysisBase#How_to_access_file_metadata_in_C)
  //float beamEnergy(0); CHECK( retrieveMetadata("/TagInfo","beam_energy",beamEnergy) );
  //std::vector<float> bunchPattern; CHECK( retrieveMetadata("/Digitiation/Parameters","BeamIntensityPattern",bunchPattern) );



  return StatusCode::SUCCESS;
}

StatusCode JGTowerReader::BuildBlocksFromTowers(std::vector<TowerObject::Block>& blocks, const xAOD::JGTowerContainer towers, const int blockRows, const int blockCols, const bool useNegTowers){

  blocks.clear();
  TowerObject::TowerGrid grid = TowerObject::TowerGrid(towers);

  for(const xAOD::JGTower* seed: towers){
    float eta = std::abs(seed->eta()); 
    int r = blockRows; int c = blockCols; 
    if(eta>2.9) {r = 1; c = 1;}//The towers beyond eta = 2.9 get much larger and more complicated. Make the blocks a single tower... 
    int seedIndex = std::find(towers.begin(), towers.end(), seed) - towers.begin();

    std::vector<int> neighbors = grid.neighbors(*seed, r, c);
    float seed_Et = seed->et();
    if(!useNegTowers && seed->et() < 0)continue;

    double block_area(0.0);
    double sum_deta(0.0);
    double sum_dphi(0.0);
    double block_pt(seed_Et);
    double neighbor_pt = 0;

    for(const int& neighborIndex: neighbors){
      const xAOD::JGTower* neighbor = towers.at(neighborIndex);
      block_area += neighbor->deta()*neighbor->dphi();
      sum_deta += neighbor->deta();
      sum_dphi += neighbor->dphi();
      neighbor_pt = neighbor->et();
      if(!useNegTowers && neighbor->et() < 0)continue;
      block_pt += neighbor_pt;
    }

    TowerObject::Block block(block_pt, seed->eta(), seed->phi(), 0.0);
    block.seedIndex(seedIndex);
    block.numEta(blockCols);
    block.numPhi(blockRows);
    block.area(block_area);
    block.sumDeta(sum_deta);
    block.sumDphi(sum_dphi);
    block.numConstituents(neighbors.size());

    blocks.push_back(block);
  }

  std::sort(blocks.rbegin(), blocks.rend());

  return StatusCode::SUCCESS;
}

//L1Calo algorithms are here:
StatusCode JGTowerReader::JFexAlg(const xAOD::JGTowerContainer* jTs){

  ATH_MSG_DEBUG("Found " << jTs->size() << " jTowers");
  
  ATH_MSG_DEBUG("JFexAlg: BuildMET");
  CHECK(METAlg::NoiseCut_MET(jTs, "jNOISECUT", jT_noise, /*m_useNegTowers*/ false));
  if (!buildbins) {
      CHECK(METAlg::build_jFEX_bins(jFEX_bins, jFEX_bins_core, jTs ));
      buildbins=true;
  }

  xAOD::JGTowerContainer* jTs_PUsub = new xAOD::JGTowerContainer();
  xAOD::JGTowerAuxContainer* jTs_PUsubAux = new xAOD::JGTowerAuxContainer();
  jTs_PUsub->setStore(jTs_PUsubAux);
  CHECK(METAlg::jXERHO(jTs , "jXERHO" , jTowerArea , jFEX_bins , jFEX_bins_core , m_jXERHO_fixed_noise_cut , m_jXERHO_rho_up_threshold , m_jXERHO_min_noise_cut , jTs_PUsub));
  ATH_MSG_DEBUG("JFexAlg: Done");

  if (m_makeRoundJetsPUsub) {
      ATH_MSG_DEBUG("JFexAlg: JetsPUsub");
      if(m_makeRoundJets) {
          ATH_MSG_DEBUG("JFexAlg: JetsPUsub, makeRoundJets");
          if( JetAlg::m_SeedMap.find("jRoundSeedsPUsub") == JetAlg::m_SeedMap.end() )
              CHECK( JetAlg::SeedGrid(jTs_PUsub, "jRoundSeedsPUsub", m_dumpSeedsEtaPhi) );

          ATH_MSG_DEBUG("JFexAlg using JetsPUsub: SeedFinding with jJetSeeds; m_jJet_seed_size = " << m_jJet_seed_size << ", m_jJet_max_r = " << m_jJet_max_r);
          CHECK( JetAlg::SeedFinding( jTs_PUsub, "jRoundSeedsPUsub", m_jJetRound_seed_size, m_jJetRound_max_r, jT_noise,
				      /*m_jJetRound_seed_tower_noise_multiplier*/ 0,  m_jJetRound_seed_min_ET_MeV, /*electron*/false) );

          ATH_MSG_DEBUG("JFexAlg usinf JetsPUsub: BuildRoundJet");
          CHECK( JetAlg::BuildRoundJet(jTs_PUsub, "jRoundSeedsPUsub", "jRoundJetsPUsub", m_jJetRound_r, jT_noise, /*m_jJetRound_jet_tower_noise_multiplier*/ 0,  m_jJetRound_jet_min_ET_MeV, m_saveSeeds) );
      }
  }

  if(m_makeSquareJets) {
    // find all seeds
    // the diameter of seed, and its range to be local maximum
    // Careful to ensure the range set to be no tower double counted
    if( JetAlg::m_SeedMap.find("jSeeds") == JetAlg::m_SeedMap.end() ){
      ATH_MSG_DEBUG( "JFexAlg: SeedGrid");
      CHECK( JetAlg::SeedGrid(jTs, "jSeeds", m_dumpSeedsEtaPhi) );
    }

    ATH_MSG_DEBUG( "JFexAlg: SeedFinding with jSeeds; m_jJet_seed_size = " 
                   << m_jJet_seed_size << ", m_jJet_max_r = " << m_jJet_max_r);
    CHECK( JetAlg::SeedFinding( jTs, "jSeeds", m_jJet_seed_size, m_jJet_max_r, jT_noise,
                                m_jJet_seed_tower_noise_multiplier, 
                                m_jJet_seed_min_ET_MeV,/*electron*/false) );

    ATH_MSG_DEBUG("JFexAlg: BuildJet");
    CHECK( JetAlg::BuildJet(jTs, "jSeeds", "jJets", m_jJet_r, jT_noise, m_jJet_jet_tower_noise_multiplier,  m_jJet_jet_min_ET_MeV, m_saveSeeds) );
  }
  if(m_makeRoundJets) {
    if( JetAlg::m_SeedMap.find("jRoundSeeds") == JetAlg::m_SeedMap.end() )
      CHECK( JetAlg::SeedGrid(jTs, "jRoundSeeds", m_dumpSeedsEtaPhi) );

    ATH_MSG_DEBUG("JFexAlg: SeedFinding with jJetSeeds; m_jJet_seed_size = " << m_jJet_seed_size << ", m_jJet_max_r = " << m_jJet_max_r);
    CHECK( JetAlg::SeedFinding( jTs, "jRoundSeeds", m_jJetRound_seed_size, m_jJetRound_max_r, jT_noise, 
                                m_jJetRound_seed_tower_noise_multiplier, 
                                m_jJetRound_seed_min_ET_MeV,/*electron*/false) );

    ATH_MSG_DEBUG("JFexAlg: BuildRoundJet");
    CHECK( JetAlg::BuildRoundJet(jTs, "jRoundSeeds", "jRoundJets", m_jJetRound_r, jT_noise, m_jJetRound_jet_tower_noise_multiplier, m_jJetRound_jet_min_ET_MeV, m_saveSeeds) );
  }

  if(m_makeRoundLargeRJets) {
    ATH_MSG_DEBUG("JFexAlg: BuildRoundLargeRJet");
    CHECK( JetAlg::BuildRoundJet(jTs, "jRoundSeeds", "jRoundLargeRJets", m_jJetRound_LargeR_r, jT_noise, m_jJetRound_jet_tower_noise_multiplier,  m_jJetRound_LargeR_jet_min_ET_MeV,/*m_saveSeeds*/ false) );
    CHECK( JetAlg::BuildRoundJet(jTs_PUsub, "jRoundSeedsPUsub", "jRoundLargeRJetsPUsub", m_jJetRound_LargeR_r, jT_noise, /*m_jJetRound_jet_tower_noise_multiplier*/ 0, m_jJetRound_LargeR_jet_min_ET_MeV, false) );
  }
  
  if(m_makeJetsFromMap) {
    ATH_MSG_DEBUG("JFexAlg: Build jets from map");
    CHECK( BuildJetsFromMap(jTs) );
  }
  
  delete jTs_PUsub;
  delete jTs_PUsubAux;


  return StatusCode::SUCCESS;
}

StatusCode JGTowerReader::GFexAlg(const xAOD::JGTowerContainer* gTs){

  // sort out the wrong-size list of noise vector
  if(gTs->size() > gT_noise.size()) {
    ATH_MSG_ERROR("Found " << gTs->size() << " gTowers, but the noise vector only has " << gT_noise.size() << " entries");
    ATH_MSG_WARNING("Setting the gTower noise vector to have the right number of entries, all 1000");
    gT_noise.clear();
    for(int i=0; i<int(gTs->size()); i++){
      gT_noise.push_back(1000);
    }
  }

  //build gCaloTowers
  xAOD::JGTowerAuxContainer* gCaloTowersAux = new xAOD::JGTowerAuxContainer();
  xAOD::JGTowerContainer* gCaloTowers = new xAOD::JGTowerContainer();
  gCaloTowers->setStore(gCaloTowersAux);

  //add container for rhoA,B,C using Run2 EnergySumRoIAux container 
  xAOD::EnergySumRoIAuxInfo* RhoContAux = new xAOD::EnergySumRoIAuxInfo();
  xAOD::EnergySumRoI* RhoCont = new xAOD::EnergySumRoI();
  RhoCont->setStore(RhoContAux);

  for(unsigned int t = 0; t < gTs->size(); t++){
    const xAOD::JGTower* gt_em = gTs->at(t);
    const float eta = gt_em->eta();
    const float phi = gt_em->phi();
    const float deta = gt_em->deta();
    const float dphi = gt_em->dphi();
    float totalEt = 0;

    if(fabs(eta) < 3.15 && gt_em->sampling() == 0){
      const xAOD::JGTower* gt_had = gTs->at(t+544);
      if(std::abs(eta-gt_had->eta())>0.01)ATH_MSG_WARNING("Unmatched ETA: " << eta << " " << gt_had->eta());
      if(std::abs(phi-gt_had->phi())>0.01)ATH_MSG_WARNING("Unmatched PHI: " << phi << " "<< gt_had->phi());
      totalEt = gt_em->et() + gt_had->et();
    }
    //if(t > 544) totalEt = 0;
    if(fabs(eta)>=3.15){
      //For the case where eta > 3.15, the sampling is always 2. Not sure that is quite what we want, but that is how the 
      //identifiers are configured for now... 
      if(gTs->at(t)->et()>0 && gTs->at(t)->sampling()!=2)ATH_MSG_WARNING("ET: " << gTs->at(t)->et() << " sampling " << gTs->at(t)->sampling());
      totalEt=gTs->at(t)->et(); 
    }
    if(fabs(eta)<3.15 && gt_em->sampling()!=0)continue;
    //only build 1 layer for the calo towers to avoid double counting; 
    const float tEt = totalEt; 

    const std::vector<int> index(2, 0);
    xAOD::JGTower* new_tower = new xAOD::JGTower();
    gCaloTowers->push_back(new_tower);
    const int t_ = t; 
    new_tower->initialize(t_, eta, phi);
    new_tower->setdEta(deta);                                                                     
    new_tower->setdPhi(dphi);                                                                     
    new_tower->setEt(tEt);                                                                
    new_tower->setSCIndex(index);                                                                    
    new_tower->setTileIndex(index);
    const int sampling = gt_em->sampling(); 
    new_tower->setSampling(sampling);

  }

  //split towers per fpga for rho subtraction
  ConstDataVector<xAOD::JGTowerContainer> temp_a(SG::VIEW_ELEMENTS);
  ConstDataVector<xAOD::JGTowerContainer> temp_b(SG::VIEW_ELEMENTS);
  ConstDataVector<xAOD::JGTowerContainer> temp_c(SG::VIEW_ELEMENTS);

  TH1F* h_fpga_a = new TH1F("h_fpga_a", "", 50, 0, 5000);
  TH1F* h_fpga_b = new TH1F("h_fpga_b", "", 50, 0, 5000);
  TH1F* h_fpga_c = new TH1F("h_fpga_c", "", 50, 0, 5000);

  for(unsigned int t = 0; t < gCaloTowers->size(); t++){
    const xAOD::JGTower* tower = gCaloTowers->at(t);
    float eta = tower->eta();

    std::string FPGA = GFEX_pFPGA(eta); 
    if(FPGA=="A"){
      h_fpga_a->Fill(tower->et());
      temp_a.push_back(tower);
    }
    if(FPGA=="B"){
      h_fpga_b->Fill(tower->et());
      temp_b.push_back(tower);
    }
    if(FPGA=="C"){
      h_fpga_c->Fill(tower->et());
      temp_c.push_back(tower);
    }

  }
  const xAOD::JGTowerContainer* fpga_a = temp_a.asDataVector();
  const xAOD::JGTowerContainer* fpga_b = temp_b.asDataVector();
  const xAOD::JGTowerContainer* fpga_c = temp_c.asDataVector();

  float thresh_a = 3*h_fpga_a->GetRMS();
  float thresh_b = 3*h_fpga_b->GetRMS();
  float thresh_c = 3*h_fpga_c->GetRMS();

  //build pileup subtracted towers
  xAOD::JGTowerAuxContainer* pu_subAux = new xAOD::JGTowerAuxContainer();
  xAOD::JGTowerContainer* pu_sub = new xAOD::JGTowerContainer();
  pu_sub->setStore(pu_subAux);

  float rhoA = METAlg::Rho_avg_etaRings(fpga_a, m_gFEX_Rho_useNegTowers);
  float rhoB = METAlg::Rho_avg_etaRings(fpga_b, m_gFEX_Rho_useNegTowers);
  float rhoC = METAlg::Rho_avg_etaRings(fpga_c, m_gFEX_Rho_useNegTowers);
  
  if(m_gFEX_OnlyPosRho){
    if(rhoA<0) rhoA=0; 
    if(rhoB<0) rhoB=0;
    if(rhoC<0) rhoC=0; 
  }

  float rho_barrel = METAlg::Rho_avg_barrel(gCaloTowers, m_gFEX_Rho_useNegTowers);

  (*acc_rhoA)(*RhoCont) = rhoA; 
  (*acc_rhoB)(*RhoCont) = rhoB;
  (*acc_rhoC)(*RhoCont) = rhoC;
  (*acc_rho_barrel)(*RhoCont) = rho_barrel;
  
  (*acc_threshA)(*RhoCont) = thresh_a;
  (*acc_threshB)(*RhoCont) = thresh_b;
  (*acc_threshC)(*RhoCont) = thresh_c;

  for(unsigned int t = 0; t < gCaloTowers->size(); t++){
    const xAOD::JGTower* tower = gCaloTowers->at(t);
    const float eta = tower->eta();
    const float phi = tower->phi();
    const float deta = tower->deta();
    const float dphi = tower->dphi();
    float Et_sub = tower->et();

    float Area = GTowerArea(eta); 
    float rho = -999e6; float thresh = -999e6; 
    std::string FPGA = GFEX_pFPGA(eta);
    if(FPGA=="A"){rho = rhoA; thresh = thresh_a;}
    else if(FPGA=="B"){rho = rhoB; thresh = thresh_b;}
    else if(FPGA=="C"){rho = rhoC; thresh = thresh_c;}
    else ATH_MSG_WARNING("No valid FPGA found"); 

    Et_sub -= Area*rho; 
    if(Et_sub < thresh) Et_sub = 0; //Apply RMS requirement for MET 

    const std::vector<int> index(2,0);
    const int sampling = tower->sampling();
    xAOD::JGTower* new_tower = new xAOD::JGTower();
    pu_sub->push_back(new_tower);
    new_tower->initialize(t, eta, phi);
    new_tower->setdEta(deta);
    new_tower->setdPhi(dphi);
    new_tower->setEt(Et_sub);
    new_tower->setSCIndex(index);
    new_tower->setTileIndex(index);
    new_tower->setSampling(sampling);
  }//Loop over towers 

  //build gBlocks
  std::vector<TowerObject::Block> temp_gBlocks;
  std::vector<TowerObject::Block> temp_puSub_gBlocks;

  CHECK(BuildBlocksFromTowers(temp_gBlocks, *gCaloTowers, 3, 3, m_gFEX_useNegTowers));
  CHECK(BuildBlocksFromTowers(temp_puSub_gBlocks, *pu_sub, 3, 3, m_gFEX_useNegTowers));

  const std::vector<TowerObject::Block> gBlocks = temp_gBlocks;
  const std::vector<TowerObject::Block> puSub_gBlocks = temp_puSub_gBlocks;

  xAOD::JGTowerAuxContainer* gBAux = new xAOD::JGTowerAuxContainer();
  xAOD::JGTowerContainer* gBs = new xAOD::JGTowerContainer();
  gBs->setStore(gBAux);

  const std::vector<int> index(2, 0);
  
  for(unsigned int b = 0; b < gBlocks.size(); b++){
    float phi = gBlocks[b].Phi();
    float eta = gBlocks[b].Eta();
    float deta = gBlocks[b].sumDeta(); //sum of delta eta between seed tower to all others? 
    float dphi = gBlocks[b].sumDphi(); //sum of delta phi between seed tower and all others? 
    float pt = gBlocks[b].pt();
    int seedTowerIndex = gBlocks[b].seedIndex();
    const int sampling = gCaloTowers->at(seedTowerIndex)->sampling();

    xAOD::JGTower* new_block = new xAOD::JGTower();
    const int b_ = b; 
    gBs->push_back(new_block);
    new_block->initialize(b_, eta, phi);
    new_block->setdEta(deta);                                                 
    new_block->setdPhi(dphi);                                                                   
    new_block->setEt(pt);
    new_block->setSCIndex(index);  // Not sure what these are supposed to be?
    new_block->setTileIndex(index);// Not sure what these are supposed to be?
    new_block->setSampling(sampling);
  }  

  // jet algorithms
  
  CHECK(evtStore()->record(RhoCont,"EventVariables"));
  CHECK(evtStore()->record(RhoContAux,"EventVariablesAux."));
  CHECK(JetAlg::BuildFatJet(*gCaloTowers, "gL1Jets", m_gJet_r, gT_noise, m_gJet_tower_noise_multiplier,m_gJet_block_min_ET_MeV, m_gJet_jet_min_ET_MeV, rhoA, rhoB, rhoC, m_gFEX_useNegTowers));
  if(m_buildgBlockJets) CHECK(JetAlg::BuildgBlocksJets(gBs, "gBlockJets",rhoA,rhoB, rhoC));

  //gFEX MET algorithms
  std::vector<float> noNoise; 

  CHECK(METAlg::NoiseCut_MET(pu_sub, "gXERHO", noNoise, m_gFEX_useNegTowers));
  CHECK(METAlg::NoiseCut_MET(gCaloTowers,"gXENOISECUT",gT_noise, m_gFEX_useNegTowers));
  CHECK(METAlg::JwoJ_MET(pu_sub, puSub_gBlocks,"gXEJWOJRHO",m_gFEX_pTcone_cut, /*bool useRho*/ false,rhoA,rhoB,rhoC, /*m_useNegTowers*/ false));//do not use negative towers, so if the subtracted ET is < 0 you remove the tower
  CHECK(METAlg::JwoJ_MET(gCaloTowers, gBlocks, "gXEJWOJ",m_gFEX_pTcone_cut,/*bool useRho*/ false,rhoA,rhoB,rhoC, /*m_useNegTowers*/ m_gFEX_useNegTowers));
  CHECK(METAlg::JwoJ_MET(gCaloTowers, gBlocks, "gXEJWOJRHOHT",m_gFEX_pTcone_cut,/*bool useRho*/ true,rhoA,rhoB,rhoC, /*m_useNegTowers*/ m_gFEX_useNegTowers));
  CHECK(METAlg::Pufit_MET(gCaloTowers,"gXEPUFIT", m_gFEX_useNegTowers) ); 
  CHECK(METAlg::RhoRMS_LUT(gCaloTowers,"gXERHORMS_LUT",rhoA,rhoB,rhoC,m_gXERHOLUT_file, false, m_gFEX_useNegTowers) ); 
  CHECK(METAlg::RhoRMS_LUT(gCaloTowers,"gXERHORMS_LUT_offline",rhoA,rhoB,rhoC,m_gXERHOLUT_file, true, m_gFEX_useNegTowers) ); 

  //manage conatiners that have been created: save gCaloTowers and pu_sub to SG
  CHECK(evtStore()->record(gCaloTowers, "gCaloTowers"));
  CHECK(evtStore()->record(gCaloTowersAux, "gCaloTowersAux."));
  CHECK(evtStore()->record(pu_sub, "pu_subTowers"));
  CHECK(evtStore()->record(pu_subAux, "pu_subTowersAux."));
  CHECK(evtStore()->record(gBs, "gBlocks"));
  CHECK(evtStore()->record(gBAux, "gBlocksAux."));

  delete h_fpga_a;
  delete h_fpga_b;
  delete h_fpga_c;

  return StatusCode::SUCCESS;
}

StatusCode JGTowerReader::ProcessObjects(){
  
  // Ouptut Jets
  for ( auto it = JetAlg::m_JetMap.begin(); it != JetAlg::m_JetMap.end(); it++ ){
    ATH_MSG_DEBUG("Adding collection " << it->first << " with size " << it->second.size());
    xAOD::JetRoIAuxContainer* JetContAux = new xAOD::JetRoIAuxContainer();
    xAOD::JetRoIContainer* JetCont = new xAOD::JetRoIContainer();
    JetCont->setStore(JetContAux);
    for(unsigned j=0;j<it->second.size();j++){
      std::shared_ptr<JetAlg::L1Jet> jet = it->second.at(j);
      float et = jet.get()->et;
      float eta = jet.get()->eta;
      float phi = jet.get()->phi;
      CHECK(HistBookFill(Form("%s_et",it->first.Data()),50,0,500,et/1000.,1.));
      CHECK(HistBookFill(Form("%s_eta",it->first.Data()),49,-4.9,4.9,eta,1.));
      CHECK(HistBookFill(Form("%s_phi",it->first.Data()),31,-3.1416,3.1416,phi,1.));
      xAOD::JetRoI* Jet = new xAOD::JetRoI;
      JetCont->push_back(Jet);
      Jet->initialize(0x0,eta,phi);
      Jet->setEt8x8(et);    
    }
    CHECK(evtStore()->record(JetCont,it->first.Data()));
    CHECK(evtStore()->record(JetContAux,Form("%sAux.",it->first.Data())));
    ATH_MSG_DEBUG("Recording JetRoIContainer with name " << it->first.Data() << " and size " << JetCont->size());
  }
  
  // Output Seeds
  if(m_plotSeeds) {
    for ( auto it = JetAlg::m_SeedMap.begin(); it != JetAlg::m_SeedMap.end(); it++ ){     
      for(unsigned iseed_eta=0; iseed_eta<it->second->eta.size(); iseed_eta++){
        for(unsigned iseed_phi=0; iseed_phi<it->second->phi.at(iseed_eta).size(); iseed_phi++){
          if(it->second->local_max.at(iseed_eta).at(iseed_phi)) {
            CHECK(HistBookFill(Form("%s_et",it->first.Data()),50,0,500,it->second->et.at(iseed_eta).at(iseed_phi)/1000.,1.));
            CHECK(HistBookFill(Form("%s_eta",it->first.Data()),49,-4.9,4.9,it->second->eta.at(iseed_eta),1.));
            CHECK(HistBookFill(Form("%s_phi",it->first.Data()),31,-3.1416,3.1416,it->second->phi.at(iseed_eta).at(iseed_phi),1.));
          }
        }
      }
    }
  }
  
  // Output MET
  for ( auto it = METAlg::m_METMap.begin(); it != METAlg::m_METMap.end(); it++ ){
    xAOD::EnergySumRoIAuxInfo* METContAux = new xAOD::EnergySumRoIAuxInfo();
    xAOD::EnergySumRoI* METCont = new xAOD::EnergySumRoI();
    METCont->setStore(METContAux);

    std::shared_ptr<METAlg::MET> met = it->second;

    CHECK(HistBookFill(Form("MET_%s_et",it->first.Data()), 50, 0, 500, met->et*1e-3, 1.));
    METCont->setEnergyX(met->ex); 
    METCont->setEnergyY(met->ey); 
    METCont->setEnergyT(met->et);
    (*acc_mht)(*METCont) = met->mht;
    (*acc_mst)(*METCont) = met->mst;
    CHECK(evtStore()->record(METCont,Form("%s_MET",it->first.Data())));
    CHECK(evtStore()->record(METContAux,Form("%s_METAux.",it->first.Data())));
    ATH_MSG_DEBUG("Recording EnergySumRoI with name " << it->first.Data() << "_MET");
    ATH_MSG_DEBUG("Recording EnergySumRoI with name " << it->first.Data() << "_MET");

  }
  

  METAlg::m_METMap.clear();
  JetAlg::m_JetMap.clear();
  for ( auto it = JetAlg::m_SeedMap.begin(); it != JetAlg::m_SeedMap.end(); it++ ){
    it->second->local_max.clear();
  }

  return StatusCode::SUCCESS;
}

StatusCode JGTowerReader::HistBookFill(const TString name, Int_t nbinsx, const Double_t* xbins, float xvalue,float wei){

  if(std::find(hists.begin(),hists.end(),name)==hists.end()) {
    TH1F*h;
    const std::string name_str(name.Data());
    if(name_str.find("/")!=std::string::npos){
      TString name1= name_str.substr(0, name_str.find("/"));
      TString name2= name_str.substr(name_str.find("/")+1,name_str.length()-1);
      h = new TH1F( name2, name2, nbinsx, xbins);
      h->Sumw2();
      CHECK( histSvc->regHist(Form("/EXPERT/%s/%s/%s", this->name().c_str(),name1.Data(), name2.Data()),h));
    }
    else{
      h = new TH1F( name, name, nbinsx, xbins);
      h->Sumw2();
      CHECK( histSvc->regHist(Form("/EXPERT/%s/%s", this->name().c_str(), name.Data()),h));
    }
    m_hName[name]=h;
    m_hName[name]->Fill(xvalue,wei);
    hists.push_back(name);
  }
  else m_hName[name]->Fill(xvalue,wei);
  return StatusCode::SUCCESS;
}

StatusCode JGTowerReader::HistBookFill(const TString name, Int_t nbinsx, Double_t xbin_down, Double_t xbin_up, float xvalue,float wei){

  if(std::find(hists.begin(),hists.end(),name)==hists.end()) {
    TH1F*h;
    const std::string name_str(name.Data());
    if(name_str.find("/")!=std::string::npos){
      TString name1= name_str.substr(0, name_str.find("/"));
      TString name2= name_str.substr(name_str.find("/")+1,name_str.length()-1);
      h = new TH1F( name2, name2, nbinsx, xbin_down, xbin_up);
      h->Sumw2();
      CHECK( histSvc->regHist(Form("/EXPERT/%s/%s/%s", this->name().c_str(),name1.Data(), name2.Data()),h));
    }
    else{
      h = new TH1F( name, name, nbinsx, xbin_down, xbin_up);
      h->Sumw2();
      CHECK( histSvc->regHist(Form("/EXPERT/%s/%s", this->name().c_str(), name.Data()),h));
    }

    m_hName[name]=h;
    m_hName[name]->Fill(xvalue,wei);
    hists.push_back(name);
  }
  else  m_hName[name]->Fill(xvalue,wei);
  return StatusCode::SUCCESS;
}


std::vector<std::string> splitString(std::string parentString, std::string sep, bool stripEmpty) {
  std::size_t start = 0, end = 0;
  std::vector<std::string> splitVec;
  while ((end = parentString.find(sep, start)) != std::string::npos) {
    if( end-start == 0 && stripEmpty ) {;}
    else {
      splitVec.push_back(parentString.substr(start, end - start));
    }
    start = end + sep.size();
  }
  std::string part = parentString.substr(start);
  if(!(stripEmpty && part == "")) 
    splitVec.push_back(part);

  return splitVec;
}



StatusCode JGTowerReader::ReadTowerMap() {
  ATH_MSG_INFO("Looking for tower map at " << m_towerMap);
  std::string towerMapPath = PathResolverFindCalibFile(m_towerMap);
  if(towerMapPath == "") {
    ATH_MSG_DEBUG("Could not resolve " << m_towerMap);
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Resolved at " << towerMapPath);

  std::ifstream infileStream(towerMapPath);
  std::string line;

  bool inTowerBlock = false;
  bool inSeedBlock = false;
  bool inJetBlock = false;
  int firstTowerNum = -9;

  int sublinecount = -1;
  while(getline(infileStream, line)) {
    // skip commented and empty lines
    if(line.substr(0, 1) == "#") {        
      if(line == "# === start towers ===")
        inTowerBlock = true;
      if(line == "# === end towers ===")
        inTowerBlock = false;
      if(line == "# === start seeds ===")
        inSeedBlock = true;
      if(line == "# === end seeds ===")
        inSeedBlock = false;
      if(line == "# === start jets ===")
        inJetBlock = true;
      if(line == "# === end jets ===")
        inJetBlock = false;

      std::vector<std::string> splitLine = splitString(line, ": ", true);      
      if(line.find("# total towers:") != std::string::npos)
        totalTowers = std::stoi(splitLine[1]);
      if(line.find("# towers in this map:") != std::string::npos)
        towersInMap = std::stoi(splitLine[1]);
      if(line.find("# total seeds:") != std::string::npos)
        totalSeeds = std::stoi(splitLine[1]);
      if(line.find("# seeds in this map:") != std::string::npos)
        seedsInMap = std::stoi(splitLine[1]);
      if(line.find("# total jets:") != std::string::npos)
        totalJets = std::stoi(splitLine[1]);
      if(line.find("# jets in this map:") != std::string::npos)
        jetsInMap = std::stoi(splitLine[1]);
      continue;
    }

    if(line == "")
      continue;

    std::vector<std::string> splitLine = splitString(line, " ", true);

    if(inTowerBlock) {
      if(line.substr(0, 1) != " ") {
        sublinecount = 0;
          
        // check size of vector vs this entry
        int towerNum = std::stoi(splitLine[0]);
        if(firstTowerNum < 0) {
          firstTowerNum = towerNum;
        }
          
        // fill number, eta and phi
        towerMap_towerNumber.push_back(towerNum);
        towerMap_towerEta.push_back(std::stof(splitLine[1]));
        towerMap_towerPhi.push_back(std::stof(splitLine[2]));
      }
      else {
        if(sublinecount <= 0) {
          ATH_MSG_ERROR("sublinecount value is " << sublinecount << " but it should be <= 0");
          return StatusCode::FAILURE;
        }

        // tower sampling
        if(sublinecount == 1) {
          if(splitLine.size() != 1) {
            ATH_MSG_ERROR("Tower has a number of samplings that is not 1: " << splitLine.size());
            return StatusCode::FAILURE;
          }
          towerMap_towerSampling.push_back(std::stoi(splitLine[0]));
        }
        // tower layers
        else if(sublinecount == 2) {          
          std::vector<int> tempVector;
          tempVector.clear();
          for(int i = 0; i<int(splitLine.size()); i++) {
            tempVector.push_back(std::stoi(splitLine.at(i)));
          }
          towerMap_towerLayers.push_back(tempVector);
        }
        else {
          ATH_MSG_ERROR("tower sublinecount value is " << sublinecount << " but should be in [1,2]");
          return StatusCode::FAILURE;
        }
      }
    }

    if(inSeedBlock) {
      if(line.substr(0, 1) != " ") {
        sublinecount = 0;
          
        // check size of vector vs this entry
        int seedNum = std::stoi(splitLine[0]);
        if(seedNum<0 || (towerMap_seedEta.size() != (unsigned int)seedNum) ) {
          ATH_MSG_ERROR("tower map being parsed incorrectly: have " << towerMap_seedEta.size() << " and expect " << seedNum);
          return StatusCode::FAILURE;
        }
          
        // fill eta and phi
        towerMap_seedEta.push_back(std::stof(splitLine[1]));
        towerMap_seedPhi.push_back(std::stof(splitLine[2]));
      }
      else {
        if(sublinecount <= 0) {
          ATH_MSG_ERROR("sublinecount value is " << sublinecount << " but it should be <= 0");
          return StatusCode::FAILURE;
        }

        std::vector<int> tempVector;
        tempVector.clear();
        for(int i = 0; i<int(splitLine.size()); i++) {
          tempVector.push_back(std::stoi(splitLine.at(i)));
        }

        // seed towers
        if(sublinecount == 1) {
          towerMap_seedTowers.push_back(tempVector);
        }
        // seed local maxima
        else if(sublinecount == 2) {          
          towerMap_seedLocalMaxSeeds.push_back(tempVector);
        }
        else {
          ATH_MSG_ERROR("seed sublinecount value is " << sublinecount << " but should be in [1,2]");
          return StatusCode::FAILURE;
        }
      }
    }

    if(inJetBlock) {
      if(line.substr(0, 1) != " ") {
        sublinecount = 0;
          
        // check size of vector vs this entry
        int jetNum = std::stoi(splitLine[0]);
        if(jetNum<0 || (towerMap_jetEta.size() != (unsigned int)jetNum) ) {
          ATH_MSG_ERROR("tower map being parsed incorrectly: have " << towerMap_jetEta.size() << " and expect " << jetNum);
          return StatusCode::FAILURE;
        }
          
        // fill eta and phi
        towerMap_jetEta.push_back(std::stof(splitLine[1]));
        towerMap_jetPhi.push_back(std::stof(splitLine[2]));
      }
      else {
        if(sublinecount <= 0) {
          ATH_MSG_ERROR("sublinecount value is " << sublinecount << " but it should be <= 0");
          return StatusCode::FAILURE;
        }

        // jet seed
        if(sublinecount == 1) {
          if(splitLine.size() != 1) {
            ATH_MSG_ERROR("Jet has a number of seeds that is not 1: " << splitLine.size());
            return StatusCode::FAILURE;
          }
          towerMap_jetSeed.push_back(std::stoi(splitLine[0]));
        }
        // jet towers
        else if(sublinecount == 2) {          
          std::vector<int> tempVector;
          tempVector.clear();
          for(int i = 0; i<int(splitLine.size()); i++) {
            tempVector.push_back(std::stoi(splitLine.at(i)));
          }
          towerMap_jetTowers.push_back(tempVector);
        }
        else {
          ATH_MSG_ERROR("jet sublinecount value is " << sublinecount << " but should be in [1,2]");
          return StatusCode::FAILURE;
        }
      }
    }
      
    sublinecount += 1;
  }

  ATH_MSG_DEBUG("read info from map");

  // check that I got the numbers of towers, seeds, jets
  if(totalTowers==0) { ATH_MSG_ERROR("failed to extract totalTowers"); return StatusCode::FAILURE; }
  if(towersInMap==0) { ATH_MSG_ERROR("failed to extract towersInMap"); return StatusCode::FAILURE; }
  if(totalSeeds==0)  { ATH_MSG_ERROR("failed to extract totalSeeds");  return StatusCode::FAILURE; }
  if(seedsInMap==0)  { ATH_MSG_ERROR("failed to extract seedsInMap");  return StatusCode::FAILURE; }
  if(totalJets==0)   { ATH_MSG_ERROR("failed to extract totalJets");   return StatusCode::FAILURE; }
  if(jetsInMap==0)   { ATH_MSG_ERROR("failed to extract jetsInMap");   return StatusCode::FAILURE; }

  // check the numbers agree
  if(towersInMap != int(towerMap_towerEta.size())) { ATH_MSG_ERROR("mismatch in number of towers"); return StatusCode::FAILURE; }
  if(seedsInMap != int(towerMap_seedEta.size()))   { ATH_MSG_ERROR("mismatch in number of seeds");  return StatusCode::FAILURE; }
  if(jetsInMap != int(towerMap_jetEta.size()))     { ATH_MSG_ERROR("mismatch in number of jets");   return StatusCode::FAILURE; }

  ATH_MSG_DEBUG("extracted numbers agree");

  // now I have read in the map, but it might well be compressed (i.e. one quadrant, no central jets)
  double tower_max_eta = *std::max_element(towerMap_towerEta.begin(), towerMap_towerEta.end());
  double tower_min_eta = *std::min_element(towerMap_towerEta.begin(), towerMap_towerEta.end());
  double tower_max_phi = *std::max_element(towerMap_towerPhi.begin(), towerMap_towerPhi.end());
  double tower_min_phi = *std::min_element(towerMap_towerPhi.begin(), towerMap_towerPhi.end());
  int tower_max_num = *std::max_element(towerMap_towerNumber.begin(), towerMap_towerNumber.end());
  int tower_min_num = *std::min_element(towerMap_towerNumber.begin(), towerMap_towerNumber.end());
  ATH_MSG_DEBUG("Tower map tower min and max eta, phi, number:" << tower_min_eta << " - " << tower_max_eta << ", "  << tower_min_phi << " - " << tower_max_phi << ", "  << tower_min_num << " - " << tower_max_num);


  // what I now need to do is fill in the maps - down to eta = 0, and the other quadrants
  // want to keep numberings of towers, and update numberings of seeds and jets (I think) - see /afs/cern.ch/work/c/ckaldero/public/L1CaloSim/jetTowerMaps/jFexJet_towerMap_2019-02-12*.txt
  // the below is a partial and non-functional implementation
  // any takers welcome...!

  // fill in empty spots
  std::vector<int> indices;
  if(totalTowers != towersInMap) {
    ATH_MSG_DEBUG("inserting blank towers");

    // do tower numbers
    for(int i_tower=0; i_tower < totalTowers; i_tower++) {
      // if i_tower is in towerMap_towerNumber initially, then count it
      if(std::find(towerMap_towerNumber.begin(), towerMap_towerNumber.end(), i_tower) != towerMap_towerNumber.end())
        indices.push_back(i_tower);
      if(int(towerMap_towerNumber.size()) <= i_tower)
        towerMap_towerNumber.push_back(i_tower);
      else if(towerMap_towerNumber.at(i_tower) != i_tower)
        towerMap_towerNumber.insert(towerMap_towerNumber.begin()+i_tower, i_tower);
    }
    
    // debug printing - which are old and which are new
    for(int i_tower=0; i_tower < int(towerMap_towerNumber.size()); i_tower++) {
      std::cout << i_tower;
      if(std::find(indices.begin(), indices.end(), i_tower) != indices.end()) {
        std::cout << " y";
      }
      std::cout << std::endl;
    }

    std::cout << "I'm going to exit now. Remove me when map extension is working, but until then things will likely segfault" << std::endl;
    return StatusCode::FAILURE;
  }


  // this is a separate attempt to do some filling in too. But it doesn't work w.r.t. missing things at the end.
  // I might be missing some at the end too - in total expect 7712 towers and 4256 seed/jet positions thanks to doubling of barrel towers
  // these numbers have been read from the map - totalTowers, and numTowersInMap. Same for seeds and jets

  // innermost eta is 0.05
  if(tower_min_eta > 0.06) {
    // step 1: fill with tower_min_num empty towers
    std::vector<int> tempVector;
    tempVector.clear();
    for(int i_tower = tower_min_num-1; i_tower >= 0; i_tower--){
      towerMap_towerEta.insert(towerMap_towerEta.begin(), -9);
      towerMap_towerPhi.insert(towerMap_towerPhi.begin(), -9);
      towerMap_towerNumber.insert(towerMap_towerNumber.begin(), i_tower);
      towerMap_towerSampling.insert(towerMap_towerSampling.begin(), -1);
      towerMap_towerLayers.insert(towerMap_towerLayers.begin(), tempVector);
    }

    // step 2: iterate over one eta ring
    std::vector<float> central_phis;
    float eta_row = -1;
    for(int i_tower = tower_min_num; i_tower < int(towerMap_towerPhi.size()); i_tower++) {
      if(towerMap_towerSampling[i_tower] != 0)
        continue;
      if(eta_row < 0) {
        eta_row = towerMap_towerEta[i_tower];
      }
      if(eta_row != towerMap_towerEta[i_tower])
        continue;
      central_phis.push_back(towerMap_towerPhi[i_tower]);
    }

    // step 3: step down in eta
    float tower_eta = tower_min_eta - 0.1;
    int numRowsIn = 1;
    while(tower_eta > 0) {
      for(int i_phi = 0; i_phi < int(central_phis.size()); i_phi++) {
        int referenceTowerNum = tower_min_num + i_phi;
        int towerNum = referenceTowerNum - numRowsIn * int(central_phis.size());
        towerMap_towerEta[towerNum] = tower_eta;
        towerMap_towerPhi[towerNum] = towerMap_towerPhi[referenceTowerNum];
        towerMap_towerSampling[towerNum] = towerMap_towerSampling[referenceTowerNum];
        towerMap_towerLayers[towerNum] = towerMap_towerLayers[referenceTowerNum];
      }
      numRowsIn += 1;
      tower_eta -= 0.1;
    }

  }
  
  // next, expand towers to four quadrants
  tower_max_eta = *std::max_element(towerMap_towerEta.begin(), towerMap_towerEta.end());
  tower_min_eta = *std::min_element(towerMap_towerEta.begin(), towerMap_towerEta.end());
  tower_max_phi = *std::max_element(towerMap_towerPhi.begin(), towerMap_towerPhi.end());
  tower_min_phi = *std::min_element(towerMap_towerPhi.begin(), towerMap_towerPhi.end());

  ATH_MSG_DEBUG("Tower map tower min and max eta, phi, number:" << tower_min_eta << " - " << tower_max_eta << ", "  << tower_min_phi << " - " << tower_max_phi << ", "  << tower_min_num << " - " << tower_max_num);
 


  // print the outcome
  ATH_MSG_DEBUG("Tower map sizes: eta " << towerMap_towerEta.size() << ", phi " << towerMap_towerPhi.size() << ", layers " << towerMap_towerLayers.size() << ", sampling " << towerMap_towerSampling.size());
  ATH_MSG_DEBUG("Seed map sizes: eta " << towerMap_seedEta.size() << ", phi " << towerMap_seedPhi.size() << ", towers " << towerMap_seedTowers.size() << ", localMaxSeeds " << towerMap_seedLocalMaxSeeds.size());
  ATH_MSG_DEBUG("Jet map sizes: eta " << towerMap_jetEta.size() << ", phi " << towerMap_jetPhi.size() << ", towers " << towerMap_jetTowers.size() << ", seed " << towerMap_jetSeed.size());

  // check the outcome
  if(towerMap_towerEta.size() == 0) {
    ATH_MSG_ERROR("Read no towers from map!");
    return StatusCode::FAILURE;
  }
  if(towerMap_towerEta.size() != towerMap_towerPhi.size() || towerMap_towerEta.size() != towerMap_towerLayers.size() || towerMap_towerEta.size() != towerMap_towerSampling.size() ) {
    ATH_MSG_ERROR("Different number of tower eta entries to one of phi, layers or sampling!");
    return StatusCode::FAILURE;
  }

  if(towerMap_seedEta.size() == 0) {
    ATH_MSG_ERROR("Read in no seeds!");
    return StatusCode::FAILURE;
  }
  if(towerMap_seedEta.size() != towerMap_seedPhi.size() || towerMap_seedEta.size() != towerMap_seedTowers.size() || towerMap_seedEta.size() != towerMap_seedLocalMaxSeeds.size() ) {
    ATH_MSG_ERROR("Different number of seed eta entries to one of phi, towers or localMaxSeeds!");
    return StatusCode::FAILURE;
  }

  if(towerMap_jetEta.size() == 0) {
    ATH_MSG_ERROR("Read in no jets!");
    return StatusCode::FAILURE;
  }
  if(towerMap_jetEta.size() != towerMap_jetPhi.size() || towerMap_jetEta.size() != towerMap_jetTowers.size() || towerMap_jetEta.size() != towerMap_jetSeed.size() ) {
    ATH_MSG_ERROR("Different number of jet eta entries to one of phi, towers or seed!");
    return StatusCode::FAILURE;
  }


  ATH_MSG_INFO("successfully read in tower map from " << towerMapPath);
  return StatusCode::SUCCESS;
}


StatusCode JGTowerReader::CheckTowerMap(const xAOD::JGTowerContainer * jTs) {
  ATH_MSG_INFO("Checking tower map");

  // fill std::vector<int> towerMap_AODtowersIndices

  for(unsigned int i_towerMap = 0; i_towerMap < towerMap_towerEta.size(); i_towerMap++) {
    int tower_index = -1;
    for(unsigned int i_jTower = 0; i_jTower < jTs->size(); i_jTower++){
      const xAOD::JGTower *tower = (*jTs)[i_jTower];
       
      // exactly equal fails for phi (agrees to 5dp printed)
      if( fabs( 1 - (towerMap_towerEta[i_towerMap] / tower->eta() ) ) > 0.0001) 
        continue;
      if( fabs( 1 - (towerMap_towerPhi[i_towerMap] / tower->phi() ) ) > 0.0001) 
        continue;
      if(towerMap_towerSampling.at(i_towerMap) != tower->sampling())
        continue;
      tower_index = i_jTower;
      break;
    }
    if(tower_index == -1) {
      ATH_MSG_ERROR("Did not find matching J/GTower in AOD for this one in the map: tower " << i_towerMap << ", eta = " << towerMap_towerEta.at(i_towerMap) << ", phi = " << towerMap_towerPhi.at(i_towerMap) << ", sampling = " << towerMap_towerSampling.at(i_towerMap));
      return StatusCode::FAILURE;
    }
    towerMap_AODtowersIndices.push_back(tower_index);
    // the ith entry of this vector has the jTower index corresponding to the ith tower in the map
  }
  return StatusCode::SUCCESS;
}


StatusCode JGTowerReader::BuildJetsFromMap(const xAOD::JGTowerContainer*jTs) {

  ATH_MSG_DEBUG("BuildJetsFromMap: starting");
  // step 0, clear things
  towerMapSeed_ET.clear();
  towerMapSeed_isLocalMax.clear();
  

  // step 1, make all the seeds
  int nSeedsAboveThreshold = 0;
  std::cout << "n seeds: " << towerMap_seedEta.size() << std::endl;
  for(int i_seed = 0; i_seed < int(towerMap_seedEta.size()); i_seed++) {
    // ATH_MSG_DEBUG("BuildJetsFromMap: making seed " << i_seed);
    float seed_ET = 0;
    float seed_totalNoise = 0;
    for(auto tower_index : towerMap_seedTowers.at(i_seed)) {
      // jT_index is the jTower index corresponding to the tower_index tower in the map
      int jT_index = towerMap_AODtowersIndices.at(tower_index);
      const xAOD::JGTower*tower = jTs->at(jT_index);
      // only add a tower if its et is N times bigger than the noise in that tower
      // in the other algs, this is 5 * jJet_jet_thr, which is jT_noise multiplied by m_jJet_jet_thr
      if ( tower->et() > m_map_seed_tower_noise_multiplier * jT_noise.at(jT_index) ) {
        seed_ET += tower->et();
      }
      seed_totalNoise += jT_noise.at(jT_index);
      // ATH_MSG_DEBUG("BuildJetsFromMap: adding to seed " << i_seed << " with tower " << tower_index);
    }
    // set seed ET to 0 if it has less ET than N times the total noise over the seed towers
    if ( seed_ET < seed_totalNoise * m_map_seed_total_noise_multiplier ) {
      seed_ET = 0;
    }
    else {
      nSeedsAboveThreshold += 1;
      // ATH_MSG_DEBUG("adding a non-zero seed " << i_seed << " at (eta,phi)=" << towerMap_seedEta[i_seed] << "," << towerMap_seedPhi[i_seed] << ") with ET " << seed_ET);
    }
    towerMapSeed_ET.push_back(seed_ET);
  }
  ATH_MSG_DEBUG("found " << nSeedsAboveThreshold << " seeds above noise threshold");

  // Step 2, check local max of seeds
  for(int i_seed = 0; i_seed < int(towerMap_seedEta.size()); i_seed++) {
    bool isLocalMax = true;

    if(towerMapSeed_ET.at(i_seed)==0)
      isLocalMax = false;
    
    if(isLocalMax) {
      for(auto seed_index : towerMap_seedLocalMaxSeeds.at(i_seed)) {
        if(towerMapSeed_ET.at(i_seed) < towerMapSeed_ET.at(seed_index)) {
          isLocalMax = false;
          break;
        }
        if(towerMapSeed_ET.at(i_seed) == towerMapSeed_ET.at(seed_index)) {
          // follow firmware implementation: if Deta+Dphi>0: require >, if <0 then >=, if==0 then if Deta>0 then >, else >=
          float deta = towerMap_seedEta.at(seed_index) - towerMap_seedEta.at(i_seed);
          float dphi = deltaPhi(towerMap_seedPhi.at(seed_index), towerMap_seedPhi.at(i_seed));

          if(deta + dphi > 0) {
            isLocalMax = false;
            ATH_MSG_DEBUG("deta+dphi>0: need >, not local max");
          }
          else if(deta + dphi < 0) {
            ATH_MSG_DEBUG("deta+dphi<0: only need >=, is local max");
          }
          else {
            ATH_MSG_DEBUG("deta+dphi==0: must go further");
            if(deta > 0) {
              isLocalMax = false;
              ATH_MSG_DEBUG("deta>0: need >, not local max");
            }
            else {
              ATH_MSG_DEBUG("deta<0: only need >=, is local max");
            }
          }

        }
      }
    }

    if(isLocalMax)
      ATH_MSG_DEBUG("BuildJetFromMap: found a local max seed at (eta,phi)=(" << towerMap_seedEta[i_seed] << "," << towerMap_seedPhi[i_seed] << ")");

    // don't count seed if below minimum ET threshold
    if (towerMapSeed_ET.at(i_seed) < m_map_seed_min_ET_MeV) {
      if(isLocalMax)
        ATH_MSG_DEBUG("  below ET threshold");
      towerMapSeed_ET.at(i_seed) = 0;
      isLocalMax = false;
    }

    towerMapSeed_isLocalMax.push_back(isLocalMax);
  }
  
  
  // Step 3, if seed is local max then make jet
  std::vector<std::shared_ptr<JetAlg::L1Jet>> js;
  TString jetname = "jJetsFromMap";
  if(JetAlg::m_JetMap.find(jetname) == JetAlg::m_JetMap.end() )
    JetAlg::m_JetMap[jetname] = js;

  std::vector<std::shared_ptr<JetAlg::L1Jet>> ss, ss_localMax;
  TString seedname = "jSeedsFromMap";
  if(m_saveSeeds) {
    if(JetAlg::m_JetMap.find(seedname) == JetAlg::m_JetMap.end() )
      JetAlg::m_JetMap[seedname] = ss;
    if(JetAlg::m_JetMap.find(seedname+"_localMax") == JetAlg::m_JetMap.end() )
      JetAlg::m_JetMap[seedname+"_localMax"] = ss_localMax;
  }

  for(int i_jet = 0; i_jet < int(towerMap_jetEta.size()); i_jet++) {
    if(m_saveSeeds && towerMapSeed_ET.at(i_jet) > 0) {
      std::shared_ptr<JetAlg::L1Jet> s = std::make_shared<JetAlg::L1Jet>(towerMap_jetEta.at(i_jet), towerMap_jetPhi.at(i_jet), towerMapSeed_ET.at(i_jet));
      JetAlg::m_JetMap[seedname].push_back(s);
    }

    if( ! towerMapSeed_isLocalMax.at( towerMap_jetSeed.at(i_jet) ) )
      continue;

    if(m_saveSeeds && towerMapSeed_ET.at(i_jet) > 0) {
      std::shared_ptr<JetAlg::L1Jet> s = std::make_shared<JetAlg::L1Jet>(towerMap_jetEta.at(i_jet), towerMap_jetPhi.at(i_jet), towerMapSeed_ET.at(i_jet));
      JetAlg::m_JetMap[seedname+"_localMax"].push_back(s);
    }

    float jet_ET = 0;
    float jet_totalNoise = 0;
    for(auto tower_index : towerMap_jetTowers.at(i_jet)) {
      int jT_index = towerMap_AODtowersIndices.at(tower_index);
      const xAOD::JGTower *tower = jTs->at(jT_index);
      jet_totalNoise += jT_noise.at(jT_index);
      // add tower only if above N * tower noise
      if(tower->et() > jT_noise.at(jT_index) * m_map_jet_tower_noise_multiplier)
        jet_ET += jTs->at(jT_index)->et();
    }
    if(jet_ET <= jet_totalNoise * m_map_jet_total_noise_multiplier)
      continue;
    if(jet_ET < m_map_jet_min_ET_MeV)
      continue;

    ATH_MSG_DEBUG( "I have a jet! Jet " << i_jet << ", eta = " << towerMap_jetEta.at(i_jet) << ", phi = " << towerMap_jetPhi.at(i_jet) << ", ET = " << jet_ET );
    
    std::shared_ptr<JetAlg::L1Jet> j = std::make_shared<JetAlg::L1Jet>(towerMap_jetEta.at(i_jet), towerMap_jetPhi.at(i_jet), jet_ET);
    JetAlg::m_JetMap[jetname].push_back(j);
    
  }
  
  ATH_MSG_DEBUG("BuildJetFromMap: built " << JetAlg::m_JetMap[jetname].size() << " jets:");
  for(int i=0; i < int(JetAlg::m_JetMap[jetname].size()); i++) {
    ATH_MSG_DEBUG("  " << i << ": eta = " << (*JetAlg::m_JetMap[jetname][i]).eta << ", phi = " << (*JetAlg::m_JetMap[jetname][i]).phi << ", et = " << (*JetAlg::m_JetMap[jetname][i]).et );
  }    

  return StatusCode::SUCCESS;
}


StatusCode JGTowerReader::DumpTowerInfo(const xAOD::JGTowerContainer* jTs, const CaloCellContainer* scells){

  // this function dumps tower information to a text file, which should be copied to TrigT1CaloFexSim/utils/towerMapping/

  std::ofstream outfile;
  outfile.open("towerDump.txt");
  for(unsigned t=0; t < jTs->size(); t++){
    const xAOD::JGTower*tower = jTs->at(t);
    int sampling = -1;
    outfile << "tower " << t << ": eta = " << tower->eta() << ", phi = " << tower->phi() << ", deta = " << tower->deta() << ", dphi = " << tower->dphi() << ", et = " << tower->et() << std::endl;
    outfile << "  sampling = " << tower->sampling();

    // The aim here is to get supercell layers, especially in order to identify FCAL0 in the forward region
    // this method gives something else, any merge requests to make it actually output the layer would be much appreciated
    // https://gitlab.cern.ch/will/L1CaloUpgrade/blob/master/L1CaloPhase1/src/AnalysisTOBMaker.cxx maybe has some ways but it seems fairly complicated and not very easily transferrable at all
    std::vector<int> SCIndices = tower->SCIndex();
    outfile << ", " << SCIndices.size() << " SCIndices: ";
    for(int iii = 0; iii < int(SCIndices.size()); iii++) {
      sampling = scells->at(SCIndices.at(iii))->caloDDE()->getSampling();
      outfile << SCIndices.at(iii) << " (" << sampling << "), ";
    }
    outfile << std::endl;
  }
  outfile.close();

  return StatusCode::SUCCESS;
  
}
