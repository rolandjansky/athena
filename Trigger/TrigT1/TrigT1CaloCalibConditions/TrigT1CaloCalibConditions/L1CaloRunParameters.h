// -*- C++ -*-
#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALORUNPARAMETERS_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALORUNPARAMETERS_H

#include <string>
#include <iostream>

/**
 * Folder <-> Object mapping for /TRIGGER/L1Calo/V1/Conditions/RunParameters .
 * Automatically created using:
 *
 * ./CreateClassesForFolder.py /TRIGGER/L1Calo/V1/Conditions/RunParameters
 */
class L1CaloRunParameters
{
  friend std::ostream& operator<<(std::ostream& output, const L1CaloRunParameters& r);
public:
  L1CaloRunParameters() {}
  L1CaloRunParameters(unsigned int channelId, const std::string& runType, const std::string& runActionName, unsigned int runActionVersion, const std::string& readoutConfig, unsigned int readoutConfigID, const std::string& ttcConfiguration, unsigned int ttcConfigurationID, const std::string& triggerMenu, const std::string& calibration, const std::string& conditions);

  unsigned int channelId() const { return m_channelId; }
  std::string runType() const { return m_runType; }
  std::string runActionName() const { return m_runActionName; }
  unsigned int runActionVersion() const { return m_runActionVersion; }
  std::string readoutConfig() const { return m_readoutConfig; }
  unsigned int readoutConfigID() const { return m_readoutConfigID; }
  std::string ttcConfiguration() const { return m_ttcConfiguration; }
  unsigned int ttcConfigurationID() const { return m_ttcConfigurationID; }
  std::string triggerMenu() const { return m_triggerMenu; }
  std::string calibration() const { return m_calibration; }
  std::string conditions() const { return m_conditions; }

  void setChannelId(unsigned int channelId) { m_channelId = channelId; }
  void setrunType(const std::string& runType) { m_runType = runType; }
  void setrunActionName(const std::string& runActionName) { m_runActionName = runActionName; }
  void setrunActionVersion(unsigned int runActionVersion) { m_runActionVersion = runActionVersion; }
  void setreadoutConfig(const std::string& readoutConfig) { m_readoutConfig = readoutConfig; }
  void setreadoutConfigID(unsigned int readoutConfigID) { m_readoutConfigID = readoutConfigID; }
  void setttcConfiguration(const std::string& ttcConfiguration) { m_ttcConfiguration = ttcConfiguration; }
  void setttcConfigurationID(unsigned int ttcConfigurationID) { m_ttcConfigurationID = ttcConfigurationID; }
  void settriggerMenu(const std::string& triggerMenu) { m_triggerMenu = triggerMenu; }
  void setcalibration(const std::string& calibration) { m_calibration = calibration; }
  void setconditions(const std::string& conditions) { m_conditions = conditions; }

private:
  unsigned int m_channelId = 0;
  std::string m_runType = 0;
  std::string m_runActionName = 0;
  unsigned int m_runActionVersion = 0;
  std::string m_readoutConfig = 0;
  unsigned int m_readoutConfigID = 0;
  std::string m_ttcConfiguration = 0;
  unsigned int m_ttcConfigurationID = 0;
  std::string m_triggerMenu = 0;
  std::string m_calibration = 0;
  std::string m_conditions = 0;
};

#endif // TRIGT1CALOCALIBCONDITIONS_L1CALORUNPARAMETERS_H
