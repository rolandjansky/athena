/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   Scintillator.cxx
 * Author: Marco van Woerden <mvanwoer@cern.ch>
 * Description: Scintillator object that stores positions and dimensions.
 *
 * Created in February 2013.
 */

#include "Scintillator.h"

//===========================
Scintillator::Scintillator(){
//===========================
  setPosition (0,0,0);
  setDimension(0,0,0);
  setEtaDeta(0,0);
} // Scintillator::Scintillator

//======================================================================================
Scintillator::Scintillator(float r, float phi, float z, float dr, float dphi, float dz){
//======================================================================================
    setPosition (r, phi, z);
    setDimension(dr,dphi,dz);
  setEtaDeta(0,0);
} // Scintillator::Scintillator

//=============================================================================================================
Scintillator::Scintillator(float r, float phi, float z, float dr, float dphi, float dz, float eta, float deta){
//=============================================================================================================
    setPosition (r, phi, z);
    setDimension(dr,dphi,dz);
    setEtaDeta(eta,deta);
} // Scintillator::Scintillator

//============================
Scintillator::~Scintillator(){
//============================
} // Scintillator::~Scintillator

//==========================================================
void Scintillator::setPosition(float r, float phi, float z){
//==========================================================
    m_r    = r;
    m_phi  = phi;
    m_z    = z;
} // Scintillator::setPosition

//===================================================
void Scintillator::setEtaDeta(float eta, float deta){
//===================================================
    m_eta  = eta;
    m_deta = deta;
} // Scintillator::setEtaDeta

//==============================================================
void Scintillator::setDimension(float dr, float dphi, float dz){
//==============================================================
    m_dr   = dr;
    m_dphi = dphi;
    m_dz   = dz;
} // Scintillator::setPosition