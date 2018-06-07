/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <GaudiKernel/IJobOptionsSvc.h>
#include <GaudiKernel/MsgStream.h>
#include <GaudiKernel/StatusCode.h>

#include <AthenaMonitoring/AthenaMonManager.h>

//#include <xAODEventInfo/EventInfo.h>
#include <xAODForward/AFPSiHit.h>
#include <xAODForward/AFPSiHitContainer.h>

#include <AFP_Monitoring/AFPHitsMonitorTool.h>

const int AFPHitsMonitorTool::s_cNearStationIndex = 2;
const int AFPHitsMonitorTool::s_cFarStationIndex = 3;


AFPHitsMonitorTool::
AFPHitsMonitorTool( const std::string & type, const std::string & name,
      const IInterface* parent )
   : ManagedMonitorToolBase( type, name, parent )
   , m_histsDirectoryName ("AFP/")
   , m_cNearStation (s_cNearStationIndex)
   , m_cFarStation (s_cFarStationIndex)
{
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
  // for (AFPSiStationMonitor* station : m_stationsMonitors)
  //   station->bookHistogramsRecurrent(this);

   return StatusCode::SUCCESS;
}

// Description: Used for re-booking managed histograms       
StatusCode AFPHitsMonitorTool::bookHistograms( )
{
  for (AFPSiStationMonitor* station : m_stationsMonitors)
    station->bookHistograms(this);

  return StatusCode::SUCCESS;
}


StatusCode AFPHitsMonitorTool::fillHistograms()
{
  // const xAOD::EventInfo* eventInfo = 0;
  // CHECK( evtStore()->retrieve( eventInfo) );

  const xAOD::AFPSiHitContainer* afpHitContainer = 0;
  CHECK( evtStore()->retrieve( afpHitContainer, "AFPSiHitContainer") );

  for (const xAOD::AFPSiHit* hit : *afpHitContainer) {
    switch (hit->stationID()) {
    case s_cNearStationIndex:
      m_cNearStation.fillHistograms(*hit);
      break;
	  
    case s_cFarStationIndex:
      m_cFarStation.fillHistograms(*hit);
      break;

    default:
      ATH_MSG_WARNING("Unrecognised station index: "<<hit->stationID());
    }
  }

  m_cNearStation.eventEnd();
  m_cFarStation.eventEnd();
      
  return StatusCode::SUCCESS;
}


StatusCode AFPHitsMonitorTool::procHistograms( )
{
  if( endOfLumiBlockFlag() ) {
    for (AFPSiStationMonitor* station : m_stationsMonitors)
      station->endOfLumiBlock(this);
   }

   return StatusCode::SUCCESS;
}


