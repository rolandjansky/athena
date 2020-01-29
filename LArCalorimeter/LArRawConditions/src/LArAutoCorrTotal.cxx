/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "LArRawConditions/LArAutoCorrTotal.h"
#include "LArIdentifier/LArOnlineID_Base.h"

#include <cassert>

LArAutoCorrTotal::LArAutoCorrTotal(const LArOnlineID_Base *onlineID, const LArOnOffIdMapping *cabling,
                                   const size_t nGains)
    : m_onlineID(onlineID), m_cabling(cabling) {

  assert(m_onlineID);
  assert(CaloGain::LARNGAIN <= 3 && nGains > 0);

  for (size_t i = 0; i < nGains; ++i) {
    m_AutoCorrTotal[i].resize(onlineID->channelHashMax());
  }
}

LArAutoCorrTotal::~LArAutoCorrTotal() {}

bool LArAutoCorrTotal::set(const IdentifierHash &hid, const int gain,
                           std::vector<float> &autocorrtotal) {
  if (gain >= CaloGain::LARNGAIN || hid >= m_AutoCorrTotal[gain].size()) {
    return false;
  }

  m_AutoCorrTotal[gain][hid].swap(autocorrtotal);
  return true;
}

// *** compute AutoCorrTotal (nsamples-1 coeffs) for a given cell ***
const std::vector<double>
LArAutoCorrTotal::computeAutoCorr(const std::vector<float>& terms,
                                  float Nminbias) const {

  std::vector<double> vResult;
  int tsize = int(sqrt(terms.size()));
  int nsize_tot = (tsize - 1) * (tsize) / 2;
  for (int i1 = 0; i1 < tsize - 1; i1++) {
    for (int i2 = i1 + 1; i2 < tsize; i2++) {

      int index = i1 * tsize - i1 * (i1 + 1) / 2 + i2 - (i1 + 1);

      vResult.push_back((terms[index] + Nminbias * terms[nsize_tot + index]) /
                        sqrt((1. + Nminbias * terms[2 * nsize_tot + i1]) *
                             (1. + Nminbias * terms[2 * nsize_tot + i2])));
    }
  }
  return (vResult);
}

// *** retrieve AutoCorrTotal (nsamples*(nsamples-1)/2 coeffs) for a given cell
// ***
const std::vector<double>
LArAutoCorrTotal::autoCorrTotal(const IdentifierHash &hid, int gain,
                                float Nminbias) const {
    //FIXME we should check if the vector<float> of index "hid" exists before trying to compute it!
    //but we already have resized the vector<vector<float>> to size channelHashMax in the ctor
    
  return (this->computeAutoCorr(m_AutoCorrTotal[gain][hid], Nminbias));
}

const std::vector<double> LArAutoCorrTotal::autoCorrTotal(const HWIdentifier &hwid, int gain, float Nminbias) const {
    const IdentifierHash hid = m_onlineID->channel_Hash(hwid);
    return this->autoCorrTotal(hid, gain, Nminbias);
}

const std::vector<double> LArAutoCorrTotal::autoCorrTotal(const Identifier &offid, int gain, float Nminbias) const {
   const HWIdentifier hwid = m_cabling->createSignalChannelID(offid); 
   return this->autoCorrTotal(hwid, gain, Nminbias);
}

const std::vector<double>
LArAutoCorrTotal::computeRMS(const std::vector<float>& terms,
                             float Nminbias) const {

  std::vector<double> vResult;
  int tsize = int(sqrt(terms.size()));
  vResult.reserve(tsize);
  int nsize_tot = (tsize - 1) * (tsize) / 2;
  for (int i = 0; i < tsize; i++) {
    vResult.push_back(sqrt(1. + Nminbias * terms[2 * nsize_tot + i]));
  }
  return (vResult);
}

const std::vector<double> LArAutoCorrTotal::samplRMS(const IdentifierHash &hid,
                                                     int gain,
                                                     float Nminbias) const {
  return (this->computeRMS(m_AutoCorrTotal[gain][hid], Nminbias));
}

const std::vector<double> LArAutoCorrTotal::samplRMS(const HWIdentifier &hwid,
                                                     int gain,
                                                     float Nminbias) const {
    const IdentifierHash hid = m_onlineID->channel_Hash(hwid);
    return this->samplRMS(hid, gain, Nminbias);
}

const std::vector<double> LArAutoCorrTotal::samplRMS(const Identifier &offid,
                                                     int gain,
                                                     float Nminbias) const {
  const HWIdentifier hwid = m_cabling->createSignalChannelID(offid); 
  return this->samplRMS(hwid, gain, Nminbias);
}
