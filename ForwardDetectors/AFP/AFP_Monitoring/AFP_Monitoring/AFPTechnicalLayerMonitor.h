/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_MONITORING_AFPTECHNICALLAYERMONITOR_H
#define AFP_MONITORING_AFPTECHNICALLAYERMONITOR_H

#include <AFP_RawEv/AFP_SiRawData.h>

#include <list>

class LWHist1D;
class LWHist2D;
class ManagedMonitorToolBase;

/// Class used for monitoring technical parameters of one silicon layer
class AFPTechnicalLayerMonitor
{
public:
  /// @param pixelLayerID ID number of the layer to be monitored
  /// @param stationID ID number of the station in which monitored layer is installed
  AFPTechnicalLayerMonitor(const int pixelLayerID, const int stationID);

  /// Does nothing.
  ~AFPTechnicalLayerMonitor();

  /// @brief Books histograms used for monitoring
  ///
  /// @param toolToStoreHistograms pointer to the main monitoring object in which distributions should be bookd
  /// @param histsDirName additional string used to form path in ROOT file where distributions should be saved
  void bookHistograms(ManagedMonitorToolBase* toolToStoreHistograms, std::string histsDirName = "");

  /// Fills distributions with information provided by the hit.
  void fillHistograms(const AFP_SiRawData& hit);

  /// Method that should be called when event processing is finished.
  void eventEnd();

  /// @brief Process histograms at the end of lumiblock (may not be used)
  ///
  /// @param toolToStoreHistograms pointer to the main monitoring tool
  void endOfLumiBlock(ManagedMonitorToolBase* toolToStoreHistograms);

protected:
  /// Creates a name suffixed with station and layer numbers.
  std::string makeHistName (const std::string name) const;

  /// Creates a title suffixed with station and layer numbers.
  std::string makeHistTitle (const std::string title) const;

  /// ID number of the monitored pixel layer.
  const int m_pixelLayerID;

  /// ID number of the station in which the monitored layer is installed.
  const int m_stationID;
};

#endif
