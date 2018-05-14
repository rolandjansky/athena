// TrigT1CaloFexSim includes
#include "TrigT1CaloFexSim/JGTowerReader.h"
#include "TrigT1CaloFexSim/JGTower.h"
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
  declareProperty("jSeed_size",m_jSeed_size=0.2);
  declareProperty("jJet_r",m_jJet_r=0.4);
  declareProperty("gSeed_size",m_gSeed_size=0.4);
  declareProperty("gJet_r",m_gJet_r=1.0);

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
     jT_noise.push_back(noise_base*2);
  }

  for(int i=0;i<m_gTowerHashMax;i++){
     float noise_base = gh_noise->GetBinContent(i+1);
     gT_noise.push_back(noise_base*2);
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
  if(jSeeds->eta.empty()) CHECK(SeedGrid(jTs,jSeeds));
  CHECK(SeedFinding(jTs,jSeeds,0.2,0.8,jT_noise)); //the diameter of seed, and its range to be local maximum
                                          //Careful to ensure the range set to be no tower double counted
  CHECK(BuildJet(jTs, jT_noise,jSeeds, jL1Jets,0.4)); 
  CHECK(BuildMET(jTs,jT_noise,jMET));

  return StatusCode::SUCCESS;
}

StatusCode JGTowerReader::GFexAlg(const xAOD::JGTowerContainer* gTs){

// jet algorithms
  if(gSeeds->eta.empty()) CHECK(SeedGrid(gTs,gSeeds));
  CHECK(SeedFinding(gTs,gSeeds,0.4,2.0,gT_noise)); // the diameter of seed, and its range to be local maximum
                                          // Careful to ensure the range set to be no tower double counted
  CHECK(BuildJet(gTs,gT_noise,gSeeds,gL1Jets,1.0)); //default gFex jets are cone jets wih radius of 1.0
  CHECK(BuildMET(gTs,gT_noise,gMET));

  return StatusCode::SUCCESS;
}

//function to build up the location of all seeds
StatusCode JGTowerReader::SeedGrid(const xAOD::JGTowerContainer*towers, JGTowerReader::Seed*seeds){


  std::vector<float> seed_candi_eta;

  std::vector<std::pair<int,int>> seeds_candi;
  unsigned t_size = towers->size();
  float t_maxi=-999;

  for(unsigned i=0; i<t_size;i++){
     const xAOD::JGTower*tower = towers->at(i);
     float t_eta = tower->eta();
     if(t_eta>t_maxi) t_maxi=t_eta;
  }

  //arrange seeds in the order from eta=-4.9 to eta=4.9
  for(float t_eta=t_maxi-0.01; t_eta<t_maxi;){

     float tmp=999;

     for(unsigned i=0;i<t_size;i++){
        const xAOD::JGTower*tower = towers->at(i);
        std::vector<int> SC_indices = tower->SCIndex();
        if(SC_indices.size()==0) continue;

        float candi_eta = tower->eta()-tower->deta()/2;
        if(tower->eta()<0) candi_eta = tower->eta()+tower->deta()/2;
        if(fabs(tower->eta())>3.2) candi_eta = tower->eta();

        
        if( candi_eta<tmp && seed_candi_eta.size()==0) { tmp=candi_eta; t_eta = candi_eta;}
        else if( candi_eta > t_eta && candi_eta<tmp )  {  tmp = candi_eta; }

     }
     t_eta = tmp;
     seed_candi_eta.push_back(t_eta);
  }

  //arrange seeds in the order from phi=-pi to phi=pi
  for(unsigned i=0; i<seed_candi_eta.size(); i++){

     (seeds->eta).push_back(seed_candi_eta.at(i));
     std::vector<float> tmp_phi;    
     for(float t_phi=-TMath::Pi(); ;){

        float dphi=0;
        
        for(unsigned t=0;t<t_size;){
           const xAOD::JGTower*tower = towers->at(t);
           t++;
           std::vector<int> SC_indices = tower->SCIndex();
           if(SC_indices.size()==0) continue;

           float eta = tower->eta()-tower->deta()/2;
           if(tower->eta()<0) eta = tower->eta()+tower->deta()/2;
           if(fabs(tower->eta())>3.2)  eta = tower->eta();
           if(eta!=seed_candi_eta.at(i)) continue;
           dphi = tower->dphi();
           break;
        }
       tmp_phi.push_back(t_phi);
       t_phi+=dphi;

       if(t_phi>TMath::Pi()) break;
     }
  (seeds->phi).push_back(tmp_phi);
  }

  return StatusCode::SUCCESS;
}

