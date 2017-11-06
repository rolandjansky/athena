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

class AFPTechnicalStationMonitor
{
public:

  AFPTechnicalStationMonitor(const int stationID);
  ~AFPTechnicalStationMonitor();

  //  void bookHistogramsRecurrent(ManagedMonitorToolBase* toolToStoreHistograms);
  void bookHistograms(AFPTechnicalMonitorTool* toolToStoreHistograms);
  void fillHistograms(const AFP_SiRawData& hit);
  void eventEnd();		///< method to call eventEnd in layers

  void endOfLumiBlock(AFPTechnicalMonitorTool* toolToStoreHistograms); ///< process histograms at the end of lumi block

protected:
  AFPTechnicalLayerMonitor* createAndAddLayerMonitor(const int layerID);
  std::string makeName (const std::string name) const;
  std::string makeTitle (const std::string title) const;

  static const int s_firstLayerIndex;
  static const int s_secondLayerIndex;
  static const int s_thirdLayerIndex;
  
  // internal variables
  const int m_stationID;

  // layers
  std::vector<AFPTechnicalLayerMonitor*> m_layersMonitors;
  AFPTechnicalLayerMonitor* m_firstLayer;
  AFPTechnicalLayerMonitor* m_secondLayer;
  AFPTechnicalLayerMonitor* m_thirdLayer;
};

#endif
