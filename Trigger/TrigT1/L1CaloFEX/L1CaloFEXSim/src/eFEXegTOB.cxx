/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           eFEXegTOB.cxx  -  
//                              -------------------
//     begin                : 17 01 2020
//     email                : tong.qiu@cern.ch
//  **************************************************************************


#include "L1CaloFEXSim/eFEXegTOB.h"


LVL1::eFEXegTOB::eFEXegTOB():
  m_eta{99999},
  m_phi{99999},
  m_ET{99999},
  m_Reta_Core{99999},
  m_Reta_Env{99999},
  m_Rhad_EM{99999},
  m_Rhad_Had{99999},
  m_Wstot_Num{99999},
  m_Wstot_Den{99999},
  m_FPGA_ID{99999},
  m_eFEX_ID{99999},
  m_seed_UnD{1},
  m_seed{99999},
  m_tobword{0}
{}

unsigned int LVL1::eFEXegTOB::setEta(unsigned int eta) {
  m_eta = eta;
  return m_eta;
}

unsigned int LVL1::eFEXegTOB::setPhi(unsigned int phi) {
  m_phi = phi;
  return m_phi;
}

unsigned int LVL1::eFEXegTOB::setET(unsigned int et) {
  m_ET = et;
  return m_ET;
}

unsigned int LVL1::eFEXegTOB::setFPGAID(unsigned int fpgaid) {
  m_FPGA_ID = fpgaid;
  return m_FPGA_ID; 
}

unsigned int LVL1::eFEXegTOB::seteFEXID(unsigned int efexid) {
  m_eFEX_ID = efexid;
  return m_eFEX_ID; 
}

bool LVL1::eFEXegTOB::setSeedUnD(bool seedund) {
  m_seed_UnD = seedund;
  return m_seed_UnD;
}

unsigned int LVL1::eFEXegTOB::setSeed(unsigned int seed) {
  m_seed = seed;
  return m_seed;
}

unsigned int LVL1::eFEXegTOB::setRetaCore(unsigned int retaCore) {
  m_Reta_Core = retaCore;
  return m_Reta_Core;
}

unsigned int LVL1::eFEXegTOB::setRetaEnv(unsigned int retaEnv) {
  m_Reta_Env = retaEnv;
  return m_Reta_Env;
}

unsigned int LVL1::eFEXegTOB::setRhadEM(unsigned int rhadEM) {
  m_Rhad_EM = rhadEM;
  return m_Rhad_EM;
}

unsigned int LVL1::eFEXegTOB::setRhadHad(unsigned int rhadHad) {
  m_Rhad_Had = rhadHad;
  return m_Rhad_Had;
}

unsigned int LVL1::eFEXegTOB::setWstotNum(unsigned int wstot_Num) {
  m_Wstot_Num = wstot_Num;
  return m_Wstot_Num;
}

unsigned int LVL1::eFEXegTOB::setWstotDen(unsigned int wstot_Den) {
  m_Wstot_Den = wstot_Den;
  return m_Wstot_Den;
}

uint32_t LVL1::eFEXegTOB::setTobword(uint32_t tobword) {
  m_tobword = tobword;
  return m_tobword;
}