//To find the seeds as local maxima 
StatusCode JGTowerReader::SeedFinding(const xAOD::JGTowerContainer*towers, JGTowerReader::Seed*seeds, float seed_size,float range, std::vector<float> noise){

   // get the energy of each seeds which is defined as 2x2 towers in barrel and endcap, and single tower in fcal
   
   for(unsigned i=0; i<seeds->eta.size(); i++){
      std::vector<float> tmp_et;
      for(unsigned ii=0; ii<seeds->phi.at(i).size(); ii++){
         float et=0;
         float thr=0;
         float eta=seeds->eta.at(i);
         float phi=seeds->phi.at(i).at(ii);
         for(unsigned t=0; t<towers->size(); t++){
            const xAOD::JGTower*tower = towers->at(t);
            if(!inBox(tower->eta(),eta,seed_size/2,tower->phi(),phi,seed_size/2)) continue;
            if( tower->et()> 5*noise.at(t) ) et+= tower->et();
            thr += noise.at(t);
         }
         if(et<thr*6) et = 0;
         tmp_et.push_back(et);
      }
      seeds->et.push_back(tmp_et);
   }

   // determine whether the seed is with locally maximal energy
   for(unsigned iseed_eta=0; iseed_eta<seeds->eta.size(); iseed_eta++){
      std::vector<bool> tmp_max;
      for(unsigned iseed_phi=0; iseed_phi<seeds->phi.at(iseed_eta).size(); iseed_phi++){
         float et = seeds->et.at(iseed_eta).at(iseed_phi);
         // only seeds with Et>5GeV is available
         if(et<5000) {
           tmp_max.push_back(0);
           continue;
         }
         // eta_n: et higher than all seeds with smaller eta
         // eta_p: et higher than all seeds with larger eta
         // eta_0: et higher than the other seeds along the same eta ring

         bool eta_n=1, eta_p=1, eta_0=1;

         for(unsigned i=iseed_eta+1; ;i++){
            if(i>=seeds->eta.size()) break;
            if(fabs(seeds->eta.at(i)-seeds->eta.at(iseed_eta))>range) break;
            for(unsigned ii=0; ii<seeds->phi.at(i).size(); ii++){
               
               float dphi = deltaPhi(seeds->phi.at(iseed_eta).at(iseed_phi),seeds->phi.at(i).at(ii));
               if(dphi>range) continue;
               if(seeds->et.at(iseed_eta).at(iseed_phi)<=seeds->et.at(i).at(ii)){
                 eta_p = false;
                 break;  
               }
            }
         }

         for(int i=iseed_eta-1; ;i--){
            if(i<0) break;
            if(fabs(seeds->eta.at(iseed_eta)-seeds->eta.at(i))>range) break;
            for(unsigned ii=0; ii<seeds->phi.at(i).size(); ii++){
               float dphi = deltaPhi(seeds->phi.at(iseed_eta).at(iseed_phi),seeds->phi.at(i).at(ii));
               if(dphi>range) continue;
               if(seeds->et.at(iseed_eta).at(iseed_phi)<=seeds->et.at(i).at(ii)){
                 eta_n = false;
                 break;
               }
            }
         }

         for(unsigned ii=0; ii<seeds->phi.at(iseed_eta).size(); ii++){
            if(ii==iseed_phi) continue;
            float dphi = deltaPhi(seeds->phi.at(iseed_eta).at(iseed_phi),seeds->phi.at(iseed_eta).at(ii));
            if(dphi>range) continue;
            if(seeds->et.at(iseed_eta).at(iseed_phi)<=seeds->et.at(iseed_eta).at(ii)){
              eta_0 = false;
              break;
            }
         }
         tmp_max.push_back(eta_n&&eta_p&&eta_0);
      }
   seeds->local_max.push_back(tmp_max);
   }
   return StatusCode::SUCCESS;
}

StatusCode JGTowerReader::BuildJet(const xAOD::JGTowerContainer*towers,std::vector<float> noise,JGTowerReader::Seed*seeds, std::vector<JGTowerReader::L1Jet>& js, float jet_size){

  for(unsigned eta_ind=0; eta_ind<seeds->eta.size(); eta_ind++){
     for(unsigned phi_ind=0; phi_ind<seeds->phi.at(eta_ind).size(); phi_ind++){
        if(!seeds->local_max.at(eta_ind).at(phi_ind)) continue;
        float eta = seeds->eta.at(eta_ind);
        float phi = seeds->phi.at(eta_ind).at(phi_ind);
        float j_et = 0;
        for(unsigned t=0; t<towers->size(); t++){
           const xAOD::JGTower* tower = towers->at(t);
           if(fabs(tower->et())<noise.at(t)) continue; 
           if(!inBox(eta,tower->eta(),jet_size, phi, tower->phi(),jet_size)) continue;
           j_et += tower->et();
        }
        if(j_et<10000) continue;
        JGTowerReader::L1Jet j = L1Jet(eta,phi,j_et);
        js.push_back(j);
     }
  }
  return StatusCode::SUCCESS;
}

StatusCode JGTowerReader::BuildMET(const xAOD::JGTowerContainer*towers,std::vector<float> noise, JGTowerReader::MET* met){
  float met_x=0;
  float met_y=0;
  for(unsigned t=0; t<towers->size(); t++){
     const xAOD::JGTower* tower = towers->at(t); 
     if(tower->et()<noise.at(t)) continue;
     float phi=tower->phi();
     float et =tower->et();
     met_x -= et*cos(phi);
     met_y -= et*sin(phi);
  }

  float et_met = sqrt(met_x*met_x+met_y*met_y); 
  float phi_met=TMath::ACos(met_x/et_met); 
  if (met_y<0) phi_met = -phi_met;
  met->phi=phi_met;
  met->et = et_met;
  return StatusCode::SUCCESS; 
}
StatusCode JGTowerReader::ProcessObject(){

  
  for(unsigned j=0;j<jL1Jets.size();j++  ){
     JGTowerReader::L1Jet jet = jL1Jets.at(j);
     hName["jJet_et"]->Fill(jet.et);
     hName["jJet_eta"]->Fill(jet.eta);
     hName["jJet_phi"]->Fill(jet.phi);
  }
  for(unsigned j=0;j<gL1Jets.size();j++  ){
     JGTowerReader::L1Jet jet = gL1Jets.at(j);
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
