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
  declareProperty("gMax_r",m_gMax_r=0.4);
  declareProperty("gJet_r",m_gJet_r=0.4);
}


JGTowerReader::~JGTowerReader() {
  delete jSeeds;
  delete jJetSeeds;
  delete gSeeds;
  delete jMET;
  delete gMET;
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

  //events with bcid distance from bunch train <20 vetoed 
  ToolHandle<Trig::IBunchCrossingTool> m_bcTool("Trig::MCBunchCrossingTool/BunchCrossingTool");
  int distFrontBunchTrain = m_bcTool->distanceFromFront(eventInfo->bcid(), Trig::IBunchCrossingTool::BunchCrossings);

  ATH_MSG_DEBUG ("dist from front of bunch train is " << distFrontBunchTrain << "...");
  if(distFrontBunchTrain<0) {
    ATH_MSG_DEBUG ("... hasn't worked, ignoring");
  }
  else if(distFrontBunchTrain<20) {
    ATH_MSG_DEBUG ("... skipping event");
    return StatusCode::SUCCESS;
  }
  else {
    ATH_MSG_DEBUG ("... continuing");
  }

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

  // fill the seed vectors with all jtower positions
  if(m_makeSquareJets) {
    if(jSeeds->eta.empty()) CHECK(JetAlg::SeedGrid(jTs,jSeeds));
  }
  if(m_makeRoundJets) {
    if(jJetSeeds->eta.empty()) CHECK(JetAlg::SeedGrid(jTs,jJetSeeds));
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

  // find all seeds
  ATH_MSG_DEBUG("JFexAlg: SeedFinding");
  // the diameter of seed, and its range to be local maximum
  // Careful to ensure the range set to be no tower double counted
  if(m_makeSquareJets) {
    ATH_MSG_DEBUG("jSeeds: m_jSeed_size = " << m_jSeed_size << ", m_jMax_r = " << m_jMax_r);
    CHECK(JetAlg::SeedFinding(jTs,jSeeds,m_jSeed_size,m_jMax_r,jJet_thr));
  }
  if(m_makeRoundJets) {
    ATH_MSG_DEBUG("jJetSeeds: m_jJetSeed_size = " << m_jJetSeed_size << ", m_jJet_max_r = " << m_jJet_max_r);
    CHECK(JetAlg::SeedFinding(jTs,jJetSeeds,m_jJetSeed_size,m_jJet_max_r,jJet_jet_thr));
  }

  // compare jSeeds and jJetSeeds - they should be identical
  // but they are not.......
  if(m_makeSquareJets && m_makeRoundJets) {
    for(unsigned iseed_eta=0; iseed_eta<jSeeds->eta.size(); iseed_eta++){
      for(unsigned iseed_phi=0; iseed_phi<jSeeds->phi.at(iseed_eta).size(); iseed_phi++){
        if(jSeeds->local_max.at(iseed_eta).at(iseed_phi) || jJetSeeds->local_max.at(iseed_eta).at(iseed_phi)) {
          std::cout << iseed_eta << ", " << iseed_phi << " = " << jSeeds->eta.at(iseed_eta) << ", " << jSeeds->phi.at(iseed_eta).at(iseed_phi) << " - pt: " << jSeeds->et.at(iseed_eta).at(iseed_phi) << " and " << jJetSeeds->et.at(iseed_eta).at(iseed_phi) << "; local max: " << jSeeds->local_max.at(iseed_eta).at(iseed_phi) << " and " << jJetSeeds->local_max.at(iseed_eta).at(iseed_phi) << std::endl;
        }
      }
    }
  }
  
  // build initial JFexjet
  if(m_makeSquareJets) {
    ATH_MSG_DEBUG("JFexAlg: BuildJet");
    CHECK(JetAlg::BuildJet(jTs, jSeeds, jL1Jets, m_jJet_r, jJet_thr)); 
  }
  
  // build round JFexJe
  if(m_makeRoundJets) {
    ATH_MSG_DEBUG("JFexAlg: BuildRoundJet");
    CHECK(JetAlg::BuildRoundJet(jTs, jJetSeeds, jJet_L1Jets, m_jJet_jet_r, jJet_jet_thr)); 
  }

  ATH_MSG_DEBUG("JFexAlg: BuildMET");
  CHECK(METAlg::BuildMET(jTs, jMET, jT_noise));

  ATH_MSG_DEBUG("JFexAlg: Done");
  return StatusCode::SUCCESS;
}

StatusCode JGTowerReader::GFexAlg(const xAOD::JGTowerContainer* gTs){

// jet algorithms
  if(gSeeds->eta.empty()) CHECK(JetAlg::SeedGrid(gTs,gSeeds));

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
  
  CHECK(JetAlg::SeedFinding(gTs,gSeeds,m_gSeed_size,m_gMax_r,gJet_thr)); // the diameter of seed, and its range to be local maximum
                                                                         // Careful to ensure the range set to be no tower double counted
  CHECK(JetAlg::BuildJet(gTs,gSeeds,gL1Jets,m_gJet_r,gJet_thr)); //default gFex jets are cone jets wih radius of 1.0
  CHECK(METAlg::BuildMET(gTs,gMET,gT_noise));

  return StatusCode::SUCCESS;
}


StatusCode JGTowerReader::ProcessObjects(){

  // Ouptut Jets
  xAOD::JetRoIAuxContainer* jFexJetContAux = new xAOD::JetRoIAuxContainer();
  xAOD::JetRoIContainer* jFexJetCont = new xAOD::JetRoIContainer(); 
  jFexJetCont->setStore(jFexJetContAux);

  for(unsigned j=0;j<jL1Jets.size();j++  ){
     xAOD::JetRoI* jFexJet = new xAOD::JetRoI();     
     jFexJetCont->push_back(jFexJet);
     JetAlg::L1Jet jet = jL1Jets.at(j);
     CHECK(HistBookFill("jJet_et",50,0,500,jet.et/1000.,1.));
     CHECK(HistBookFill("jJet_eta",49,-4.9,4.9,jet.eta,1.));
     CHECK(HistBookFill("jJet_phi",31,-3.1416,3.1416,jet.phi,1.));
     jFexJet->initialize(0x0,jet.eta,jet.phi);
     jFexJet->setEt8x8(jet.et);
  }
  for(unsigned j=0;j<jJet_L1Jets.size();j++  ){
     JetAlg::L1Jet jet = jJet_L1Jets.at(j);
     CHECK(HistBookFill("jJet_round_et",50,0,500,jet.et/1000.,1.));
     CHECK(HistBookFill("jJet_round_eta",49,-4.9,4.9,jet.eta,1.));
     CHECK(HistBookFill("jJet_round_phi",31,-3.1416,3.1416,jet.phi,1.));
  }
  for(unsigned j=0;j<gL1Jets.size();j++  ){
     JetAlg::L1Jet jet = gL1Jets.at(j);
     CHECK(HistBookFill("gJet_et",50,0,500,jet.et/1000.,1.));
     CHECK(HistBookFill("gJet_eta",49,-4.9,4.9,jet.eta,1.));
     CHECK(HistBookFill("gJet_phi",31,-3.1416,3.1416,jet.phi,1.));

  CHECK(evtStore()->record(jFexJetCont,"jFexJets"));
  CHECK(evtStore()->record(jFexJetContAux,"jFexJetsAux."));
  //output MET
  xAOD::EnergySumRoIAuxInfo* jFexMETContAux = new xAOD::EnergySumRoIAuxInfo();
  xAOD::EnergySumRoI* jFexMETCont = new xAOD::EnergySumRoI();
  jFexMETCont->setStore(jFexMETContAux);

  CHECK(HistBookFill("jMet_et",50,0,500,jMET->et/1000.,1.));
  CHECK(HistBookFill("jMet_phi",31,-3.1416,-3.1416,jMET->phi,1.));
  jFexMETCont->setEnergyX(jMET->et*cos(jMET->phi));
  jFexMETCont->setEnergyY(jMET->et*sin(jMET->phi));  
  CHECK(evtStore()->record(jFexMETCont,"jFexMET"));
  CHECK(evtStore()->record(jFexMETContAux,"jFexMETAux."));

  
  if(m_plotSeeds) {
    for(unsigned iseed_eta=0; iseed_eta<jSeeds->eta.size(); iseed_eta++){
      for(unsigned iseed_phi=0; iseed_phi<jSeeds->phi.at(iseed_eta).size(); iseed_phi++){
        if(jSeeds->local_max.at(iseed_eta).at(iseed_phi)) {
          CHECK(HistBookFill("jSeed_et",50,0,500,jSeeds->et.at(iseed_eta).at(iseed_phi)/1000.,1.));
          CHECK(HistBookFill("jSeed_eta",49,-4.9,4.9,jSeeds->eta.at(iseed_eta),1.));
          CHECK(HistBookFill("jSeed_phi",31,-3.1416,3.1416,jSeeds->phi.at(iseed_eta).at(iseed_phi),1.));
        }
      }
    }
    for(unsigned iseed_eta=0; iseed_eta<jJetSeeds->eta.size(); iseed_eta++){
      for(unsigned iseed_phi=0; iseed_phi<jJetSeeds->phi.at(iseed_eta).size(); iseed_phi++){
        if(jJetSeeds->local_max.at(iseed_eta).at(iseed_phi)) {
          CHECK(HistBookFill("jJetSeed_et",50,0,500,jJetSeeds->et.at(iseed_eta).at(iseed_phi)/1000.,1.));
          CHECK(HistBookFill("jJetSeed_eta",49,-4.9,4.9,jJetSeeds->eta.at(iseed_eta),1.));
          CHECK(HistBookFill("jJetSeed_phi",31,-3.1416,3.1416,jJetSeeds->phi.at(iseed_eta).at(iseed_phi),1.));
        }
      }
    }
  }
  

  jL1Jets.clear();
  jJet_L1Jets.clear();
  gL1Jets.clear();
  jSeeds->et.clear();
  jSeeds->local_max.clear();
  jJetSeeds->et.clear();
  jJetSeeds->local_max.clear();
  gSeeds->et.clear();
  gSeeds->local_max.clear();


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

