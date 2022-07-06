/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TileCalibMonitorAlgorithm.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileInfo.h"

StatusCode TileCalibMonitorAlgorithm::initialize() {

  ATH_CHECK( AthMonitorAlgorithm::initialize() );

  ATH_CHECK( m_dqStatusKey.initialize() );
  ATH_CHECK( detStore()->retrieve(m_tileHWID) );
  ATH_CHECK( detStore()->retrieve(m_tileInfo, m_tileInfoName) );

  ATH_CHECK( m_cablingSvc.retrieve() );
  m_cabling = m_cablingSvc->cablingService();
  int runPeriod = m_cabling->runPeriod();

  if (runPeriod == 3) {
    std::vector<int> v = { 0x10d }; // LBA14 is demonstrator in RUN3

    if ( m_fragIDsToIgnoreDMUerrors.size() == 0) {
      m_fragIDsToIgnoreDMUerrors = v;
    }

    if ( m_fragIDsDemonstrators.size() == 0) {
      m_fragIDsDemonstrators = v;
    }
  }

  if ( m_fragIDsToIgnoreDMUerrors.size() != 0) {

    std::sort(m_fragIDsToIgnoreDMUerrors.begin(), m_fragIDsToIgnoreDMUerrors.end());

    std::ostringstream os;
    for (int fragID : m_fragIDsToIgnoreDMUerrors) {
      os << " 0x" << std::hex << fragID << std::dec;
    }

    ATH_MSG_INFO("Tile DMU errors will be ignored in drawers (frag IDs):" << os.str());
  }

  if ( m_fragIDsDemonstrators.size() != 0) {

    std::sort(m_fragIDsDemonstrators.begin(), m_fragIDsDemonstrators.end());

    std::ostringstream os;
    for (int fragID : m_fragIDsDemonstrators) {
      os << " 0x" << std::hex << fragID << std::dec;
    }

    ATH_MSG_INFO("Special settings in histograms for demonstrator modules (frag IDs):" << os.str());
  }

  return StatusCode::SUCCESS;
}
