/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigCheckForTracks.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigDetCalib
//
// ********************************************************************

#include "TrigDetCalib/TrigCheckForTracks.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

//#include "CLHEP/Units/SystemOfUnits.h"

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/MsgStream.h"

//#include "TrkTrack/TrackCollection.h"
#include "TrigInDetEvent/TrigTauTracksInfo.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrack.h"

#include "TrigSteeringEvent/PartialEventBuildingInfo.h"
#include "IRegionSelector/IRegSelSvc.h"


#include "StoreGate/StoreGateSvc.h"

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
  declareProperty("tracksName",   tracksName = "HLT_TrigL2SiTrackFinder_Tau");  
  declareProperty("tracksAlgoId",   tracksAlgoId = 6, "AlgoId as in TrigInDetTrack.h/5==strategyB" );  
  declareProperty("doNotPass",           doNotPass = false);        // pass through flag for initial beam items.
  declareProperty("lookForAnyTracks",    lookForAnyTracks = false); // any vs isolated. any is needed for initial beam items.
  declareProperty("AddCTPResult", m_addCTPResult = false, "Add the CTP result to the list of ROBs");
  declareProperty("AddL2Result",  m_addL2Result = false,  "Add the L2 result to the list of ROBs");
  declareProperty("AddEFResult",  m_addEFResult = false,  "Add the EF result to the list of ROBs");

  declareMonitoredStdContainer("nROBs",      m_nROBs);
  declareMonitoredStdContainer("detectors",  m_dets);

  declareMonitoredStdContainer("PtTrk",          m_pT);
  declareMonitoredStdContainer("PtTrk_ISO",      m_pT_Iso);
  declareMonitoredStdContainer("dR_tracks",      m_dR);
  declareMonitoredStdContainer("eta_tracks",     m_eta);
  declareMonitoredStdContainer("phi_tracks",     m_phi);
  declareMonitoredStdContainer("eta_tracks_ISO", m_ROB_eta);
  declareMonitoredStdContainer("phi_tracks_ISO", m_ROB_phi);
  declareMonitoredVariable("iso_Tracks",         n_IsoTracks);

}


TrigCheckForTracks::~TrigCheckForTracks()
{ }


// ----------------------------------------------------------------------
HLT::ErrorCode TrigCheckForTracks::hltInitialize()
// ----------------------------------------------------------------------
{
  msg() << MSG::INFO << "in initialize()" << endreq;

  // Initialize timing service
  //------------------------------
  if( service( "TrigTimerSvc", m_timerSvc).isFailure() ) {
    msg() << MSG::WARNING << name()
	<< ": Unable to locate TrigTimer Service" << endreq;
  }
  if (m_timerSvc)    
    m_timers.push_back(m_timerSvc->addItem("TrigCheckForTracks.TrigCheckForTracksTot"));

  m_acceptedEvts = 0;
  m_rejectedEvts = 0;
  m_errorEvts    = 0;

  StatusCode sc = service( "StoreGateSvc", m_storeGate);
  if( sc.isFailure() ){
    msg() << MSG::ERROR
	<< "Unable to retrieve pointer to StoreGateSvc"
	<< endreq;
    return HLT::BAD_ALGO_CONFIG;
  }
  else { msg() << MSG::DEBUG
	<< "Retrieved pointer to StoreGateSvc"
	       << endreq; }
  

  if (m_addCTPResult) m_trigResults.push_back(eformat::TDAQ_CTP);
  if (m_addL2Result)  m_trigResults.push_back(eformat::TDAQ_LVL2);
  if (m_addEFResult)  m_trigResults.push_back(eformat::TDAQ_EVENT_FILTER);

  return HLT::OK;
}


HLT::ErrorCode TrigCheckForTracks::hltBeginRun()
{
  msg() << MSG::INFO << "in hltBeginRun() " << endreq;
  return m_robSelector->setupMonitoring();
}


// ----------------------------------------------------------------------
HLT::ErrorCode TrigCheckForTracks::hltFinalize(){
// ----------------------------------------------------------------------

  msg() << MSG::INFO << "in finalize()" << endreq;
  msg() << MSG::INFO << "Events accepted/rejected/errors:  "<< m_acceptedEvts <<" / "<< m_rejectedEvts << " / "<< m_errorEvts << endreq;
  return HLT::OK;
}


