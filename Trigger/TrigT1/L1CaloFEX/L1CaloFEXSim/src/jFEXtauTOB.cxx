/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//              jFEXtauTOB - TOBs Tau Algorithm in jFEX
//                              -------------------
//     begin                : 18 02 2021
//     email                : Sergi.Rodriguez@cern.ch
//***************************************************************************

#include "L1CaloFEXSim/jFEXtauTOB.h"


LVL1::jFEXtauTOB::jFEXtauTOB():
  m_eta{99999},
  m_phi{99999},
  m_ET{99999},
  m_Iso{99999},
  m_Sat{99999}
{}

//in total, 32 bits
//eta 5 bits local eta coord within fpga core area
//phi 4 bits local " "
//E_T 11 bits, transverse energy
//ISO. 11 bits reserved
//Saturation, 1 bit, on jFEX input	

void LVL1::jFEXtauTOB::setEta(unsigned int eta)
{
  m_eta = eta;
}

void LVL1::jFEXtauTOB::setPhi(unsigned int phi)
{
  m_phi = phi;
}

void LVL1::jFEXtauTOB::setET(unsigned int et)
{
  m_ET = et;
}

void LVL1::jFEXtauTOB::setIso(unsigned int Iso)
{
  m_Iso = Iso;
}

void LVL1::jFEXtauTOB::setSat(unsigned int sat)
{
  m_Sat = sat;
}

unsigned int LVL1::jFEXtauTOB::GetEta()
{
  return m_eta;	
}

unsigned int  LVL1::jFEXtauTOB::GetPhi()
{
  return m_phi;  
}

unsigned int  LVL1::jFEXtauTOB::GetET()
{
  return m_ET;
}

unsigned int  LVL1::jFEXtauTOB::GetIso()
{
  return m_Iso;
}

unsigned int  LVL1::jFEXtauTOB::GetSat()
{
  return m_Sat;
}
