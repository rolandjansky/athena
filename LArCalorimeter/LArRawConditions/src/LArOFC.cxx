/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArOFC.h"
#include "LArIdentifier/LArOnlineID_Base.h"

#include <cassert>

LArOFC::LArOFC(const LArOnlineID_Base* onlineID, const LArOnOffIdMapping* cabling, const size_t nGains) :
  m_onlineID(onlineID),
  m_cabling(cabling) {

  assert(m_onlineID); 
  assert(nGains<=CaloGain::LARNGAIN && nGains>0);

  for (size_t i=0;i<nGains;++i) {
    m_OFCa[i].resize(onlineID->channelHashMax());
    m_OFCb[i].resize(onlineID->channelHashMax());
  }

}

LArOFC::~LArOFC() {}

bool LArOFC::setOFC(const IdentifierHash& hid, const int gain, std::pair<std::vector<float>,std::vector<float>> ofcab) {
    if (gain >= CaloGain::LARNGAIN || hid >= m_OFCa[gain].size()) return false;
    m_OFCa[gain][hid].swap(ofcab.first);
    m_OFCb[gain][hid].swap(ofcab.second);
    return true;
}

ILArOFC::OFCRef_t LArOFC::OFC_a(const HWIdentifier &hwid, int gain, int) const {
    const IdentifierHash hid = m_onlineID->channel_Hash(hwid);
    return m_OFCa[gain][hid];
}

ILArOFC::OFCRef_t LArOFC::OFC_a(const Identifier &id, int gain, int) const {
    const HWIdentifier hwid = m_cabling->createSignalChannelID(id);
    return this->OFC_a(hwid, gain);
}

ILArOFC::OFCRef_t LArOFC::OFC_b(const HWIdentifier &hwid, int gain, int) const {
    const IdentifierHash hid = m_onlineID->channel_Hash(hwid);
    return m_OFCb[gain][hid];
}

ILArOFC::OFCRef_t LArOFC::OFC_b(const Identifier &id, int gain, int) const {
    const HWIdentifier hwid = m_cabling->createSignalChannelID(id);
    return this->OFC_b(hwid, gain);
}

float LArOFC::timeOffset(const HWIdentifier&, int) const {
    return 0;
}
float LArOFC::timeOffset(const Identifier&, int) const {
    return 0;
}
unsigned LArOFC::nTimeBins(const HWIdentifier&, int) const {
    return 1;
}
unsigned LArOFC::nTimeBins(const Identifier&, int) const {
    return 1;
}
float LArOFC::timeBinWidth(const HWIdentifier&, int) const {
    return 1;
}
float LArOFC::timeBinWidth(const Identifier&, int) const {
    return 1;
}
