/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//              jFEXsumETTOB - TOBs Sum ET Algorithm in jFEX
//                              -------------------
//     begin                : 12 05 2021
//     email                : Sergi.Rodriguez@cern.ch
//***************************************************************************

#include "L1CaloFEXSim/jFEXsumETTOB.h"


LVL1::jFEXsumETTOB::jFEXsumETTOB():
  m_ETlow{99999},
  m_EThigh{99999},
  m_satlow{99999},
  m_sathigh{99999}
{}

//in total, 32 bits
//eta 5 bits local eta coord within fpga core area
//phi 4 bits local " "
//E_T 11 bits, transverse energy
//ISO. 11 bits reserved
//Saturation, 1 bit, on jFEX input	

void LVL1::jFEXsumETTOB::setETlow(unsigned int Etlow)
{
  m_ETlow = Etlow;
}

void LVL1::jFEXsumETTOB::setEThigh(unsigned int Ethigh)
{
  m_EThigh = Ethigh;
}

void LVL1::jFEXsumETTOB::setSatlow(unsigned int Satlow)
{
  m_satlow = Satlow;
}

void LVL1::jFEXsumETTOB::setSathigh(unsigned int Sathigh)
{
  m_sathigh = Sathigh;
}




unsigned int LVL1::jFEXsumETTOB::GetETlow()
{
  return m_ETlow;	
}

unsigned int  LVL1::jFEXsumETTOB::GetEThigh()
{
  return m_EThigh;  
}

unsigned int  LVL1::jFEXsumETTOB::GetSatlow()
{
  return m_satlow;
}

unsigned int  LVL1::jFEXsumETTOB::GetSatHigh()
{
  return m_sathigh;
}

