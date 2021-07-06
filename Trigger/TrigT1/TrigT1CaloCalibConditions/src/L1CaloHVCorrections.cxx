/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloHVCorrections.h"

L1CaloHVCorrections::L1CaloHVCorrections() :
  m_channelId(0),
  m_rxMean(0)
{}

L1CaloHVCorrections::L1CaloHVCorrections(unsigned int channelId,
					 float rxMean,
                         std::vector<int> &&affectedCells,
				         std::vector<float> &&layerMeans) :
  m_channelId(channelId),
  m_rxMean(rxMean),
  m_affectedCells(std::move(affectedCells)),
  m_layerMeans(std::move(layerMeans))
{}
  
L1CaloHVCorrections::L1CaloHVCorrections(const L1CaloRxCoolChannelId& channelId,
					 float rxMean,
                     std::vector<int> &&affectedCells,
				     std::vector<float> &&layerMeans) :
  m_channelId(channelId),
  m_rxMean(rxMean),
  m_affectedCells(std::move(affectedCells)),
  m_layerMeans(std::move(layerMeans))
{}
