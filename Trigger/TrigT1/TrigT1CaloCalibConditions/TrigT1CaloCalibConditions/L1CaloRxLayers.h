/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1CALORXLAYERS_H
#define L1CALORXLAYERS_H

#include <vector>

#include "TrigT1CaloCalibConditions/L1CaloRxCoolChannelId.h"

/**
 * Class that holds information about calo layers that make up receivers.
 *
 * @author Peter Faulkner
 */

class L1CaloRxLayers
{
 public:

    L1CaloRxLayers();
    L1CaloRxLayers(unsigned int channelId,
                      const std::vector<int>& names,
	     	      const std::vector<int>& ncells);
    L1CaloRxLayers(const L1CaloRxCoolChannelId& channelId,
                      const std::vector<int>& names,
		      const std::vector<int>& ncells);
    
    virtual ~L1CaloRxLayers() {}

    const L1CaloRxCoolChannelId& channelId() const { return m_channelId; }
    const std::vector<int>& names()          const { return m_names; }
    const std::vector<int>& ncells()         const { return m_ncells; }

 private:
    L1CaloRxCoolChannelId m_channelId;

    std::vector<int> m_names;
    std::vector<int> m_ncells;
};

#endif // L1CALORXLAYERS_H
