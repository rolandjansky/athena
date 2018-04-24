/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_MONITORING_AFPSISTATIONMONITOR_H
#define AFP_MONITORING_AFPSISTATIONMONITOR_H

#include "AFP_Monitoring/IAFPSiStationMonitor.h"
#include "AFP_Monitoring/IAFPSiLayerMonitor.h"

#include <xAODForward/AFPSiHit.h>

#include "AthenaBaseComps/AthAlgTool.h"

#include <string>

// forward declarations
class AFPHitsMonitorTool;
class AFPSiLayerMonitor;

/// @brief Class representing an object monitoring one AFP station with silicon detectors
///
/// Python options:
/// * `stationID` - ID number of station in which is the monitored layer (default: -1)
/// * `layersMonitors` - array of tools monitoring layers
class AFPSiStationMonitor : virtual public IAFPSiStationMonitor, public AthAlgTool
{
public:
  /// Declares python properties.
  AFPSiStationMonitor(const std::string& type,
		      const std::string& name,
		      const IInterface* parent);

  /// Does nothing.
  ~AFPSiStationMonitor();

  /// Retrieves all tools in array #m_layersMonitors.
  StatusCode initialize() override;

  /// Does nothing
  StatusCode finalize() override;

  /// @copydoc IAFPSiStationMonitor::bookHistograms()
  ///
  /// Books histograms specific to monitoring whole station but also
  /// executes AFPSiLayerMonitor::bookHistograms() for each object in
  /// #m_layersMonitors.
  void bookHistograms(AFPHitsMonitorTool* toolToStoreHistograms) override;

  /// @copydoc IAFPSiStationMonitor::fillHistograms()
  ///
  /// Fills histograms specific to monitoring the whole station and
  /// calls AFPSiLayerMonitor::fillHistograms() for all objects in
  /// #m_layersMonitors.
  void fillHistograms(const xAOD::AFPSiHit& hit) override;

  /// @copydoc IAFPSiStationMonitor::eventEnd()
  ///
  /// Calls IAFPSiLayerMonitor::eventEnd() for all monitors in #m_layersMonitors.
  void eventEnd() override;

  /// @copydoc IAFPSiStationMonitor::endOfLumiBlock()
  void endOfLumiBlock(AFPHitsMonitorTool* toolToStoreHistograms) override;

  /// @copydoc AFPSiStationMonitor::m_stationID
  int stationID () const override {return m_stationID;}

  /// @copydoc AFPSiStationMonitor::m_layersMonitors
  const ToolHandleArray<IAFPSiLayerMonitor>& layersMonitors () const override {return m_layersMonitors;}

  /// @copydoc IAFPSiStationMonitor::makeName()
  ///
  /// Returns a string with station number and provided text.
  ///
  /// @param name string to be added in front of the standard text e.g. "Station1"
  std::string makeName (const std::string name) const override;

  /// @copydoc IAFPSiStationMonitor::makeTitle()
  ///
  /// Returns a string with station number and provided text.
  ///
  /// @param title string to be added in front of the standard text e.g. " in station 1"
  std::string makeTitle (const std::string title) const override;

  /// @copydoc IAFPSiStationMonitor::setAllLayersParent()
  ///
  /// Executes IAFPSilayerMonitor::setParentMonitor() on all tools in #m_layersMonitors.
  void setAllLayersParent (AFPHitsMonitorTool* parent) override;

protected:
  /// ID number of the monitored station
  int m_stationID;

  /// Array of tools monitoring planes in the station
  ToolHandleArray<IAFPSiLayerMonitor> m_layersMonitors;
};

#endif
