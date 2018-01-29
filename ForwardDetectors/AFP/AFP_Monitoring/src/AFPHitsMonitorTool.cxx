/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <GaudiKernel/IJobOptionsSvc.h>
#include <GaudiKernel/MsgStream.h>
#include <GaudiKernel/StatusCode.h>

#include <AthenaMonitoring/AthenaMonManager.h>

#include <LWHists/TH1F_LW.h>
#include <TH1.h>
#include <TProfile.h>

//#include <xAODEventInfo/EventInfo.h>
#include <xAODForward/AFPSiHit.h>
#include <xAODForward/AFPStationID.h>
#include <xAODForward/AFPSiHitContainer.h>

#include "AFP_Monitoring/AFPHitsMonitorTool.h"
#include "AFP_Monitoring/AFPSiStationMonitor.h"
#include "AFP_Monitoring/AFPSiLayerMonitor.h"



AFPHitsMonitorTool::
AFPHitsMonitorTool( const std::string & type, const std::string & name,
      const IInterface* parent )
   : ManagedMonitorToolBase( type, name, parent )
   , m_histsDirectoryName ("AFP/")
   , m_cNearStation (xAOD::AFPStationID::nearC)
   , m_cFarStation (xAOD::AFPStationID::farC)
   , m_aNearStation (xAOD::AFPStationID::nearA)
   , m_aFarStation (xAOD::AFPStationID::farA)
{
  m_stationsMonitors.push_back(&m_aFarStation);
  m_stationsMonitors.push_back(&m_aNearStation);
  m_stationsMonitors.push_back(&m_cNearStation);
  m_stationsMonitors.push_back(&m_cFarStation);
}


AFPHitsMonitorTool::
~AFPHitsMonitorTool()
{
}

// Description: Used for rebooking unmanaged histograms       
StatusCode AFPHitsMonitorTool::bookHistogramsRecurrent()
{
   return StatusCode::SUCCESS;
}

// Description: Used for re-booking managed histograms       
StatusCode AFPHitsMonitorTool::bookHistograms()
{
  for (AFPSiStationMonitor* station : m_stationsMonitors)
    station->bookHistograms(this);

  return StatusCode::SUCCESS;
}

StatusCode AFPHitsMonitorTool::fillHistograms()
{
  const xAOD::AFPSiHitContainer* afpHitContainer = nullptr;
  CHECK( evtStore()->retrieve( afpHitContainer, "AFPSiHitContainer") );

  for (const xAOD::AFPSiHit* hit : *afpHitContainer) {
    bool matchStation = false;
    for (AFPSiStationMonitor* station : m_stationsMonitors)
      if (station->stationID() == hit->stationID()) {
  	station->fillHistograms(*hit);
  	matchStation = true;
  	break;
      }

    if (matchStation == false)
      ATH_MSG_WARNING("Unrecognised station index: "<<hit->stationID());
  }

  for (AFPSiStationMonitor* station : m_stationsMonitors)
    station->eventEnd();
      
  return StatusCode::SUCCESS;
}


void AFPHitsMonitorTool::makeLayerSummaryHist (const std::string inputHistName, const std::string outputHistName, const std::string outputHistTitle) 
{
  ManagedMonitorToolBase::MonGroup managedBookingLumiBlock(this, histsDirectoryName(), lumiBlock, ManagedMonitorToolBase::ATTRIB_MANAGED);   // to re-booked every luminosity block

  // make histogram
  int totalPlanes = 0;
  for (AFPSiStationMonitor* station : m_stationsMonitors)
    totalPlanes += station->layersMonitors().size();
  
  TProfile* outputHist = new TProfile(outputHistName.data(),
				      outputHistTitle.data(),
				      totalPlanes, - 0.5, totalPlanes + 0.5);    
  
  regHist( outputHist, managedBookingLumiBlock ).ignore();

  TAxis* axis = outputHist->GetXaxis();
  int binCounter = 1;
  for (const AFPSiStationMonitor* station : m_stationsMonitors)
    for (const AFPSiLayerMonitor* layer : station->layersMonitors()) {
      std::stringstream binName;
      binName<<"st."<<station->stationID();
      binName<<"/lay."<<layer->layerID();
      axis->SetBinLabel (binCounter, binName.str().data());
      TH1* hist = nullptr;
      if (getHist(hist, layer->makeHistName(inputHistName), layer->histsDirName(), lumiBlock).isSuccess()) {
	const double mean = hist->GetMean()*hist->GetEntries();
      
	const double errorSq = hist->GetMeanError()*hist->GetMeanError();
	const double entriesSq = hist->GetEntries()*hist->GetEntries();
	const double valueSq = mean * mean;
	const double meanError = sqrt( errorSq*entriesSq + (valueSq/hist->GetEntries()) );

	outputHist->SetBinEntries(binCounter, hist->GetEntries());
	outputHist->SetBinContent(binCounter, mean);
	outputHist->SetBinError(binCounter, meanError);
      }
      else
	ATH_MSG_WARNING ("Missing "<<layer->makeHistName(inputHistName)<<" does not exist, but is needed for a summary.");

      
      ++binCounter;
    }
} // close m_hitsPerPlaneSummary



StatusCode AFPHitsMonitorTool::procHistograms( )
{
  if( endOfLumiBlockFlag() ) {

    for (AFPSiStationMonitor* station : m_stationsMonitors)
      station->endOfLumiBlock(this);

    makeLayerSummaryHist ("h_hitMultiplicity",
    			  "h_hitsPerPlaneSummary",
    			  "Summary mean number of hits per plane per event");

    makeLayerSummaryHist ("h_hitMultiplicityHotSpot",
    			  "h_hitsPerPlaneHotSpotSummary",
    			  "Summary mean number of hits per plane per event in hot spot");

    makeLayerSummaryHist ("h_timeOverThreshold",
    			  "h_timeOverthresholdSummary",
    			  "Summary mean number of time over threshold per plane");

  }


  // if ( endOfEventsBlockFlag() ) {
  // }

  // if ( endOfRunFlag() ) {
  // }
  
  return StatusCode::SUCCESS;
}


