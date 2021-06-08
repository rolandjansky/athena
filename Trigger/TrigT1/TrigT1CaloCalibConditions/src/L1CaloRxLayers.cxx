/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloRxLayers.h"

L1CaloRxLayers::L1CaloRxLayers() :
  m_channelId(0)
{}

L1CaloRxLayers::L1CaloRxLayers(unsigned int channelId,
                               std::vector<int>&& names,
			                   std::vector<int>&& ncells) :
  m_channelId(channelId),
  m_names(std::move(names)),
  m_ncells(std::move(ncells))
{}
  
L1CaloRxLayers::L1CaloRxLayers(const L1CaloRxCoolChannelId& channelId,
                               std::vector<int>&& names,
			                   std::vector<int>&& ncells) :
  m_channelId(channelId),
  m_names(std::move(names)),
  m_ncells(std::move(ncells))
{}
