/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration  
*/
//***************************************************************************  
//		jFEXmetAlgo - Algorithm for MET Algorithm in jFEX
//                              -------------------
//     begin                : 14 05 2021
//     email                : Sergi.Rodriguez@cern.ch
//***************************************************************************
#include <iostream>
#include <vector>
#include <stdio.h>
#include <math.h>
#include "L1CaloFEXSim/jFEXmetAlgo.h"
#include "L1CaloFEXSim/jFEXmetTOB.h"
#include "L1CaloFEXSim/jTower.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"
#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"

namespace LVL1{

//Default Constructor
LVL1::jFEXmetAlgo::jFEXmetAlgo(const std::string& type, const std::string& name, const IInterface* parent): AthAlgTool(type, name, parent) {
    declareInterface<IjFEXmetAlgo>(this);
}

/** Destructor */
LVL1::jFEXmetAlgo::~jFEXmetAlgo() {
}

StatusCode LVL1::jFEXmetAlgo::initialize() {
    ATH_CHECK(m_jFEXmetAlgo_jTowerContainerKey.initialize());
    return StatusCode::SUCCESS;
}

//calls container for TT
StatusCode LVL1::jFEXmetAlgo::safetyTest() {

    SG::ReadHandle<jTowerContainer> jk_jFEXmetAlgo_jTowerContainer(m_jFEXmetAlgo_jTowerContainerKey);
    if(! jk_jFEXmetAlgo_jTowerContainer.isValid()) {
        ATH_MSG_FATAL("Could not retrieve  jk_jFEXmetAlgo_jTowerContainer " << m_jFEXmetAlgo_jTowerContainerKey.key());
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode LVL1::jFEXmetAlgo::reset() {
    m_FPGA.clear();
    m_FPGA_phi02.clear();
    m_FPGA_fcal.clear();
    return StatusCode::SUCCESS;
}

//Setup for the central region
void LVL1::jFEXmetAlgo::setup(int FPGA[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width]) {

    ATH_MSG_DEBUG("---------------- jFEXmetAlgo::setup ----------------");
    m_FPGA.resize(FEXAlgoSpaceDefs::jFEX_algoSpace_height);

    for(int iphi=0;iphi<FEXAlgoSpaceDefs::jFEX_algoSpace_height;iphi++){
        for(int ieta=8;ieta<16;ieta++){
            m_FPGA[iphi].push_back(FPGA[iphi][ieta]);
        }
    }

}

//Setup for the forward region
void LVL1::jFEXmetAlgo::setup(int FPGA[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width]) {

    ATH_MSG_DEBUG("---------------- jFEXmetAlgo::setup ----------------");
    m_FPGA.resize(FEXAlgoSpaceDefs::jFEX_algoSpace_height);
    for(int iphi=0;iphi<FEXAlgoSpaceDefs::jFEX_algoSpace_height;iphi++){
        for(int ieta=8;ieta<17;ieta++){
            m_FPGA[iphi].push_back(FPGA[iphi][ieta]);
        }
    }
    m_FPGA_phi02.resize(16);
    for(int iphi=0;iphi<16;iphi++){
        for(int ieta=17;ieta<21;ieta++){
            m_FPGA_phi02[iphi].push_back(FPGA[iphi][ieta]);
        }
    }    
    m_FPGA_fcal.resize(8);
    for(int iphi=0;iphi<8;iphi++){
        for(int ieta=21;ieta<FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width;ieta++){
            m_FPGA_fcal[iphi].push_back(FPGA[iphi][ieta]);
        }
    }    
    
}



//this function calculates met in the central barrels
void LVL1::jFEXmetAlgo::buildBarrelmet()
{
    ATH_MSG_DEBUG("---------------- jFEXmetAlgo::buildBarrelmet ----------------");
    SG::ReadHandle<jTowerContainer> jk_jFEXmetAlgo_jTowerContainer(m_jFEXmetAlgo_jTowerContainerKey);

    m_Totalmet_Xcoord=0;
    m_Totalmet_Ycoord=0;    
    m_met.clear();
    m_met.resize(32,0);
    m_met_angle.clear();
    m_met_angle.resize(32,0);
    
    for(uint iphi=0;iphi<m_FPGA.size();iphi++){
        for(uint ieta=0;ieta<m_FPGA[iphi].size();ieta++){
            m_met[iphi]+=getTTowerET(m_FPGA[iphi][ieta]);
            
        }
        const LVL1::jTower * tmpTower = jk_jFEXmetAlgo_jTowerContainer->findTower(m_FPGA[iphi][0]);
        m_met_angle[iphi]=tmpTower->centrePhi();
    }
    
    buildMetXComponent();
    buildMetYComponent();
}

//this function calculates met in the forward modules 
void LVL1::jFEXmetAlgo::buildFWDmet()
{
    ATH_MSG_DEBUG("---------------- jFEXmetAlgo::buildFWDmet ----------------");
    SG::ReadHandle<jTowerContainer> jk_jFEXmetAlgo_jTowerContainer(m_jFEXmetAlgo_jTowerContainerKey);
    
    m_Totalmet_Xcoord=0;
    m_Totalmet_Ycoord=0;
    
    
    //Granularity of phi = 0.1
    m_met.clear();
    m_met.resize(m_FPGA.size(),0);
    m_met_angle.clear();
    m_met_angle.resize(m_FPGA.size(),0);
    
    for(uint iphi=0;iphi<m_FPGA.size();iphi++){
        for(uint ieta=0;ieta<m_FPGA[iphi].size();ieta++){
            m_met[iphi]+=getTTowerET(m_FPGA[iphi][ieta]);
        }
        const LVL1::jTower * tmpTower = jk_jFEXmetAlgo_jTowerContainer->findTower(m_FPGA[iphi][0]);
        m_met_angle[iphi]=tmpTower->centrePhi();
    }
    buildMetXComponent();
    buildMetYComponent();
    
    //Granularity of phi = 0.2
    m_met.clear();
    m_met.resize(m_FPGA_phi02.size(),0);
    m_met_angle.clear();
    m_met_angle.resize(m_FPGA_phi02.size(),0);
    
    for(uint iphi=0;iphi<m_FPGA_phi02.size();iphi++){
        for(uint ieta=0;ieta<m_FPGA_phi02[iphi].size();ieta++){
            m_met[iphi]+=getTTowerET(m_FPGA_phi02[iphi][ieta]);
        }
        const LVL1::jTower * tmpTower = jk_jFEXmetAlgo_jTowerContainer->findTower(m_FPGA_phi02[iphi][0]);
        m_met_angle[iphi]=tmpTower->centrePhi();
    }
    buildMetXComponent();
    buildMetYComponent();
    
    //Granularity of phi = 0.4
    m_met.clear();
    m_met.resize(m_FPGA_fcal.size(),0);
    m_met_angle.clear();
    m_met_angle.resize(m_FPGA_fcal.size(),0);
    
    for(uint iphi=0;iphi<m_FPGA_fcal.size();iphi++){
        for(uint ieta=0;ieta<m_FPGA_fcal[iphi].size();ieta++){
            m_met[iphi]+=getTTowerET(m_FPGA_fcal[iphi][ieta]);
        }
        const LVL1::jTower * tmpTower = jk_jFEXmetAlgo_jTowerContainer->findTower(m_FPGA_fcal[iphi][0]);
        m_met_angle[iphi]=M_PI/64 + tmpTower->phi()*(4*M_PI/32);
    }
    buildMetXComponent();
    buildMetYComponent();
}

//build Met X component for the central barrels
void LVL1::jFEXmetAlgo::buildMetXComponent()
{
    
    m_met_Xcoord.clear();
    m_met_Xcoord.resize(m_met.size(),0);
    
    //computing the X and Y component of MET
    for(uint iphi=0;iphi<m_met.size();iphi++){
        m_met_Xcoord[iphi]=m_met[iphi]*cos(m_met_angle[iphi]);
    }
    
    //Summing all X coordinate
    for(auto met_val : m_met_Xcoord){
        m_Totalmet_Xcoord += met_val;
    }
    
}

//return the X component of the Met
int LVL1::jFEXmetAlgo::GetMetXComponent()
{
    return m_Totalmet_Xcoord;
}

//build Met Y component for the central barrels
void LVL1::jFEXmetAlgo::buildMetYComponent()
{
        
    m_met_Ycoord.clear();
    m_met_Ycoord.resize(m_met.size(),0);
    
    //computing the X and Y component of MET
    for(uint iphi=0;iphi<m_met.size();iphi++){
        m_met_Ycoord[iphi]=m_met[iphi]*sin(m_met_angle[iphi]); 
    }
    
    //Summing all Y coordinate
    for(auto met_val : m_met_Ycoord){
        m_Totalmet_Ycoord += met_val;
    }
    
}

//return the Y component of the Met
int LVL1::jFEXmetAlgo::GetMetYComponent()
{
    return m_Totalmet_Ycoord;
}
         
std::unique_ptr<jFEXmetTOB> LVL1::jFEXmetAlgo::getmetTOBs(){

  std::unique_ptr<jFEXmetTOB> tob = std::make_unique<jFEXmetTOB>();


  tob->setMetX(m_Totalmet_Xcoord);
  tob->setMetY(m_Totalmet_Ycoord); 
  tob->setSat(0); 
  tob->setRes(0);
  return tob;
}

//Gets the ET for the TT. This ET is EM + HAD
int LVL1::jFEXmetAlgo::getTTowerET(unsigned int TTID ) {
    if(TTID == 0) {
        return 0;
    } 
    
    if(m_map_Etvalues.find(TTID) != m_map_Etvalues.end()) {
        return m_map_Etvalues[TTID][1];
    }
    
    //we shouldn't arrive here
    return 0;
    
}


void LVL1::jFEXmetAlgo::setFPGAEnergy(std::unordered_map<int,std::vector<int> > et_map){
    m_map_Etvalues=et_map;
}
}// end of namespace LVL1
