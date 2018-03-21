/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_MONITORING_AFPSILAYERMONITOR_H
#define AFP_MONITORING_AFPSILAYERMONITOR_H

#include "AFP_Monitoring/IAFPSiLayerMonitor.h"

// FrameWork includes
#include <xAODForward/AFPSiHit.h>
#include "AthenaBaseComps/AthAlgTool.h"

// forward declarations
class LWHist2D;
class TH1;

class ManagedMonitorToolBase;

class AFPSiLayerMonitor : virtual public IAFPSiLayerMonitor, public AthAlgTool
{
public:
  AFPSiLayerMonitor(const std::string& type,
		    const std::string& name,
		    const IInterface* parent);
  ~AFPSiLayerMonitor();

  StatusCode initialize() override;
  StatusCode finalize() override;
  
  //  void bookHistogramsRecurrent(ManagedMonitorToolBase* toolToStoreHistograms);
  void bookHistograms(ManagedMonitorToolBase* toolToStoreHistograms, std::string histsDirName = "") override;
  void fillHistograms(const xAOD::AFPSiHit& hit) override;
  void eventEnd() override;		///< method that should be called when event processing is finished

  void endOfLumiBlock() override; ///< Process histograms at the end of lumiblock

  /// number of hits counted so far in the event
  unsigned int hitsInEvent() const override {return m_hitsInEvent;}

  int layerID () const override {return m_pixelLayerID;}

  std::string makeHistName (const std::string name) const override; ///< create a name suffixed with station and layer numbers
  std::string makeHistTitle (const std::string title) const override; ///< create a title suffixed with station and layer numbers

  const std::string& histsDirName () const override {return m_histsDirName;}

protected:
  // internal variables
  int m_pixelLayerID;
  int m_stationID;

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
