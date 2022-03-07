/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration  
*/
//***************************************************************************  
//		jFEXPileupAndNoise - Algorithm for Pileup and Noise in jFEX
//                              -------------------
//     begin                : 24 05 2021
//     email                : Sergi.Rodriguez@cern.ch
//***************************************************************************
#include <iostream>
#include <vector>
#include <stdio.h>
#include <math.h>
#include "L1CaloFEXSim/jTower.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"
#include "L1CaloFEXSim/jFEXPileupAndNoise.h"
#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"

namespace LVL1{

//Default Constructor
LVL1::jFEXPileupAndNoise::jFEXPileupAndNoise(const std::string& type, const std::string& name, const IInterface* parent): AthAlgTool(type, name, parent) {
    declareInterface<IjFEXPileupAndNoise>(this);
}

/** Destructor */
LVL1::jFEXPileupAndNoise::~jFEXPileupAndNoise() {
}

StatusCode LVL1::jFEXPileupAndNoise::initialize() {
    
    ATH_CHECK(m_jTowerContainerKey.initialize());
    return StatusCode::SUCCESS;
}

//calls container for TT
StatusCode LVL1::jFEXPileupAndNoise::safetyTest() {
    
    m_jTowerContainer = SG::ReadHandle<jTowerContainer>(m_jTowerContainerKey);
    if(! m_jTowerContainer.isValid()) {
        ATH_MSG_FATAL("Could not retrieve jTowerContainer " << m_jTowerContainerKey.key());
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode LVL1::jFEXPileupAndNoise::reset() {

    m_is_FWD=0;
    m_apply_pileup2jets=0; 
    m_apply_pileup2met=0;
    m_apply_noise2jets=0;
    m_apply_noise2met=0;

    reset_conters();
    
    return StatusCode::SUCCESS;
}

void LVL1::jFEXPileupAndNoise::reset_conters() {
   
    m_rho_EM   = 0;
    m_rho_HAD1 = 0;
    m_rho_HAD2 = 0;
    m_rho_HAD3 = 0;
    m_rho_FCAL = 0;
    
    m_count_rho_EM   = 0;
    m_count_rho_HAD1 = 0;
    m_count_rho_HAD2 = 0;
    m_count_rho_HAD3 = 0;
    m_count_rho_FCAL = 0;    
    
}


//Setup for the central region, duplicate fuction 
void LVL1::jFEXPileupAndNoise::setup(int FPGA[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width]) {

    ATH_MSG_DEBUG("---------------- jFEXPileupAndNoise::setup ----------------");
    m_is_FWD = 0; //central region
    std::copy(&FPGA[0][0], &FPGA[0][0] + FEXAlgoSpaceDefs::jFEX_algoSpace_height*FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width, &m_FPGA_central[0][0]);

}


//Setup for the forward region, duplicate fuction 
void LVL1::jFEXPileupAndNoise::setup(int FPGA[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width]) {
    
    ATH_MSG_DEBUG("---------------- jFEXPileupAndNoise::setup ----------------");
    m_is_FWD = 1; //forward region
    std::copy(&FPGA[0][0], &FPGA[0][0] + FEXAlgoSpaceDefs::jFEX_algoSpace_height*FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width, &m_FPGA_forward[0][0]);
   
}


std::vector<float> LVL1::jFEXPileupAndNoise::CalculatePileup(){
    
    reset_conters();
    if(m_is_FWD){ // for the FWD mapping
        for(int iphi=0;iphi<FEXAlgoSpaceDefs::jFEX_algoSpace_height;iphi++){
            for(int ieta=0;ieta<FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width;ieta++){
                
                int TTID = m_FPGA_forward[iphi][ieta];
                if(TTID == 0) continue; //skipping TTID iqual to 0
                //storing the energies
                int tmp_energy_EM  = getET_EM(TTID);
                int tmp_energy_HAD = getET_HAD(TTID);
                int tmp_energy     = getTTowerET(TTID);
                float tmp_EM_Area    = getTTArea_EM(TTID);
                float tmp_HD_Area    = getTTArea_HAD(TTID);
                m_FPGA_ET_forward_EM[iphi][ieta]  = tmp_energy_EM ; 
                m_FPGA_ET_forward_HAD[iphi][ieta] = tmp_energy_HAD;
                
                //calculating rho's
                
                int tmp_eta = getTTowerEta(TTID);
                if(tmp_eta < 32){
                    if(tmp_energy_EM > m_et_low and tmp_energy_EM < m_et_high) {//lower and upper threshold must be get from the L1Calo DB
                        m_rho_EM += tmp_energy_EM / tmp_EM_Area; 
                        m_count_rho_EM++;
                    }
                }
                
                if(tmp_eta < 15){
                    if(tmp_energy_HAD > m_et_low and tmp_energy_HAD < m_et_high){ //lower and upper threshold must be get from the L1Calo DB                    
                        m_rho_HAD1 += tmp_energy_HAD / tmp_HD_Area; 
                        m_count_rho_HAD1++;
                    }
                }
                else if(tmp_eta < 16 ){
                    if(tmp_energy_HAD > m_et_low and tmp_energy_HAD < m_et_high){ //lower and upper threshold must be get from the L1Calo DB        
                        m_rho_HAD2 += tmp_energy_HAD / tmp_HD_Area; 
                        m_count_rho_HAD2++;
                    }
                }
                else if(tmp_eta > 30 and TTID>=700000 ){
                    m_FPGA_ET_central_HAD[iphi][ieta] = tmp_energy; //This corrects the FCAL layer 0 (which is an EM layer)
                    if(tmp_energy > m_et_low and tmp_energy < m_et_high){ //lower and upper threshold must be get from the L1Calo DB         
                        m_rho_FCAL += tmp_energy / (tmp_HD_Area*tmp_EM_Area); // FCAL is treated differently.. but when HAD layer -> EM_Area = 1 and viceversa with this we dont need to divide into FCAL layers
                        m_count_rho_FCAL++;
                    }
                }                 
                else if(tmp_eta <= 32 ){
                    if(tmp_energy_HAD > m_et_low and tmp_energy_HAD < m_et_high){ //lower and upper threshold must be get from the L1Calo DB       
                        m_rho_HAD3 += tmp_energy_HAD / tmp_HD_Area;
                        m_count_rho_HAD3++;
                    }
                }

                         
            }
        }//end of iphi loop
        
    }
    else{
        for(int iphi=0;iphi<FEXAlgoSpaceDefs::jFEX_algoSpace_height;iphi++){
            for(int ieta=0;ieta<FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width;ieta++){
                //storing the energies
                int TTID = m_FPGA_central[iphi][ieta];
                int tmp_energy_EM  = getET_EM(TTID);
                int tmp_energy_HAD = getET_HAD(TTID);
                int tmp_energy     = getTTowerET(TTID);
                float tmp_EM_Area    = getTTArea_EM(TTID);
                float tmp_HD_Area    = getTTArea_HAD(TTID);
                m_FPGA_ET_central_EM[iphi][ieta]  = tmp_energy_EM ; 
                m_FPGA_ET_central_HAD[iphi][ieta] = tmp_energy_HAD;
                //calculating rho's
                
                int tmp_eta = getTTowerEta(TTID);
                
                if(tmp_eta < 32){
                    if(tmp_energy_EM > m_et_low and tmp_energy_EM < m_et_high){ //lower and upper threshold must be get from the L1Calo DB
                        m_rho_EM += tmp_energy_EM / tmp_EM_Area; 
                        m_count_rho_EM++;
                    }
                }
                
                if(tmp_eta < 15){
                    if(tmp_energy_HAD > m_et_low and tmp_energy_HAD < m_et_high){ //lower and upper threshold must be get from the L1Calo DB   
                        m_rho_HAD1 += tmp_energy_HAD / tmp_HD_Area; 
                        m_count_rho_HAD1++;
                    }
                }
                else if(tmp_eta < 16 ){
                    if(tmp_energy_HAD > m_et_low and tmp_energy_HAD < m_et_high){ //lower and upper threshold must be get from the L1Calo DB        
                        m_rho_HAD2 += tmp_energy_HAD / tmp_HD_Area; 
                        m_count_rho_HAD2++;
                    }
                }
                else if(tmp_eta > 30  and TTID>=700000 ){ //this should not happen since  central jFEX is not FWD
                    m_FPGA_ET_central_HAD[iphi][ieta] = tmp_energy; //This corrects the FCAL layer 0 (which is an EM layer)
                    if(tmp_energy > m_et_low and tmp_energy < m_et_high){ //lower and upper threshold must be get from the L1Calo DB         
                        m_rho_FCAL += tmp_energy / (tmp_HD_Area*tmp_EM_Area); // FCAL is treated differently.. but when HAD layer -> EM_Area = 1 and viceversa with this we dont need to divide into FCAL layers
                        m_count_rho_FCAL++;
                    }
                }                 
                else if(tmp_eta < 32 ){
                    if(tmp_energy_HAD > m_et_low and tmp_energy_HAD < m_et_high){ //lower and upper threshold must be get from the L1Calo DB       
                        m_rho_HAD3 += tmp_energy_HAD / tmp_HD_Area; 
                        m_count_rho_HAD3++;
                    }
                }
                

                         
            }
        }//end of iphi loop
    }
    //calculating rho values for each region
    if(m_count_rho_EM   != 0) m_rho_EM/=m_count_rho_EM;
    if(m_count_rho_HAD1 != 0) m_rho_HAD1/=m_count_rho_HAD1;
    if(m_count_rho_HAD2 != 0) m_rho_HAD2/=m_count_rho_HAD2;
    if(m_count_rho_HAD3 != 0) m_rho_HAD3/=m_count_rho_HAD3;
    if(m_count_rho_FCAL != 0) m_rho_FCAL/=m_count_rho_FCAL;
    
    std::vector<float> rho_values {m_rho_EM,m_rho_HAD1,m_rho_HAD2,m_rho_HAD3,m_rho_FCAL};
    
    SubtractPileup();
    
    return rho_values;
}

void LVL1::jFEXPileupAndNoise::SubtractPileup(){
    if(m_is_FWD){ // for the FWD mapping
        
        for(int iphi=0;iphi<FEXAlgoSpaceDefs::jFEX_algoSpace_height;iphi++){
            for(int ieta=0;ieta<FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width;ieta++){ 
                int TTID = m_FPGA_forward[iphi][ieta];
                if(TTID == 0) continue; //skipping TTID iqual to 0
                int tmp_eta = getTTowerEta(TTID);
                int tmp_EM_Area = getTTArea_EM(TTID);
                int tmp_HD_Area = getTTArea_HAD(TTID);
                    
                if(tmp_eta < 32){
                    m_FPGA_ET_forward_EM[iphi][ieta]=m_FPGA_ET_forward_EM[iphi][ieta]-m_rho_EM * tmp_EM_Area; 
                }
                
                if(tmp_eta < 15){
                    m_FPGA_ET_forward_HAD[iphi][ieta]=m_FPGA_ET_forward_HAD[iphi][ieta]-m_rho_HAD1 * tmp_HD_Area; 
                }
                else if(tmp_eta < 16 ){
                    m_FPGA_ET_forward_HAD[iphi][ieta]=m_FPGA_ET_forward_HAD[iphi][ieta]-m_rho_HAD2 * tmp_HD_Area; 
                }
                else if(tmp_eta > 30 and TTID>=700000){
                    m_FPGA_ET_forward_HAD[iphi][ieta]=m_FPGA_ET_forward_HAD[iphi][ieta]-m_rho_FCAL * (tmp_HD_Area*tmp_EM_Area); 
                } 
                else if(tmp_eta < 32 ){
                    m_FPGA_ET_forward_HAD[iphi][ieta]=m_FPGA_ET_forward_HAD[iphi][ieta]-m_rho_HAD3 * tmp_HD_Area; 
                }
                
                
            }
        }
           
    }
    else{
        
        for(int iphi=0;iphi<FEXAlgoSpaceDefs::jFEX_algoSpace_height;iphi++){
            for(int ieta=0;ieta<FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width;ieta++){ 
                int TTID = m_FPGA_central[iphi][ieta];
                int tmp_eta = getTTowerEta(TTID);
                int tmp_EM_Area = getTTArea_EM(TTID);
                int tmp_HD_Area = getTTArea_HAD(TTID);
                    
                if(tmp_eta < 32){
                    m_FPGA_ET_central_EM[iphi][ieta]=m_FPGA_ET_central_EM[iphi][ieta]-m_rho_EM * tmp_EM_Area; 
                }
                
                if(tmp_eta < 15){
                    m_FPGA_ET_central_HAD[iphi][ieta]=m_FPGA_ET_central_HAD[iphi][ieta]-m_rho_HAD1 * tmp_HD_Area; 
                }
                else if(tmp_eta < 16 ){
                    m_FPGA_ET_central_HAD[iphi][ieta]=m_FPGA_ET_central_HAD[iphi][ieta]-m_rho_HAD2 * tmp_HD_Area; 
                }
                else if(tmp_eta > 30  and TTID>=700000 ){ 
                    m_FPGA_ET_central_HAD[iphi][ieta]=m_FPGA_ET_central_HAD[iphi][ieta]-m_rho_FCAL * (tmp_HD_Area*tmp_EM_Area); 
                } 
                else if(tmp_eta < 32 ){
                    m_FPGA_ET_central_HAD[iphi][ieta]=m_FPGA_ET_central_HAD[iphi][ieta]-m_rho_HAD3 * tmp_HD_Area; 
                }
                
                
            }
        }
                   
    }
    

}


//Flags that allow to apply the pileup/noise to the objets
void LVL1::jFEXPileupAndNoise::ApplyPileup2Jets  (bool b){
    m_apply_pileup2jets = b;
}

void LVL1::jFEXPileupAndNoise::ApplyPileup2Met  (bool b){
    m_apply_pileup2met = b;
}

void LVL1::jFEXPileupAndNoise::ApplyNoise2Jets  (bool b){
    m_apply_noise2jets = b;
}

void LVL1::jFEXPileupAndNoise::ApplyNoise2Met  (bool b){
    m_apply_noise2met = b;
}



std::unordered_map<int,std::vector<int> > LVL1::jFEXPileupAndNoise::Get_EM_Et_values(){
    
    // map for energies sent to the FPGA
    m_map_Etvalues_EM.clear();

    
    // tmp variable to fill the map
    std::vector<int> v_energies;
    v_energies.clear();
    v_energies.resize(2,0);
    
    
    if(m_is_FWD){ // for the FWD mapping
        
        for(int iphi=0;iphi<FEXAlgoSpaceDefs::jFEX_algoSpace_height;iphi++){
            for(int ieta=0;ieta<FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width;ieta++){
                int TTID = m_FPGA_forward[iphi][ieta];
                if(TTID == 0) continue;
                //reset variables
                v_energies.clear();
                v_energies.resize(2,0);
                
                //saving the SG energy
                int tmp_TotalEt_jet=getET_EM(TTID);
                int tmp_TotalEt_met=getET_EM(TTID);
                
                // if true changing the raw energy to the pileup subtracted energy for jets
                if(m_apply_pileup2jets){
                    tmp_TotalEt_jet = m_FPGA_ET_forward_EM[iphi][ieta];
                }
                
                // if true changing the raw energy to the pileup subtracted energy for met
                if(m_apply_pileup2met){
                    tmp_TotalEt_met = m_FPGA_ET_forward_EM[iphi][ieta];
                }
                
                v_energies[0]=tmp_TotalEt_jet;
                v_energies[1]=tmp_TotalEt_met;

                m_map_Etvalues_EM.insert(std::make_pair(TTID, v_energies));
            }
        }
           
    }
    else{
        
        for(int iphi=0;iphi<FEXAlgoSpaceDefs::jFEX_algoSpace_height;iphi++){
            for(int ieta=0;ieta<FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width;ieta++){ 
                int TTID = m_FPGA_central[iphi][ieta];
                //reset variables
                v_energies.clear();
                v_energies.resize(2,0);
                
                //saving the SG energy
                int tmp_TotalEt_jet=getET_EM(TTID);
                int tmp_TotalEt_met=getET_EM(TTID);
                
                // if true changing the raw energy to the pileup subtracted energy for jets
                if(m_apply_pileup2jets){
                    tmp_TotalEt_jet = m_FPGA_ET_central_EM[iphi][ieta];
                }
                
                // if true changing the raw energy to the pileup subtracted energy for met
                if(m_apply_pileup2met){
                    tmp_TotalEt_met = m_FPGA_ET_central_EM[iphi][ieta];
                }
                
                v_energies[0]=tmp_TotalEt_jet;
                v_energies[1]=tmp_TotalEt_met;

                m_map_Etvalues_EM.insert(std::make_pair(TTID, v_energies));                
                
                
            }
        }
                   
    }

    if(m_apply_noise2jets || m_apply_noise2met) ApplyNoiseCuts(m_map_Etvalues_EM,0);

    return m_map_Etvalues_EM;
}

std::unordered_map<int,std::vector<int> > LVL1::jFEXPileupAndNoise::Get_HAD_Et_values(){
    
    // map for energies sent to the FPGA
    m_map_Etvalues_HAD.clear();

    
    // tmp variable to fill the map
    std::vector<int> v_energies;
    v_energies.clear();
    v_energies.resize(2,0);
    
    
    if(m_is_FWD){ // for the FWD mapping
        
        for(int iphi=0;iphi<FEXAlgoSpaceDefs::jFEX_algoSpace_height;iphi++){
            for(int ieta=0;ieta<FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width;ieta++){
                int TTID = m_FPGA_forward[iphi][ieta];
                if(TTID == 0) continue;
                //reset variables
                v_energies.clear();
                v_energies.resize(2,0);
                
                //saving the SG energy
                int tmp_TotalEt_jet=getET_HAD(TTID);
                int tmp_TotalEt_met=getET_HAD(TTID);
                
                // if true changing the raw energy to the pileup subtracted energy for jets
                if(m_apply_pileup2jets){
                    tmp_TotalEt_jet = m_FPGA_ET_forward_HAD[iphi][ieta];
                }
                
                // if true changing the raw energy to the pileup subtracted energy for met
                if(m_apply_pileup2met){
                    tmp_TotalEt_met = m_FPGA_ET_forward_HAD[iphi][ieta];
                }
                
                v_energies[0]=tmp_TotalEt_jet;
                v_energies[1]=tmp_TotalEt_met;

                m_map_Etvalues_HAD.insert(std::make_pair(TTID, v_energies));
            }
        }
           
    }
    else{
        
        for(int iphi=0;iphi<FEXAlgoSpaceDefs::jFEX_algoSpace_height;iphi++){
            for(int ieta=0;ieta<FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width;ieta++){ 
                int TTID = m_FPGA_central[iphi][ieta];
                //reset variables
                v_energies.clear();
                v_energies.resize(2,0);
                
                //saving the SG energy
                int tmp_TotalEt_jet=getET_HAD(TTID);
                int tmp_TotalEt_met=getET_HAD(TTID);
                
                // if true changing the raw energy to the pileup subtracted energy for jets
                if(m_apply_pileup2jets){
                    tmp_TotalEt_jet = m_FPGA_ET_central_HAD[iphi][ieta];
                }
                
                // if true changing the raw energy to the pileup subtracted energy for met
                if(m_apply_pileup2met){
                    tmp_TotalEt_met = m_FPGA_ET_central_HAD[iphi][ieta];
                }
                
                v_energies[0]=tmp_TotalEt_jet;
                v_energies[1]=tmp_TotalEt_met;

                m_map_Etvalues_HAD.insert(std::make_pair(TTID, v_energies));                
                
                
            }
        }
                   
    }
    if(m_apply_noise2jets || m_apply_noise2met) ApplyNoiseCuts(m_map_Etvalues_HAD,1);

    return m_map_Etvalues_HAD;
}

void LVL1::jFEXPileupAndNoise::ApplyNoiseCuts(std::unordered_map<int,std::vector<int> > & map_Etvalues,int layer ){
    
    const LVL1::jTower *tmpTower;
    
    for(auto [key,vec] : map_Etvalues){
        tmpTower = m_jTowerContainer->findTower(key);
        
        int Jet_NoiseCut = tmpTower->getNoiseForJet(layer);
        int Met_NoiseCut = tmpTower->getNoiseForMet(layer);
        
        if(m_apply_noise2jets && map_Etvalues[key][0]<Jet_NoiseCut){ // Et for jets
            map_Etvalues[key][0]=0.;
        }        
        if(m_apply_noise2met && map_Etvalues[key][1]<Met_NoiseCut){ // Et for Met
            map_Etvalues[key][1]=0.;
        }

    }
    
}




std::unordered_map<int,std::vector<int> > LVL1::jFEXPileupAndNoise::GetEt_values(){
    
    // map for energies sent to the FPGA
    std::unordered_map<int,std::vector<int> > map_Etvalues;
    map_Etvalues.clear();
    
    
    /* 
     *  The vector Et_energy has size 2
     *  Et_energy[0] is used un the Jet algos
     *  Et_energy[1] is used un the Met/SumEt algos
     */
    std::vector<int> Et_energy;
    
    for(auto [key,vec] : m_map_Etvalues_EM){
        
        Et_energy.clear();
        Et_energy.resize(2,0);
        
        Et_energy[0]=m_map_Etvalues_EM[key][0]+m_map_Etvalues_HAD[key][0];
        Et_energy[1]=m_map_Etvalues_EM[key][1]+m_map_Etvalues_HAD[key][1];
        map_Etvalues[key] = Et_energy;
    }
       
    return map_Etvalues;
}


//Gets Eta of the TT
int LVL1::jFEXPileupAndNoise::getTTowerEta(unsigned int TTID) {

    const LVL1::jTower *tmpTower = m_jTowerContainer->findTower(TTID);
    if(m_is_FWD) return std::abs(tmpTower->centreEta()*10); //For FWD we use the centre, due to the non-equal graularity
    return tmpTower->eta();
}
//Gets ET of the TT
int LVL1::jFEXPileupAndNoise::getTTowerET(unsigned int TTID) {
    
    const LVL1::jTower *tmpTower = m_jTowerContainer->findTower(TTID);
    return tmpTower->getTotalET();
}
//Gets EM ET of the TT
int LVL1::jFEXPileupAndNoise::getET_EM(unsigned int TTID) {
    
    const LVL1::jTower *tmpTower = m_jTowerContainer->findTower(TTID);
    return tmpTower->getET_EM();
}
//Gets HAD ET of the TT
int LVL1::jFEXPileupAndNoise::getET_HAD(unsigned int TTID) {

    const LVL1::jTower *tmpTower = m_jTowerContainer->findTower(TTID);
    return tmpTower->getET_HAD();
}

//Get Area of a EM TT
float LVL1::jFEXPileupAndNoise::getTTArea_EM(unsigned int TTID) {

    const LVL1::jTower *tmpTower = m_jTowerContainer->findTower(TTID);
    return tmpTower->getTTowerArea(0);
}

//Get Area of a HAD TT
float LVL1::jFEXPileupAndNoise::getTTArea_HAD(unsigned int TTID) {

    const LVL1::jTower *tmpTower = m_jTowerContainer->findTower(TTID);
    return tmpTower->getTTowerArea(1);
}





}// end of namespace LVL1
