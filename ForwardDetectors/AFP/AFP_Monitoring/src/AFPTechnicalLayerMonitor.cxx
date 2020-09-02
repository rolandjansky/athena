/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>

#include <AthenaMonitoring/ManagedMonitorToolBase.h>

#include <LWHists/TH1F_LW.h>

#include "../AFP_Monitoring/AFPTechnicalLayerMonitor.h"

AFPTechnicalLayerMonitor::AFPTechnicalLayerMonitor(const int pixelLayerID, const int stationID)
   : m_pixelLayerID (pixelLayerID)
   ,m_stationID (stationID)
{
  // make sure that in first event time-over threshold can be overwriten
}


AFPTechnicalLayerMonitor::~AFPTechnicalLayerMonitor()
{
}

// Description: Used for re-booking managed histograms       
void AFPTechnicalLayerMonitor::bookHistograms(ManagedMonitorToolBase* toolToStoreHistograms, const std::string histsDirName)
{
  
  // ********** Per lumi block **********
  ManagedMonitorToolBase::MonGroup managed_booking_lumiBlock( toolToStoreHistograms, histsDirName.data(), toolToStoreHistograms->lumiBlock);   // to re-booked every luminosity block
  
}


void AFPTechnicalLayerMonitor::fillHistograms(const AFP_SiRawData& /*hit*/)
{
  // fill histograms

}

void AFPTechnicalLayerMonitor::eventEnd()
{

}

void AFPTechnicalLayerMonitor::endOfLumiBlock(ManagedMonitorToolBase* /* toolToStoreHistograms */)
{

}


std::string AFPTechnicalLayerMonitor::makeHistName (const std::string name) const
{
  std::stringstream histName;
  histName<<name<<"St"<<m_stationID<<"Layer"<<m_pixelLayerID;
  
  return histName.str();
}


std::string AFPTechnicalLayerMonitor::makeHistTitle (const std::string title) const
{
  std::stringstream histTitle;
  histTitle<<title<<" in station "<<m_stationID<<" for layer "<<m_pixelLayerID;
  
  return histTitle.str();
}
