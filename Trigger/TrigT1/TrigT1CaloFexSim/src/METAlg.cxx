/*
 *   Copyright (C) 2002-2019 for the benefit of the ATLAS collaboration
*/
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"
#include "TrigT1CaloFexSim/METAlg.h"
#include "TrigT1CaloFexSim/JGTower.h"

//MET algorithms
#include "TrigT1CaloFexSim/Rho.h"
#include "TrigT1CaloFexSim/Softkiller.h"
#include "TrigT1CaloFexSim/JwoJ.h"
#include "TrigT1CaloFexSim/Pufit.h"
std::map<TString, std::shared_ptr<METAlg::MET>> METAlg::m_METMap;

//------------------------------------------------------------------------------------------------
StatusCode METAlg::Baseline_MET(const xAOD::JGTowerContainer*towers, TString metName, std::vector<float> noise, bool useNegTowers){
  
  float met_x=0;
  float met_y=0;
  
  for(unsigned t=0; t<towers->size(); t++){
    const xAOD::JGTower* tower = towers->at(t);
    if(noise.size()>0){
      if(tower->sampling()==0       && tower->eta()<3.2 && tower->et()<4.5*noise.at(t)) continue;
      else if(tower->sampling()!=0  && tower->et()<5.*noise.at(t)) continue;
    }
    float phi=tower->phi();
    float et =tower->et();
    if(!useNegTowers && et < 0) continue;
    met_x -= et*cos(phi);
    met_y -= et*sin(phi);
  }
  
  float et_met = sqrt(met_x*met_x+met_y*met_y);
  
  float phi_met=0;
  if(et_met > 0)
    phi_met = TMath::ACos(met_x/et_met);

  if (met_y<0) phi_met = -phi_met;

  std::shared_ptr<MET> met  = std::make_shared<MET>();
  met->phi=phi_met;
  met->et = et_met;
  met->rho = 0;

  if(m_METMap.find(metName)==m_METMap.end()) m_METMap[metName] = met;
  return StatusCode::SUCCESS;
}
//------------------------------------------------------------------------------------------------
StatusCode METAlg::SubtractRho_MET(const xAOD::JGTowerContainer* towers, TString metName, bool useEtaBins, bool useRMS, bool useMedian, bool useNegTowers){
  
  float EtMiss = 0;
  float n = 3.;
  float Ex = 0, Ey = 0, Ex_ = 0, Ey_ = 0;
  float threshold  = 0;
  
  int fpga = 0;
  //can calculate rho as either the average or median gTower energy in the barrel
  if(metName.Contains("RhoSubA")) fpga = 1;
  if(metName.Contains("RhoSubB")) fpga = 2;
  if(metName.Contains("RhoSubC")) fpga = 3;
  
  //std::cout<<metName<<": "<<fpga<<std::endl;

  float rho = Rho_bar(towers, useEtaBins, fpga, 0);
  
  if(useMedian) rho = Rho_med(towers, useNegTowers);

  unsigned int size = towers->size();

  TH1F* h_Et = new TH1F("h_Et", "", 50, 0, 5000);
  if(useRMS){
  
    for(unsigned int t = 0; t < size; t++){
      const xAOD::JGTower* tower = towers->at(t);
      //float eta = tower->eta();

      float Et = tower->et();

      if(!useNegTowers && Et < 0) continue;
      h_Et->Fill(Et);
    }
    
    threshold = n*(h_Et->GetRMS());
    //std::cout<<"3 sigma = "<<threshold<<std::endl;
  }

  for(unsigned int t = 0; t < size; t++){
    const xAOD::JGTower* tower = towers->at(t);

    float Et = tower->et();
    float phi = tower->phi();
    float eta = tower->eta();

    //std::cout<<"Looking at tower ("<<eta<<", "<<phi<<")"<<std::endl;
    if(!useNegTowers && Et < 0) continue;
    
    //float deta = tower->deta();
    //float dphi = tower->dphi();

    float area = 0.;//deta*dphi/default_area;
    if(eta >= -2.5 && eta < 0 && fpga == 1){
      if(eta > -2.4) area = 1.;
      else area = 0.5;
    }
    if(eta > 0 && eta <= 2.5 && fpga == 2){
      if(eta < 2.4) area = 1.;
      else area = 0.5;
    }
    if(fabs(eta) > 2.5 && fpga == 3){
      if(fabs(eta) < 3.2) area = 1.;
      else area = 4.;
    }
    //std::cout<<"area = "<<area<<std::endl;
    float Et_sub = 0;

    if(useEtaBins) Et_sub = TMath::Abs(Et) - area*rho;

    else{
      if(fabs(eta < 2.4)) Et_sub = TMath::Abs(Et) - rho;
      else Et_sub = TMath::Abs(Et) - 4*rho;
    }

    //std::cout<<"Et_sub = "<<Et<<" - "<<area*rho<<" = "<<Et_sub<<std::endl;
    if(Et_sub < threshold) continue;
    
    //if tower originally had negative Et, keep Et_sub negative
    if(useNegTowers && Et < 0) Et_sub = -Et_sub;
    
    Ex_ = Et_sub*TMath::Cos(phi);
    Ey_ = Et_sub*TMath::Sin(phi);
    
    Ex += Ex_;
    Ey += Ey_;
  }
  
  EtMiss = TMath::Sqrt(Ex*Ex + Ey*Ey);
  float phi_met = 0;

  //std::cout<<"Ex = "<<Ex<<", Ey = "<<Ey<<std::endl;
  //std::cout<<" EtMiss = "<<EtMiss<<std::endl;
  if(EtMiss != 0) phi_met = TMath::ACos(Ex/EtMiss);
  if (Ey<0) phi_met = -phi_met;

  //ATH_MSG_INFO("Dumping event rho: " << rho ); 
  std::shared_ptr<MET> met  = std::make_shared<MET>();
  met->phi = phi_met;
  met->et = EtMiss;
  met->rho = rho;

  //std::cout<<"rho = "<<rho<<std::endl;
  if(m_METMap.find(metName)==m_METMap.end()) m_METMap[metName] = met;
  delete h_Et;  
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------------------------
StatusCode METAlg::Softkiller_MET(const xAOD::JGTowerContainer* towers, TString metName, bool useNegTowers){


  float median = 0;
  unsigned int size = towers->size();
  
  TH2F* grid = ConstructGrid(towers, useNegTowers);
  
  //compress grid into 1D vector of towers
  std::vector<float> EtMaxPerPatch = FlattenHistogram(grid);
  
  //find the true median of the tower energies
  median = Et_median_true(&EtMaxPerPatch);
  
  float rho = Rho_bar(towers, false,0,0);
  float Ex = 0, Ey = 0, Ex_ = 0, Ey_ = 0;
  
  for(unsigned int t = 0; t < size; t++){
    const xAOD::JGTower* tower = towers->at(t);
    float eta = TMath::Abs(tower->eta());
    float phi = tower->phi();
    float Et = tower->et();
    
    float Et_sub = 0;
    
    if(eta < 2.4){
      Et_sub = TMath::Abs(Et) - rho;
      if(Et_sub < median) continue;
    }else{
      Et_sub = TMath::Abs(Et) - 4*rho;
      if(Et_sub < 4*median) continue;
    }
    
    //if the tower originally had negative Et, keep Et_sub negative
    if(useNegTowers && Et < 0) Et_sub = -Et_sub;
    Ex_ = Et_sub*TMath::Cos(phi);
    Ey_ = Et_sub*TMath::Sin(phi);
    
    Ex += Ex_;
    Ey += Ey_;
  }
  
  float EtMiss = TMath::Sqrt(Ex*Ex + Ey*Ey);
  float phi_met = 0;
  if(EtMiss != 0) phi_met = TMath::ACos(Ex/EtMiss);
  if (Ey<0) phi_met = -phi_met;

  std::shared_ptr<MET> met  = std::make_shared<MET>();
  met->phi=phi_met;
  met->et =EtMiss;
  met->rho = rho;
  if(m_METMap.find(metName)==m_METMap.end()) m_METMap[metName] = met;

  delete grid;
  return StatusCode::SUCCESS;
}

StatusCode METAlg::JwoJ_MET(const xAOD::JGTowerContainer* towers, TString metName, float pTcone_cut, bool useEtaBins, bool useRho, bool useNegTowers){
  

  //unsigned int size = towers->size();

  float rho = 0;
  if(useRho) rho = Rho_bar(towers, useEtaBins, 0, false);  
  else rho = 0;
  std::vector<float> Et_values = Run_JwoJ(towers, rho, pTcone_cut, useEtaBins, useNegTowers);

  //set fit parameters for calculating MET
  //Look up table for parameters a,b,c depending on scalar sumEt
  //Optimized for resolution in external framework
  float ax;
  float bx;
  float cx;

  float ay;
  float by;
  float cy;
  
  if(Et_values[0] <= 500*Gaudi::Units::GeV){
    ax = 1.45;
    bx = 1.15;
    cx = 1.;

    ay = 1.45;
    by = 1.1;
    cy = 1.5;
  }
  else if(Et_values[0] <= 700*Gaudi::Units::GeV){
    ax = 1.35;
    bx = 1.05;
    cx = -1.5;

    ay = 1.35;
    by = 0.95;
    cy = 0.;
  }
  else if(Et_values[0] <= 900*Gaudi::Units::GeV){
    ax = 1.35;
    bx = 1.0;
    cx = 0.5;

    ay = 1.35;
    by = 0.95;
    cy = -1.;
  }
  else if(Et_values[0] <= 1100*Gaudi::Units::GeV){
    ax = 1.3;
    bx = 0.95;
    cx = -1.;

    ay = 1.3;
    by = 0.95;
    cy = -1;
  }
  else if(Et_values[0] <= 1300*Gaudi::Units::GeV){
    ax = 1.3;
    bx = 0.9;
    cx = 0.75;

    ay = 1.25;
    by = 0.8;
    cy = 0.5;
  }
  else if(Et_values[0] <= 1500*Gaudi::Units::GeV){
    ax = 1.25;
    bx = 0.8;
    cx = 0.5;

    ay = 1.25;
    by = 0.8;
    cy = 0.5;
  }
  else if(Et_values[0] <= 1700*Gaudi::Units::GeV){
    ax = 1.3;
    bx = 0.75;
    cx = 1.5;

    ay = 1.25;
    by = 0.75;
    cy = 0.5;
  }
  else{
    ax = 1.25;
    bx = 0.75;
    cx = 0.75;

    ay = 1.25;
    by = 0.75;
    cy = 2.;
  }
  //a is hard term from gBlocks with pT > 25 GEV
  //b is total MET term computed from all gTowers 
  float Ex = ax*(Et_values[1])+ bx*Et_values[3] + cx;
  float Ey = ay*(Et_values[2])+ by*Et_values[4] + cy;

  float EtMiss = TMath::Sqrt(Ex*Ex + Ey*Ey);
  float phi = 0;

  float mht_phi = 0;
  if(Et_values[5]>0) mht_phi = TMath::ACos(Et_values[1]/Et_values[5]);
  if(Et_values[2] < 0) mht_phi = -mht_phi;
  
  float mst_phi = 0;
  if(Et_values[6]>0) mst_phi = TMath::ACos(Et_values[3]/Et_values[6]);
  if(Et_values[4] < 0) mst_phi = -mst_phi;

  if(EtMiss > 0) phi = TMath::ACos(Ex/EtMiss);

  if(Ey < 0) phi = -phi;

  std::shared_ptr<MET> met  = std::make_shared<MET>();
  met->phi=phi;
  met->et =EtMiss;
  //met->rho = rho;
  //std::cout<<"Hard term = "<<Et_values[5]<<std::endl;
  //std::cout<<"Soft term = "<<Et_values[6]<<std::endl;
  met->mht_x = Et_values[1];
  met->mht_y = Et_values[2];

  met->mst_x = Et_values[3];
  met->mst_y = Et_values[4];

  met->mst = Et_values[6];
  met->mht = Et_values[5];
  met->scalar_Et = Et_values[0];
  if(m_METMap.find(metName)==m_METMap.end()) m_METMap[metName] = met;

  return StatusCode::SUCCESS;
}

StatusCode METAlg::Pufit_MET(const xAOD::JGTowerContainer*towers, TString metName, bool useNegTowers){


  std::vector<float> EtMiss = Run_PUfit(towers, 3, useNegTowers);

  std::shared_ptr<MET> met  = std::make_shared<MET>();
  met->phi=EtMiss[1];
  met->et=EtMiss[0];

  if(m_METMap.find(metName)==m_METMap.end()) m_METMap[metName] = met;

  return StatusCode::SUCCESS;
}

float METAlg::Rho_avg(const xAOD::JGTowerContainer* towers, const bool useNegTowers){
  float rho = 0;
  float et_max = 10*Gaudi::Units::GeV;

  const unsigned int size = towers->size();
  int length = 0;

  for(unsigned int i = 0; i < size; i++){
    const xAOD::JGTower* tower = towers->at(i);
    float eta = tower->eta();
    float Et = tower->et();

    if(!useNegTowers && Et < 0) continue;

    if(TMath::Abs(eta) < 2.4){
      if(Et < et_max){
	rho+=Et;
	length++;
      }
    }
  }
  float rho_bar = rho/length;
  return rho_bar;
}

float METAlg::Rho_avg_etaRings(const xAOD::JGTowerContainer* towers, int fpga, bool useNegTowers){
  float rho = 0;
  float et_max = 10*Gaudi::Units::GeV;

  const unsigned int size = towers->size();
  float area_a = 0;
  float area_b = 0;
  float area_c = 0;

  for(unsigned int i = 0; i < size; i++){
    const xAOD::JGTower* tower = towers->at(i);
    
    float eta = tower->eta();
    float Et = tower->et();

    if(!useNegTowers && Et < 0) continue;

    if(eta < 0 && eta >=-2.5){
      if(eta > -2.4) area_a += 1.;
      else area_a += 0.5;
    }
    if(eta <= 2.5 && eta > 0){
      if(eta < 2.4) area_b += 1.;
      else area_b += 0.5;
    }
    if(fabs(eta) > 2.5){
      if(fabs(eta) < 3.2) area_c += 1.;
      else area_c += 4.;
    }
    
    if(Et < et_max){
      if(fpga == 1) rho+=Et/area_a;
      if(fpga == 2) rho+=Et/area_b;
      if(fpga == 3) rho+=Et/area_c;
    }

  }
  return rho;
}



