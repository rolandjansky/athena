/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_MONITORING_AFPSISTATIONMONITOR_H
#define AFP_MONITORING_AFPSISTATIONMONITOR_H

#include "AFP_Monitoring/IAFPSiStationMonitor.h"
#include "AFP_Monitoring/IAFPSiLayerMonitor.h"

#include <xAODForward/AFPSiHit.h>

#include "AthenaBaseComps/AthAlgTool.h"

#include <string>

// forward declarations
class AFPHitsMonitorTool;
class AFPSiLayerMonitor;

class AFPSiStationMonitor : virtual public IAFPSiStationMonitor, public AthAlgTool
{
public:
  AFPSiStationMonitor(const std::string& type,
		      const std::string& name,
		      const IInterface* parent);
  ~AFPSiStationMonitor();

  StatusCode initialize() override;
  StatusCode finalize() override;

  //  void bookHistogramsRecurrent(ManagedMonitorToolBase* toolToStoreHistograms);
  void bookHistograms(AFPHitsMonitorTool* toolToStoreHistograms) override;
  void fillHistograms(const xAOD::AFPSiHit& hit) override;
  void eventEnd() override;		///< method to call eventEnd in layers

  void endOfLumiBlock(AFPHitsMonitorTool* toolToStoreHistograms) override; ///< process histograms at the end of lumi block

  int stationID () const override {return m_stationID;}
  const ToolHandleArray<IAFPSiLayerMonitor>& layersMonitors () const override {return m_layersMonitors;}

  std::string makeName (const std::string name) const override;
  std::string makeTitle (const std::string title) const override;

protected:
  // internal variables
  int m_stationID;

  // layers
  ToolHandleArray<IAFPSiLayerMonitor> m_layersMonitors;
};

#endif
