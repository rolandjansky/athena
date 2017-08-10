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
   ,m_lvl1(nullptr)
   ,m_lvl1Seed(nullptr)
{
  // make sure that in first event time-over threshold can be overwriten
  m_seed.Set_ToT(0);
}


AFPTechnicalLayerMonitor::~AFPTechnicalLayerMonitor()
{
}

// Description: Used for re-booking managed histograms       
void AFPTechnicalLayerMonitor::bookHistograms(ManagedMonitorToolBase* toolToStoreHistograms, const std::string histsDirName)
{
  
  // ********** Per lumi block **********
  ManagedMonitorToolBase::MonGroup managed_booking_lumiBlock( toolToStoreHistograms, histsDirName.data(), toolToStoreHistograms->lumiBlock);   // to re-booked every luminosity block
  
  // ----- hit map -----
  // create histogram name and title
  std::string lvl1Name = makeHistName("h_lvl1");
  std::string lvl1Title = makeHistName("lvl1");

  // create and register histogram
  m_lvl1  = TH1F_LW::create(lvl1Name.data(),
			    lvl1Title.data(),
			    16, -0.5, 15.5);

  toolToStoreHistograms->regHist(m_lvl1, managed_booking_lumiBlock ).ignore();


  std::string lvl1SeedName = makeHistName("h_lvl1Seed");
  std::string lvl1SeedTitle = makeHistName("lvl1 for hit with largest time over threshold");
  m_lvl1Seed  = TH1F_LW::create(lvl1SeedName.data(),
			    lvl1SeedTitle.data(),
			    16, -0.5, 15.5);

  toolToStoreHistograms->regHist(m_lvl1Seed, managed_booking_lumiBlock ).ignore();

}


void AFPTechnicalLayerMonitor::fillHistograms(const AFP_RawData& hit)
{
  // fill histograms
  m_lvl1->Fill(hit.Get_lvl1());

  // look for hit with largest time over threshold
  if (hit.Get_ToT() >= m_seed.Get_ToT())
    m_seed = hit;
}

void AFPTechnicalLayerMonitor::eventEnd()
{
  m_lvl1Seed->Fill(m_seed.Get_lvl1());
}

void AFPTechnicalLayerMonitor::endOfLumiBlock(ManagedMonitorToolBase* /*toolToStoreHistograms*/)
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
