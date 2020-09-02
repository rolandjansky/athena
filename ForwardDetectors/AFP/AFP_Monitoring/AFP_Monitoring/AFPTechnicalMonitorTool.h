/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFPTECHNICALMONITORTOOL_H
#define AFPTECHNICALMONITORTOOL_H

#include "AFP_Monitoring/AFPTechnicalStationMonitor.h"

#include <AthenaMonitoring/ManagedMonitorToolBase.h>

#include <vector>
#include <string>

class LWHist1D;

/// Class monitoring technical AFP parameters
class AFPTechnicalMonitorTool : public ManagedMonitorToolBase
{
public:
  /// Adds two stations monitors on C side to #m_stationsMonitors.  
  AFPTechnicalMonitorTool( const std::string & type, const std::string & name,
		      const IInterface* parent ); 

  /// Does nothing.
  virtual ~AFPTechnicalMonitorTool();

  /// Does nothing.
  virtual StatusCode bookHistogramsRecurrent();

  /// @brief Books histograms for general monitoring and executes
  /// AFPTechnicalStationMonitor::bookHistograms() for each element in
  /// #m_stationsMonitors.
  virtual StatusCode bookHistograms();

  /// @brief Fills general histograms and calls AFPTechnicalStationMonitor::fillHistograms and AFPTechnicalStationMonitor::eventEnd for each object in #m_stationsMonitors.
  virtual StatusCode fillHistograms();

  /// @brief Does nothing
  virtual StatusCode procHistograms();

  /// @brief name of directory to store histograms
  std::string histsDirectoryName() {return m_histsDirectoryName;}

protected:
  /// ID number of the near station on C side
  static const int s_cNearStationIndex;
  /// ID number of the far station on C side
  static const int s_cFarStationIndex;

  /// Name of the Root file path where histograms should be saved
  const std::string m_histsDirectoryName;

  /// Station on C side that is closer to the interaction point.
  AFPTechnicalStationMonitor m_cNearStation;
  /// Station on C side that is further away from the interaction point.
  AFPTechnicalStationMonitor m_cFarStation;

  /// Vector of all stations monitors
  std::vector<AFPTechnicalStationMonitor*> m_stationsMonitors;
  
  // histograms
  LWHist1D* m_bcIDAfp;		///< distribution of bcID sent by AFP
  LWHist1D* m_bcIDATLAS;		///< distribution of bcID sent by ATLAS
  LWHist1D* m_bcIDDiffAfpAtlas;		///< distribution of difference between ATLAS and AFP bcID  
};

#endif
