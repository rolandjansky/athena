/*
 *   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// TrigT1CaloFexSim includes
#include "TrigT1CaloFexSim/JGTowerReader.h"
#include "TrigT1CaloFexSim/JGTower.h"
#include "TrigT1CaloFexSim/JetAlg.h"
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


JGTowerReader::JGTowerReader( const std::string& name, ISvcLocator* pSvcLocator ) : 
AthAlgorithm( name, pSvcLocator ),
histSvc("THistSvc",name){

  declareProperty("outputNoise",m_outputNoise=false);
  declareProperty("debugJetAlg", m_debugJetAlg=false);
  declareProperty("dumpTowerInfo", m_dumpTowerInfo=false);
  declareProperty("dumpSeedsEtaPhi", m_dumpSeedsEtaPhi=false);
  declareProperty("noise_file", m_noise_file="");

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

  declareProperty("gJet_seed_size", m_gJet_seed_size=0.2);
  declareProperty("gJet_max_r", m_gJet_max_r=1.0);  //gFEX constructs large radius jets
  declareProperty("gJet_r", m_gJet_r=1.0);
  declareProperty("gJet_seed_tower_noise_multiplier", m_gJet_seed_tower_noise_multiplier = 1.0);
  declareProperty("gJet_seed_total_noise_multiplier", m_gJet_seed_total_noise_multiplier = 1.0);
  declareProperty("gJet_seed_min_ET_MeV", m_gJet_seed_min_ET_MeV = 2000.0);
  declareProperty("gJet_jet_tower_noise_multiplier", m_gJet_jet_tower_noise_multiplier = 1.0);
  declareProperty("gJet_jet_total_noise_multiplier", m_gJet_jet_total_noise_multiplier = 0.0);
  declareProperty("gJet_jet_min_ET_MeV", m_gJet_jet_min_ET_MeV = 2000.0);

  declareProperty("useRMS", m_useRMS=false);
  declareProperty("useMedian", m_useMedian=false);
  declareProperty("useNegTowers", m_useNegTowers=false);
  declareProperty("pTcone_cut", m_pTcone_cut=25);  //cone threshold for Jets without Jets: declared in GeV
}


JGTowerReader::~JGTowerReader() {
  delete jSeeds;
  delete jJetSeeds;
  delete gSeeds;

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

  std::ifstream noise_exist(m_noise_file.c_str());
  if(noise_exist){
    TFile*noise = new TFile(m_noise_file.c_str());
    TH1F*jh_noise = (TH1F*)noise->Get("jT_noise");
    TH1F*gh_noise = (TH1F*)noise->Get("gT_noise");
  
    for(int i=0;i<jh_noise->GetSize();i++){
       jT_noise.push_back( jh_noise->GetBinContent(i+1) );
    }
  
    for(int i=0;i<gh_noise->GetSize();i++){
       gT_noise.push_back( gh_noise->GetBinContent(i+1) );
    } 
  }
  // read in the tower map
  if(m_makeJetsFromMap) {
    CHECK( ReadTowerMap() );
  }

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
  int distFrontBunchTrain = m_bcTool->distanceFromFront(eventInfo->bcid(), Trig::IBunchCrossingTool::BunchCrossings);
  CHECK(HistBookFill("distFrontBunchTrain",100,0,100, distFrontBunchTrain, 1.));

  const CaloCellContainer* scells = 0;
  CHECK( evtStore()->retrieve( scells, "SCell") );

  const xAOD::JGTowerContainer* jTowers =0;
  CHECK( evtStore()->retrieve( jTowers,"JTower"));
  //when noise file is not available, set the noise as constant for EM, Had, and FCal respectively
  if(jT_noise.size()==0){
    for( const auto &jT : *jTowers){
       if(jT->sampling()==0) jT_noise.push_back(450);
       else if(jT->sampling()==1) jT_noise.push_back(2400);
       else jT_noise.push_back(2000);
    }
  }

  const xAOD::JGTowerContainer* gTowers =0;
  CHECK( evtStore()->retrieve( gTowers,"GTower"));
  //when noise file is not available, set the noise as constant for EM, Had, and FCal respectively
  if(gT_noise.size()==0){
    for(const auto &gT : *gTowers){
       gT_noise.push_back(1500);
    }
  }


  ATH_MSG_DEBUG ("Successfully retrieved cells, jTowers and gTowers");

  // make and check tower mapping
  if(m_eventCount==1 && m_dumpTowerInfo) {
    CHECK( DumpTowerInfo(jTowers, scells) );
  }
  if(m_eventCount==1 && m_makeJetsFromMap) {
    CHECK( CheckTowerMap(jTowers) );
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

//L1Calo algorithms are here:
StatusCode JGTowerReader::JFexAlg(const xAOD::JGTowerContainer* jTs){

  ATH_MSG_DEBUG("Found " << jTs->size() << " jTowers");
  


  if(m_makeSquareJets) {
    // find all seeds
    // the diameter of seed, and its range to be local maximum
    // Careful to ensure the range set to be no tower double counted
    if( JetAlg::m_SeedMap.find("jSeeds") == JetAlg::m_SeedMap.end() )
      CHECK( JetAlg::SeedGrid(jTs, "jSeeds", m_dumpSeedsEtaPhi) );
    ATH_MSG_DEBUG("JFexAlg: SeedFinding with jSeeds; m_jJet_seed_size = " << m_jJet_seed_size << ", m_jJet_max_r = " << m_jJet_max_r);
    CHECK( JetAlg::SeedFinding(jTs, "jSeeds", m_jJet_seed_size, m_jJet_max_r, jT_noise, m_jJet_seed_tower_noise_multiplier, m_jJet_seed_total_noise_multiplier, m_jJet_seed_min_ET_MeV, m_debugJetAlg) );
    ATH_MSG_DEBUG("JFexAlg: BuildJet");
    CHECK( JetAlg::BuildJet(jTs, "jSeeds", "jJets", m_jJet_r, jT_noise, m_jJet_jet_tower_noise_multiplier, m_jJet_jet_total_noise_multiplier, m_jJet_jet_min_ET_MeV, m_debugJetAlg, m_saveSeeds) );
  }
  if(m_makeRoundJets) {
    if( JetAlg::m_SeedMap.find("jRoundSeeds") == JetAlg::m_SeedMap.end() )
      CHECK( JetAlg::SeedGrid(jTs, "jRoundSeeds", m_dumpSeedsEtaPhi) );
    ATH_MSG_DEBUG("JFexAlg: SeedFinding with jJetSeeds; m_jJet_seed_size = " << m_jJet_seed_size << ", m_jJet_max_r = " << m_jJet_max_r);
    CHECK( JetAlg::SeedFinding(jTs, "jRoundSeeds", m_jJetRound_seed_size, m_jJetRound_max_r, jT_noise, m_jJetRound_seed_tower_noise_multiplier, m_jJetRound_seed_total_noise_multiplier, m_jJetRound_seed_min_ET_MeV, m_debugJetAlg) );
    ATH_MSG_DEBUG("JFexAlg: BuildRoundJet");
    CHECK( JetAlg::BuildRoundJet(jTs, "jRoundSeeds", "jRoundJets", m_jJetRound_r, jT_noise, m_jJetRound_jet_tower_noise_multiplier, m_jJetRound_jet_total_noise_multiplier, m_jJetRound_jet_min_ET_MeV, m_debugJetAlg, m_saveSeeds) );
  }
  if(m_makeJetsFromMap) {
    ATH_MSG_DEBUG("JFexAlg: Build jets from map");
    CHECK( BuildJetsFromMap(jTs) );
  }
  
  ATH_MSG_DEBUG("JFexAlg: BuildMET");
  CHECK(METAlg::Baseline_MET(jTs, "jBaseline", jT_noise, m_useNegTowers));
  ATH_MSG_DEBUG("JFexAlg: Done");

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
  
  // jet algorithms
  if(JetAlg::m_SeedMap.find("gSeeds") == JetAlg::m_SeedMap.end())
    CHECK(JetAlg::SeedGrid(gTs,"gSeeds",m_dumpSeedsEtaPhi));
  CHECK(JetAlg::SeedFinding(gTs, "gSeeds", m_gJet_seed_size, m_gJet_max_r, gT_noise, m_gJet_seed_tower_noise_multiplier, m_gJet_seed_total_noise_multiplier, m_gJet_seed_min_ET_MeV, m_debugJetAlg));

  // CHECK(JetAlg::SeedFinding(gTs,gSeeds,m_gJet_seed_size,m_gJet_max_r,gJet_thr)); // the diameter of seed, and its range to be local maximum
                                                                         // Careful to ensure the range set to be no tower double counted
  //CHECK(JetAlg::BuildJet(gTs,gSeeds,gL1Jets,m_gJet_r,gJet_thr, m_debugJetAlg)); //default gFex jets are cone jets wih radius of 1.0
  
  CHECK(JetAlg::BuildFatJet(*gTs, "gL1Jets", m_gJet_r, gT_noise, m_gJet_jet_tower_noise_multiplier, m_gJet_jet_total_noise_multiplier, m_gJet_jet_min_ET_MeV));
  //gFEX MET algorithms
  CHECK(METAlg::Baseline_MET(gTs,"gBaseline",gT_noise, m_useNegTowers)); //basic MET reconstruction with a 4 sigma noise cut applied
  CHECK(METAlg::SubtractRho_MET(gTs,"rho_sub",m_useRMS,m_useMedian,m_useNegTowers)); //pileup subtracted MET, can apply dynamic noise cut and use either median or avg rho
  CHECK(METAlg::Softkiller_MET(gTs, "SK", m_useNegTowers) ); //pileup subtracted SoftKiller (with avg rho)
  CHECK(METAlg::JwoJ_MET(gTs,"JwoJ",m_pTcone_cut, m_useNegTowers) ); //Jets without Jets
  CHECK(METAlg::Pufit_MET(gTs,"PUfit", m_useNegTowers) ); //L1 version of PUfit, using gTowers
  
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
    CHECK(HistBookFill(Form("MET_%s_phi",it->first.Data()), 31, -3.1416, 3.1416, met->phi, 1.));
    METCont->setEnergyX(met->et*cos(met->phi));
    METCont->setEnergyY(met->et*sin(met->phi));
    METCont->setEnergyT(met->et);
    CHECK(evtStore()->record(METCont,Form("%s_MET",it->first.Data())));
    CHECK(evtStore()->record(METContAux,Form("%s_METAux",it->first.Data())));
  }
  

  METAlg::m_METMap.clear();
  JetAlg::m_JetMap.clear();
  for ( auto it = JetAlg::m_SeedMap.begin(); it != JetAlg::m_SeedMap.end(); it++ ){
    it->second->local_max.clear();
    it->second->et.clear();
  }

  return StatusCode::SUCCESS;
}

StatusCode JGTowerReader::HistBookFill(const TString name, Int_t nbinsx, const Double_t* xbins, float xvalue,float wei){

  if(std::find(hists.begin(),hists.end(),name)==hists.end()) {
    TH1F*h = new TH1F( name, name, nbinsx, xbins);
    h->Sumw2();
    CHECK( histSvc->regHist(Form("/OUTPUT/%s",name.Data()),h));
    hName[name]=h;
    hName[name]->Fill(xvalue,wei);
    hists.push_back(name);
  }
  else hName[name]->Fill(xvalue,wei);
  return StatusCode::SUCCESS;
}

StatusCode JGTowerReader::HistBookFill(const TString name, Int_t nbinsx, Double_t xbin_down, Double_t xbin_up, float xvalue,float wei){

  if(std::find(hists.begin(),hists.end(),name)==hists.end()) {
    TH1F*h=new TH1F( name, name, nbinsx, xbin_down,xbin_up);
    h->Sumw2();
    CHECK( histSvc->regHist(Form("/OUTPUT/%s",name.Data()),h));
    hName[name]=h;
    hName[name]->Fill(xvalue,wei);
    hists.push_back(name);
  }
  else  hName[name]->Fill(xvalue,wei);
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
  std::string towerMapPath = PathResolverFindDataFile(m_towerMap);
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
        if(int(towerMap_towerEta.size()) != towerNum) {
          ATH_MSG_ERROR("tower map being parsed incorrectly: have " << towerMap_towerEta.size() << " and expect " << towerNum);
          return StatusCode::FAILURE;
        }
          
        // fill eta and phi
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
        if(int(towerMap_seedEta.size()) != seedNum) {
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
        if(int(towerMap_jetEta.size()) != jetNum) {
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


StatusCode JGTowerReader::CheckTowerMap(const xAOD::JGTowerContainer*jTs) {
  ATH_MSG_INFO("Checking tower map");

  // fill std::vector<int> towerMap_AODtowersIndices

  for(int i_towerMap = 0; i_towerMap < int(towerMap_towerEta.size()); i_towerMap++) {
    int tower_index = -1;
    for(int i_jTower = 0; i_jTower < int(jTs->size()); i_jTower++){
      const xAOD::JGTower *tower = jTs->at(i_jTower);

      // exactly equal fails for phi (agrees to 5dp printed)
      if( fabs( 1 - (towerMap_towerEta.at(i_towerMap) / tower->eta() ) ) > 0.0001) 
        continue;
      if( fabs( 1 - (towerMap_towerPhi.at(i_towerMap) / tower->phi() ) ) > 0.0001) 
        continue;
      if(towerMap_towerSampling.at(i_towerMap) != tower->sampling())
        continue;
      tower_index = i_jTower;
      break;
    }
    if(tower_index == -1) {
      ATH_MSG_ERROR("Did not find matching tower in AOD for this one in the map: tower " << i_towerMap << ", eta = " << towerMap_towerEta.at(i_towerMap) << ", phi = " << towerMap_towerPhi.at(i_towerMap) << ", sampling = " << towerMap_towerSampling.at(i_towerMap));
      return StatusCode::FAILURE;
    }
    towerMap_AODtowersIndices.push_back(tower_index);
    // the ith entry of this vector has the jTower index corresponding to the ith tower in the map
  }
  return StatusCode::SUCCESS;
}


StatusCode JGTowerReader::BuildJetsFromMap(const xAOD::JGTowerContainer*jTs) {

  ATH_MSG_DEBUG("BuildJetsFromMap: starting");

  // step 1, make all the seeds
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
      seed_totalNoise += tower->et();
      // ATH_MSG_DEBUG("BuildJetsFromMap: adding to seed " << i_seed << " with tower " << tower_index);
    }
    // set seed ET to 0 if it has less ET than N times the total noise over the seed towers
    if(seed_ET < seed_totalNoise * m_map_seed_total_noise_multiplier)
      seed_ET = 0;
    else
      // ATH_MSG_DEBUG("adding a non-zero seed " << i_seed << " at (eta,phi)=" << towerMap_seedEta[i_seed] << "," << towerMap_seedPhi[i_seed] << ") with ET " << seed_ET);
      ;

    towerMapSeed_ET.push_back(seed_ET);
  }


  // Step 2, check local max of seeds
  for(int i_seed = 0; i_seed < int(towerMap_seedEta.size()); i_seed++) {
    bool isLocalMax = true;
    for(auto seed_index : towerMap_seedLocalMaxSeeds.at(i_seed)) {
      if(towerMapSeed_ET.at(i_seed) <= towerMapSeed_ET.at(seed_index)) {
        isLocalMax = false;
        break;
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
