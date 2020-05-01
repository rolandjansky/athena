/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "xAODCaloEvent/CaloClusterChangeSignalState.h"

CaloClusterChangeSignalState::CaloClusterChangeSignalState (xAOD::CaloCluster* pClus, xAOD::CaloCluster::State s): 
  m_clus (pClus),
  m_oldstate (pClus->signalState())
{
  pClus->setSignalState(s);
  //std::cout << "SignalStateHelper: Setting state for clu " << pClus << std::endl;
}


CaloClusterChangeSignalState::~CaloClusterChangeSignalState()
{
  //std::cout << "SignalStateHelper: Resetting state for clu " << m_clus << std::endl;
  if (m_clus)
    m_clus->setSignalState(m_oldstate);
}

