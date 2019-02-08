/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigT1CaloCalibConditions/L1CaloRunParameters.h"

L1CaloRunParameters::L1CaloRunParameters(unsigned int channelId, const std::string& runType, const std::string& runActionName, unsigned int runActionVersion, const std::string& readoutConfig, unsigned int readoutConfigID, const std::string& ttcConfiguration, unsigned int ttcConfigurationID, const std::string& triggerMenu, const std::string& calibration, const std::string& conditions)
 : m_channelId(channelId)
 , m_runType(runType)
 , m_runActionName(runActionName)
 , m_runActionVersion(runActionVersion)
 , m_readoutConfig(readoutConfig)
 , m_readoutConfigID(readoutConfigID)
 , m_ttcConfiguration(ttcConfiguration)
 , m_ttcConfigurationID(ttcConfigurationID)
 , m_triggerMenu(triggerMenu)
 , m_calibration(calibration)
 , m_conditions(conditions)
{
}

std::ostream& operator<<(std::ostream& output, const L1CaloRunParameters& r) {
  output << "channelID: " << std::hex << r.channelId() << std::dec
         << ", runType: " << r.runType();

  return output;
}
