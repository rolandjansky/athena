/*
 Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//              jFEXSmallRJetTOB - Algorithm for small R jet Algorithm in jFEX
//                              -------------------
//     begin                : 03 11 2020
//     email                : varsiha.sothilingam@cern.ch
//***************************************************************************

#include "L1CaloFEXSim/jFEXSmallRJetTOB.h"


LVL1::jFEXSmallRJetTOB::jFEXSmallRJetTOB():
  m_eta{99999},
  m_phi{99999},
  m_ET{99999},
  m_Res{99999},
  m_Sat{99999}
{}

//in total, 32 bits
//eta 5 bits local eta coord within fpga core area
//phi 4 bits local " "
//E_T 11 bits, transverse energy
//Res. 11 bits reserved
//Saturation, 1 bit, on jFEX input	

unsigned int LVL1::jFEXSmallRJetTOB::setEta(unsigned int eta)
{
  m_eta = eta;
  return m_eta;	
}

unsigned int LVL1::jFEXSmallRJetTOB::setPhi(unsigned int phi)
{
  m_phi = phi;
  return m_phi;  
}

unsigned int LVL1::jFEXSmallRJetTOB::setET(unsigned int et)
{
  m_ET = et;
  return m_ET;
}

unsigned int LVL1::jFEXSmallRJetTOB::setRes(unsigned int res)
{
  m_Res = res;
  return m_Res;
}

unsigned int LVL1::jFEXSmallRJetTOB::setSat(unsigned int sat)
{
  m_Sat = sat;
  return m_Sat;
}
