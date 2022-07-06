/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//              jFEXForwardElecTOB - TOBs Forward Elec  Algorithm in jFEX
//                              -------------------
//     begin                : 28.11.2021
//     email                : ulla.blumenschein@cern.ch
//***************************************************************************

#include "L1CaloFEXSim/jFEXForwardElecTOB.h"

LVL1::jFEXForwardElecTOB::jFEXForwardElecTOB():
  m_eta{99999},
  m_phi{99999},
  m_ET{99999},
  m_Iso{99999},
  m_EMf1{99999},
  m_EMf2{99999},
  m_Sat{99999},
  m_Res{99999}
{}

// Data locations within word                                                                                                             
// Sat: 0                                                                                                                                 
// Phi 1-4                                                                                                                                
// Eta 5-9                                                                                                                                
// ET: 10-20                                                                                                                              
// Iso: 21-22                                                                                                                             
// FracA: 23-24                                                                                                                           
// FracB: 25-26                                                                                                                           
// Res: 27-31     

void LVL1::jFEXForwardElecTOB::setEta(unsigned int eta)
{
  m_eta = eta;
}

void LVL1::jFEXForwardElecTOB::setPhi(unsigned int phi)
{
  m_phi = phi;
}

void LVL1::jFEXForwardElecTOB::setET(unsigned int et)
{
  m_ET = et;
}

void LVL1::jFEXForwardElecTOB::setIso(unsigned int iso)
{
  m_Iso = iso;
}

void LVL1::jFEXForwardElecTOB::setEMf1(unsigned int emf1)
{
  m_EMf1 = emf1;
}

void LVL1::jFEXForwardElecTOB::setEMf2(unsigned int emf2)
{
  m_EMf2 = emf2;
}


void LVL1::jFEXForwardElecTOB::setSat(unsigned int sat)
{
  m_Sat = sat;
}

void LVL1::jFEXForwardElecTOB::setRes(unsigned int res)
{
  m_Res = res;
}


unsigned int LVL1::jFEXForwardElecTOB::GetEta()
{
  return m_eta;	
}

unsigned int  LVL1::jFEXForwardElecTOB::GetPhi()
{
  return m_phi;  
}

unsigned int  LVL1::jFEXForwardElecTOB::GetET()
{
  return m_ET;
}

unsigned int  LVL1::jFEXForwardElecTOB::GetIso()
{
  return m_Iso;
}

unsigned int  LVL1::jFEXForwardElecTOB::GetEMf1()
{
  return m_EMf1;
}

unsigned int  LVL1::jFEXForwardElecTOB::GetEMf2()
{
  return m_EMf2;
}

unsigned int  LVL1::jFEXForwardElecTOB::GetSat()
{
  return m_Sat;
}

unsigned int  LVL1::jFEXForwardElecTOB::GetRes()
{
  return m_Res;
}
