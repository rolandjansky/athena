/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_MONITORING_AFPSISTATIONMONITOR_H
#define AFP_MONITORING_AFPSISTATIONMONITOR_H

#include <vector>
#include <string>

#include <xAODForward/AFPSiHit.h>

// forward declarations for lightweight histograms
class LWHist1D;
class LWHist2D;

class AFPHitsMonitorTool;
class AFPSiLayerMonitor;

class AFPSiStationMonitor
{
public:

  AFPSiStationMonitor(const int stationID);
  ~AFPSiStationMonitor();

  //  void bookHistogramsRecurrent(ManagedMonitorToolBase* toolToStoreHistograms);
  void bookHistograms(AFPHitsMonitorTool* toolToStoreHistograms);
  void fillHistograms(const xAOD::AFPSiHit& hit);
  void eventEnd();		///< method to call eventEnd in layers

  void endOfLumiBlock(AFPHitsMonitorTool* toolToStoreHistograms); ///< process histograms at the end of lumi block

  int stationID () const {return m_stationID;}
  const std::vector<AFPSiLayerMonitor*>& layersMonitors () const {return m_layersMonitors;}


protected:
  AFPSiLayerMonitor* createAndAddLayerMonitor(const int layerID);
  std::string makeName (const std::string name) const;
  std::string makeTitle (const std::string title) const;

  static const int s_firstLayerIndex;
  static const int s_secondLayerIndex;
  static const int s_thirdLayerIndex;
  static const int s_fourthLayerIndex;
  
  // internal variables
  const int m_stationID;

  // layers
  std::vector<AFPSiLayerMonitor*> m_layersMonitors;
  AFPSiLayerMonitor* m_firstLayer;
  AFPSiLayerMonitor* m_secondLayer;
  AFPSiLayerMonitor* m_thirdLayer;
  AFPSiLayerMonitor* m_fourthLayer;

  // histograms
};

#endif
