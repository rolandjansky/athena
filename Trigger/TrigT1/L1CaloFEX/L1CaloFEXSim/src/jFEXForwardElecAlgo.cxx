/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//		jFEXForwardElecAlgo - Algorithm for Forward Electron Algorithm in jFEX
//                              -------------------
//     begin                : 16 11 2021
//     email                : Sergi.Rodriguez@cern.ch
//***************************************************************************
#include <iostream>
#include <vector>
#include <stdio.h>
#include <math.h>
#include "L1CaloFEXSim/jFEXForwardElecAlgo.h"
#include "L1CaloFEXSim/jTower.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"

namespace LVL1 {

//Default Constructor
LVL1::jFEXForwardElecAlgo::jFEXForwardElecAlgo(const std::string& type, const std::string& name, const IInterface* parent): AthAlgTool(type, name, parent) {
    declareInterface<IjFEXForwardElecAlgo>(this);
}

/** Destructor */
LVL1::jFEXForwardElecAlgo::~jFEXForwardElecAlgo() {
}

StatusCode LVL1::jFEXForwardElecAlgo::initialize() {
    ATH_CHECK(m_jTowerContainerKey.initialize());
    return StatusCode::SUCCESS;
}

//calls container for TT
StatusCode LVL1::jFEXForwardElecAlgo::safetyTest() {
    
    m_jTowerContainer = SG::ReadHandle<jTowerContainer>(m_jTowerContainerKey);
    if(! m_jTowerContainer.isValid()) {
        ATH_MSG_FATAL("Could not retrieve jTowerContainer " << m_jTowerContainerKey.key());
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode LVL1::jFEXForwardElecAlgo::reset() {
    return StatusCode::SUCCESS;
}

void LVL1::jFEXForwardElecAlgo::setup() {
    
    // this is just to setup the algorithm, you will need more functions (you can take a look at jTau or jJ algorithms) 
    ATH_MSG_DEBUG("---------------- jFEXForwardElecAlgo::setup ----------------");

}




//Gets the EM Et for the TT.
int LVL1::jFEXForwardElecAlgo::getTTowerET_EM(unsigned int TTID ) {
    if(TTID == 0) {
        return 0;
    }

    if(m_map_Etvalues_EM.find(TTID) != m_map_Etvalues_EM.end()) {
        return m_map_Etvalues_EM[TTID][0];
    }

    //we shouldn't arrive here
    return 0;

}

//Gets the HAD Et for the TT.
int LVL1::jFEXForwardElecAlgo::getTTowerET_HAD(unsigned int TTID ) {
    if(TTID == 0) {
        return 0;
    }

    if(m_map_Etvalues_HAD.find(TTID) != m_map_Etvalues_HAD.end()) {
        return m_map_Etvalues_HAD[TTID][0];
    }

    //we shouldn't arrive here
    return 0;

}

//Gets the Total Et for the TT.
int LVL1::jFEXForwardElecAlgo::getTTowerTotalET(unsigned int TTID ) {
    return getTTowerET_HAD(TTID) + getTTowerET_EM(TTID);

}

//Gets Phi of the TT
int LVL1::jFEXForwardElecAlgo::getPhi(unsigned int TTID ) {
    if(TTID == 0) {
        return 0;
    }

    const LVL1::jTower * tmpTower = m_jTowerContainer->findTower(TTID);
    return tmpTower->centrePhi();
}
//Gets Eta of the TT
int LVL1::jFEXForwardElecAlgo::getEta(unsigned int TTID ) {
    if(TTID == 0) {
        return 0;
    }

    const LVL1::jTower * tmpTower = m_jTowerContainer->findTower(TTID);
    return tmpTower->centreEta();
}

void LVL1::jFEXForwardElecAlgo::setFPGAEnergy(std::unordered_map<int,std::vector<int> > et_map_EM,std::unordered_map<int,std::vector<int> > et_map_HAD) {
    m_map_Etvalues_EM=et_map_EM;
    m_map_Etvalues_HAD=et_map_HAD;
}



}// end of namespace LVL1
