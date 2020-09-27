/*
 *   Copyright (C) 2002-2020 for the benefit of the ATLAS collaboration
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

#include "TrigT1CaloFexSim/Softkiller.h"
#include "TrigT1CaloFexSim/JwoJ.h"
#include "TrigT1CaloFexSim/Pufit.h"
std::map<TString, std::shared_ptr<METAlg::MET>> METAlg::m_METMap;

std::vector <int> METAlg::check_in_bin (const float &eta, const float &phi, const std::vector < std::pair < float, float> > &eta_bins, const  std::vector < std::pair < float, float> > &phi_bins, const float &phi_offset) {
    int calculation_bin=-1;
    std::vector < int > result;
    for (const auto &eta_bin : eta_bins) {
        for (const auto &phi_bin : phi_bins) {
            calculation_bin++;
            if (eta< eta_bin.first || eta > eta_bin.second) continue;;
            float phi_t=(phi/TMath::Pi())+1;
            if (phi_t<phi_bin.first) phi_t=phi_t+2;
            if (phi_t< phi_bin.first+phi_offset || phi_t> phi_bin.second+phi_offset) continue;
            result.push_back(calculation_bin);
        }
    }
    return result;
}

StatusCode METAlg::build_jFEX_bins( std::vector < std::vector < int > > &bins, std::vector < std::vector < int > > &bins_core, const xAOD::JGTowerContainer* towers ) {
    std::vector <int> *HAD1towers = new std::vector <int>;
    std::vector <int> *HAD2towers = new std::vector <int>;
    std::vector <int> *HAD3towers = new std::vector <int>;
    std::vector <int> *HADtowers  = new std::vector <int>;
    std::vector <int> *EMtowers   = new std::vector <int>;
    std::vector <int> *FCAL1      = new std::vector <int>;
    std::vector <int> *FCAL2      = new std::vector <int>;
    std::vector <int> *FCAL3      = new std::vector <int>;
    std::vector <int> *fullFCAL = new std::vector <int>;
    std::vector < std::pair < float , float > > eta_bins = {  
        {-1.6 , 0.8}  , 
        {-0.8 , 1.6}  , 
        {-2.4 , 0.0}  , 
        {0.0  , 2.4}  , 
        {-4.9 , -0.8} , 
        {0.8  , 4.9}
    };
    std::vector < std::pair < float , float > > eta_bins_core = {
        {-0.8 , 0.0}    , 
        {0.0  , 0.8}  , 
        {-1.6 , -0.8} , 
        {0.8  , 1.6}  , 
        {-4.9 , -1.6} , 
        {1.6  , 4.9}
    };

    // mutiple of pi                , only positive
    std::vector < std::pair < float , float > > phi_bins = { {0.0       , 1.0}  , {1.0  , 2.0} };
    float phi_offset = +0.25;
    for (unsigned int i=0; i <7744; i++){
        if ( i> 0    && i< 1696 ) EMtowers->push_back(i);
        if ( i>=3392 && i< 5088 ) EMtowers->push_back(i);
        if ( i>=6784 && i< 6816 ) EMtowers->push_back(i);
        if ( i>=6848 && i< 6880 ) EMtowers->push_back(i);
        if ( i>=6912 && i< 6976 ) EMtowers->push_back(i);
        if ( i>=1696 && i< 3392 ) HADtowers->push_back(i);
        if ( i>=5088 && i< 6784 ) HADtowers->push_back(i);
        if ( i>=6816 && i< 6848 ) HADtowers->push_back(i);
        if ( i>=6880 && i< 6912 ) HADtowers->push_back(i);
        if ( i>=6976 && i< 7168 ) FCAL1->push_back(i);
        if ( i>=7168 && i< 7296 ) FCAL2->push_back(i);
        if ( i>=7296 && i< 7360 ) FCAL3->push_back(i);
        if ( i>=7360 && i< 7552 ) FCAL1->push_back(i);
        if ( i>=7552 && i< 7680 ) FCAL2->push_back(i);
        if ( i>=7680 && i< 7744 ) FCAL3->push_back(i);
    }
    HAD1towers->clear();
    HAD2towers->clear();
    HAD3towers->clear();
    for (const auto &i : *HADtowers) {
        const xAOD::JGTower* tower = towers->at(i);
        float aeta = std::abs(tower->eta());
        if (aeta< 1.5) HAD1towers->push_back(i);
        else if (aeta< 1.6) HAD2towers->push_back(i);
        else HAD3towers->push_back(i);
    }

    fullFCAL->reserve(fullFCAL->size() + distance(FCAL1->begin(), FCAL1->end()));
    fullFCAL->insert(fullFCAL->end(), FCAL1->begin(), FCAL1->end());
    fullFCAL->reserve(fullFCAL->size() + distance(FCAL2->begin(), FCAL2->end()));
    fullFCAL->insert(fullFCAL->end(), FCAL2->begin(), FCAL2->end());
    fullFCAL->reserve(fullFCAL->size() + distance(FCAL3->begin(), FCAL3->end()));
    fullFCAL->insert(fullFCAL->end(), FCAL3->begin(), FCAL3->end());

    std::vector < float > rho               ( bins.size ()  , 0.0 ) ;
    std::vector < std::vector <int> * > regions;
    regions = { HAD1towers, HAD2towers, HAD3towers, EMtowers, fullFCAL };
    
    int totalnum=0;
    for (const auto &region: regions) {
        totalnum+=region->size();
    }

    int offset_steps=eta_bins.size()*phi_bins.size();
    int offset_core_steps=eta_bins_core.size()*phi_bins.size();

    bins.clear();
    bins_core.clear();
    int offset=0;
    int offset_core=0;
    for (const auto &region: regions) {
        bins.resize(offset_steps+bins.size());
        bins_core.resize(offset_core_steps+bins_core.size());
        for(const auto &i : *region){
            const xAOD::JGTower* tower = towers->at(i);
            float eta = tower->eta();
            float phi = tower->phi();
            std::vector<int> bin= check_in_bin(eta,phi,eta_bins,phi_bins, phi_offset);
            std::vector<int> bin_core= check_in_bin(eta,phi,eta_bins_core,phi_bins, phi_offset);
            for (const auto j: bin) bins.at(offset+j).push_back(i);
            for (const auto j: bin_core) bins_core.at(offset+j).push_back(i);
        }
        offset+=offset_steps;
        offset_core+=offset_core_steps;
    }

    int num_bins_core=0;
    int total_bins_core=0;
    for (const auto &bin: bins_core) {
        if (bin.size() !=0 ) num_bins_core++;
        total_bins_core+=bin.size();
    }

    int num_bins=0;
    int total_bins=0;
    for (const auto &bin: bins) {
        if (bin.size() !=0 ) num_bins++;
        total_bins+=bin.size();
    }

    for (size_t bin=0; bin< bins_core.size(); bin++) {
        if (bins_core.at(bin).size() == 0) bins.at(bin).clear();
    }

    int num_bins_c=0;
    int total_bins_c=0;
    for (const auto &bin: bins) {
        if (bin.size() !=0 ) num_bins_c++;
        total_bins_c+=bin.size();
    }
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------------------------
StatusCode METAlg::NoiseCut_MET(const xAOD::JGTowerContainer*towers, TString metName, std::vector<float> noise, bool useNegTowers){
  
  float met_x=0;
  float met_y=0;
  float sumET=0;
  
  for(unsigned t=0; t<towers->size(); t++){
    const xAOD::JGTower* tower = towers->at(t);
    if(noise.size()>0){
      if(tower->sampling()==0       && tower->eta()<3.2 && tower->et()<4.5*noise.at(t)) continue;
      else if(tower->sampling()!=0  && tower->et()<5.*noise.at(t)) continue;
    }
    float phi=tower->phi();
    float et =tower->et();
    if(!useNegTowers && et < 0) continue;
    sumET += et;
    met_x -= et*cos(phi);
    met_y -= et*sin(phi);
  }
  
  
  std::shared_ptr<MET> met  = std::make_shared<MET>();
  met->ex=met_x; 
  met->ey=met_y; 
  met->et = sumET;
  met->rho = 0;

  if(m_METMap.find(metName)==m_METMap.end()) m_METMap[metName] = met;
  return StatusCode::SUCCESS;
}
//------------------------------------------------------------------------------------------------
StatusCode METAlg::jXERHO(const xAOD::JGTowerContainer* towers, TString metName, const std::vector<float> jTowerArea, const std::vector < std::vector < int > > jFEX_bins, const std::vector < std::vector < int > > jFEX_bins_core, float fixed_noise_cut, float rho_up_threshold, float min_noise_cut, xAOD::JGTowerContainer* towers_PUsub ){
    std::vector < int   > count  ( jFEX_bins.size ( ) , -1  ) ;
    std::vector < float > jT_Sum ( jFEX_bins.size ( ) , 0.0 ) ;
    std::vector < float > rho    ( jFEX_bins.size ( ) , 0.0 ) ;

    // rho calculation
    for (unsigned int calculation_bin=0; calculation_bin<jFEX_bins.size(); calculation_bin++) {
        for(const auto &i: jFEX_bins.at(calculation_bin)){
            const xAOD::JGTower* tower = towers->at(i);
            float Et = tower->et();
            float Et_area = Et / jTowerArea[i];

            if (count.at(calculation_bin) == -1) count[calculation_bin]=0;

            if ((fixed_noise_cut<Et_area) && (Et_area<=rho_up_threshold)) {
                jT_Sum[calculation_bin] +=Et_area;
                count[calculation_bin]++;
            }
        }
        if (count.at(calculation_bin) !=0) rho[calculation_bin]=jT_Sum.at(calculation_bin)/count.at(calculation_bin);
    }

    //Missing Et calculation for met and met_sub
    float MET_sub_x=0.0;
    float MET_sub_y=0.0;
    float sumET_sub=0.0;
    
    std::vector < float > rho_tower_et;
    rho_tower_et.resize(7744,0.0);
    for (unsigned int calculation_bin=0; calculation_bin<jFEX_bins_core.size(); calculation_bin++) {
        for(const auto &i: jFEX_bins_core.at(calculation_bin)){
            const xAOD::JGTower* tower = towers->at(i);
            float phi=tower->phi();
            float Et = tower->et();
            float Et_sub=0.;
            float Et_diff=Et;
            Et_diff=(Et-rho.at(calculation_bin)*jTowerArea[i]);
            if (Et_diff/jTowerArea[i]>min_noise_cut) {
                Et_sub=Et_diff;
            }
            sumET_sub += Et_sub;
            MET_sub_x -= Et_sub*cos(phi);
            MET_sub_y -= Et_sub*sin(phi);
            rho_tower_et[i]=Et_sub;
        }
    }
    
    towers_PUsub->clear();
    
    for ( size_t i =0 ; i< 7744; i++) {
        xAOD::JGTower* new_tower = new xAOD::JGTower(*(towers->at(i)));
        towers_PUsub->push_back(new_tower);
        const xAOD::JGTower* tower = towers->at(i);
        float eta=tower->eta();
        float phi=tower->phi();
        float deta=tower->deta();
        float dphi=tower->dphi();
        const int t_ = i;
        const int sampling = tower->sampling(); 
        new_tower->initialize(t_, eta, phi);
        new_tower->setdEta(deta);                                                                     
        new_tower->setdPhi(dphi);                                                                     
        new_tower->setEt(rho_tower_et[i]);                                                                
        const std::vector<int> SCindex= tower->SCIndex();
        const std::vector<int> TileIndex= tower->TileIndex();
        new_tower->setSCIndex(SCindex);                                                                    
        new_tower->setTileIndex(TileIndex);
        new_tower->setSampling(sampling);
    }

    float et_MET_sub = sqrt(MET_sub_x * MET_sub_x + MET_sub_y * MET_sub_y);
    float phi_MET_sub=TMath::ACos(MET_sub_x/et_MET_sub);
    if (MET_sub_y<0) {
        phi_MET_sub = -phi_MET_sub;
    }

    std::shared_ptr<MET> met  = std::make_shared<MET>();
    met->ex=MET_sub_x; 
    met->ey=MET_sub_y; 
    met->phi = phi_MET_sub;
    met->et = sumET_sub;


    if(m_METMap.find(metName)==m_METMap.end()) m_METMap[metName] = met;

    return StatusCode::SUCCESS;
}

StatusCode METAlg::SubtractRho_MET(const xAOD::JGTowerContainer* towers, TString metName, bool useEtaBins, bool useRMS, bool useNegTowers){
  /*
    This function is currently depreciated, as we currently compute a set of towers after pileup
    subtraction that are passed to the NoiseCut_MET function with a set of noise cuts identical to 0 
    However, because it may be useful in the future for a slightly different algorithm, we leave the function here for now.
    B. Carlson March 27, 2020 
   */
  float EtMiss = 0;
  float n = 3.;
  float Ex = 0, Ey = 0, Ex_ = 0, Ey_ = 0;
  float threshold  = 0;
  
  float rho = Rho_avg_barrel(towers,useNegTowers); 

  unsigned int size = towers->size();

  TH1F* h_Et = new TH1F("h_Et", "", 50, 0, 5000);
  if(useRMS){
  
    for(unsigned int t = 0; t < size; t++){
      const xAOD::JGTower* tower = towers->at(t);
      //float eta = tower->eta();

      float Et = tower->et();

      if(!useNegTowers && Et < 0) continue;
      //if(Et > et_max)continue; 
      h_Et->Fill(Et);
    }
    
    threshold = n*(h_Et->GetRMS());
  }

  for(unsigned int t = 0; t < size; t++){
    const xAOD::JGTower* tower = towers->at(t);

    float Et = tower->et();
    float phi = tower->phi();
    float eta = tower->eta();

    if(!useNegTowers && Et < 0) continue;
    
    float area = GTowerArea(eta); 
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

  //std::cout<<"Ex = "<<Ex<<", Ey = "<<Ey<<std::endl;
  //std::cout<<" EtMiss = "<<EtMiss<<std::endl;
  //if(EtMiss != 0) phi_met = TMath::ACos(Ex/EtMiss);
  //if (Ey<0) phi_met = -phi_met;

  //ATH_MSG_INFO("Dumping event rho: " << rho ); 
  std::shared_ptr<MET> met  = std::make_shared<MET>();
  //met->phi = phi_met;
  met->ex=Ex; 
  met->ey=Ey; 
  met->et = EtMiss;
  met->rho = rho;

  //std::cout<<"rho = "<<rho<<std::endl;
  if(m_METMap.find(metName)==m_METMap.end()) m_METMap[metName] = met;
  delete h_Et;  
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------------------------
StatusCode METAlg::Softkiller_MET(const xAOD::JGTowerContainer* towers, TString metName, bool useNegTowers){
  /*
    This function is currently depreciated, but could be updated fairly easily. March 2020 
    This function is designed to compute MET after applying SK 
    First the standard rho correction is applied
    Then towers that have ET > median are included in the MET calculation 
   */

  float median = 0;
  unsigned int size = towers->size();
  
  TH2F* grid = ConstructGrid(towers, useNegTowers);
  
  //compress grid into 1D vector of towers
  std::vector<float> EtMaxPerPatch = FlattenHistogram(grid);
  
  //find the true median of the tower energies
  median = Et_median_true(&EtMaxPerPatch);
  float rho = Rho_avg_barrel(towers, useNegTowers);
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
  std::shared_ptr<MET> met  = std::make_shared<MET>();
  met->ex=Ex; 
  met->ey=Ey; 
  met->et =EtMiss;
  met->rho = rho;
  if(m_METMap.find(metName)==m_METMap.end()) m_METMap[metName] = met;

  delete grid;
  return StatusCode::SUCCESS;
}

StatusCode METAlg::JwoJ_MET(const xAOD::JGTowerContainer* towers, const std::vector<TowerObject::Block> gBlocks, TString metName, float pTcone_cut, bool useRho, float RhoA, float RhoB, float RhoC, bool useNegTowers){

  /*
    Function retrieve components of jets without jets
    Then evaluate the calibrated version using the coefficients below 
   */

  std::vector<float> Et_values = Run_JwoJ(towers, gBlocks, pTcone_cut, useRho, RhoA, RhoB, RhoC, useNegTowers);

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

 

  float mht_phi = 0;
  if(Et_values[5]>0) mht_phi = TMath::ACos(Et_values[1]/Et_values[5]);
  if(Et_values[2] < 0) mht_phi = -mht_phi;
  
  float mst_phi = 0;
  if(Et_values[6]>0) mst_phi = TMath::ACos(Et_values[3]/Et_values[6]);
  if(Et_values[4] < 0) mst_phi = -mst_phi;
  std::shared_ptr<MET> met  = std::make_shared<MET>();
  met->ex=Ex; 
  met->ey=Ey; 
  met->et =Et_values[0];
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


  std::array<float, 3> EtMiss = Run_PUfit(towers, 3, useNegTowers);

  std::shared_ptr<MET> met  = std::make_shared<MET>();
  met->ex=EtMiss[1];
  met->ey=EtMiss[2]; 
  met->et=EtMiss[0];

  if(m_METMap.find(metName)==m_METMap.end()) m_METMap[metName] = met;

  return StatusCode::SUCCESS;
}

float METAlg::Rho_avg_barrel(const xAOD::JGTowerContainer* towers, const bool useNegTowers){
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
      //estimating rho using the barrel, excluding the one tower that is 0.1x0.2 
      if(Et < et_max){
	rho+=Et;
	length++;
      }
    }
  }
  float rho_bar = rho/length;
  return rho_bar;
}

float METAlg::Rho_avg_etaRings(const xAOD::JGTowerContainer* towers, bool useNegTowers){
  /*
    This function accounts for the different area of trigger towers, and 
    returns the average energy per tower, in units of the area for 0.2x0.2 towers. 
    To use this energy for towers that are larger or smaller, multiply by the ratio
    of towers sizes, e.g., 2, 4, etc. 
  */

  float rho = 0;
  float et_max = 10*Gaudi::Units::GeV;

  const unsigned int size = towers->size();
  float A_covered = 0; //Number of towers that pass selections E < max and >0 (if we allow negative towers) 

  for(unsigned int i = 0; i < size; i++){
    const xAOD::JGTower* tower = towers->at(i);
    
    float eta = tower->eta();
    float Et = tower->et();
    float Area = GTowerArea(eta); 

    if(!useNegTowers && Et < 0) continue;
    if(Et > et_max)continue;

    A_covered += Area; 
    rho+=Et; //Total ET 
    
  }

  //Note that this function is designed to compute the area 
  //for a collection of towers of a defined eta range. 
  rho = rho/A_covered; 
  return rho;
}



