/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>

#include <AthenaMonitoring/ManagedMonitorToolBase.h>

#include <LWHists/TH1F_LW.h>
#include <LWHists/TH2F_LW.h>

#include "../AFP_Monitoring/AFPSiLayerMonitor.h"

AFPSiLayerMonitor::AFPSiLayerMonitor(const int pixelLayerID, const int stationID)
   : m_pixelLayerID (pixelLayerID)
   ,m_stationID (stationID)
   ,m_hitsInEvent (0)
   ,m_hitMap(nullptr)
   ,m_hitMultiplicity(nullptr)
   ,m_timeOverThreshold(nullptr)
{

}


AFPSiLayerMonitor::~AFPSiLayerMonitor()
{
}

// Description: Used for re-booking managed histograms       
void AFPSiLayerMonitor::bookHistograms(ManagedMonitorToolBase* toolToStoreHistograms, const std::string histsDirName)
{
  
  // ********** Per lumi block **********
  ManagedMonitorToolBase::MonGroup managed_booking_lumiBlock( toolToStoreHistograms, histsDirName.data(), toolToStoreHistograms->lumiBlock);   // to re-booked every luminosity block
  
  // ----- hit map -----
  // create histogram name and title
  std::string hitMapName = makeHistName("h_hitMap");
  std::string hitMapTitle = makeHistName("Map of hits");

  const int nRows = 80;
  const int nColumns = 336;


  // create and register histogram
  m_hitMap  = TH2F_LW::create(hitMapName.data(),
			      hitMapTitle.data(),
			      nRows, 0.5, nRows + 0.5,
			      nColumns, 0.5, nColumns + 0.5);

  toolToStoreHistograms->regHist(m_hitMap, managed_booking_lumiBlock ).ignore();


  // ----- hit multiplicity -----
  // create histogram name and title
  std::string hitMultiplicityName = makeHistName("h_hitMultiplicity");
  std::string hitMultiplicityTitle = makeHistTitle("Number of hits per event");

  m_hitMultiplicity = TH1F_LW::create(hitMultiplicityName.data(),
				      hitMultiplicityTitle.data(),
				      40, -0.5, 39.5);

  toolToStoreHistograms->regHist( m_hitMultiplicity, managed_booking_lumiBlock ).ignore();


  // ----- hit charge -----
  // create histogram name and title
  std::string timeOverThresholdName = makeHistName("h_timeOverThreshold");
  std::string timeOverThresholdTitle = makeHistTitle("Time over threshold");

  m_timeOverThreshold = TH1F_LW::create(timeOverThresholdName.data(),
				      timeOverThresholdTitle.data(),
					16, -0.5, 15.5);

  toolToStoreHistograms->regHist( m_timeOverThreshold, managed_booking_lumiBlock ).ignore();
}


void AFPSiLayerMonitor::fillHistograms(const xAOD::AFPSiHit& hit)
{
  // update variables
  m_hitsInEvent++;

  if (m_hitsInEvent <= 2) {
    m_rowIDs.push_back(hit.pixelRowIDChip());
    m_colIDs.push_back(hit.pixelColIDChip());
  }

  // fill histograms
  m_hitMap->Fill(hit.pixelColIDChip(), hit.pixelRowIDChip());
  m_timeOverThreshold->Fill(hit.timeOverThreshold());
}

void AFPSiLayerMonitor::eventEnd()
{
  // fill histograms
  m_hitMultiplicity->Fill(m_hitsInEvent);

  // reset variables
  m_hitsInEvent = 0;
  m_rowIDs.clear();
  m_colIDs.clear();
}

void AFPSiLayerMonitor::endOfLumiBlock(ManagedMonitorToolBase* /* toolToStoreHistograms */)
{
  
}


std::string AFPSiLayerMonitor::makeHistName (const std::string name) const
{
  std::stringstream histName;
  histName<<name<<"St"<<m_stationID<<"Layer"<<m_pixelLayerID;
  
  return histName.str();
}


std::string AFPSiLayerMonitor::makeHistTitle (const std::string title) const
{
  std::stringstream histTitle;
  histTitle<<title<<" in station "<<m_stationID<<" for layer "<<m_pixelLayerID;
  
  return histTitle.str();
  
}
