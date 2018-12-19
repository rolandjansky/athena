/*
 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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



JGTowerReader::JGTowerReader( const std::string& name, ISvcLocator* pSvcLocator ) : 
AthAlgorithm( name, pSvcLocator ),
histSvc("THistSvc",name){

  declareProperty("outputNoise",m_outputNoise=false);
  declareProperty("debugJetAlg", m_debugJetAlg=false);
  declareProperty("dumpTowersEtaPhi",m_dumpTowersEtaPhi=false);
  declareProperty("dumpSeedsEtaPhi",m_dumpSeedsEtaPhi=false);
  declareProperty("noise_file",m_noise_file="");

  declareProperty("makeSquareJets", m_makeSquareJets = true);
  declareProperty("jJet_threshold",m_jJet_thr=2.0);
  declareProperty("jSeed_size",m_jSeed_size=0.2);
  declareProperty("jMax_r",m_jMax_r=0.4);
  declareProperty("jJet_r",m_jJet_r=0.4);

  declareProperty("makeRoundJets", m_makeRoundJets = false); 
  declareProperty("jJet_jet_threshold",m_jJet_jet_thr=2.0);
  declareProperty("jJetSeed_size",m_jJetSeed_size=0.3);
  declareProperty("jJet_max_r",m_jJet_max_r=0.4);
  declareProperty("jJet_jet_r",m_jJet_jet_r=0.4);

  declareProperty("plotSeeds", m_plotSeeds = false);

  declareProperty("gJet_threshold",m_gJet_thr=2.0);
  declareProperty("gSeed_size",m_gSeed_size=0.2);
  declareProperty("gMax_r",m_gMax_r=1.0);  //gFEX constructs large radius jets
  declareProperty("gJet_r",m_gJet_r=1.0);

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

  TFile*noise = new TFile(m_noise_file.c_str());
  TH1F*jh_noise = (TH1F*)noise->Get("jT_noise");
  TH1F*gh_noise = (TH1F*)noise->Get("gT_noise");

  for(int i=0;i<jh_noise->GetSize();i++){ // nbins + 2
    float noise_base = jh_noise->GetBinContent(i+1);
    jT_noise.push_back(noise_base);
    jJet_thr.push_back(noise_base*m_jJet_thr);
    jJet_jet_thr.push_back(noise_base*m_jJet_jet_thr);
  }
  
  for(int i=0;i<gh_noise->GetSize();i++){
     float noise_base = gh_noise->GetBinContent(i+1);
     gT_noise.push_back(noise_base);
     gJet_thr.push_back(noise_base*m_gJet_thr);
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

  const xAOD::JGTowerContainer* gTowers =0;
  CHECK( evtStore()->retrieve( gTowers,"GTower"));

  ATH_MSG_DEBUG ("Successfully retrieved cells, jTowers and gTowers");

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
  
  // dump tower eta and phi
  if(m_dumpTowersEtaPhi) {
    std::cout << "tower eta phi" << std::endl;
    std::cout << "i_tower" << "\t" << "eta" << "\t" << "phi" << std::endl;
    for(unsigned t=0; t<jTs->size(); t++){
      const xAOD::JGTower *tower = jTs->at(t);
      std::cout << t << "\t" << tower->eta() << "\t" << tower->phi() << std::endl;
    }
    m_dumpTowersEtaPhi = false; // only do this once per run
  }

  // sort out the wrong-size list of noise vector
  if(jTs->size() > jT_noise.size()) {
    ATH_MSG_ERROR("Found " << jTs->size() << " jTowers, but the noise vector only has " << jT_noise.size() << " entries");
    ATH_MSG_WARNING("Setting the jTower noise vector to have the right number of entries, all 500");
    jT_noise.clear();
    jJet_thr.clear();
    jJet_jet_thr.clear();
    for(int i=0; i<int(jTs->size()); i++){
      jT_noise.push_back(500);
      jJet_thr.push_back(500 * m_jJet_thr);
      jJet_jet_thr.push_back(500 * m_jJet_jet_thr);
    }
  }


  if(m_makeSquareJets) {
    // find all seeds
    // the diameter of seed, and its range to be local maximum
    // Careful to ensure the range set to be no tower double counted
    if(JetAlg::m_SeedMap.find("jSeeds")==JetAlg::m_SeedMap.end())CHECK(JetAlg::SeedGrid(jTs,"jSeeds",m_dumpSeedsEtaPhi));
    ATH_MSG_DEBUG("JFexAlg: SeedFinding with jSeeds; m_jSeed_size = " << m_jSeed_size << ", m_jMax_r = " << m_jMax_r);
    CHECK(JetAlg::SeedFinding(jTs,"jSeeds",m_jSeed_size,m_jMax_r,jJet_thr,m_debugJetAlg));
    ATH_MSG_DEBUG("JFexAlg: BuildJet");
    CHECK(JetAlg::BuildJet(jTs, "jSeeds", "jL1Jet", m_jJet_r, jJet_thr, m_debugJetAlg));
  }
  if(m_makeRoundJets) {
    if(JetAlg::m_SeedMap.find("jRoundSeeds")==JetAlg::m_SeedMap.end())CHECK(JetAlg::SeedGrid(jTs,"jRoundSeeds",m_dumpSeedsEtaPhi));
    ATH_MSG_DEBUG("JFexAlg: SeedFinding with jJetSeeds; m_jJetSeed_size = " << m_jJetSeed_size << ", m_jJet_max_r = " << m_jJet_max_r);
    CHECK(JetAlg::SeedFinding(jTs,"jRoundSeeds",m_jJetSeed_size,m_jJet_max_r,jJet_jet_thr,m_debugJetAlg));
    ATH_MSG_DEBUG("JFexAlg: BuildRoundJet");
    CHECK(JetAlg::BuildRoundJet(jTs, "jRoundSeeds", "jRoundJet", m_jJet_jet_r, jJet_jet_thr, m_debugJetAlg));
  }
  
  ATH_MSG_DEBUG("JFexAlg: BuildMET");
  CHECK(METAlg::Baseline_MET(jTs, "jBaseline", jT_noise, m_useNegTowers));
  ATH_MSG_DEBUG("JFexAlg: Done");

  return StatusCode::SUCCESS;
}

StatusCode JGTowerReader::GFexAlg(const xAOD::JGTowerContainer* gTs){

// jet algorithms
  if(JetAlg::m_SeedMap.find("gSeeds")==JetAlg::m_SeedMap.end()) CHECK(JetAlg::SeedGrid(gTs,"gSeeds",m_dumpSeedsEtaPhi));
  CHECK(JetAlg::SeedFinding(gTs,"gSeeds",m_gSeed_size,m_gMax_r,gJet_thr,m_debugJetAlg));

  // sort out the wrong-size list of noise vector
  if(gTs->size() > gT_noise.size()) {
    ATH_MSG_ERROR("Found " << gTs->size() << " gTowers, but the noise vector only has " << gT_noise.size() << " entries");
    ATH_MSG_WARNING("Setting the gTower noise vector to have the right number of entries, all 1000");
    gT_noise.clear();
    gJet_thr.clear();
    for(int i=0; i<int(gTs->size()); i++){
      gT_noise.push_back(1000);
      gJet_thr.push_back(1000 * m_gJet_thr);
    }
  }
  
  // CHECK(JetAlg::SeedFinding(gTs,gSeeds,m_gSeed_size,m_gMax_r,gJet_thr)); // the diameter of seed, and its range to be local maximum
                                                                         // Careful to ensure the range set to be no tower double counted
  //CHECK(JetAlg::BuildJet(gTs,gSeeds,gL1Jets,m_gJet_r,gJet_thr, m_debugJetAlg)); //default gFex jets are cone jets wih radius of 1.0
  
  CHECK(JetAlg::BuildFatJet(*gTs, "gL1Jets", m_gJet_r, gJet_thr));
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

  // Output Jets
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

