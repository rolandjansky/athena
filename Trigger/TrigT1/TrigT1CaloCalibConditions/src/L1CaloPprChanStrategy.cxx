/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigT1CaloCalibConditions/L1CaloPprChanStrategy.h"

#include <iostream>
#include <iomanip>
#include <string>

L1CaloPprChanStrategy::L1CaloPprChanStrategy(unsigned int channelId,
                                             const std::string& strategy,
                                             unsigned int code,
                                             const std::string& timingRegime,
                                             const std::string& description)
    : m_channelId(channelId),
      m_strategy(strategy),
      m_code(code),
      m_timingRegime(timingRegime),
      m_description(description) {}

std::ostream& operator<<(std::ostream& output, const L1CaloPprChanStrategy& r) {
  output << "channelID: " << std::hex << r.m_channelId << std::dec
         << ", strategy: " << r.m_strategy << ", code: " << r.m_code
         << ", m_timingRegime: " << r.m_timingRegime
         << ", description: " << r.m_description;

  return output;
}
