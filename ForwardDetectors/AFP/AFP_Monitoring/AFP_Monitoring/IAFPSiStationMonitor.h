/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_MONITORING_IAFPSISTATIONMONITOR_H
#define AFP_MONITORING_IAFPSISTATIONMONITOR_H

#include "AFP_Monitoring/IAFPSiLayerMonitor.h"

#include <xAODForward/AFPSiHit.h>

#include "GaudiKernel/IAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include <string>

// forward declarations for lightweight histograms
class AFPHitsMonitorTool;
class AFPSiLayerMonitor;

static const InterfaceID IID_IAFPSiStationMonitor ("IAFPSiStationMonitor", 1, 0);

class IAFPSiStationMonitor : virtual public IAlgTool
{
public:
  static const InterfaceID &interfaceID()
  {return IID_IAFPSiStationMonitor;}

  virtual ~IAFPSiStationMonitor() {}

  virtual StatusCode initialize() = 0;
  virtual StatusCode finalize() = 0;

  virtual void bookHistograms(AFPHitsMonitorTool* toolToStoreHistograms) = 0;
  virtual void fillHistograms(const xAOD::AFPSiHit& hit) = 0;
  virtual void eventEnd() = 0;		///< method to call eventEnd in layers

  virtual void endOfLumiBlock(AFPHitsMonitorTool* toolToStoreHistograms) = 0; ///< process histograms at the end of lumi block

  virtual int stationID () const = 0;
  virtual const ToolHandleArray<IAFPSiLayerMonitor>& layersMonitors () const = 0;

  virtual std::string makeName (const std::string name) const = 0;
  virtual std::string makeTitle (const std::string title) const = 0;

  virtual void setAllLayersParent (AFPHitsMonitorTool* parent) = 0;
};

#endif
