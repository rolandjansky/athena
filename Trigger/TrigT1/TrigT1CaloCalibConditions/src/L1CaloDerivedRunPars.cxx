/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigT1CaloCalibConditions/L1CaloDerivedRunPars.h"

#include <iomanip>

L1CaloDerivedRunPars::L1CaloDerivedRunPars(unsigned int channelId, const std::string& timingRegime, const std::string& tierZeroTag)
 : m_channelId(channelId)
 , m_timingRegime(timingRegime)
 , m_tierZeroTag(tierZeroTag)
{
}

std::ostream& operator<<(std::ostream& output, const L1CaloDerivedRunPars& r) {
  output << "channelID: " << std::hex << r.channelId() << std::dec
         << ", timingRegime: " << r.timingRegime() 
         << ", tierZeroTag: " << r.tierZeroTag();

  return output;
}
