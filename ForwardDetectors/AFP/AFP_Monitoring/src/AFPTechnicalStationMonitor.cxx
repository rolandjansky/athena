/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>

#include <LWHists/TH1F_LW.h>
#include <LWHists/TH2F_LW.h>

#include "../AFP_Monitoring/AFPTechnicalMonitorTool.h"
#include "../AFP_Monitoring/AFPTechnicalLayerMonitor.h"
#include "../AFP_Monitoring/AFPTechnicalStationMonitor.h"

#include "AFP_RawEv/AFP_SiRawData.h"


const int AFPTechnicalStationMonitor::s_firstLayerIndex = 0;
const int AFPTechnicalStationMonitor::s_secondLayerIndex = 1;
const int AFPTechnicalStationMonitor::s_thirdLayerIndex = 2;


AFPTechnicalStationMonitor::AFPTechnicalStationMonitor(const int stationID)
  : m_stationID (stationID)
  , m_firstLayer (nullptr)
  , m_secondLayer (nullptr)
  , m_thirdLayer (nullptr)
{
  m_firstLayer = createAndAddLayerMonitor(s_firstLayerIndex);
  m_secondLayer = createAndAddLayerMonitor(s_secondLayerIndex);
  m_thirdLayer = createAndAddLayerMonitor(s_thirdLayerIndex);
}


AFPTechnicalStationMonitor::~AFPTechnicalStationMonitor()
{
  for (AFPTechnicalLayerMonitor* layerMonitor : m_layersMonitors)
    delete layerMonitor;
}

// Description: Used for re-booking managed histograms       
void AFPTechnicalStationMonitor::bookHistograms(AFPTechnicalMonitorTool* toolToStoreHistograms)
{
  std::string histsDirName = toolToStoreHistograms->histsDirectoryName();
  histsDirName += makeName("");

  // register histograms in layers
  for (AFPTechnicalLayerMonitor* layerMonitor : m_layersMonitors)
    layerMonitor->bookHistograms(toolToStoreHistograms, histsDirName);
}


void AFPTechnicalStationMonitor::fillHistograms(const AFP_SiRawData& hit)
{
  switch (hit.link()%4) {
  case s_firstLayerIndex:
    m_firstLayer->fillHistograms(hit);
    break;
  case s_secondLayerIndex:
    m_secondLayer->fillHistograms(hit);
    break;
  case s_thirdLayerIndex:
    m_thirdLayer->fillHistograms(hit);
    break;
    //  default:
    //    ATH_MSG_WARNING("Unrecognised layer number: "<<hit.pixelLayerID());
  }

}

AFPTechnicalLayerMonitor* AFPTechnicalStationMonitor::createAndAddLayerMonitor (const int layerID)
{
  AFPTechnicalLayerMonitor* newMonitor = new AFPTechnicalLayerMonitor (layerID, m_stationID);
  m_layersMonitors.push_back(newMonitor);

  return newMonitor;
}

void AFPTechnicalStationMonitor::eventEnd()
{
  for (AFPTechnicalLayerMonitor* layer : m_layersMonitors)
    layer->eventEnd();
}


void AFPTechnicalStationMonitor::endOfLumiBlock(AFPTechnicalMonitorTool* toolToStoreHistograms)
{
  /// Calls endOfLumiBlock on each layer.

  for (AFPTechnicalLayerMonitor* layer : m_layersMonitors)
    layer->endOfLumiBlock(toolToStoreHistograms);
}

std::string AFPTechnicalStationMonitor::makeName (const std::string name) const
{
  std::stringstream histName;
  histName <<name<<"Station"<<m_stationID;

  return histName.str();
}

std::string AFPTechnicalStationMonitor::makeTitle (const std::string title) const
{
  std::stringstream histTitle;
  histTitle <<title<<" in station "<<m_stationID;

  return histTitle.str();
}
