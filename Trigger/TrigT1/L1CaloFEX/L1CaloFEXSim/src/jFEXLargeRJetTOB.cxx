/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//              jFEXLargeRJetAlgo - Algorithm for large R jet Algorithm in jFEX
//                              -------------------
//     begin                : 22 01 2021
//     email                : varsiha.sothilingam@cern.ch
//***************************************************************************

#include "L1CaloFEXSim/jFEXLargeRJetTOB.h"

LVL1::jFEXLargeRJetTOB::jFEXLargeRJetTOB():
  m_eta{99999},
  m_phi{99999},
  m_ET{99999},
  m_Sub{99999},
  m_Sat{99999}
{}

unsigned int LVL1::jFEXLargeRJetTOB::setEta(unsigned int eta)
{
  m_eta = eta;
  return m_eta;
}

unsigned int LVL1::jFEXLargeRJetTOB::setPhi(unsigned int phi)
{
  m_phi = phi;
  return m_phi;
}

unsigned int LVL1::jFEXLargeRJetTOB::setET(unsigned int et)
{
  m_ET = et;
  return m_ET;
}

unsigned int LVL1::jFEXLargeRJetTOB::setSub(unsigned int Sub)
{
  m_Sub = Sub;
  return m_Sub;
}

unsigned int LVL1::jFEXLargeRJetTOB::setSat(unsigned int sat)
{
  m_Sat = sat;
  return m_Sat;
}
 
