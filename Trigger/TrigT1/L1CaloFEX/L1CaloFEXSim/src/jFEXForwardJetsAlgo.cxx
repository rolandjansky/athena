/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration  
*/
//***************************************************************************  
//		jFEXForwardJetsAlgo - Algorithm for forward Jets in jFEX
//                              -------------------
//     begin                : 07 06 2021
//     email                : varsiha.sothilingam@cern.ch
//***************************************************************************  
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "L1CaloFEXSim/jFEXForwardJetsAlgo.h"
#include "L1CaloFEXSim/jTower.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "L1CaloFEXSim/jFEXForwardJetsInfo.h"
#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"

namespace LVL1{

//Default Constructor
LVL1::jFEXForwardJetsAlgo::jFEXForwardJetsAlgo(const std::string& type, const std::string& name, const IInterface* parent):
   AthAlgTool(type, name, parent)   
  {
  declareInterface<IjFEXForwardJetsAlgo>(this);
  }

/** Destructor */
LVL1::jFEXForwardJetsAlgo::~jFEXForwardJetsAlgo()
{
}
StatusCode LVL1::jFEXForwardJetsAlgo::initialize()
{
   ATH_CHECK(m_jFEXForwardJetsAlgo_jTowerContainerKey.initialize());
   return StatusCode::SUCCESS;

}

//calls container for TT
StatusCode LVL1::jFEXForwardJetsAlgo::safetyTest() {
    m_jTowerContainer = SG::ReadHandle<jTowerContainer>(m_jFEXForwardJetsAlgo_jTowerContainerKey);
    if(! m_jTowerContainer.isValid()) {
        ATH_MSG_FATAL("Could not retrieve jTowerContainer " << m_jFEXForwardJetsAlgo_jTowerContainerKey.key());

        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode LVL1::jFEXForwardJetsAlgo::reset() {
    return StatusCode::SUCCESS;
}

void LVL1::jFEXForwardJetsAlgo::setup(int inputTable[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width], int jfex, int fpga) {
    std::copy(&inputTable[0][0], &inputTable[0][0] + (FEXAlgoSpaceDefs::jFEX_algoSpace_height*FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width), &m_jFEXalgoTowerID[0][0]);
    m_jfex=jfex;
    m_fpga=fpga;
    
}

//Gets geometric global centre Eta and Phi coord of the TT
//Has the advantage over the individual eta, phi methods that it does only one tower search
std::array<float,3> LVL1::jFEXForwardJetsAlgo::globalEtaPhiEt(int TTID) {
    if(TTID == 0) {
        return {999,999,-999};
    }
    
    float TT_Et = -999;
    if(m_map_Etvalues.find(TTID) != m_map_Etvalues.end()) {
        TT_Et = m_map_Etvalues[TTID][0];
    }    

    const LVL1::jTower *tmpTower = m_jTowerContainer->findTower(TTID);
    return {tmpTower->centreEta(),tmpTower->centrePhi(),TT_Et};
}

std::unordered_map<int, jFEXForwardJetsInfo> LVL1::jFEXForwardJetsAlgo::FcalJetsTowerIDLists() {
    
    std::unordered_map<int, jFEXForwardJetsInfo> FCALJetTowerIDLists;

    std::vector<int> lower_centre_neta;
    std::vector<int> upper_centre_neta;
    m_lowerEM_eta = 0;
    m_upperEM_eta = 0;

    //STEP 1: check if we are in module 0 or 5 and assign corrrect eta FEXAlgoSpace parameters
    if(m_jfex == 0) {
        //Module 0
        lower_centre_neta.assign({FEXAlgoSpaceDefs::jFEX_algoSpace_C_EMB_start_eta, FEXAlgoSpaceDefs::jFEX_algoSpace_C_EMIE_start_eta, FEXAlgoSpaceDefs::jFEX_algoSpace_C_FCAL_start_eta});
        upper_centre_neta.assign({FEXAlgoSpaceDefs::jFEX_algoSpace_C_EMB_end_eta, FEXAlgoSpaceDefs::jFEX_algoSpace_C_EMIE_end_eta,FEXAlgoSpaceDefs::jFEX_algoSpace_C_FCAL_end_eta });
        m_lowerEM_eta = FEXAlgoSpaceDefs::jFEX_algoSpace_C_lowerEM_eta;
        m_upperEM_eta = FEXAlgoSpaceDefs::jFEX_algoSpace_C_upperEM_eta;
    }
    else {
        //Module 5
        lower_centre_neta.assign({FEXAlgoSpaceDefs::jFEX_algoSpace_A_EMB_eta, FEXAlgoSpaceDefs::jFEX_algoSpace_A_EMIE_eta, FEXAlgoSpaceDefs::jFEX_algoSpace_A_FCAL_start_eta});
        upper_centre_neta.assign({FEXAlgoSpaceDefs::jFEX_algoSpace_A_EMIE_eta, FEXAlgoSpaceDefs::jFEX_algoSpace_A_FCAL_start_eta, FEXAlgoSpaceDefs::jFEX_algoSpace_A_FCAL_end_eta});

        m_lowerEM_eta = FEXAlgoSpaceDefs::jFEX_algoSpace_A_lowerEM_eta;
        m_upperEM_eta = FEXAlgoSpaceDefs::jFEX_algoSpace_A_upperEM_eta;

    }
    
    

    //STEP 2: define phi FEXAlgoSpace parameters
    std::vector<int> lower_centre_nphi{FEXAlgoSpaceDefs::jFEX_algoSpace_EMB_start_phi, FEXAlgoSpaceDefs::jFEX_algoSpace_EMIE_start_phi,  FEXAlgoSpaceDefs::jFEX_algoSpace_FCAL_start_phi};
    std::vector<int> upper_centre_nphi{FEXAlgoSpaceDefs::jFEX_algoSpace_EMB_end_phi, FEXAlgoSpaceDefs::jFEX_algoSpace_EMIE_end_phi,  FEXAlgoSpaceDefs::jFEX_algoSpace_FCAL_end_phi};

    //STEP 3: loop over different EM/FCAL0 eta phi core fpga regions. These are potential central trigger towers for jets
    for(int myCounter = 0; myCounter<3; myCounter++) {
        for(int centre_nphi = lower_centre_nphi[myCounter]; centre_nphi < upper_centre_nphi[myCounter]; centre_nphi++) {
            for(int centre_neta = lower_centre_neta[myCounter]; centre_neta < upper_centre_neta[myCounter]; centre_neta++) {

                //STEP 4: define TTID which will be the key for class in map
                int myTTIDKey = m_jFEXalgoTowerID[centre_nphi][centre_neta];

                //STEP 5: ignore when tower ID is zero. Should not happend though
                if(myTTIDKey == 0) {
                    continue;
                }

                //STEP 6: define class
                jFEXForwardJetsInfo TriggerTowerInformation;

                //STEP 7: fill local eta phi coords in the fpga matrix "m_jFEXalgoTowerID"
                TriggerTowerInformation.setCentreLocalTTPhi(centre_nphi);
                TriggerTowerInformation.setCentreLocalTTEta(centre_neta);

                const auto [centreTT_eta,centreTT_phi,centreTT_Et] = globalEtaPhiEt(myTTIDKey);
                TriggerTowerInformation.setCentreTTPhi(centreTT_phi);
                TriggerTowerInformation.setCentreTTEta(centreTT_eta);
                TriggerTowerInformation.includeTTinSeed(myTTIDKey);
                TriggerTowerInformation.addToSeedET(centreTT_Et);
                TriggerTowerInformation.includeTTinSearchWindow(myTTIDKey);
                TriggerTowerInformation.addToSearchWindowET(centreTT_Et);
                
                //STEP 8: loop over all FCAL0 TTIDs and fill TT IDs for seed and calculate seed energy
                for(int nphi = 0; nphi < FEXAlgoSpaceDefs::jFEX_algoSpace_height; nphi++) {
                    for(int neta = m_lowerEM_eta; neta < m_upperEM_eta; neta++) {
                        
                        int auxTTID = m_jFEXalgoTowerID[nphi][neta];
                        auto [TT_eta,TT_phi,TT_Et] = globalEtaPhiEt(auxTTID);
                        
                        if(auxTTID == myTTIDKey || auxTTID == 0) continue;
                        
                        //This corrects the overlap of FPGA 0 with FPGA 3 and viceversa
                        if(m_fpga==0 || m_fpga==3) { 
                            if(m_fpga==0) {
                                if(TT_phi>M_PI){
                                   TT_phi = TT_phi-m_2PI; 
                                }
                            }
                            else {
                                if(TT_phi<M_PI){
                                   TT_phi = TT_phi+m_2PI; 
                                }
                            }
                        }
                        
                        // cast float to int to avoid misbehaviours
                        int DeltaR = std::round(std::sqrt(std::pow((centreTT_eta - TT_eta)/0.1,2) + std::pow((centreTT_phi - TT_phi)/m_TT_Size_phi,2))*10);

                        if(DeltaR < 20 ) {
                            //STEP 9.0: fill TTID in seed
                            TriggerTowerInformation.includeTTinSeed(auxTTID);
                            //STEP 10.0: add ET value to seed
                            TriggerTowerInformation.addToSeedET(TT_Et);
                        }
                        else if(DeltaR < 40 ) {
                            TriggerTowerInformation.addToFirstEnergyRingET(TT_Et);
                            if(m_storeEnergyRingTTIDs) {
                                TriggerTowerInformation.includeTTIDinFirstER(auxTTID);
                            }
                        }
                        else if(DeltaR < 80){
                            TriggerTowerInformation.addToSecondEnergyRingET(TT_Et);
                            if(m_storeEnergyRingTTIDs) {
                                TriggerTowerInformation.includeTTIDinSecondER(auxTTID);
                            }    
                        }
                        
                        
                        if(DeltaR < 30 ) {
                            //STEP 9.1: fill TTID in search window
                            TriggerTowerInformation.includeTTinSearchWindow(auxTTID);
                            //STEP 10.1: add ET value to seed
                            TriggerTowerInformation.addToSearchWindowET(TT_Et);
                        }
                    }
                }
                //STEP 11: Store filled class into map, with central TT ID as Key
                FCALJetTowerIDLists[myTTIDKey] = TriggerTowerInformation;
            }//end of centre_neta loop
        }//end of centre_nphi loop
    }//end of myCounter loop
    
    return FCALJetTowerIDLists;
}



std::unordered_map<int, jFEXForwardJetsInfo> LVL1::jFEXForwardJetsAlgo::isSeedLocalMaxima() {
    //std::vector<int> localMaximas;
    std::unordered_map<int, jFEXForwardJetsInfo> localMaximaCandidates = FcalJetsTowerIDLists();
    std::unordered_map<int, jFEXForwardJetsInfo> localMaximaList ;

    uint isLocalMaxima = 0;
    
    for (auto& [myTTKey,myFCALJetInfoClass] : localMaximaCandidates){
        //Local maxima check takes place here
        isLocalMaxima = 0;
        float centre_phi = myFCALJetInfoClass.getCentreTTPhi();
        float centre_eta = myFCALJetInfoClass.getCentreTTEta();
        int centre_energy = myFCALJetInfoClass.getSeedET();

        const std::vector<int> TTinSW = myFCALJetInfoClass.getTTinSearchWindow();
        
        for (const int iTTinSW : TTinSW) {
            if(iTTinSW == myTTKey) continue;
            auto [seed_eta,seed_phi,seed_Et] = globalEtaPhiEt(iTTinSW);

            //This corrects the overlap of FPGA 0 with FPGA 3 and viceversa
            if(m_fpga==0 || m_fpga==3) { 
                if(m_fpga==0) {
                    if(seed_phi>M_PI){
                       seed_phi = seed_phi-m_2PI; 
                    }
                }
                else {
                    if(seed_phi<M_PI){
                       seed_phi = seed_phi+m_2PI; 
                    }
                }
            }


            int delta_phi = std::round((seed_phi - centre_phi)*100);
            int delta_eta = std::round((seed_eta - centre_eta)*100);
            
            int seed_energy = 0;
            
            //get the seed Et for iTTinSW, and check if it is a Local Maxima 
            if(localMaximaCandidates.count(iTTinSW)==1 ){
              seed_energy = localMaximaCandidates[iTTinSW].getSeedET();
            }
            // for the overlap regions there is not SeedET calculation, therefore the calculation is needed
            else{
                for(int nphi = 0; nphi < FEXAlgoSpaceDefs::jFEX_algoSpace_height; nphi++) {
                    for(int neta = m_lowerEM_eta; neta < m_upperEM_eta; neta++) {
                        int auxTTID = m_jFEXalgoTowerID[nphi][neta];
                        
                        if(auxTTID==0 ) {
                            continue;
                        }
                        auto [TT_eta,TT_phi,TT_Et] = globalEtaPhiEt(auxTTID);
                        
                        //This corrects the overlap of FPGA 0 with FPGA 3 and viceversa
                        if(m_fpga==0 || m_fpga==3) { 
                            if(m_fpga==0) {
                                if(TT_phi>M_PI){
                                   TT_phi = TT_phi-m_2PI; 
                                }
                            }
                            else {
                                if(TT_phi<M_PI){
                                   TT_phi = TT_phi+m_2PI; 
                                }
                            }
                        }
                        
                        // cast float to int to avoid misbehaviours
                        int DeltaR = std::round(std::sqrt(std::pow((seed_eta - TT_eta)/0.1,2) + std::pow((seed_phi - TT_phi)/m_TT_Size_phi,2))*10);
                        if(DeltaR < 20 ) {
                            seed_energy+=TT_Et;
                        }
                    }
                }
            }
            
            if( (delta_eta + delta_phi) < 0 || ((delta_eta + delta_phi) == 0 && delta_eta < 0) ) {
                if(centre_energy >= seed_energy) {
                    isLocalMaxima++;
                }
            }
            else {   
                if(centre_energy > seed_energy) {
                    isLocalMaxima++;
                }
            }
        }
        //if it is a local maxima, we save the TT ID
        if((isLocalMaxima == TTinSW.size()-1) && (isLocalMaxima !=0)) {
            localMaximaList[myTTKey] = myFCALJetInfoClass;
        }
    }
    return localMaximaList;
}


std::unordered_map<int, jFEXForwardJetsInfo> LVL1::jFEXForwardJetsAlgo::calculateJetETs() {

    // setting the lower/upper eta range for the FCAL 2 and 3 since they are not added in the seed information yet 
    int lowerFCAL_eta = FEXAlgoSpaceDefs::jFEX_algoSpace_C_lowerFCAL_eta;
    int upperFCAL_eta = FEXAlgoSpaceDefs::jFEX_algoSpace_C_upperFCAL_eta;
    if(m_jfex == 5) {
        //Module 5
        lowerFCAL_eta = FEXAlgoSpaceDefs::jFEX_algoSpace_A_lowerFCAL_eta;
        upperFCAL_eta = FEXAlgoSpaceDefs::jFEX_algoSpace_A_upperFCAL_eta;
    } 
    // Adding the FCAL 2 and 3 TT in the seed, 1st and 2nd energy rings
    std::unordered_map<int, jFEXForwardJetsInfo> localMaximas = isSeedLocalMaxima();
    for(auto& [myTTKey,myFCALJetInfoClass] : localMaximas) {
        float centreTT_phi = myFCALJetInfoClass.getCentreTTPhi();
        float centreTT_eta = myFCALJetInfoClass.getCentreTTEta();
        for(int nphi = 0; nphi < 8; nphi++) {
            for(int neta = lowerFCAL_eta; neta < upperFCAL_eta; neta++) {
                
                int auxTTID = m_jFEXalgoTowerID[nphi][neta];
                
                auto [TT_eta,TT_phi,TT_Et] = globalEtaPhiEt(auxTTID);
                
                //This corrects the overlap of FPGA 0 with FPGA 3 and viceversa
                if(m_fpga==0 || m_fpga==3) {
                    if(m_fpga==0) {
                        if(TT_phi>M_PI) {
                            TT_phi = TT_phi-m_2PI;
                        }
                    }
                    else {
                        if(TT_phi<M_PI) {
                            TT_phi = TT_phi+m_2PI;
                        }
                    }
                }
                
                // cast float to int to avoid misbehaviours
                int DeltaR = std::round(std::sqrt(std::pow((centreTT_eta - TT_eta)/0.1,2) + std::pow((centreTT_phi - TT_phi)/m_TT_Size_phi,2))*10); 
                if(DeltaR < 20 ) {
                    myFCALJetInfoClass.addToSeedET(TT_Et);
                    myFCALJetInfoClass.includeTTinSeed(auxTTID);
                }
                else if(DeltaR < 40 ) {
                    myFCALJetInfoClass.addToFirstEnergyRingET(TT_Et);
                    if(m_storeEnergyRingTTIDs) {
                        myFCALJetInfoClass.includeTTIDinFirstER(auxTTID);
                    }
                }
                else if(DeltaR < 80) {
                    myFCALJetInfoClass.addToSecondEnergyRingET(TT_Et);
                    if(m_storeEnergyRingTTIDs) {
                        myFCALJetInfoClass.includeTTIDinSecondER(auxTTID);
                    }
                }
            }
        }
        
    }
    //Returning the complite information 
    return localMaximas;
}


void LVL1::jFEXForwardJetsAlgo::setFPGAEnergy(std::unordered_map<int,std::vector<int> > et_map){
    m_map_Etvalues=et_map;
}

}// end of namespace LVL1

