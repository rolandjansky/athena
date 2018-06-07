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


AFPSiStationMonitor::AFPSiStationMonitor(const int stationID)
  : m_stationID (stationID)
  , m_firstLayer (nullptr)
  , m_secondLayer (nullptr)
  , m_thirdLayer (nullptr)
{
  m_firstLayer = createAndAddLayerMonitor(s_firstLayerIndex);
  m_secondLayer = createAndAddLayerMonitor(s_secondLayerIndex);
  m_thirdLayer = createAndAddLayerMonitor(s_thirdLayerIndex);
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

  const int nRows = 80;
  const int nColumns = 336;
  

  std::string histsDirName = toolToStoreHistograms->histsDirectoryName();
  histsDirName += makeName("");

  // register histograms in layers
  for (AFPSiLayerMonitor* layerMonitor : m_layersMonitors)
    layerMonitor->bookHistograms(toolToStoreHistograms, histsDirName);

  
  // ********** Per lumi block **********
  ManagedMonitorToolBase::MonGroup managed_booking_lumiBlock( toolToStoreHistograms, histsDirName.data(), toolToStoreHistograms->lumiBlock);   // to re-booked every luminosity block

  m_correlationRow12 = TH2F_LW::create(makeName("h_corrRow12").data(),
				       makeTitle("Correlation of rows between planes 1 and 2").data(),
				       nColumns, 0.5, nColumns + 0.5,
				       nColumns, 0.5, nColumns + 0.5);
  toolToStoreHistograms->regHist( m_correlationRow12, managed_booking_lumiBlock ).ignore();
  

  m_correlationRow13 = TH2F_LW::create(makeName("h_corrRow13").data(),
				       makeTitle("Correlation of rows between planes 1 and 3").data(),
				       nColumns, 0.5, nColumns + 0.5,
				       nColumns, 0.5, nColumns + 0.5);
  toolToStoreHistograms->regHist( m_correlationRow13, managed_booking_lumiBlock ).ignore();

  m_correlationRow23 = TH2F_LW::create(makeName("h_corrRow23").data(),
				       makeTitle("Correlation of rows between planes 2 and 3").data(),
				       nColumns, 0.5, nColumns + 0.5,
				       nColumns, 0.5, nColumns + 0.5);
  toolToStoreHistograms->regHist( m_correlationRow23, managed_booking_lumiBlock ).ignore();

  m_correlationCol12 = TH2F_LW::create(makeName("h_corrCol12").data(),
				       makeTitle("Correlation of columns between planes 1 and 2").data(),
				       nRows, 0.5, nRows + 0.5,				       
				       nRows, 0.5, nRows + 0.5);
  toolToStoreHistograms->regHist( m_correlationCol12, managed_booking_lumiBlock ).ignore();

  m_correlationCol13 = TH2F_LW::create(makeName("h_corrCol13").data(),
				       makeTitle("Correlation of columns between planes 1 and 3").data(),
				       nRows, 0.5, nRows + 0.5,				       
				       nRows, 0.5, nRows + 0.5);
  toolToStoreHistograms->regHist( m_correlationCol13, managed_booking_lumiBlock ).ignore();

  m_correlationCol23 = TH2F_LW::create(makeName("h_corrCol23").data(),
				       makeTitle("Correlation of columns between planes 2 and 3").data(),
				       nRows, 0.5, nRows + 0.5,				       
				       nRows, 0.5, nRows + 0.5);
  toolToStoreHistograms->regHist( m_correlationCol23, managed_booking_lumiBlock ).ignore();


  const int diffRange = 9;
  
  m_corr1DRow12 = TH1F_LW::create(makeName("h_corr1DRow12").data(),
				  makeTitle("Correlation of rows between planes 1 and 2").data(),
				  2*diffRange + 1, -diffRange - 0.5, diffRange + 0.5);
  toolToStoreHistograms->regHist( m_corr1DRow12, managed_booking_lumiBlock ).ignore();

  m_corr1DRow13 = TH1F_LW::create(makeName("h_corr1DRow13").data(),
				  makeTitle("Correlation of rows between planes 1 and 3").data(),
				  2*diffRange + 1, -diffRange - 0.5, diffRange + 0.5);
  toolToStoreHistograms->regHist( m_corr1DRow13, managed_booking_lumiBlock ).ignore();
  
  m_corr1DRow23 = TH1F_LW::create(makeName("h_corr1DRow23").data(),
				  makeTitle("Correlation of rows between planes 2 and 3").data(),
				  2*diffRange + 1, -diffRange - 0.5, diffRange + 0.5);
  toolToStoreHistograms->regHist( m_corr1DRow23, managed_booking_lumiBlock ).ignore();
  

  
  m_corr1DCol12 = TH1F_LW::create(makeName("h_corr1DCol12").data(),
				  makeTitle("Correlation of columns between planes 1 and 2").data(),
				  2*diffRange + 1, -diffRange - 0.5, diffRange + 0.5);
  toolToStoreHistograms->regHist( m_corr1DCol12, managed_booking_lumiBlock ).ignore();

  m_corr1DCol13 = TH1F_LW::create(makeName("h_corr1DCol13").data(),
				  makeTitle("Correlation of columns between planes 1 and 3").data(),
				  2*diffRange + 1, -diffRange - 0.5, diffRange + 0.5);
  toolToStoreHistograms->regHist( m_corr1DCol13, managed_booking_lumiBlock ).ignore();
  
  m_corr1DCol23 = TH1F_LW::create(makeName("h_corr1DCol23").data(),
				  makeTitle("Correlation of columns between planes 2 and 3").data(),
				  2*diffRange + 1, -diffRange - 0.5, diffRange + 0.5);
  toolToStoreHistograms->regHist( m_corr1DCol23, managed_booking_lumiBlock ).ignore();
  

}



