/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloRxLayers.h"

L1CaloRxLayers::L1CaloRxLayers() :
  m_channelId(0)
{}

L1CaloRxLayers::L1CaloRxLayers(unsigned int channelId,
                               const std::vector<int>& names,
			       const std::vector<int>& ncells) :
  m_channelId(channelId),
  m_names(names),
  m_ncells(ncells)
{}
  
L1CaloRxLayers::L1CaloRxLayers(const L1CaloRxCoolChannelId& channelId,
                               const std::vector<int>& names,
			       const std::vector<int>& ncells) :
  m_channelId(channelId),
  m_names(names),
  m_ncells(ncells)
{}
