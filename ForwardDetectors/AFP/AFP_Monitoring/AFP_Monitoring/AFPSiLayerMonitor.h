/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_MONITORING_AFPSILAYERMONITOR_H
#define AFP_MONITORING_AFPSILAYERMONITOR_H

#include <list>

#include <xAODForward/AFPSiHit.h>

// forward declarations for lightweight histograms
class LWHist1D;
class LWHist2D;

class ManagedMonitorToolBase;

class AFPSiLayerMonitor
{
public:
  AFPSiLayerMonitor(const int pixelLayerID, const int stationID);
  ~AFPSiLayerMonitor();

  //  void bookHistogramsRecurrent(ManagedMonitorToolBase* toolToStoreHistograms);
  void bookHistograms(ManagedMonitorToolBase* toolToStoreHistograms, std::string histsDirName = "");
  void fillHistograms(const xAOD::AFPSiHit& hit);
  void eventEnd();		///< method that should be called when event processing is finished

  void endOfLumiBlock(ManagedMonitorToolBase* toolToStoreHistograms); ///< Process histograms at the end of lumiblock

  /// number of hits counted so far in the event
  unsigned int hitsInEvent() const {return m_hitsInEvent;}
  const std::list<int> rowIDs() const {return m_rowIDs;}
  const std::list<int> colIDs() const {return m_colIDs;}

protected:
  std::string makeHistName (const std::string name) const; ///< create a name suffixed with station and layer numbers
  std::string makeHistTitle (const std::string title) const; ///< create a title suffixed with station and layer numbers

  // internal variables
  const int m_pixelLayerID;
  const int m_stationID;

  unsigned int m_hitsInEvent;
  std::list<int> m_rowIDs;
  std::list<int> m_colIDs;

  // histograms
  LWHist2D* m_hitMap;
  LWHist1D* m_hitMultiplicity;
  LWHist1D* m_timeOverThreshold;

  
};

#endif
