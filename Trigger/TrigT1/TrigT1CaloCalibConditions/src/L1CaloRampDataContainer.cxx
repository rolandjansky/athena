/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloRampDataContainer.h"

const L1CaloRampData* L1CaloRampDataContainer::rampData(unsigned int channelId) const
{
    const_iterator pos(m_rampDataMap.find(channelId));
    if(pos == m_rampDataMap.end()) return 0;
    return &(pos->second);
}

L1CaloRampData* L1CaloRampDataContainer::rampData(unsigned int channelId)
{
    iterator pos(m_rampDataMap.find(channelId));
    if(pos == m_rampDataMap.end()) return 0;
    return &(pos->second);
}

const L1CaloRampData* L1CaloRampDataContainer::rampData(const L1CaloCoolChannelId& channelId) const
{
    return this->rampData(channelId.id());
}

L1CaloRampData* L1CaloRampDataContainer::rampData(const L1CaloCoolChannelId& channelId)
{
    return this->rampData(channelId.id());
}


void L1CaloRampDataContainer::addRampData(unsigned int channelId, const L1CaloRampData& rampData)
{
    m_rampDataMap[channelId] = rampData;
}

void L1CaloRampDataContainer::addRampData(const L1CaloCoolChannelId& channelId, const L1CaloRampData& rampData)
{
    this->addRampData(channelId.id(), rampData);
}
