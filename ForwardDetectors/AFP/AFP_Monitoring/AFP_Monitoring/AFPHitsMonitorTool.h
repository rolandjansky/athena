/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFPHITSMONITORTOOL_H
#define AFPHITSMONITORTOOL_H

#include <vector>
#include <string>

#include <AthenaMonitoring/ManagedMonitorToolBase.h>

#include "AFPSiStationMonitor.h"

class AFPHitsMonitorTool : public ManagedMonitorToolBase
{
public:

  AFPHitsMonitorTool( const std::string & type, const std::string & name,
		      const IInterface* parent ); 

  virtual ~AFPHitsMonitorTool();

  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

  ///@brief name of directory to store histograms
  std::string histsDirectoryName() {return m_histsDirectoryName;}

protected:
  static const int s_cNearStationIndex;
  static const int s_cFarStationIndex;
  const std::string m_histsDirectoryName;
  
  AFPSiStationMonitor m_cNearStation;		///< station on C side that is closer to the interaction point
  AFPSiStationMonitor m_cFarStation;		///< station on C side that is further away from the interaction point

  std::vector<AFPSiStationMonitor*> m_stationsMonitors;
  
};

#endif
