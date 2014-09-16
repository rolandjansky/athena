/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAlignmentData/MuonAlignmentPar.h"

MuonAlignmentPar::MuonAlignmentPar() 
{ }

MuonAlignmentPar::~MuonAlignmentPar() 
{ }



void MuonAlignmentPar::setAmdbId(std::string type, int jff, int jzz, int job) 
{
  m_Type = type;
  m_Jff  = jff;
  m_Jzz  = jzz;
  m_Job  = job;  

  return;
}

void MuonAlignmentPar::getAmdbId(std::string& type, int& jff, int& jzz, int& job) 
{
  type = m_Type;
  jff  = m_Jff;
  jzz  = m_Jzz;
  job  = m_Job;  

  return;
}

