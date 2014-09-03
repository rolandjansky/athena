/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloEvent/CaloClusterBadChannelData.h"

CaloClusterBadChannelData::CaloClusterBadChannelData(float eta,float phi,CaloSampling::CaloSample layer,const CaloBadChannel& flag)
{
 m_eta = eta;
 m_phi = phi;
 m_layer = layer;
 m_flag=flag;
}

CaloClusterBadChannelData::CaloClusterBadChannelData() 
{
 m_eta=-999.;
 m_phi=-999.;
 m_layer=CaloSampling::Unknown;
 m_flag=(unsigned int)(0);
}

CaloClusterBadChannelData::CaloClusterBadChannelData(const CaloClusterBadChannelData* pBad) 
{
 this->m_eta = pBad->getEta();
 this->m_phi = pBad->getPhi();
 this->m_layer = pBad->getLayer();
 this->m_flag=pBad->getFlag();
}

CaloClusterBadChannelData::~CaloClusterBadChannelData() {}
