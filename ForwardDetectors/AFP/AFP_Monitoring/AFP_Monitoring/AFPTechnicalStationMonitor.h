/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_MONITORING_AFPTECHNICALSTATIONMONITOR_H
#define AFP_MONITORING_AFPTECHNICALSTATIONMONITOR_H

#include <vector>
#include <string>


// forward declarations for lightweight histograms
class LWHist1D;
class LWHist2D;

class AFPTechnicalMonitorTool;
class AFPTechnicalLayerMonitor;
class AFP_SiRawData;

/// A class monitoring technical parameters of one station
class AFPTechnicalStationMonitor
{
public:
  /// Creates monitors of 3 layers
  AFPTechnicalStationMonitor(const int stationID);

  /// Deletes all objects in #m_layersMonitors.
  ~AFPTechnicalStationMonitor();

  /// Books general station monitoring histograms and executes AFPTechnicalLayerMonitor::bookHistograms() for all objects in #m_layersMonitors.
  void bookHistograms(AFPTechnicalMonitorTool* toolToStoreHistograms);

  /// Fills general station monitoring histograms and executes AFPTechnicalLayerMonitor::fillHistograms()
  void fillHistograms(const AFP_SiRawData& hit);

  /// Method to call eventEnd in layers. Executes AFPTechnicalLayerMonitor::eventEnd() for each element in #m_layersMonitors.
  void eventEnd();

  /// @brief Process histograms at the end of lumi block.
  ///
  /// @param toolToStoreHistograms pointer to the parent monitoring object (may not be used)
  void endOfLumiBlock(AFPTechnicalMonitorTool* toolToStoreHistograms);

protected:
  /// @brief Creates a new AFPTechnicalLayerMonitor object and adds it to #m_layersMonitors
  ///
  /// @param layerID ID number of the layer which new object will be monitoring
  /// @return pointer to the new object
  AFPTechnicalLayerMonitor* createAndAddLayerMonitor(const int layerID);

  /// @brief Create a string with station number. Used for creating names of distributions.
  ///
  /// @param name text which should be prepended to the standard one e.g. "Station1"
  std::string makeName (const std::string name) const;

  /// @brief Create a string with station number. Used for creating titles of distributions.
  ///
  /// @param title text which should be prepended to the standard one e.g. " in station 1"
  std::string makeTitle (const std::string title) const;

  /// ID number of the first layer
  static const int s_firstLayerIndex;
  /// ID number of the second layer
  static const int s_secondLayerIndex;
  /// ID number of the third layer
  static const int s_thirdLayerIndex;
  
  /// ID number of the station which is being monitored
  const int m_stationID;

  /// Vector of tools monitoring layers in the station
  std::vector<AFPTechnicalLayerMonitor*> m_layersMonitors;

  /// Tool monitoring the first layer in the station
  AFPTechnicalLayerMonitor* m_firstLayer;
  /// Tool monitoring the second layer in the station
  AFPTechnicalLayerMonitor* m_secondLayer;
  /// Tool monitoring the third layer in the station
  AFPTechnicalLayerMonitor* m_thirdLayer;
};

#endif
