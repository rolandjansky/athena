/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TileConditions/TileBadChannels.h"


TileBadChannels::TileBadChannels() {

}


TileBadChannels::~TileBadChannels() {

}

void TileBadChannels::addAdcStatus(const HWIdentifier channel_id, const HWIdentifier adc_id, TileBchStatus adcStatus) {
  m_channelStatus[channel_id] += adcStatus;
  m_adcStatus[adc_id] += adcStatus;
}


const TileBchStatus& TileBadChannels::getAdcStatus(const HWIdentifier adc_id) const {

  std::map<const HWIdentifier, TileBchStatus>::const_iterator adcStatus = m_adcStatus.find(adc_id);
  if (adcStatus == m_adcStatus.end()) {
    return m_defaultStatus;
  } else {
    return adcStatus->second;
  }

}

const TileBchStatus& TileBadChannels::getChannelStatus(const HWIdentifier channel_id) const {

  std::map<const HWIdentifier, TileBchStatus>::const_iterator channelStatus = m_channelStatus.find(channel_id);
  if (channelStatus == m_channelStatus.end()) {
    return m_defaultStatus;
  } else {
    return channelStatus->second;
  }

}
