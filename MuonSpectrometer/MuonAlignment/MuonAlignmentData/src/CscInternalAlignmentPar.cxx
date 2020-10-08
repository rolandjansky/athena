/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAlignmentData/CscInternalAlignmentPar.h"

CscInternalAlignmentPar::CscInternalAlignmentPar():
  MuonAlignmentPar(),
  m_wireLayer(0),
  m_S(0.0),
  m_Z(0.0),
  m_T(0.0),
  m_rotS(0.0),
  m_rotZ(0.0),
  m_rotT(0.0)
{ }

void CscInternalAlignmentPar::setParameters(float s, float z, float t, 
			     float rotS, float rotZ, float rotT)
{
  m_S = s;
  m_Z = z;
  m_T = t;
  m_rotS = rotS;
  m_rotZ = rotZ;
  m_rotT = rotT;
}

void CscInternalAlignmentPar::getParameters(float& s, float& z, float& t, 
			     float& rotS, float& rotZ, float& rotT) const
{
  s    = m_S;
  z    = m_Z;
  t    = m_T;
  rotS = m_rotS;
  rotZ = m_rotZ;
  rotT = m_rotT;
}

void CscInternalAlignmentPar::setAmdbId(const std::string& type, int jff, int jzz, int job, int wireLayer) 
{
  m_Type = type;
  m_Jff  = jff;
  m_Jzz  = jzz;
  m_Job  = job;  
  m_wireLayer = wireLayer; 

  return;
}

void CscInternalAlignmentPar::getAmdbId(std::string& type, int& jff, int& jzz, int& job, int& wireLayer)  const
{
  type = m_Type;
  jff  = m_Jff;
  jzz  = m_Jzz;
  job  = m_Job;  
  wireLayer = m_wireLayer; 

  return;
}
