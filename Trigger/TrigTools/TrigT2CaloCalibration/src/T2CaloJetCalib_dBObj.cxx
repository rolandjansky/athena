/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT2CaloCalibration/T2CaloJetCalib_dBObj.h"

/** @class T2CaloJetCalib_dBObj
    
    LVL 2 jet calibration dB data objects
    Contains the weights, eta range, and energy cuts

    @author Malachi Schram
 */

// Constructor
T2CaloJetCalib_dBObj::T2CaloJetCalib_dBObj()
  : m_calib_weights(0), m_ve_cuts(0), m_eta_ranges(0)
{}

//Destructor 
T2CaloJetCalib_dBObj::~T2CaloJetCalib_dBObj(){}

// Set the calibration weight
void T2CaloJetCalib_dBObj::set_calib_weights(calib_dt this_calib_dt){
  m_calib_weights=this_calib_dt;
}

// Retrieves the energy cuts 
void T2CaloJetCalib_dBObj::set_ve_cuts(calib_dt this_calib_dt){
  m_ve_cuts=this_calib_dt;
}

// Retrieves the eta range of the weights 
void T2CaloJetCalib_dBObj::set_eta_ranges(calib_dt this_calib_dt){
  m_eta_ranges=this_calib_dt;
}
