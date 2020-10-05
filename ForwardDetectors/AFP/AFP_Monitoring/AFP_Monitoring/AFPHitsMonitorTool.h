/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFPHITSMONITORTOOL_H
#define AFPHITSMONITORTOOL_H

#include "AFP_Monitoring/IAFPSiStationMonitor.h"

#include "LumiBlockComps/ILumiBlockMuTool.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "xAODForward/AFPSiHitContainer.h"
#include "StoreGate/ReadHandleKey.h"

#include <vector>
#include <string>

class AFPSiLayerSummaryManager;

/// @brief Main monitoring class for silicon detectors
///
/// This class manages monitors for silicon detectors in all stations
/// and layers. It is responsible for storing objects creating summary
/// distributions i.e. distributions with mean values for each station
/// and layer. It also provides information about pile-up for layers
/// monitors.
///
/// Python properties:
/// * `stationsMonitors` - array of tools for monitoring stations
/// * `luminosityTool` - luminosity tool for reading pile-up information.
class AFPHitsMonitorTool : public ManagedMonitorToolBase
{
public:
  /// @brief Constructor declaring python properties and creating subobjects
  ///
  /// * Creates new AFPSiLayerSummaryManager object and assigns it to #m_summaryManager.
  /// * Creates default tool for luminosity "LumiBlockMuTool/LumiBlockMuTool".
  /// * Declares python properties.
  AFPHitsMonitorTool( const std::string & type, const std::string & name,
		      const IInterface* parent ); 

  /// Deletes #m_summaryManager
  virtual ~AFPHitsMonitorTool();

  /// @brief Prepares and configures the tool.
  ///
  /// * Retrieves all tools and sets parent in all tools to "this".
  /// * Creates and adds to the #m_summaryManager summary distributions of
  ///   * `h_hitsPerPlaneScaledMean` - mean number of hits in an event per plane.
  ///   * `h_hitsPerPlaneScaledHotSpotMean` - mean number of hits in a hotspot in an event per plane.
  ///   * `h_timeOverThresholdMean` - mean hitstime over threshold per plane.
  StatusCode initialize() override;
  
  /// Does nothing.
  StatusCode bookHistogramsRecurrent() override;

  /// Books all histograms with proper configuration.
  ///
  /// Executes histograms booking on each station in the
  /// #m_stationsMonitors list and #m_summaryManager.
  StatusCode bookHistograms() override;

  /// Fills all histograms.
  ///
  /// Runs histograms filling for each hit (
  /// AFPSiStationMonitor::fillHistograms() ) and after processing all
  /// hits ( AFPSiStationMonitor::eventEnd() ).
  StatusCode fillHistograms() override;

  /// Calls AFPSiStationMonitor::endOfLumiblock();
  StatusCode procHistograms() override;

  ///@brief name of directory to store histograms
  std::string histsDirectoryName() {return m_histsDirectoryName;}

  /// Array of tool handles for monitoring each station #m_stationsMonitors
  const ToolHandleArray<IAFPSiStationMonitor>* stationsMonitors () const {return &m_stationsMonitors;}

  /// Manager of summary distributions #m_summaryManager
  AFPSiLayerSummaryManager* summaryManager () const {return m_summaryManager;}
  
  /// Average number of interactions per crossing in a given event
  double pileUp () const {return m_lumiBlockMuTool->averageInteractionsPerCrossing();}

protected:
  /// name of directory for all histograms
  const std::string m_histsDirectoryName;

  /// Array of tools for monitoring one station (configurable in python)
  ToolHandleArray<IAFPSiStationMonitor> m_stationsMonitors;

  /// @brief Object managing summary histograms.
  ///
  /// The summary histograms are of type TProfile and contain on Y
  /// axis mean value of a distribution and on X axis station and
  /// layer number. The manager contains objects responsible for
  /// booking appropriate TProfiles and filling them.
  AFPSiLayerSummaryManager* m_summaryManager;

  /// Tool handle to tool providing information about luminosity e.g. pile-up
  ToolHandle<ILumiBlockMuTool>  m_lumiBlockMuTool;
  
  /// Use only if you are sure you know what you are doing
  ///
  /// @note Make sure that #m_summaryManager is not a better choice for you instead of this method.
  /// 
  /// @warning The method cannot be used online, because it can be run
  /// only when the distributions are already filled e.g. at the end
  /// of luminosity block or run.
  ///
  /// A method creating summary distributions from filled
  /// histograms. It can be used in AFPHitsMonitorTool::procHistograms
  /// e.g. at the end of luminosity block. It loops over all declared
  /// stations and layers monitors and creates a TProfile with bins
  /// for each layer. On Y axis mean value of o given histogram is
  /// inserted.
  void makeLayerSummaryHist (const std::string inputHistName, const std::string outputHistName, const std::string outputHistTitle);

private:
  SG::ReadHandleKey<xAOD::AFPSiHitContainer> m_afpHitContainerKey
  { this ,"AFPHitContianerKey", "AFPSiHitContainer" };
};

#endif
