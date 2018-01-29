/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#include <AsgTools/MsgStreamMacros.h>

#include <sstream>

#include <LWHists/TH1F_LW.h>
#include <LWHists/TH2F_LW.h>

#include "../AFP_Monitoring/AFPHitsMonitorTool.h"
#include "../AFP_Monitoring/AFPSiLayerMonitor.h"
#include "../AFP_Monitoring/AFPSiStationMonitor.h"

const int AFPSiStationMonitor::s_firstLayerIndex = 0;
const int AFPSiStationMonitor::s_secondLayerIndex = 1;
const int AFPSiStationMonitor::s_thirdLayerIndex = 2;
const int AFPSiStationMonitor::s_fourthLayerIndex = 3;


AFPSiStationMonitor::AFPSiStationMonitor(const int stationID)
  : m_stationID (stationID)
  , m_firstLayer (nullptr)
  , m_secondLayer (nullptr)
  , m_thirdLayer (nullptr)
  , m_fourthLayer (nullptr)
{
  m_firstLayer = createAndAddLayerMonitor(s_firstLayerIndex);
  m_secondLayer = createAndAddLayerMonitor(s_secondLayerIndex);
  m_thirdLayer = createAndAddLayerMonitor(s_thirdLayerIndex);
  m_fourthLayer = createAndAddLayerMonitor(s_fourthLayerIndex);
}


AFPSiStationMonitor::~AFPSiStationMonitor()
{
  for (AFPSiLayerMonitor* layerMonitor : m_layersMonitors)
    delete layerMonitor;
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
  for (AFPSiLayerMonitor* layerMonitor : m_layersMonitors)
    layerMonitor->bookHistograms(toolToStoreHistograms, histsDirName);
}



void AFPSiStationMonitor::fillHistograms(const xAOD::AFPSiHit& hit)
{
  const int hitLayerID = hit.pixelLayerID();
  for (AFPSiLayerMonitor* layerMonitor : m_layersMonitors)
    if (layerMonitor->layerID() == hitLayerID) {
      layerMonitor->fillHistograms(hit);
      break;
    }
}

AFPSiLayerMonitor* AFPSiStationMonitor::createAndAddLayerMonitor (const int layerID)
{
  AFPSiLayerMonitor* newMonitor = new AFPSiLayerMonitor (layerID, m_stationID);
  m_layersMonitors.push_back(newMonitor);

  return newMonitor;
}

void AFPSiStationMonitor::eventEnd()
{
  // must be at the end because it zeroes hitsInEvent
  for (AFPSiLayerMonitor* layerMonitor : m_layersMonitors)
    layerMonitor->eventEnd();
}


void AFPSiStationMonitor::endOfLumiBlock(AFPHitsMonitorTool* toolToStoreHistograms)
{
  // Calls endOfLumiBlock on each layer.
  for (AFPSiLayerMonitor* layer : m_layersMonitors)
    layer->endOfLumiBlock(toolToStoreHistograms);
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
