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

protected:
  AFPSiLayerMonitor* createAndAddLayerMonitor(const int layerID);
  std::string makeName (const std::string name) const;
  std::string makeTitle (const std::string title) const;
  void fill1DHitCorrelations (AFPSiLayerMonitor* firstLayer, AFPSiLayerMonitor* secondLayer, LWHist1D* histogramRow, LWHist1D* histogramCol);
  void fill2DHitCorrelations (AFPSiLayerMonitor* firstLayer, AFPSiLayerMonitor* secondLayer, LWHist2D* histogramRow, LWHist2D* histogramCol);

  static const int s_firstLayerIndex;
  static const int s_secondLayerIndex;
  static const int s_thirdLayerIndex;
  
  // internal variables
  const int m_stationID;

  // layers
  std::vector<AFPSiLayerMonitor*> m_layersMonitors;
  AFPSiLayerMonitor* m_firstLayer;
  AFPSiLayerMonitor* m_secondLayer;
  AFPSiLayerMonitor* m_thirdLayer;

  // histograms
  LWHist2D* m_correlationRow12;
  LWHist2D* m_correlationRow13;
  LWHist2D* m_correlationRow23;
  LWHist2D* m_correlationCol12;
  LWHist2D* m_correlationCol13;
  LWHist2D* m_correlationCol23;

  LWHist1D* m_corr1DRow12;
  LWHist1D* m_corr1DRow13;
  LWHist1D* m_corr1DRow23;

  LWHist1D* m_corr1DCol12;
  LWHist1D* m_corr1DCol13;
  LWHist1D* m_corr1DCol23;

};

#endif
