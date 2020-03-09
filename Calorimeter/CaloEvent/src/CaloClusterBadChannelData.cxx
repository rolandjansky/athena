/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloEvent/CaloClusterBadChannelData.h"

CaloClusterBadChannelData::CaloClusterBadChannelData(float eta,
                                                     float phi,
                                                     CaloSampling::CaloSample layer,
                                                     const CaloBadChannel& flag)
  : m_eta (eta),
    m_phi (phi),
    m_layer (layer),
    m_flag (flag)
{
}

CaloClusterBadChannelData::CaloClusterBadChannelData() 
  : m_eta (-999),
    m_phi (-999),
    m_layer (CaloSampling::Unknown),
    m_flag (0u)
{
}

CaloClusterBadChannelData::CaloClusterBadChannelData(const CaloClusterBadChannelData* pBad) 
  : m_eta (pBad->m_eta),
    m_phi (pBad->m_phi),
    m_layer (pBad->m_layer),
    m_flag (pBad->m_flag)
{
}

CaloClusterBadChannelData::~CaloClusterBadChannelData() {}
