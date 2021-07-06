/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//              jFEXmetTOB - TOBs MET Algorithm in jFEX
//                              -------------------
//     begin                : 14 05 2021
//     email                : Sergi.Rodriguez@cern.ch
//***************************************************************************

#include "L1CaloFEXSim/jFEXmetTOB.h"


LVL1::jFEXmetTOB::jFEXmetTOB():
  m_MET_X{99999},
  m_MET_Y{99999},
  m_Sat{99999},
  m_Res{99999}
{}

//in total, 32 bits
//MET X coordenate 15 bits
//Saturation in jFEX 1 bit
//MET Y coordinate 15 bits
//REseved 1 bit 

void LVL1::jFEXmetTOB::setMetX(unsigned int metX)
{
  m_MET_X = metX;
}

void LVL1::jFEXmetTOB::setMetY(unsigned int metY)
{
  m_MET_Y = metY;
}

void LVL1::jFEXmetTOB::setSat(unsigned int metsat)
{
  m_Sat = metsat;
}

void LVL1::jFEXmetTOB::setRes(unsigned int metres)
{
  m_Res = metres;
}




unsigned int LVL1::jFEXmetTOB::GetMetX()
{
  return m_MET_X;	
}

unsigned int  LVL1::jFEXmetTOB::GetMetY()
{
  return m_MET_Y;  
}

unsigned int  LVL1::jFEXmetTOB::GetSat()
{
  return m_Sat;
}

unsigned int  LVL1::jFEXmetTOB::GetRes()
{
  return m_Res;
}

