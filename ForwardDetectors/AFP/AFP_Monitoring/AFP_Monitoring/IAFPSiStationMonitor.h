/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_MONITORING_IAFPSISTATIONMONITOR_H
#define AFP_MONITORING_IAFPSISTATIONMONITOR_H

#include "AFP_Monitoring/IAFPSiLayerMonitor.h"

#include <xAODForward/AFPSiHit.h>

#include "GaudiKernel/IAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include <string>

class AFPHitsMonitorTool;
class AFPSiLayerMonitor;

static const InterfaceID IID_IAFPSiStationMonitor ("IAFPSiStationMonitor", 1, 0);

/// Interface for an object monitoring one AFP station with silicon detectors.
class IAFPSiStationMonitor : virtual public IAlgTool
{
public:
  static const InterfaceID &interfaceID()
  {return IID_IAFPSiStationMonitor;}

  /// Does nothing
  virtual ~IAFPSiStationMonitor() {}

  /// Initialise the tool
  virtual StatusCode initialize() = 0;

  /// Finalise the tool
  virtual StatusCode finalize() = 0;

  /// @brief Books histograms for stations monitoring.
  ///
  /// @param toolToStoreHistograms pointer to the monitoring object in which histograms should be booked
  virtual void bookHistograms(AFPHitsMonitorTool* toolToStoreHistograms) = 0;

  /// @brief Fills stations monitoring histograms.
  ///
  /// @param hit hit used to fill the histograms
  virtual void fillHistograms(const xAOD::AFPSiHit& hit) = 0;

  /// @brief Method called once per event after processing all hits.
  virtual void eventEnd() = 0;

  /// @brief Method called at the end of luminosity block to process histograms.
  ///
  /// @param toolToStoreHistograms main monitoring tool (may not be used)
  virtual void endOfLumiBlock(AFPHitsMonitorTool* toolToStoreHistograms) = 0;

  /// ID number of the monitored station
  virtual int stationID () const = 0;

  /// Array of tools monitoring planes in the station
  virtual const ToolHandleArray<IAFPSiLayerMonitor>& layersMonitors () const = 0;

  /// @brief Return a string useful for creating a name of the distribution.
  virtual std::string makeName (const std::string name) const = 0;

  /// @brief Return a string useful for creating a name of the distribution.
  virtual std::string makeTitle (const std::string title) const = 0;

  /// @brief Sets main monitor tool as a parent in all layers tools.
  virtual void setAllLayersParent (AFPHitsMonitorTool* parent) = 0;
};

#endif
