/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFPHITSMONITORTOOL_H
#define AFPHITSMONITORTOOL_H

#include "AFPSiStationMonitor.h"

#include <AthenaMonitoring/ManagedMonitorToolBase.h>

#include <vector>
#include <string>

#include "AFP_Monitoring/IAFPSiStationMonitor.h"

class AFPHitsMonitorTool : public ManagedMonitorToolBase
{
public:

  AFPHitsMonitorTool( const std::string & type, const std::string & name,
		      const IInterface* parent ); 

  virtual ~AFPHitsMonitorTool();

  StatusCode initialize() override;
  
  StatusCode bookHistogramsRecurrent() override;
  StatusCode bookHistograms() override;
  StatusCode fillHistograms() override;
  StatusCode procHistograms() override;

  ///@brief name of directory to store histograms
  std::string histsDirectoryName() {return m_histsDirectoryName;}

protected:
  const std::string m_histsDirectoryName; ///< name of directory for all histograms

  
  // AFPSiStationMonitor m_cNearStation;		///< station on C side that is closer to the interaction point
  // AFPSiStationMonitor m_cFarStation;		///< station on C side that is further away from the interaction point
  // AFPSiStationMonitor m_aNearStation;		///< station on A side that is closer to the interaction point
  // AFPSiStationMonitor m_aFarStation;		///< station on A side that is further away from the interaction point

  ToolHandleArray<IAFPSiStationMonitor> m_stationsMonitors;
  
  void makeLayerSummaryHist (const std::string inputHistName, const std::string outputHistName, const std::string outputHistTitle);
};

#endif
