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

class AFPHitsMonitorTool;

class AFPSiLayerMonitor : virtual public IAFPSiLayerMonitor, public AthAlgTool
{
public:
  AFPSiLayerMonitor(const std::string& type,
		    const std::string& name,
		    const IInterface* parent);
  ~AFPSiLayerMonitor();

  StatusCode initialize() override;
  StatusCode finalize() override;
  
  void setParentMonitor (AFPHitsMonitorTool* parent) override {m_parentMonitor = parent;}

  //  void bookHistogramsRecurrent(ManagedMonitorToolBase* toolToStoreHistograms);
  void bookHistograms(ManagedMonitorToolBase* toolToStoreHistograms, std::string histsDirName = "") override;
  void fillHistograms(const xAOD::AFPSiHit& hit) override;
  void eventEnd() override;		///< method that should be called when event processing is finished

  void endOfLumiBlock() override; ///< Process histograms at the end of lumiblock

  int layerID () const override {return m_pixelLayerID;}

  std::string makeHistName (const std::string name) const override; ///< create a name suffixed with station and layer numbers
  std::string makeHistTitle (const std::string title) const override; ///< create a title suffixed with station and layer numbers

  const std::string& histsDirName () const override {return m_histsDirName;}

  const std::string& layerFullName() const override {return m_layerFullName;}

  double correctHitsForPileUp (double hits) const {return hits - (m_hitsScaleFactor * m_parentMonitor->pileUp());}

  double hitsInEvent () const override {return m_hitsInEvent;}
  double hitsInEventScaled () const override {return correctHitsForPileUp(m_hitsInEvent);}

  double hitsInEventHotSpot () const override {return m_hitsInEventHotSpot;}
  double hitsInEventHotSpotScaled () const override {return correctHitsForPileUp(m_hitsInEventHotSpot);}

protected:
  // internal variables
  int m_pixelLayerID;
  int m_stationID;
  std::string m_layerFullName;

  AFPHitsMonitorTool* m_parentMonitor;
  std::string m_histsDirName;

  double m_hitsScaleFactor;
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
