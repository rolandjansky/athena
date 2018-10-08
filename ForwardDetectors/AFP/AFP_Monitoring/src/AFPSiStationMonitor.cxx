/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_Monitoring/AFPHitsMonitorTool.h"
#include "AFP_Monitoring/IAFPSiLayerMonitor.h"
#include "AFP_Monitoring/AFPSiStationMonitor.h"

#include <sstream>

AFPSiStationMonitor::AFPSiStationMonitor(const std::string& type,
					 const std::string& name,
					 const IInterface* parent) : 
  AthAlgTool  (type, name, parent)
{
  declareInterface<IAFPSiStationMonitor>(this);

  declareProperty("stationID", m_stationID = -1, "ID number of station in which is the monitored layer.");
  declareProperty("layersMonitors", m_layersMonitors, "Array of tools monitoring layers.");
}

StatusCode AFPSiStationMonitor::initialize()
{
  if (m_layersMonitors.size() != 0) {
    // loop over tools
    for (ToolHandle<IAFPSiLayerMonitor>& layerMon : m_layersMonitors) {
      // retrieve tools
      if (layerMon.retrieve().isFailure())
	ATH_MSG_WARNING("Failed to retrieve layerMon " << layerMon);
    }
  }
  else
    ATH_MSG_WARNING("No layers monitors in station "<<m_stationID);

  return StatusCode::SUCCESS;
}

StatusCode AFPSiStationMonitor::finalize()
{
  return StatusCode::SUCCESS;
}

AFPSiStationMonitor::~AFPSiStationMonitor()
{
}

// Description: Used for re-booking managed histograms       
void AFPSiStationMonitor::bookHistograms(AFPHitsMonitorTool* toolToStoreHistograms)
{
  // if( toolToStoreHistograms->m_environment == AthenaMonManager::online ) {
  //    // book histograms that are only made in the online environment...
  // }

  // if( toolToStoreHistograms->m_dataType == AthenaMonManager::cosmics ) {
  //    // book histograms that are only relevant for cosmics data...
  // }
 
  std::string histsDirName = toolToStoreHistograms->histsDirectoryName();
  histsDirName += makeName("");

  // register histograms in layers
  for (ToolHandle<IAFPSiLayerMonitor>& layerMonitor : m_layersMonitors)
    layerMonitor->bookHistograms(toolToStoreHistograms, histsDirName);
}

void AFPSiStationMonitor::fillHistograms(const xAOD::AFPSiHit& hit)
{
  const int hitLayerID = hit.pixelLayerID();
  for (ToolHandle<IAFPSiLayerMonitor> layerMonitor : m_layersMonitors)
    if (layerMonitor->layerID() == hitLayerID) {
      layerMonitor->fillHistograms(hit);
      break;
    }
}

void AFPSiStationMonitor::eventEnd()
{
  // must be at the end because it zeroes hitsInEvent
  for (ToolHandle<IAFPSiLayerMonitor> layerMonitor : m_layersMonitors)
    layerMonitor->eventEnd();
}


void AFPSiStationMonitor::endOfLumiBlock(AFPHitsMonitorTool* /*toolToStoreHistograms*/)
{
  // Calls endOfLumiBlock on each layer.
  for (ToolHandle<IAFPSiLayerMonitor> layer : m_layersMonitors)
    layer->endOfLumiBlock();
}

std::string AFPSiStationMonitor::makeName (const std::string name) const
{
  std::stringstream histName;
  histName <<name<<"Station"<<m_stationID;

  return histName.str();
}

std::string AFPSiStationMonitor::makeTitle (const std::string title) const
{
  std::stringstream histTitle;
  histTitle <<title<<" in station "<<m_stationID;

  return histTitle.str();
}

void AFPSiStationMonitor::setAllLayersParent (AFPHitsMonitorTool* parent)
{
  for (ToolHandle<IAFPSiLayerMonitor> layer : m_layersMonitors)
    layer->setParentMonitor(parent);
}