void AFPSiStationMonitor::fillHistograms(const xAOD::AFPSiHit& hit)
{
  switch (hit.pixelLayerID()) {
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

AFPSiLayerMonitor* AFPSiStationMonitor::createAndAddLayerMonitor (const int layerID)
{
  AFPSiLayerMonitor* newMonitor = new AFPSiLayerMonitor (layerID, m_stationID);
  m_layersMonitors.push_back(newMonitor);

  return newMonitor;
}

void AFPSiStationMonitor::eventEnd()
{
  if (m_firstLayer->hitsInEvent() <= 2) {
    if (m_secondLayer->hitsInEvent() <= 2) {
      fill2DHitCorrelations (m_firstLayer, m_secondLayer, m_correlationRow12, m_correlationCol12);
      fill1DHitCorrelations (m_firstLayer, m_secondLayer, m_corr1DRow12, m_corr1DCol12);
    }

    if (m_thirdLayer->hitsInEvent() <= 2) {
      fill2DHitCorrelations (m_firstLayer, m_thirdLayer, m_correlationRow13, m_correlationCol13);
      fill1DHitCorrelations (m_firstLayer, m_thirdLayer, m_corr1DRow13, m_corr1DCol13);
    }
  }

  if (m_secondLayer->hitsInEvent() <= 2) {
    if (m_thirdLayer->hitsInEvent() <= 2) {
      fill2DHitCorrelations (m_secondLayer, m_thirdLayer, m_correlationRow23, m_correlationCol23);
      fill1DHitCorrelations (m_secondLayer, m_thirdLayer, m_corr1DRow23, m_corr1DCol23);
    }
  }

  // must be at the end because it zeroes hitsInEvent
  m_firstLayer->eventEnd();
  m_secondLayer->eventEnd();
  m_thirdLayer->eventEnd();
}


void AFPSiStationMonitor::endOfLumiBlock(AFPHitsMonitorTool* toolToStoreHistograms)
{
  /// Calls endOfLumiBlock on each layer.

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

void AFPSiStationMonitor::fill2DHitCorrelations (AFPSiLayerMonitor* firstLayer, AFPSiLayerMonitor* secondLayer, LWHist2D* histogramRow, LWHist2D* histogramCol)
{
  for (int row1 : firstLayer->rowIDs())
    for (int row2 : secondLayer->rowIDs())
      histogramRow->Fill(row1, row2);
   
  for (int col1 : firstLayer->colIDs())
    for (int col2 : secondLayer->colIDs())
      histogramCol->Fill(col1, col2);
}

void AFPSiStationMonitor::fill1DHitCorrelations (AFPSiLayerMonitor* firstLayer, AFPSiLayerMonitor* secondLayer, LWHist1D* histogramRow, LWHist1D* histogramCol)
{
  for (int row1 : firstLayer->rowIDs())
    for (int row2 : secondLayer->rowIDs())
      histogramRow->Fill(row2 - row1);
   
  for (int col1 : firstLayer->colIDs())
    for (int col2 : secondLayer->colIDs())
      histogramCol->Fill(col2 - col1);
}
