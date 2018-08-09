/*
 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// TrigT1CaloFexSim includes
#include "TrigT1CaloFexSim/JGTowerReader.h"
#include "TrigT1CaloFexSim/JGTower.h"
#include "TrigT1CaloFexSim/JetAlg.h"
#include "TSystem.h"
#include "xAODEventInfo/EventInfo.h"
//uncomment the line below to use the HistSvc for outputting trees and histograms
#include "GaudiKernel/ITHistSvc.h"
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

#include "TFile.h"



JGTowerReader::JGTowerReader( const std::string& name, ISvcLocator* pSvcLocator ) : AthAlgorithm( name, pSvcLocator ){

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
  //
  //This is called once, before the start of the event loop
  //Retrieves of tools you have configured in the joboptions go here
  //

  //HERE IS AN EXAMPLE
  //We will create a histogram and a ttree and register them to the histsvc
  //Remember to uncomment the configuration of the histsvc stream in the joboptions
  //
  ServiceHandle<ITHistSvc> histSvc("THistSvc",name());

  CHECK( histSvc.retrieve() );
  CHECK(detStore()->retrieve(m_jTowerId));
  CHECK(detStore()->retrieve(m_gTowerId));

  m_jTowerHashMax=m_jTowerId->tower_hash_max();
  m_gTowerHashMax=m_gTowerId->tower_hash_max();
 
  TFile*noise = new TFile(m_noise_file.c_str());
  TH1F*jh_noise = (TH1F*)noise->Get("jT_noise");
  TH1F*gh_noise = (TH1F*)noise->Get("gT_noise");

  for(int i=0;i<m_jTowerHashMax;i++){
     float noise_base = jh_noise->GetBinContent(i+1);
     jT_noise.push_back(noise_base);
     jJet_thr.push_back(noise_base*m_jJet_thr);
  }

  for(int i=0;i<m_gTowerHashMax;i++){
     float noise_base = gh_noise->GetBinContent(i+1);
     gT_noise.push_back(noise_base*2);
     gJet_thr.push_back(noise_base*m_gJet_thr);
  } 

  

  if(m_outputNoise){

    for(int i=0; i<m_jTowerHashMax; i++){

       char hist_name[50];
       sprintf(hist_name,"Hist_Et_jT%d",i);
       hName[hist_name]=new TH1F(hist_name,hist_name,100,-5000,15000);       
       CHECK( histSvc->regHist(Form("/OUTPUT/JTower_Et/%s",hist_name),hName[hist_name]));
    }

    
    int m_gTowerHashMax=m_gTowerId->tower_hash_max();  

    for(int i=0; i<m_gTowerHashMax; i++){
    
       char hist_name[50];
       sprintf(hist_name,"Hist_Et_gT%d",i);
       hName[hist_name]=new TH1F(hist_name,hist_name,100,-7000,18000);
       CHECK( histSvc->regHist(Form("/OUTPUT/GTower_Et/%s",hist_name),hName[hist_name]));
    }

  }

  char hist_name[50];
  sprintf(hist_name,"jJet_et");
  hName[hist_name]=new TH1F(hist_name,hist_name,100,0,500000);
  CHECK( histSvc->regHist(Form("/OUTPUT/%s",hist_name),hName[hist_name]));

  sprintf(hist_name,"gJet_et");
  hName[hist_name]=new TH1F(hist_name,hist_name,100,0,1000000);
  CHECK( histSvc->regHist(Form("/OUTPUT/%s",hist_name),hName[hist_name]));


  sprintf(hist_name,"jJet_eta");
  hName[hist_name]=new TH1F(hist_name,hist_name,49,-4.9,4.9);
  CHECK( histSvc->regHist(Form("/OUTPUT/%s",hist_name),hName[hist_name]));

  sprintf(hist_name,"gJet_eta");
  hName[hist_name]=new TH1F(hist_name,hist_name,49,-4.9,4.9);
  CHECK( histSvc->regHist(Form("/OUTPUT/%s",hist_name),hName[hist_name]));


  sprintf(hist_name,"jJet_phi");
  hName[hist_name]=new TH1F(hist_name,hist_name,62,-3.14,3.14);
  CHECK( histSvc->regHist(Form("/OUTPUT/%s",hist_name),hName[hist_name]));

  sprintf(hist_name,"gJet_phi");
  hName[hist_name]=new TH1F(hist_name,hist_name,62,-3.14,3.14);
  CHECK( histSvc->regHist(Form("/OUTPUT/%s",hist_name),hName[hist_name]));
                                                                            

  sprintf(hist_name,"jMet_et");
  hName[hist_name]=new TH1F(hist_name,hist_name,100,0,500000);
  CHECK( histSvc->regHist(Form("/OUTPUT/%s",hist_name),hName[hist_name]));

  sprintf(hist_name,"gMet_et");
  hName[hist_name]=new TH1F(hist_name,hist_name,100,0,500000);
  CHECK( histSvc->regHist(Form("/OUTPUT/%s",hist_name),hName[hist_name]));

  sprintf(hist_name,"jMet_phi");
  hName[hist_name]=new TH1F(hist_name,hist_name,62,-3.14,3.14);
  CHECK( histSvc->regHist(Form("/OUTPUT/%s",hist_name),hName[hist_name]));

  sprintf(hist_name,"gMet_phi");
  hName[hist_name]=new TH1F(hist_name,hist_name,62,-3.14,3.14);
  CHECK( histSvc->regHist(Form("/OUTPUT/%s",hist_name),hName[hist_name]));


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
  if(distFrontBunchTrain<20)   return StatusCode::SUCCESS;

  const CaloCellContainer* scells = 0;
  CHECK( evtStore()->retrieve( scells, "SCell") );

  const xAOD::JGTowerContainer* jTowers =0;
  CHECK( evtStore()->retrieve( jTowers,"JTower"));

  const xAOD::JGTowerContainer* gTowers =0;
  CHECK( evtStore()->retrieve( gTowers,"GTower"));



  if(m_outputNoise){

    int m_jTowerHashMax=m_jTowerId->tower_hash_max();
    for(int hs=0; hs<m_jTowerHashMax;hs++){
       const xAOD::JGTower*jT = jTowers->at(hs);
       float Et = jT->et();
       if(!Et) continue;
       char hist_name[50];
       sprintf(hist_name,"Hist_Et_jT%d",hs);
       hName[hist_name]->Fill(Et);  
    }

    int m_gTowerHashMax=m_gTowerId->tower_hash_max();
    for(int hs=0; hs<m_gTowerHashMax;hs++){
       const xAOD::JGTower*gT = gTowers->at(hs);
       float Et = gT->et();
       if(!Et) continue;
       char hist_name[50];
       sprintf(hist_name,"Hist_Et_gT%d",hs);
       hName[hist_name]->Fill(Et);
    }
  }

  //
  //Your main analysis code goes here
  //If you will use this algorithm to perform event skimming, you
  //should ensure the setFilterPassed method is called
  //If never called, the algorithm is assumed to have 'passed' by default
  //

  
  CHECK(JFexAlg(jTowers));
  CHECK(GFexAlg(gTowers));
  CHECK(ProcessObject());


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
                                          //Careful to ensure the range set to be no tower double counted
  CHECK(JetAlg::BuildJet(jTs, jJet_thr,jSeeds, jL1Jets,m_jJet_r)); 
  CHECK(METAlg::BuildMET(jTs,jT_noise,jMET));

  return StatusCode::SUCCESS;
}

StatusCode JGTowerReader::GFexAlg(const xAOD::JGTowerContainer* gTs){

// jet algorithms
  if(gSeeds->eta.empty()) CHECK(JetAlg::SeedGrid(gTs,gSeeds));
  CHECK(JetAlg::SeedFinding(gTs,gSeeds,m_gSeed_size,m_gMax_r,gJet_thr)); // the diameter of seed, and its range to be local maximum
                                          // Careful to ensure the range set to be no tower double counted
  CHECK(JetAlg::BuildJet(gTs,gJet_thr,gSeeds,gL1Jets,m_gJet_r)); //default gFex jets are cone jets wih radius of 1.0
  CHECK(METAlg::BuildMET(gTs,gT_noise,gMET));

  return StatusCode::SUCCESS;
}

StatusCode JGTowerReader::ProcessObject(){

  
  for(unsigned j=0;j<jL1Jets.size();j++  ){
     JetAlg::L1Jet jet = jL1Jets.at(j);
     hName["jJet_et"]->Fill(jet.et);
     hName["jJet_eta"]->Fill(jet.eta);
     hName["jJet_phi"]->Fill(jet.phi);
  }
  for(unsigned j=0;j<gL1Jets.size();j++  ){
     JetAlg::L1Jet jet = gL1Jets.at(j);
     hName["gJet_et"]->Fill(jet.et);
     hName["gJet_eta"]->Fill(jet.eta);
     hName["gJet_phi"]->Fill(jet.phi);

  }
  hName["jMet_et"]->Fill(jMET->et);
  hName["gMet_et"]->Fill(gMET->et);
  hName["jMet_phi"]->Fill(jMET->phi);
  hName["gMet_phi"]->Fill(gMET->phi);
  
  jL1Jets.clear();
  gL1Jets.clear();
  jSeeds->et.clear();
  jSeeds->local_max.clear();
  gSeeds->et.clear();
  gSeeds->local_max.clear();
  return StatusCode::SUCCESS;
}
