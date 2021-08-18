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
    ATH_CHECK(m_jFEXPileupAndNoise_jTowerContainerKey.initialize());
    return StatusCode::SUCCESS;
}

//calls container for TT
StatusCode LVL1::jFEXPileupAndNoise::safetyTest() {

    SG::ReadHandle<jTowerContainer> jk_jFEXPileupAndNoise_jTowerContainer(m_jFEXPileupAndNoise_jTowerContainerKey);
    if(! jk_jFEXPileupAndNoise_jTowerContainer.isValid()) {
        ATH_MSG_FATAL("Could not retrieve  jk_jFEXPileupAndNoise_jTowerContainer " << m_jFEXPileupAndNoise_jTowerContainerKey.key());
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode LVL1::jFEXPileupAndNoise::reset() {
    m_is_FWD=0;
    m_apply_jets=0; 
    m_apply_met=0;

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


std::vector<int> LVL1::jFEXPileupAndNoise::CalculatePileup(){
    
    reset_conters();
    if(m_is_FWD){ // for the FWD mapping
        for(int iphi=0;iphi<FEXAlgoSpaceDefs::jFEX_algoSpace_height;iphi++){
            for(int ieta=0;ieta<FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width;ieta++){
                if(m_FPGA_forward[iphi][ieta] == 0) continue; //skipping TTID iqual to 0
                //storing the energies
                int tmp_energy_EM  = getET_EM(m_FPGA_forward[iphi][ieta]);
                int tmp_energy_HAD = getET_HAD(m_FPGA_forward[iphi][ieta]);
                int tmp_energy = getTTowerET(m_FPGA_forward[iphi][ieta]);
                m_FPGA_ET_forward_EM[iphi][ieta]  = tmp_energy_EM ; 
                m_FPGA_ET_forward_HAD[iphi][ieta] = tmp_energy_HAD;
                
                //calculating rho's
                
                int tmp_eta = getTTowerEta(m_FPGA_forward[iphi][ieta]);
                if(tmp_eta < 32){
                    if(tmp_energy_EM > 0 and tmp_energy_EM < 20000) {//lower and upper threshold must be get from the L1Calo DB
                        m_rho_EM += tmp_energy_EM * 1.0; // the 1.0 should be a float value from the L1Calo DB
                        m_count_rho_EM++;
                    }
                }
                
                if(tmp_eta < 15){
                    if(tmp_energy_HAD > 0 and tmp_energy_HAD < 20000){ //lower and upper threshold must be get from the L1Calo DB                    
                        m_rho_HAD1 += tmp_energy_HAD * 1.0; // the 1.0 should be a float value from the L1Calo DB
                        m_count_rho_HAD1++;
                    }
                }
                else if(tmp_eta < 16 ){
                    if(tmp_energy_HAD > 0 and tmp_energy_HAD < 20000){ //lower and upper threshold must be get from the L1Calo DB        
                        m_rho_HAD2 += tmp_energy_HAD * 1.0; // the 1.0 should be a float value from the L1Calo DB
                        m_count_rho_HAD2++;
                    }
                }
                else if(tmp_eta > 30 and m_FPGA_forward[iphi][ieta]>=700000 ){
                    m_FPGA_ET_central_HAD[iphi][ieta] = tmp_energy; //This corrects the FCAL layer 0 (which is an EM layer)
                    if(tmp_energy > 0 and tmp_energy < 20000){ //lower and upper threshold must be get from the L1Calo DB         
                        m_rho_FCAL += tmp_energy * 1.0; // the 1.0 should be a float value from the L1Calo DB
                        m_count_rho_FCAL++;
                    }
                }                 
                else if(tmp_eta <= 32 ){
                    if(tmp_energy_HAD > 0 and tmp_energy_HAD < 20000){ //lower and upper threshold must be get from the L1Calo DB       
                        m_rho_HAD3 += tmp_energy_HAD * 1.0; // the 1.0 should be a float value from the L1Calo DB
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
                int tmp_energy_EM = getET_EM(m_FPGA_central[iphi][ieta]);
                int tmp_energy_HAD = getET_HAD(m_FPGA_central[iphi][ieta]);
                int tmp_energy = getTTowerET(m_FPGA_central[iphi][ieta]);
                m_FPGA_ET_central_EM[iphi][ieta]  = tmp_energy_EM ; 
                m_FPGA_ET_central_HAD[iphi][ieta] = tmp_energy_HAD;
                
                //calculating rho's
                
                int tmp_eta = getTTowerEta(m_FPGA_central[iphi][ieta]);
                if(tmp_eta < 32){
                    if(tmp_energy_EM > 0 and tmp_energy_EM < 20000){ //lower and upper threshold must be get from the L1Calo DB
                        m_rho_EM += tmp_energy_EM * 1.0; // the 1.0 should be a float value from the L1Calo DB
                        m_count_rho_EM++;
                    }
                }
                
                if(tmp_eta < 15){
                    if(tmp_energy_HAD > 0 and tmp_energy_HAD < 20000){ //lower and upper threshold must be get from the L1Calo DB                    
                        m_rho_HAD1 += tmp_energy_HAD * 1.0; // the 1.0 should be a float value from the L1Calo DB
                        m_count_rho_HAD1++;
                    }
                }
                else if(tmp_eta < 16 ){
                    if(tmp_energy_HAD > 0 and tmp_energy_HAD < 20000){ //lower and upper threshold must be get from the L1Calo DB        
                        m_rho_HAD2 += tmp_energy_HAD * 1.0; // the 1.0 should be a float value from the L1Calo DB
                        m_count_rho_HAD2++;
                    }
                }
                else if(tmp_eta > 30  and m_FPGA_central[iphi][ieta]>=700000 ){ //this should not happen since  central jFEX is not FWD
                    m_FPGA_ET_central_HAD[iphi][ieta] = tmp_energy; //This corrects the FCAL layer 0 (which is an EM layer)
                    if(tmp_energy > 0 and tmp_energy < 20000){ //lower and upper threshold must be get from the L1Calo DB         
                        m_rho_FCAL += tmp_energy * 1.0; // the 1.0 should be a float value from the L1Calo DB
                        m_count_rho_FCAL++;
                    }
                }                 
                else if(tmp_eta < 32 ){
                    if(tmp_energy_HAD > 0 and tmp_energy_HAD < 20000){ //lower and upper threshold must be get from the L1Calo DB       
                        m_rho_HAD3 += tmp_energy_HAD * 1.0; // the 1.0 should be a float value from the L1Calo DB
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

    std::vector<int> rho_values {m_rho_EM,m_rho_HAD1,m_rho_HAD2,m_rho_HAD3,m_rho_FCAL};
    
    
    SubtractPileup();
    
    
    return rho_values;
}

void LVL1::jFEXPileupAndNoise::SubtractPileup(){
    if(m_is_FWD){ // for the FWD mapping
        
        for(int iphi=0;iphi<FEXAlgoSpaceDefs::jFEX_algoSpace_height;iphi++){
            for(int ieta=0;ieta<FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width;ieta++){ 
                if(m_FPGA_forward[iphi][ieta] == 0) continue; //skipping TTID iqual to 0
                int tmp_eta = getTTowerEta(m_FPGA_forward[iphi][ieta]);
                    
                if(tmp_eta < 32){
                    m_FPGA_ET_forward_EM[iphi][ieta]=m_FPGA_ET_forward_EM[iphi][ieta]-m_rho_EM * 1.0; // the 1.0 should be a float value from the L1Calo DB
                }
                
                if(tmp_eta < 15){
                    m_FPGA_ET_forward_HAD[iphi][ieta]=m_FPGA_ET_forward_HAD[iphi][ieta]-m_rho_HAD1 * 1.0; // the 1.0 should be a float value from the L1Calo DB
                }
                else if(tmp_eta < 16 ){
                    m_FPGA_ET_forward_HAD[iphi][ieta]=m_FPGA_ET_forward_HAD[iphi][ieta]-m_rho_HAD2 * 1.0; // the 1.0 should be a float value from the L1Calo DB
                }
                else if(tmp_eta > 30 and m_FPGA_central[iphi][ieta]>=700000){
                    m_FPGA_ET_forward_HAD[iphi][ieta]=m_FPGA_ET_forward_HAD[iphi][ieta]-m_rho_FCAL * 1.0; // the 1.0 should be a float value from the L1Calo DB
                } 
                else if(tmp_eta < 32 ){
                    m_FPGA_ET_forward_HAD[iphi][ieta]=m_FPGA_ET_forward_HAD[iphi][ieta]-m_rho_HAD3 * 1.0; // the 1.0 should be a float value from the L1Calo DB
                }
                
                
            }
        }
           
    }
    else{
        
        for(int iphi=0;iphi<FEXAlgoSpaceDefs::jFEX_algoSpace_height;iphi++){
            for(int ieta=0;ieta<FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width;ieta++){ 
                
                int tmp_eta = getTTowerEta(m_FPGA_central[iphi][ieta]);
                    
                if(tmp_eta < 32){
                    m_FPGA_ET_central_EM[iphi][ieta]=m_FPGA_ET_central_EM[iphi][ieta]-m_rho_EM * 1.0; // the 1.0 should be a float value from the L1Calo DB
                }
                
                if(tmp_eta < 15){
                    m_FPGA_ET_central_HAD[iphi][ieta]=m_FPGA_ET_central_HAD[iphi][ieta]-m_rho_HAD1 * 1.0; // the 1.0 should be a float value from the L1Calo DB
                }
                else if(tmp_eta < 16 ){
                    m_FPGA_ET_central_HAD[iphi][ieta]=m_FPGA_ET_central_HAD[iphi][ieta]-m_rho_HAD2 * 1.0; // the 1.0 should be a float value from the L1Calo DB
                }
                else if(tmp_eta > 30  and m_FPGA_central[iphi][ieta]>=700000 ){ 
                    m_FPGA_ET_central_HAD[iphi][ieta]=m_FPGA_ET_central_HAD[iphi][ieta]-m_rho_FCAL * 1.0; // the 1.0 should be a float value from the L1Calo DB
                } 
                else if(tmp_eta < 32 ){
                    m_FPGA_ET_central_HAD[iphi][ieta]=m_FPGA_ET_central_HAD[iphi][ieta]-m_rho_HAD3 * 1.0; // the 1.0 should be a float value from the L1Calo DB
                }
                
                
            }
        }
                   
    }
    

}


//Flags that allow to apply the pileup to the objets
void LVL1::jFEXPileupAndNoise::ApplyPileupJets  (){
    m_apply_jets=1;
}

void LVL1::jFEXPileupAndNoise::ApplyPileupMet  (){
    m_apply_met=1;
}



std::map<int,std::vector<int> > LVL1::jFEXPileupAndNoise::Get_EM_Et_values(){
    
    // map for energies sent to the FPGA
    std::map<int,std::vector<int> > map_Etvalues;
    map_Etvalues.clear();
    
    // tmp variable to fill the map
    std::vector<int> v_enegies;
    v_enegies.clear();
    v_enegies.resize(2,0);
    
    
    if(m_is_FWD){ // for the FWD mapping
        
        for(int iphi=0;iphi<FEXAlgoSpaceDefs::jFEX_algoSpace_height;iphi++){
            for(int ieta=0;ieta<FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width;ieta++){
                if(m_FPGA_forward[iphi][ieta] == 0) continue;
                //reset variables
                v_enegies.clear();
                v_enegies.resize(2,0);
                
                //saving the SG energy
                int tmp_TotalEt_jet=getET_EM(m_FPGA_forward[iphi][ieta]);
                int tmp_TotalEt_met=getET_EM(m_FPGA_forward[iphi][ieta]);
                
                // if true changing the raw energy to the pileup subtracted energy for jets
                if(m_apply_jets){
                    tmp_TotalEt_jet = m_FPGA_ET_forward_EM[iphi][ieta];
                }
                
                // if true changing the raw energy to the pileup subtracted energy for met
                if(m_apply_met){
                    tmp_TotalEt_met = m_FPGA_ET_forward_EM[iphi][ieta];
                }
                
                v_enegies[0]=tmp_TotalEt_jet;
                v_enegies[1]=tmp_TotalEt_met;

                map_Etvalues.insert(std::make_pair(m_FPGA_forward[iphi][ieta], v_enegies));
            }
        }
           
    }
    else{
        
        for(int iphi=0;iphi<FEXAlgoSpaceDefs::jFEX_algoSpace_height;iphi++){
            for(int ieta=0;ieta<FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width;ieta++){ 
                
                //reset variables
                v_enegies.clear();
                v_enegies.resize(2,0);
                
                //saving the SG energy
                int tmp_TotalEt_jet=getET_EM(m_FPGA_central[iphi][ieta]);
                int tmp_TotalEt_met=getET_EM(m_FPGA_central[iphi][ieta]);
                
                // if true changing the raw energy to the pileup subtracted energy for jets
                if(m_apply_jets){
                    tmp_TotalEt_jet = m_FPGA_ET_central_EM[iphi][ieta];
                }
                
                // if true changing the raw energy to the pileup subtracted energy for met
                if(m_apply_met){
                    tmp_TotalEt_met = m_FPGA_ET_central_EM[iphi][ieta];
                }
                
                v_enegies[0]=tmp_TotalEt_jet;
                v_enegies[1]=tmp_TotalEt_met;

                map_Etvalues.insert(std::make_pair(m_FPGA_central[iphi][ieta], v_enegies));                
                
                
            }
        }
                   
    }

    if(m_apply_noise) ApplyNoiseCuts(map_Etvalues,m_noisecut_EM_Jet,m_noisecut_EM_Met);

    return map_Etvalues;
}



std::map<int,std::vector<int> > LVL1::jFEXPileupAndNoise::Get_HAD_Et_values(){
    
    // map for energies sent to the FPGA
    std::map<int,std::vector<int> > map_Etvalues;
    map_Etvalues.clear();
    
    // tmp variable to fill the map
    std::vector<int> v_enegies;
    v_enegies.clear();
    v_enegies.resize(2,0);
    
    
    if(m_is_FWD){ // for the FWD mapping
        
        for(int iphi=0;iphi<FEXAlgoSpaceDefs::jFEX_algoSpace_height;iphi++){
            for(int ieta=0;ieta<FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width;ieta++){
                if(m_FPGA_forward[iphi][ieta] == 0) continue;
                //reset variables
                v_enegies.clear();
                v_enegies.resize(2,0);
                
                //saving the SG energy
                int tmp_TotalEt_jet=getET_HAD(m_FPGA_forward[iphi][ieta]);
                int tmp_TotalEt_met=getET_HAD(m_FPGA_forward[iphi][ieta]);
                
                // if true changing the raw energy to the pileup subtracted energy for jets
                if(m_apply_jets){
                    tmp_TotalEt_jet = m_FPGA_ET_forward_HAD[iphi][ieta];
                }
                
                // if true changing the raw energy to the pileup subtracted energy for met
                if(m_apply_met){
                    tmp_TotalEt_met = m_FPGA_ET_forward_HAD[iphi][ieta];
                }
                
                v_enegies[0]=tmp_TotalEt_jet;
                v_enegies[1]=tmp_TotalEt_met;

                map_Etvalues.insert(std::make_pair(m_FPGA_forward[iphi][ieta], v_enegies));
            }
        }
           
    }
    else{
        
        for(int iphi=0;iphi<FEXAlgoSpaceDefs::jFEX_algoSpace_height;iphi++){
            for(int ieta=0;ieta<FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width;ieta++){ 
                
                //reset variables
                v_enegies.clear();
                v_enegies.resize(2,0);
                
                //saving the SG energy
                int tmp_TotalEt_jet=getET_HAD(m_FPGA_central[iphi][ieta]);
                int tmp_TotalEt_met=getET_HAD(m_FPGA_central[iphi][ieta]);
                
                // if true changing the raw energy to the pileup subtracted energy for jets
                if(m_apply_jets){
                    tmp_TotalEt_jet = m_FPGA_ET_central_HAD[iphi][ieta];
                }
                
                // if true changing the raw energy to the pileup subtracted energy for met
                if(m_apply_met){
                    tmp_TotalEt_met = m_FPGA_ET_central_HAD[iphi][ieta];
                }
                
                v_enegies[0]=tmp_TotalEt_jet;
                v_enegies[1]=tmp_TotalEt_met;

                map_Etvalues.insert(std::make_pair(m_FPGA_central[iphi][ieta], v_enegies));                
                
                
            }
        }
                   
    }
    if(m_apply_noise) ApplyNoiseCuts(map_Etvalues,m_noisecut_HAD_Jet,m_noisecut_HAD_Met);

    return map_Etvalues;
}



void LVL1::jFEXPileupAndNoise::ApplyNoiseCuts(std::map<int,std::vector<int> > & map_Etvalues, int Jet_NoiseCut, int Met_NoiseCut){

    for(auto [key,vec] : map_Etvalues){
        
        if(map_Etvalues[key][0]<Jet_NoiseCut){ // Et for jets
            map_Etvalues[key][0]=0.;
        }
        if(map_Etvalues[key][1]<Met_NoiseCut){ // Et for Met
            map_Etvalues[key][1]=0.;
        }
    }
    
}




std::map<int,std::vector<int> > LVL1::jFEXPileupAndNoise::GetEt_values(){
    
    // map for energies sent to the FPGA
    std::map<int,std::vector<int> > map_Etvalues;
    std::map<int,std::vector<int> > map_Etvalues_EM;
    std::map<int,std::vector<int> > map_Etvalues_HAD;
    map_Etvalues.clear();
    map_Etvalues_EM.clear();
    map_Etvalues_HAD.clear();    
    
    
    /* 
     *  The vector Et_energy has size 2
     *  Et_energy[0] is used un the Jet algos
     *  Et_energy[1] is used un the Met/SumEt algos
     */
    std::vector<int> Et_energy;

    
    map_Etvalues_EM = Get_EM_Et_values();
    map_Etvalues_HAD = Get_HAD_Et_values();
    
    
    for(auto [key,vec] : map_Etvalues_EM){
        
        Et_energy.clear();
        Et_energy.resize(2,0);
        
        Et_energy[0]=map_Etvalues_EM[key][0]+map_Etvalues_HAD[key][0];
        Et_energy[1]=map_Etvalues_EM[key][1]+map_Etvalues_HAD[key][1];
        map_Etvalues[key] = Et_energy;
    }
       
    return map_Etvalues;
}



//Gets Eta of the TT
int LVL1::jFEXPileupAndNoise::getTTowerEta(unsigned int TTID) {

    SG::ReadHandle<jTowerContainer> jk_jFEXPileupAndNoise_jTowerContainer(m_jFEXPileupAndNoise_jTowerContainerKey);
    const LVL1::jTower * tmpTower = jk_jFEXPileupAndNoise_jTowerContainer->findTower(TTID);
    if(m_is_FWD) return std::abs(tmpTower->centreEta()*10); //For FWD we use the centre, due to the non-equal graularity
    
    return tmpTower->eta();
}
//Gets ET of the TT
int LVL1::jFEXPileupAndNoise::getTTowerET(unsigned int TTID) {
    
    SG::ReadHandle<jTowerContainer> jk_jFEXPileupAndNoise_jTowerContainer(m_jFEXPileupAndNoise_jTowerContainerKey);
    const LVL1::jTower * tmpTower = jk_jFEXPileupAndNoise_jTowerContainer->findTower(TTID);
    return tmpTower->getTotalET();
}
//Gets EM ET of the TT
int LVL1::jFEXPileupAndNoise::getET_EM(unsigned int TTID) {

    SG::ReadHandle<jTowerContainer> jk_jFEXPileupAndNoise_jTowerContainer(m_jFEXPileupAndNoise_jTowerContainerKey);
    const LVL1::jTower * tmpTower = jk_jFEXPileupAndNoise_jTowerContainer->findTower(TTID);
    return tmpTower->getET_EM();
}
//Gets HAD ET of the TT
int LVL1::jFEXPileupAndNoise::getET_HAD(unsigned int TTID) {

    SG::ReadHandle<jTowerContainer> jk_jFEXPileupAndNoise_jTowerContainer(m_jFEXPileupAndNoise_jTowerContainerKey);
    const LVL1::jTower * tmpTower = jk_jFEXPileupAndNoise_jTowerContainer->findTower(TTID);
    return tmpTower->getET_HAD();
}





}// end of namespace LVL1
