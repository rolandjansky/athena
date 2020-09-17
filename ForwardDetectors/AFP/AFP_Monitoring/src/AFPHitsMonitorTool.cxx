/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_Monitoring/AFPHitsMonitorTool.h"
#include "AFP_Monitoring/IAFPSiStationMonitor.h"
#include "AFP_Monitoring/IAFPSiLayerMonitor.h"
#include "AFP_Monitoring/AFPSiLayerSummaryManager.h"

#include <xAODForward/AFPSiHit.h>
#include <xAODForward/AFPStationID.h>
#include <xAODForward/AFPSiHitContainer.h>

#include <AthenaMonitoring/AthenaMonManager.h>
#include <GaudiKernel/MsgStream.h>
#include <GaudiKernel/StatusCode.h>

#include <LWHists/TH1F_LW.h>
#include <TH1.h>
#include <TProfile.h>

AFPHitsMonitorTool::
AFPHitsMonitorTool( const std::string & type, const std::string & name,
      const IInterface* parent )
  : ManagedMonitorToolBase( type, name, parent ),
    m_histsDirectoryName ("AFP/"),
    m_summaryManager(new AFPSiLayerSummaryManager),
    m_lumiBlockMuTool("LumiBlockMuTool/LumiBlockMuTool")
{
  declareProperty("stationsMonitors", m_stationsMonitors, "Array of station monitors.");
  declareProperty("luminosityTool", m_lumiBlockMuTool, "Luminosity tool used for reading pile-up value.");
}


AFPHitsMonitorTool::
~AFPHitsMonitorTool()
{
  delete m_summaryManager;
}

StatusCode AFPHitsMonitorTool::initialize()
{
  if (m_stationsMonitors.size() != 0) {
    // loop over tools
    for (ToolHandle<IAFPSiStationMonitor>& stationMon : m_stationsMonitors) {
      if (stationMon.retrieve().isFailure())
	ATH_MSG_WARNING("Failed to retrieve AlgTool " << stationMon);
      else
	stationMon->setAllLayersParent(this);
    }
  }
  else {
    ATH_MSG_ERROR("No station monitors defined. Aborting.");
    return StatusCode::FAILURE;
  }


  ManagedMonitorToolBase::MonGroup managedBookingLumiBlock(this, histsDirectoryName() + "shifter/", lumiBlock, ManagedMonitorToolBase::ATTRIB_MANAGED);   // to re-booked every luminosity block
  AFPSiLayerSummaryProfileBase* timeOverThresholdMean = m_summaryManager->createSummaryHits(this, 
											    managedBookingLumiBlock, 
											    "h_timeOverThresholdMeanp", 
											    "Mean hitstime over threshold per plane",
											    &xAOD::AFPSiHit::timeOverThreshold);
  timeOverThresholdMean->profile()->SetYTitle("mean time-over-threshold");

  AFPSiLayerSummaryProfileBase* hitsPerPlaneScaledMean = 
    m_summaryManager->createSummaryEventEnd(this, 
					    managedBookingLumiBlock, 
					    "h_hitsPerPlaneScaledMean",
					    "Mean number of hits in an event per plane",
					    &IAFPSiLayerMonitor::hitsInEventScaled);
  hitsPerPlaneScaledMean->profile()->SetYTitle("Mean number of hits in an event.");

  AFPSiLayerSummaryProfileBase* hitsPerPlaneScaledHotSpotMean = 
    m_summaryManager->createSummaryEventEnd(this, 
					    managedBookingLumiBlock, 
					    "h_hitsPerPlaneScaledHotSpotMean",
					    "Mean number of hits in a hotspot in an event per plane",
					    &IAFPSiLayerMonitor::hitsInEventHotSpotScaled);
  hitsPerPlaneScaledHotSpotMean->profile()->SetYTitle("Mean number of hits in hotspot in an event");
  
  return StatusCode::SUCCESS;
}

// Description: Used for rebooking unmanaged histograms       
StatusCode AFPHitsMonitorTool::bookHistogramsRecurrent()
{
   return StatusCode::SUCCESS;
}

// Description: Used for re-booking managed histograms       
StatusCode AFPHitsMonitorTool::bookHistograms()
{
  for (ToolHandle<IAFPSiStationMonitor>& station : m_stationsMonitors)
    station->bookHistograms(this);

  m_summaryManager->book();

  return StatusCode::SUCCESS;
}

StatusCode AFPHitsMonitorTool::fillHistograms()
{
  const xAOD::AFPSiHitContainer* afpHitContainer = nullptr;
  CHECK( evtStore()->retrieve( afpHitContainer, "AFPSiHitContainer") );

  for (const xAOD::AFPSiHit* hit : *afpHitContainer) {
    bool matchStation = false;
    for (ToolHandle<IAFPSiStationMonitor>& station : m_stationsMonitors)
      if (station->stationID() == hit->stationID()) {
  	station->fillHistograms(*hit);
  	matchStation = true;
  	break;
      }

    if (matchStation == false)
      ATH_MSG_WARNING("Unrecognised station index: "<<hit->stationID());
  }


  for (ToolHandle<IAFPSiStationMonitor>& station : m_stationsMonitors)
    station->eventEnd();

  return StatusCode::SUCCESS;
}


void AFPHitsMonitorTool::makeLayerSummaryHist (const std::string inputHistName, const std::string outputHistName, const std::string outputHistTitle) 
{
  ManagedMonitorToolBase::MonGroup managedBookingLumiBlock(this, histsDirectoryName(), lumiBlock, ManagedMonitorToolBase::ATTRIB_MANAGED);   // to re-booked every luminosity block

  // make histogram
  int totalPlanes = 0;
  for (const ToolHandle<IAFPSiStationMonitor>& station : m_stationsMonitors)
    totalPlanes += station->layersMonitors().size();
  
  TProfile* outputHist = new TProfile(outputHistName.data(),
				      outputHistTitle.data(),
				      totalPlanes, - 0.5, totalPlanes + 0.5);    
  
  regHist( outputHist, managedBookingLumiBlock ).ignore();

  TAxis* axis = outputHist->GetXaxis();
  int binCounter = 1;
  for (const ToolHandle<IAFPSiStationMonitor>& station : m_stationsMonitors)
    for (const ToolHandle<IAFPSiLayerMonitor>& layer : station->layersMonitors()) {
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

    for (ToolHandle<IAFPSiStationMonitor>& station : m_stationsMonitors)
      station->endOfLumiBlock(this);
  }


  // if ( endOfEventsBlockFlag() ) {
  // }

  // if ( endOfRunFlag() ) {
  // }
  
  return StatusCode::SUCCESS;
}