// ----------------------------------------------------------------------
HLT::ErrorCode TrigCheckForTracks::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& /*tes_in*/,  unsigned int output) {
// ----------------------------------------------------------------------

  if (m_executedEvent) {
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "*** Not Executing this TrigCheckForTracks " << name() << ", already executed"  << endreq;
    }

    return HLT::OK;
  }

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "***  Executing this TrigCheckForTracks : " << name() << endreq;
  }


  //--> PEB Related Stuff

  PartialEventBuildingInfo* pebInfo = config()->getPEBI();
  if(!pebInfo){
    if (msgLvl() <= MSG::DEBUG) msg() << "*** Not Executing this TrigCheckForTracks " << name() << ", not a calib chain" << endreq;
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

  const TrigInDetTrackCollection* tracks = 0;

  if (m_storeGate->transientContains<TrigInDetTrackCollection>(tracksName)) {
    if (msgLvl() <= MSG::DEBUG ) {
      msg()  << MSG::DEBUG << "*** TrackCollection with name "<< tracksName <<" found in StoreGate (transientContains)" << endreq;
    }
  }
  else {
    if (msgLvl() <= MSG::DEBUG ) {
      msg()  << MSG::DEBUG << "*** No TrackCollection with name" << tracksName << " found in StoreGate (transientContains)" << endreq;
    }
    return HLT::OK;
  }
  
  StatusCode sc = m_storeGate->retrieve(tracks,tracksName);
  
  msg()  << MSG::DEBUG << "***** Status code: "<< sc << " for key: " << tracksName << endreq;

  if (sc.isFailure()) {
    if (msgLvl() <= MSG::DEBUG ) {
      msg()  << MSG::DEBUG << "No TrackCollection with name "<<tracksName<<" found in StoreGate" << endreq;
    }
    m_errorEvts++;
    m_rejectedEvts++;
  } else {
    if (msgLvl() <= MSG::DEBUG ) {
      msg()  << MSG::DEBUG << "TrackCollection with name "<<tracksName<<" found in StoreGate" << endreq;
      msg()  << MSG::DEBUG << "Retrieved "<< tracks->size() <<" reconstructed tracks from StoreGate" << endreq;
    }
    

    // Have found tracks...
    // Now loop over the tracks to find the isolated ones.

    if (tracks->size()!=0) {    
      
      
      int count_IsoTracks = 0;

      for (TrigInDetTrackCollection::const_iterator it = tracks->begin(); it!=tracks->end(); ++it)
	{

	  if ((*it)->algorithmId()!=tracksAlgoId) continue;

	  //dump tracks
	  const TrigInDetTrackFitPar* ip = (*it)->param();
	  
	  //	  msg() << MSG::INFO << (**it) << endreq;

	  m_pT.push_back(ip->pT());
	  m_eta.push_back(ip->eta());
	  m_phi.push_back(ip->phi0());

	  //	  msg() << MSG::INFO << "pT " << ip->pT() << " eta " << ip->eta() << endreq;	  

	  if (fabs(ip->pT()) < m_pT_min || fabs(ip->eta())>m_etaEdge || fabs(ip->eta())<m_etaLowEdge ) continue;
	  
	  bool gotIsoTrack = true;
	  
	  if (!lookForAnyTracks) 
	    {

	      for (TrigInDetTrackCollection::const_iterator jt = tracks->begin(); jt!=tracks->end(); ++jt)
		{

		  if ((*jt)->algorithmId()!=tracksAlgoId) continue;

		  const TrigInDetTrackFitPar* jp = (*jt)->param();
		  
		  //	      msg() << MSG::INFO << "pT " << jp->pT() << " eta " << jp->eta() << endreq;
		  

		  double dphi = fabs(ip->phi0() - jp->phi0());
		  if(dphi>M_PI) dphi = 2*M_PI-dphi;
		  
		  double dR = sqrt(pow((ip->eta() - jp->eta()),2) + pow(dphi,2));
		  m_dR.push_back(dR);
		  
		  //	      msg() << MSG::INFO << "DR Tracks " << dR << endreq; 
		  
		  if ((fabs(jp->pT()) > m_pT_min_iso) && (dR > m_dR0_overlap) && (dR < m_dR0 ) ) {
		    gotIsoTrack = false;	      
		    continue;
		  }
		}
	    }  

	    if (gotIsoTrack){
	    
	    count_IsoTracks += 1;

	    m_ROB_eta.push_back(ip->eta());
	    m_ROB_phi.push_back(ip->phi0());
	    m_pT_Iso.push_back(ip->pT());

//	    double etaMIN = std::max(-m_etaEdge, ip->eta() - m_etaWidth);
//	    double etaMAX = std::min(m_etaEdge,  ip->eta() + m_etaWidth);

	    double etaMIN = ip->eta() - m_etaWidth;
	    double etaMAX = ip->eta() + m_etaWidth;
	    
	    double phiMIN = ip->phi0() - m_phiWidth;
	    double phiMAX = ip->phi0() + m_phiWidth;
	    while (phiMIN < 0)      phiMIN += 2*M_PI;
	    while (phiMAX > 2*M_PI) phiMAX -= 2*M_PI;
	    
	    TrigRoiDescriptor _roi( ip->eta(), etaMIN, etaMAX, ip->phi0(), phiMIN, phiMAX );

	    // now add ROBs
	    HLT::ErrorCode ec = m_robSelector->fillPEBInfo(*pebInfo, _roi, &m_dets, &m_nROBs);
	    if (ec != HLT::OK) return ec;
	    
	    
	    // Add extra ROBs
	    for (std::vector<uint32_t>::const_iterator rob = m_extraROBs.begin(); rob != m_extraROBs.end(); rob++)
	      pebInfo->add(*rob);
	    
            if(m_trigResults.size()!=0)
                pebInfo->addSubDetector(m_trigResults);

	    if (!doNotPass){
	      //generate output here
	      //addRoI(output);
              HLT::TriggerElement* te = addRoI(output);
              te->setActiveState(true);
            }// else 
            //  te->setActiveState(false);

	  }
	}
    
      m_acceptedEvts++;
      n_IsoTracks = count_IsoTracks ;  
      msg() << MSG::DEBUG << "Found "<< n_IsoTracks <<" Isolated ("<< lookForAnyTracks  <<") Tracks" << endreq;
    }
    else {
      m_rejectedEvts++;
    }
  }


  m_executedEvent = true;
  

  afterExecMonitors().ignore();

  return HLT::OK;
}

bool TrigCheckForTracks::reset() {
  if ( !AllTEAlgo::reset() ) return false;
    m_executedEvent = false; 
    AllTEAlgo::reset();   
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
    return true;
  }

