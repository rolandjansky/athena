/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#include "AFP_Monitoring/AFPSiLayerSummaryProfileBase.h"
#include "AFP_Monitoring/IAFPSiStationMonitor.h"
#include "AFP_Monitoring/IAFPSiLayerMonitor.h"
#include "AFP_Monitoring/AFPHitsMonitorTool.h"

AFPSiLayerSummaryProfileBase::AFPSiLayerSummaryProfileBase (AFPHitsMonitorTool* parentTool, const ManagedMonitorToolBase::MonGroup& monGrp, const std::string name, const std::string title) : m_parentTool(parentTool), m_monGrp (monGrp)
{
  // count number of layers in monitor
  int totalPlanes = 0;
  for (const ToolHandle<IAFPSiStationMonitor>& station : *(m_parentTool->stationsMonitors()) )
    totalPlanes += station->layersMonitors().size();

  // create TProfile object
  m_profile = new TProfile(name.data(),
			   title.data(),
			   totalPlanes, - 0.5, totalPlanes + 0.5);    

  // set names of bins
  TAxis* axis = m_profile->GetXaxis();
  int binCounter = 1;
  for (const ToolHandle<IAFPSiStationMonitor>& station : *(m_parentTool->stationsMonitors()))
    for (const ToolHandle<IAFPSiLayerMonitor>& layer : station->layersMonitors()) {
      axis->SetBinLabel (binCounter, layer->layerFullName().data());
      ++binCounter;
    }  
}


void AFPSiLayerSummaryProfileBase::book ()
{
  m_parentTool->regHist(m_profile, m_monGrp).ignore();
}
