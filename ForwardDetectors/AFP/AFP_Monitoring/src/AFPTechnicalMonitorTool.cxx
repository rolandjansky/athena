/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <GaudiKernel/IJobOptionsSvc.h>
#include <GaudiKernel/MsgStream.h>
#include <GaudiKernel/StatusCode.h>
#include <AthenaMonitoring/AthenaMonManager.h>
#include <LWHists/TH1F_LW.h>

#include <xAODEventInfo/EventInfo.h>
#include <AFP_RawEv/AFP_SiRawData.h>
#include <AFP_RawEv/AFP_RawContainer.h>

#include <AFP_Monitoring/AFPTechnicalMonitorTool.h>

const int AFPTechnicalMonitorTool::s_cNearStationIndex = 2;
const int AFPTechnicalMonitorTool::s_cFarStationIndex = 3;


AFPTechnicalMonitorTool::
AFPTechnicalMonitorTool( const std::string & type, const std::string & name,
			 const IInterface* parent )
  : ManagedMonitorToolBase( type, name, parent )
  , m_histsDirectoryName ("AFP/Technical/")
  , m_cNearStation (s_cNearStationIndex)
  , m_cFarStation (s_cFarStationIndex)
{
  m_stationsMonitors.push_back(&m_cNearStation);
  m_stationsMonitors.push_back(&m_cFarStation);
}


AFPTechnicalMonitorTool::
~AFPTechnicalMonitorTool()
{
}

// Description: Used for rebooking unmanaged histograms       
StatusCode AFPTechnicalMonitorTool::bookHistogramsRecurrent()
{
  return StatusCode::SUCCESS;
}

// Description: Used for re-booking managed histograms       
StatusCode AFPTechnicalMonitorTool::bookHistograms( )
{
  // ********** Per lumi block **********
  ManagedMonitorToolBase::MonGroup managed_booking_lumiBlock(this, m_histsDirectoryName.data(), lumiBlock);   // to re-booked every luminosity block

  // book histograms in stations
  for (AFPTechnicalStationMonitor* station : m_stationsMonitors)
    station->bookHistograms(this);

  // book general histograms
  m_bcIDATLAS  = TH1F_LW::create("bcIDATLAS",
				 "Distribution of bcID from AFP",
				 3564, -0.5, 3563.5);
  m_bcIDATLAS->SetXTitle("bcID^{ATLAS}");
  m_bcIDATLAS->SetYTitle("number of events");
  regHist(m_bcIDATLAS, managed_booking_lumiBlock ).ignore();

  
  // create and register histogram only if Raw data is processed
  if(m_environment == AthenaMonManager::online || m_environment == AthenaMonManager::tier0Raw) {
    m_bcIDAfp  = TH1F_LW::create("bcIDAfp",
				 "Distribution of bcID from AFP",
				 3564, -0.5, 3563.5);
    m_bcIDAfp->SetXTitle("bcID^{AFP}");
    m_bcIDAfp->SetYTitle("number of events");
    regHist(m_bcIDAfp, managed_booking_lumiBlock ).ignore();

  
    m_bcIDDiffAfpAtlas  = TH1F_LW::create("bcIDDiffAfpAtlas",
					  "Distribution of difference between bcID from AFP and ATLAS",
					  21, -10.5, 10.5);
    m_bcIDDiffAfpAtlas->SetXTitle("bcID^{AFP} - bcID^{ATLAS}");
    m_bcIDDiffAfpAtlas->SetYTitle("number of events");
    regHist(m_bcIDDiffAfpAtlas, managed_booking_lumiBlock ).ignore();
  }
  
  return StatusCode::SUCCESS;
}


StatusCode AFPTechnicalMonitorTool::fillHistograms()
{
  // read information
  const xAOD::EventInfo* eventInfo = 0;
  CHECK( evtStore()->retrieve( eventInfo) );

  // create and register histogram only if Raw data is processed 
  if(m_environment == AthenaMonManager::online || m_environment == AthenaMonManager::tier0Raw) {

    // read information
    const AFP_RawContainer* afpContainer = 0;
    CHECK(evtStore()->retrieve(afpContainer));

    for (const AFP_SiRawCollection& hitCollection : afpContainer->collectionsSi()) {
      for (const AFP_SiRawData& hit : hitCollection.dataRecords()) {
    	if (hit.link() <= 3) {	// hit.link() is unsigned, so always greater than 0
    	  m_cNearStation.fillHistograms(hit);
    	}
    	else if (hit.link() >= 8 && hit.link() <= 11) {
    	  m_cFarStation.fillHistograms(hit);
    	}
    	else
    	  ATH_MSG_WARNING("Unrecognised station index: "<<hit.link());
      }
    }

    for (AFPTechnicalStationMonitor* station : m_stationsMonitors)
      station->eventEnd();
    
    // fill histograms
    m_bcIDAfp->Fill(afpContainer->bcId());
    m_bcIDDiffAfpAtlas->Fill(afpContainer->bcId() - eventInfo->bcid());
  }

  m_bcIDATLAS->Fill(eventInfo->bcid());
  
  return StatusCode::SUCCESS;
}


StatusCode AFPTechnicalMonitorTool::procHistograms( )
{
  // if( endOfLumiBlockFlag() ) {
  //   for (AFPTechnicalStationMonitor* station : m_stationsMonitors)
  //     station->endOfLumiBlock(this);
  // }

  return StatusCode::SUCCESS;
}


