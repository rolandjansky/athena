/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1CALOHVCORRECTIONS_H
#define L1CALOHVCORRECTIONS_H

#include <vector>

#include "TrigT1CaloCalibConditions/L1CaloRxCoolChannelId.h"

/**
 * Class that holds mean HV corrections for receivers.
 *
 * @author Peter Faulkner
 */

class L1CaloHVCorrections final
{
 public:

    L1CaloHVCorrections();
    L1CaloHVCorrections(unsigned int channelId,
                        float rxMean,
			std::vector<int> &&affectedCells,
			std::vector<float> &&layerMeans);
    L1CaloHVCorrections(const L1CaloRxCoolChannelId& channelId,
                        float rxMean,
			std::vector<int> &&affectedCells,
			std::vector<float> &&layerMeans);
    
    ~L1CaloHVCorrections() = default;

    const L1CaloRxCoolChannelId& channelId() const { return m_channelId; }
    float rxMean()                           const { return m_rxMean; }
    const std::vector<int>& affectedCells()  const { return m_affectedCells; }
    const std::vector<float>& layerMeans()   const { return m_layerMeans; }

 private:
    L1CaloRxCoolChannelId m_channelId;

    float m_rxMean;
    std::vector<int> m_affectedCells;
    std::vector<float> m_layerMeans;
};

#endif // L1CALOHVCORRECTIONS_H
