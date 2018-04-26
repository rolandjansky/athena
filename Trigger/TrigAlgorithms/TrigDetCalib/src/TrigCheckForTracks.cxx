/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigCheckForTracks.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigDetCalib
//
// ********************************************************************

#include "TrigCheckForTracks.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IJobOptionsSvc.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrkTrack/TrackCollection.h"

#include "TrigSteeringEvent/PartialEventBuildingInfo.h"
#include "IRegionSelector/IRegSelSvc.h"

#include <string>
#include <vector>



class ISvcLocator;


TrigCheckForTracks::TrigCheckForTracks(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::AllTEAlgo(name, pSvcLocator),
  m_executedEvent(false),
  m_robSelector("TrigROBSelector", this )
{

  declareProperty("ROBSelector", m_robSelector, "The ROB Selector Tool");
  declareProperty("PEBInfoLabel", m_pebLabel = "dunno", "The label under which the PartialEventBuildingInfo object is stored");
  declareProperty("extraROBs", m_extraROBs);

  //  declareProperty("doMonitoring", m_doMonitoring = true, "switch on/off monitoring" );
  declareProperty("AcceptAll",    m_acceptAll = false);
  declareProperty("dR0" ,         m_dR0 = 0.3);
  declareProperty("dR0_overlap",  m_dR0_overlap = 0.005);
  declareProperty("pT_min",       m_pT_min = 1000.);
  declareProperty("pT_min_iso",   m_pT_min_iso = 1000.);
  declareProperty("phi_Width",    m_phiWidth = 0.1);
  declareProperty("eta_Width",    m_etaWidth = 0.1);
  declareProperty("etaEdge",      m_etaEdge = 5.0, "Upper limit of |eta| range");
  declareProperty("etaLowEdge",   m_etaLowEdge = 0.0, "Lower limit of |eta| range");
  declareProperty("tracksName",   m_tracksName = "HLT_TrigFastTrackFinder_Tau");  
  declareProperty("doNotPass",           m_doNotPass = false);        // pass through flag for initial beam items.
  declareProperty("lookForAnyTracks",    m_lookForAnyTracks = false); // any vs isolated. any is needed for initial beam items.
  declareProperty("AddCTPResult", m_addCTPResult = false, "Add the CTP result to the list of ROBs");
  declareProperty("AddL2Result",  m_addL2Result = false,  "Add the L2 result to the list of ROBs");
  declareProperty("AddEFResult",  m_addEFResult = false,  "Add the EF result to the list of ROBs");
  declareProperty("AddHLTResult",  m_addHLTResult = false,  "Add the EF result to the list of ROBs");

  declareMonitoredStdContainer("nROBs",      m_nROBs);
  declareMonitoredStdContainer("detectors",  m_dets);

  declareMonitoredStdContainer("PtTrk",          m_pT);
  declareMonitoredStdContainer("PtTrk_ISO",      m_pT_Iso);
  declareMonitoredStdContainer("dR_tracks",      m_dR);
  declareMonitoredStdContainer("eta_tracks",     m_eta);
  declareMonitoredStdContainer("phi_tracks",     m_phi);
  declareMonitoredStdContainer("eta_tracks_ISO", m_ROB_eta);
  declareMonitoredStdContainer("phi_tracks_ISO", m_ROB_phi);
  declareMonitoredVariable("iso_Tracks",         m_n_IsoTracks);

}


TrigCheckForTracks::~TrigCheckForTracks()
{ }


// ----------------------------------------------------------------------
HLT::ErrorCode TrigCheckForTracks::hltInitialize()
// ----------------------------------------------------------------------
{
  // Initialize timing service
  //------------------------------
  if( service( "TrigTimerSvc", m_timerSvc).isFailure() ) {
    ATH_MSG_WARNING( name()
                     << ": Unable to locate TrigTimer Service"  );
  }
  if (m_timerSvc)    
    m_timers.push_back(m_timerSvc->addItem("TrigCheckForTracks.TrigCheckForTracksTot"));

  m_acceptedEvts = 0;
  m_rejectedEvts = 0;
  m_errorEvts    = 0;

  if (m_addCTPResult) m_trigResults.push_back(eformat::TDAQ_CTP);
  if (m_addL2Result)  m_trigResults.push_back(eformat::TDAQ_LVL2);
  if (m_addEFResult)  m_trigResults.push_back(eformat::TDAQ_EVENT_FILTER);
  if (m_addEFResult)  m_trigResults.push_back(eformat::TDAQ_HLT);

  return HLT::OK;
}


HLT::ErrorCode TrigCheckForTracks::hltBeginRun()
{
  return m_robSelector->setupMonitoring();
}


// ----------------------------------------------------------------------
HLT::ErrorCode TrigCheckForTracks::hltFinalize(){
// ----------------------------------------------------------------------

  ATH_MSG_INFO( "Events accepted/rejected/errors:  "<< m_acceptedEvts <<" / "<< m_rejectedEvts << " / "<< m_errorEvts  );
  return HLT::OK;
}


// ----------------------------------------------------------------------
HLT::ErrorCode TrigCheckForTracks::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& /*tes_in*/,  unsigned int output) {
// ----------------------------------------------------------------------

  if (m_executedEvent) {
    ATH_MSG_DEBUG( "*** Not Executing this TrigCheckForTracks " << name() << ", already executed"   );
    return HLT::OK;
  }

  ATH_MSG_DEBUG( "***  Executing this TrigCheckForTracks : " << name()  );


  //--> PEB Related Stuff

  PartialEventBuildingInfo* pebInfo = config()->getPEBI();
  if(!pebInfo){
    ATH_MSG_DEBUG( "*** Not Executing this TrigCheckForTracks " << name() << ", not a calib chain" );
    return HLT::OK;
  }
  //--< PEB Related Stuff

  beforeExecMonitors().ignore();


  // The code retrieves tracks from the StoreGate. Please note that 
  // this is not compatible with the caching model implemented by 
  // Tomasz and Rustem for the fex and combo algorithms. 
  // AllTE algorithms are not included in the caching model.
  // Retrieving objects from the StoreGate is not something that 
  // should be done for other algorithms than AllTE 
  // - so retrieving objects from the StoreGate is 
  // **not** allowed in fex or combo algorithms.
  // More on caching:
  // https://twiki.cern.ch/twiki/bin/view/Atlas/HltAlgorithmsCaching


  // Retrieve Tracks from StoreGate:

  const TrackCollection* tracks = 0;

  if (evtStore()->transientContains<TrackCollection>(m_tracksName)) {
    ATH_MSG_DEBUG( MSG::DEBUG << "*** TrackCollection with name "<< m_tracksName <<" found in StoreGate (transientContains)"  );
  }
  else {
    ATH_MSG_DEBUG( "*** No TrackCollection with name" << m_tracksName << " found in StoreGate (transientContains)"  );
    return HLT::OK;
  }
  
  StatusCode sc = evtStore()->retrieve(tracks,m_tracksName);
  
  ATH_MSG_DEBUG( "***** Status code: "<< sc << " for key: " << m_tracksName  );

  if (sc.isFailure()) {
    ATH_MSG_DEBUG( "No TrackCollection with name "<<m_tracksName<<" found in StoreGate"  );
    m_errorEvts++;
    m_rejectedEvts++;
  } else {
    ATH_MSG_DEBUG( "TrackCollection with name "<<m_tracksName<<" found in StoreGate"  );
    ATH_MSG_DEBUG( "Retrieved "<< tracks->size() <<" reconstructed tracks from StoreGate"  );

    // Have found tracks...
    // Now loop over the tracks to find the isolated ones.

    if (tracks->size()!=0) {    
      
      
      int count_IsoTracks = 0;

      for (TrackCollection::const_iterator it = tracks->begin(); it!=tracks->end(); ++it)
	{
	  
	  const Trk::Perigee *ip = (*it)->perigeeParameters();
	  if (!ip) continue;
	  
	  
	  m_pT.push_back(ip->pT());
	  m_eta.push_back(ip->eta());
	  m_phi.push_back(ip->parameters()[Trk::phi]);

	  
	  ATH_MSG_DEBUG( "pT " << ip->pT() << " eta " << ip->eta());

	  if (fabs(ip->pT()) < m_pT_min || fabs(ip->eta())>m_etaEdge || fabs(ip->eta())<m_etaLowEdge ) continue;
	  
	  bool gotIsoTrack = true;
	  
	  if (!m_lookForAnyTracks) 
	    {

	      for (TrackCollection::const_iterator jt = tracks->begin(); jt!=tracks->end(); ++jt)
		{


		  const Trk::Perigee *jp = (*jt)->perigeeParameters();
		  

		  double dphi = fabs(ip->parameters()[Trk::phi] - jp->parameters()[Trk::phi]);
		  if(dphi>M_PI) dphi = 2*M_PI-dphi;
		  
		  double dR = sqrt(pow((ip->eta() - jp->eta()),2) + pow(dphi,2));
		  m_dR.push_back(dR);
		  
		  if ((fabs(jp->pT()) > m_pT_min_iso) && (dR > m_dR0_overlap) && (dR < m_dR0 ) ) {
		    gotIsoTrack = false;	      
		    continue;
		  }
		}
	    }  

	    if (gotIsoTrack){
	    
	    count_IsoTracks += 1;

	    m_ROB_eta.push_back(ip->eta());
	    m_ROB_phi.push_back(ip->parameters()[Trk::phi]);
	    m_pT_Iso.push_back(ip->pT());


	    double etaMIN = ip->eta() - m_etaWidth;
	    double etaMAX = ip->eta() + m_etaWidth;
	    
	    double phiMIN = ip->parameters()[Trk::phi] - m_phiWidth;
	    double phiMAX = ip->parameters()[Trk::phi] + m_phiWidth;
	    while (phiMIN < 0)      phiMIN += 2*M_PI;
	    while (phiMAX > 2*M_PI) phiMAX -= 2*M_PI;
	    
	    TrigRoiDescriptor roi( ip->eta(), etaMIN, etaMAX, ip->parameters()[Trk::phi], phiMIN, phiMAX );

	    // now add ROBs
	    HLT::ErrorCode ec = m_robSelector->fillPEBInfo(*pebInfo, roi, &m_dets, &m_nROBs);
	    if (ec != HLT::OK) return ec;
	    
	    
	    // Add extra ROBs
	    for (std::vector<uint32_t>::const_iterator rob = m_extraROBs.begin(); rob != m_extraROBs.end(); rob++)
	      pebInfo->add(*rob);
	    
            if(m_trigResults.size()!=0)
                pebInfo->addSubDetector(m_trigResults);

	    if (!m_doNotPass){
	      //generate output here
	      //addRoI(output);
              HLT::TriggerElement* te = addRoI(output);
              te->setActiveState(true);
            }// else 
            //  te->setActiveState(false);

	  }
	}
    
      m_acceptedEvts++;
      m_n_IsoTracks = count_IsoTracks ;  
      ATH_MSG_DEBUG( "Found "<< m_n_IsoTracks <<" Isolated ("<< m_lookForAnyTracks  <<") Tracks"  );
    }
    else {
      m_rejectedEvts++;
    }
  }


  m_executedEvent = true;
  

  afterExecMonitors().ignore();

  return HLT::OK;
}

HLT::ErrorCode TrigCheckForTracks::hltEndEvent() {
    m_executedEvent = false; 
    m_robSelector->reset();
    m_dets.clear();
    m_nROBs.clear();
    m_pT.clear();
    m_pT_Iso.clear();
    m_dR.clear();
    m_eta.clear();
    m_phi.clear();
    m_ROB_eta.clear();
    m_ROB_phi.clear();
    return HLT::OK;
  }

