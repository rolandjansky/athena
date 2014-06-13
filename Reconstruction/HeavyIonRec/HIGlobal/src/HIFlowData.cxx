/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HIFlowData.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "HIGlobal/HICaloUtil.h"
#include "HIGlobal/HIFlowData.h"

#include "StoreGate/StoreGateSvc.h"

// psi for e()
void HIFlowData::Ssin(float *ssin)
{ 
  for(int i=0;i<HICaloUtil::Unknown;i++) m_Ssin[i] = ssin[i];
  return;
}
void HIFlowData::Scos(float *scos)
{ 
  for(int i=0;i<HICaloUtil::Unknown;i++) m_Scos[i] = scos[i];
  return;
}
void HIFlowData::Psi(float *psi)
{ 
  for(int i=0;i<HICaloUtil::Unknown;i++) m_Psi[i] = psi[i];
  return;
}
// psi for et()
void HIFlowData::SsinEt(float *ssin)
{ 
  for(int i=0;i<HICaloUtil::Unknown;i++) m_SsinEt[i] = ssin[i];
  return;
}
void HIFlowData::ScosEt(float *scos)
{ 
  for(int i=0;i<HICaloUtil::Unknown;i++) m_ScosEt[i] = scos[i];
  return;
}
void HIFlowData::PsiEt(float *psi)
{ 
  for(int i=0;i<HICaloUtil::Unknown;i++) m_PsiEt[i] = psi[i];
  return;
}

// v2 for e()
void HIFlowData::v2EtaEMB1(std::vector<float> *v2)
{
  for(int i=0;i<HICaloUtil::Unknown;i++){
    for(int j=0;j< (int)v2[i].size();j++){
      m_v2EtaEMB1[i].push_back(v2[i][j]);
    }
  }
}
void HIFlowData::v2EtaFCAL0(std::vector<float> *v2)
{
  for(int i=0;i<HICaloUtil::Unknown;i++){
    for(int j=0;j< (int)v2[i].size();j++){
      m_v2EtaFCAL0[i].push_back(v2[i][j]);
    }
  }
}
void HIFlowData::SumE(std::vector<float> *s)
{
  for(int i=0;i<HICaloUtil::Unknown;i++){
    for(int j=0;j< (int)s[i].size();j++){
      m_SumE[i].push_back(s[i][j]);
    }
  }
}
// v2 for et()
void HIFlowData::v2EtaEtEMB1(std::vector<float> *v2)
{
  for(int i=0;i<HICaloUtil::Unknown;i++){
    for(int j=0;j< (int)v2[i].size();j++){
      m_v2EtaEtEMB1[i].push_back(v2[i][j]);
    }
  }
}
void HIFlowData::v2EtaEtFCAL0(std::vector<float> *v2)
{
  for(int i=0;i<HICaloUtil::Unknown;i++){
    for(int j=0;j< (int)v2[i].size();j++){
      m_v2EtaEtFCAL0[i].push_back(v2[i][j]);
    }
  }
}
void HIFlowData::SumEt(std::vector<float> *s)
{
  for(int i=0;i<HICaloUtil::Unknown;i++){
    for(int j=0;j< (int)s[i].size();j++){
      m_SumEt[i].push_back(s[i][j]);
    }
  }
}
