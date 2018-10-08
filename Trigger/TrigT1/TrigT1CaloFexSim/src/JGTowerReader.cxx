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
  declareProperty("jJet_threshold",m_jJet_thr=2.0);
  declareProperty("jSeed_size",m_jSeed_size=0.2);
  declareProperty("jMax_r",m_jMax_r=0.4);
  declareProperty("jJet_r",m_jJet_r=0.4);
  declareProperty("gJet_threshold",m_gJet_thr=2.0);
  declareProperty("gSeed_size",m_gSeed_size=0.2);
  declareProperty("gMax_r",m_gMax_r=0.4);
  declareProperty("gJet_r",m_gJet_r=0.4);
}


JGTowerReader::~JGTowerReader() {

  delete jSeeds;
  delete gSeeds;
  delete jMET;
  delete gMET;
  jL1Jets.clear();
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

  for(int i=0;i<jh_noise->GetSize();i++){
     float noise_base = jh_noise->GetBinContent(i+1);
     jT_noise.push_back(noise_base);
     jJet_thr.push_back(noise_base*m_jJet_thr);
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
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  setFilterPassed(false); //optional: start with algorithm not passed

  const xAOD::EventInfo* eventInfo = 0;
  CHECK( evtStore()->retrieve( eventInfo, "EventInfo" ) );

  //events with bcid distance from bunch train <20 vetoed 
  ToolHandle<Trig::IBunchCrossingTool> m_bcTool("Trig::MCBunchCrossingTool/BunchCrossingTool");
  int distFrontBunchTrain = m_bcTool->distanceFromFront(eventInfo->bcid(), Trig::IBunchCrossingTool::BunchCrossings);

  const CaloCellContainer* scells = 0;
  CHECK( evtStore()->retrieve( scells, "SCell") );

  const xAOD::JGTowerContainer* jTowers =0;
  CHECK( evtStore()->retrieve( jTowers,"JTower"));

  const xAOD::JGTowerContainer* gTowers =0;
  CHECK( evtStore()->retrieve( gTowers,"GTower"));

  if(distFrontBunchTrain<20){
    CHECK(JFexAlg(jTowers)); // all the functions for JFex shall be executed here
    CHECK(GFexAlg(gTowers)); // all the functions for GFex shall be executed here
  }

  CHECK(ProcessObjects());  // this is the function to make output as well as memory cleaning
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

// jet algorithms
  if(jSeeds->eta.empty()) CHECK(JetAlg::SeedGrid(jTs,jSeeds));
  
  CHECK(JetAlg::SeedFinding(jTs,jSeeds,m_jSeed_size,m_jMax_r,jJet_thr)); //the diameter of seed, and its range to be local maximum
  CHECK(JetAlg::BuildJet(jTs, jSeeds, jL1Jets,m_jJet_r,jJet_thr)); 
  CHECK(METAlg::BuildMET(jTs,jMET,jT_noise));

  return StatusCode::SUCCESS;
}

StatusCode JGTowerReader::GFexAlg(const xAOD::JGTowerContainer* gTs){

// jet algorithms
  if(gSeeds->eta.empty()) CHECK(JetAlg::SeedGrid(gTs,gSeeds));
  CHECK(JetAlg::SeedFinding(gTs,gSeeds,m_gSeed_size,m_gMax_r,gJet_thr)); // the diameter of seed, and its range to be local maximum
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


  jL1Jets.clear();
  gL1Jets.clear();
  jSeeds->et.clear();
  jSeeds->local_max.clear();
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

