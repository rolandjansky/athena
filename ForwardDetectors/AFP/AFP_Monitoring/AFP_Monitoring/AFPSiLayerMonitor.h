/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_MONITORING_AFPSILAYERMONITOR_H
#define AFP_MONITORING_AFPSILAYERMONITOR_H

#include <xAODForward/AFPSiHit.h>

// forward declarations for lightweight histograms
class LWHist2D;
class TH1;

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

  int layerID () const {return m_pixelLayerID;}

  std::string makeHistName (const std::string name) const; ///< create a name suffixed with station and layer numbers
  std::string makeHistTitle (const std::string title) const; ///< create a title suffixed with station and layer numbers

  const std::string& histsDirName () const {return m_histsDirName;}

protected:
  // internal variables
  const int m_pixelLayerID;
  const int m_stationID;

  std::string m_histsDirName;
  
  unsigned int m_hitsInEvent;

  // histograms
  LWHist2D* m_hitMap;

  // must be TH1 because overflows need to be included in mean
  TH1* m_hitMultiplicity;
  TH1* m_timeOverThreshold;

  int m_hotSpotStartRow;
  int m_hotSpotEndRow;
  int m_hotSpotStartCol;
  int m_hotSpotEndCol;
  unsigned int m_hitsInEventHotSpot;
  //  must be TH1 because overflows need to be included in mean
  TH1* m_hitMultiplicityHotSpot; 
};

#endif
