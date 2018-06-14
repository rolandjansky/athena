/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_Monitoring/AFPSiLayerMonitor.h"
#include "AFP_Monitoring/AFPHitsMonitorTool.h"
#include "AFP_Monitoring/AFPSiLayerSummaryManager.h"

#include "xAODForward/AFPStationID.h"

#include <AthenaMonitoring/ManagedMonitorToolBase.h>

#include <LWHists/TH1F_LW.h>
#include <LWHists/TH2F_LW.h>

#include <sstream>

AFPSiLayerMonitor::AFPSiLayerMonitor(const std::string& type,
				     const std::string& name,
				     const IInterface* parent) : 
  AthAlgTool  (type, name, parent),
  m_parentMonitor (nullptr),
  m_hitsInEvent (0),
  m_hitMap(nullptr),
  m_hitMultiplicity(nullptr),
  m_timeOverThreshold(nullptr),
  m_hitMultiplicityHotSpot(nullptr)
{
  declareInterface<IAFPSiLayerMonitor>(this);

  declareProperty("pixelLayerID", m_pixelLayerID = -1, "ID number of pixel layer.");
  declareProperty("stationID", m_stationID = -1, "ID number of station in which is the monitored layer.");

  declareProperty("hitsScaleFactor", m_hitsScaleFactor = 0.04, "Scale factor for normalising hits in event to pile-up.");
  
  declareProperty("hotSpotStartRow", m_hotSpotStartRow = 0, "First row of the hot-spot (included in the hot-spot).");
  declareProperty("hotSpotEndRow", m_hotSpotEndRow = 50, "Last row of the hot-spot (included in the hot-spot).");
  declareProperty("hotSpotStartCol", m_hotSpotStartCol = 0, "First column of the hot-spot (included in the hot-spot).");
  declareProperty("hotSpotEndCol", m_hotSpotEndCol = 30, "Last column of the hot-spot (included in the hot-spot).");
}

AFPSiLayerMonitor::~AFPSiLayerMonitor()
{
}

StatusCode AFPSiLayerMonitor::initialize()
{
  // set full name
  std::stringstream fullName;
  fullName<<"st."<<m_stationID;
  fullName<<"/lay."<<m_pixelLayerID;
  m_layerFullName = fullName.str();
  
  return StatusCode::SUCCESS;
}

StatusCode AFPSiLayerMonitor::finalize()
{
  return StatusCode::SUCCESS;
}

// Description: Used for re-booking managed histograms       
void AFPSiLayerMonitor::bookHistograms(ManagedMonitorToolBase* toolToStoreHistograms, const std::string histsDirName)
{
  m_histsDirName = histsDirName;
  
  // ********** Per lumi block **********
  ManagedMonitorToolBase::MonGroup managed_booking_lumiBlock( toolToStoreHistograms, histsDirName.data(), toolToStoreHistograms->lumiBlock);   // to re-booked every luminosity block
  ManagedMonitorToolBase::MonGroup managed_booking_run( toolToStoreHistograms, histsDirName.data(), toolToStoreHistograms->run);   // to re-booked every run
  
  // ----- hit map -----
  // create histogram name and title
  std::string hitMapName = makeHistName("h_hitMap");
  std::string hitMapTitle = makeHistName("Map of hits");

  constexpr int nRows = 80;
  constexpr int nColumns = 336;

  // create and register histogram
  m_hitMap  = TH2F_LW::create(hitMapName.data(),
			      hitMapTitle.data(),
			      nRows, 0.5, nRows + 0.5,
			      nColumns, 0.5, nColumns + 0.5);
  m_hitMap->SetXTitle("column ID");
  m_hitMap->SetYTitle("row ID");

  toolToStoreHistograms->regHist(m_hitMap, managed_booking_run).ignore();


  // ----- hit multiplicity -----
  // create histogram name and title
  std::string hitMultiplicityName = makeHistName("h_hitMultiplicity");
  std::string hitMultiplicityTitle = makeHistTitle("Number of hits per event");

  m_hitMultiplicity = new TH1F(hitMultiplicityName.data(),
			       hitMultiplicityTitle.data(),
			       40, -0.5, 39.5);
  m_hitMultiplicity->StatOverflows(); // need to use overflows for calculation of mean
  m_hitMultiplicity->SetXTitle("number of hits in an event");
  m_hitMultiplicity->SetYTitle("events");

  toolToStoreHistograms->regHist( m_hitMultiplicity, managed_booking_lumiBlock ).ignore();


  // create histogram name and title
  std::string hitMultiplicityNameHotSpot = makeHistName("h_hitMultiplicityHotSpot");
  std::string hitMultiplicityTitleHotSpot = makeHistTitle("Number of hits per event in the hot spot");

  m_hitMultiplicityHotSpot = new TH1F(hitMultiplicityNameHotSpot.data(),
				      hitMultiplicityTitleHotSpot.data(),
				      40, -0.5, 39.5);
  m_hitMultiplicityHotSpot->StatOverflows(); // need to use overflows for calculation of mean
  m_hitMultiplicityHotSpot->SetXTitle("number of hits in hotspot in an event");
  m_hitMultiplicityHotSpot->SetYTitle("events");

  toolToStoreHistograms->regHist( m_hitMultiplicityHotSpot, managed_booking_lumiBlock ).ignore();


  // ----- hit charge -----
  // create histogram name and title
  std::string timeOverThresholdName = makeHistName("h_timeOverThreshold");
  std::string timeOverThresholdTitle = makeHistTitle("Time over threshold");

  m_timeOverThreshold = new TH1F(timeOverThresholdName.data(),
				 timeOverThresholdTitle.data(),
				 16, -0.5, 15.5);
  m_timeOverThreshold->StatOverflows();  // need to use overflows for calculation of mean
  m_timeOverThreshold->SetXTitle("time-over-threshold");
  m_timeOverThreshold->SetYTitle("number of hits");

  toolToStoreHistograms->regHist( m_timeOverThreshold, managed_booking_lumiBlock ).ignore();
}


void AFPSiLayerMonitor::fillHistograms(const xAOD::AFPSiHit& hit)
{
  // update variables
  m_hitsInEvent++;

  if (hit.pixelColIDChip() >= m_hotSpotStartCol && hit.pixelColIDChip() <= m_hotSpotEndCol)
    if (hit.pixelRowIDChip() >= m_hotSpotStartRow && hit.pixelRowIDChip() <= m_hotSpotEndRow)
      m_hitsInEventHotSpot++;

  // fill histograms
  m_hitMap->Fill(hit.pixelColIDChip(), hit.pixelRowIDChip());
  m_timeOverThreshold->Fill(hit.timeOverThreshold());

  // fill summary histograms
  m_parentMonitor->summaryManager()->fillHits(layerFullName(), hit);
}

void AFPSiLayerMonitor::eventEnd()
{
  // fill histograms
  m_hitMultiplicity->Fill(m_hitsInEvent);
  m_hitMultiplicityHotSpot->Fill(m_hitsInEventHotSpot);

  m_parentMonitor->summaryManager()->fillEventEnd(layerFullName(), this);

  // reset variables
  m_hitsInEvent = 0;
  m_hitsInEventHotSpot = 0;
}

void AFPSiLayerMonitor::endOfLumiBlock()
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

double AFPSiLayerMonitor::correctHitsForPileUp (double hits) const 
{
  return hits - (m_hitsScaleFactor * m_parentMonitor->pileUp());
}
