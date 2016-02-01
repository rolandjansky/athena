// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/TrigL2ElectronHypoMoni.cxx
 **
 **   Description: Hypothesis algorithms to be run at Level 2 after 
 **                tracking algorithms: creates TrigElectrons
 **
 **   Authour: R.Goncalo <r.goncalo@rhul.ac.uk> Thu Jan 20 22:02:43 BST 2005
 **
 **   Created:      Sat Mar  1 19:55:56 GMT 2005
 **   Modified:     RG 18 Mar 06 - fix to always generate TrigElectrons
 **                              - use key2keyStore for collection names
 **                 James Keates 17 Oct - Added monitoring histograms.
 **                 Valeria perez reale 29 Jan - migration to new steering
 **                
 **************************************************************************/ 

//#include "TrigInDetToolInterfaces/ITrigInDetTrackExtrapolator.h"
//#include "RecoToolInterfaces/IExtrapolateToCaloTool.h"
#include "TrigEgammaHypo/TrigL2ElectronHypoMoni.h"

class ISvcLocator;

TrigL2ElectronHypoMoni::TrigL2ElectronHypoMoni(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator)//, m_trackExtrapolator("ExtrapolateToCaloTool")
{

  // Read cuts - should probably get these from an xml file
  declareProperty("AcceptAll",            m_acceptAll=true);
  declareProperty("EtaBins",              m_etabin);
  declareProperty("TrackPt",              m_trackPtthr=5.0*CLHEP::GeV);
  declareProperty("CaloTrackdETA",        m_calotrackdeta); //loose cut
  declareProperty("CaloTrackdPHI",        m_calotrackdphi); //loose cut
  declareProperty("CaloTrackdEoverPLow",  m_calotrackdeoverp_low);
  declareProperty("CaloTrackdEoverPHigh", m_calotrackdeoverp_high);
  declareProperty("TrackAlgoId",          m_trackalgoID=0);
  declareProperty("dETACLUSTERthr",       m_detacluster=0.1);
  declareProperty("dPHICLUSTERthr",       m_dphicluster=0.1);
  declareProperty("RCalBarrelFace",       m_RCAL=1470.0*CLHEP::mm);
  declareProperty("ZCalEndcapFace",       m_ZCAL=3800.0*CLHEP::mm);
//  declareProperty( "TrackExtrapolator",    m_trackExtrapolator);
}

TrigL2ElectronHypoMoni::~TrigL2ElectronHypoMoni()
{ }

HLT::ErrorCode TrigL2ElectronHypoMoni::hltInitialize()
{
   if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "Initialization:" << endreq;
  }

/*
  StatusCode sc = toolSvc()->retrieveTool(m_trackExtrapolatorName,
					  m_trackExtrapolatorName,
					  m_trackExtrapolator);
  StatusCode sc = m_trackExtrapolator.retrieve();
  if ( sc.isFailure() ) {
    msg() << MSG::FATAL << "Unable to locate TrackExtrapolator tool " << endreq;
    return HLT::BAD_JOB_SETUP;
  }
*/

  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "Initialization completed successfully:" << endreq;
  }
  
  //Declare histograms
  
  m_CaloTrackdETAHist = new TH1F("CaloTrackdETAMon", "Calo Track dETA", 50, 0, 0.4);
  m_CaloTrackdETAHist->GetXaxis()->SetTitle("Calo Track dETA");
  m_CaloTrackdPHIHist = new TH1F("CaloTrackdPHIMon", "Calo Track dPHI", 50, 0, 0.2);
  m_CaloTrackdPHIHist->GetXaxis()->SetTitle("Calo Track dPHI");
  m_CaloTrackdEoverPHist = new TH1F("CaloTrackdEoverPMon", "Calo Track dE over P", 50, 0, 2);
  m_CaloTrackdEoverPHist->GetXaxis()->SetTitle("Calo Track dE over P");
  m_trackPtHist = new TH1F("trackPtMon", "Track Pt", 50, 0,40 );
  m_trackPtHist->GetXaxis()->SetTitle("Track Pt [GeV]");
  m_trackAlgoIDHist = new TH1F("trackAlgoIDMon", "Track Algo ID", 5, 0, 5 );
  m_trackAlgoIDHist->GetXaxis()->SetTitle("Track Algo ID");
  m_dETACLUSTERHist = new TH1F("dETACLUSTERMon", "dEta Cluster", 50, 0, 0.1);
  m_dETACLUSTERHist->GetXaxis()->SetTitle("dEta Cluster");
  m_dPHICLUSTERHist = new TH1F("dPHICLUSTERMon", "dPhi Cluster", 50, 0, 0.1);
  m_dPHICLUSTERHist->GetXaxis()->SetTitle("dPhi Cluster");
  
  

  ITHistSvc *rootHistSvc;
  if (!service("THistSvc", rootHistSvc, true).isSuccess()) {
    msg() << MSG::ERROR << "Unable to locate THistSvc" << endreq;
    return HLT::BAD_JOB_SETUP;
  }
  //  book
  std::string path = "/EXMON/lv2/L2ElectronHypo/technical/";
  if ( rootHistSvc->regHist(path+m_trackPtHist->GetName(),m_trackPtHist).isFailure())
    msg() << MSG::WARNING << "Can't book" << path+m_trackPtHist->GetName() << endreq;
  if ( rootHistSvc->regHist(path+m_trackAlgoIDHist->GetName(),m_trackAlgoIDHist).isFailure())
    msg() << MSG::WARNING << "Can't book" << path+m_trackAlgoIDHist->GetName() << endreq;
  if ( rootHistSvc->regHist(path+m_CaloTrackdETAHist->GetName(),m_CaloTrackdETAHist).isFailure())
    msg() << MSG::WARNING << "Can't book" << path+m_CaloTrackdETAHist->GetName() << endreq;
  if ( rootHistSvc->regHist(path+m_CaloTrackdPHIHist->GetName(),m_CaloTrackdPHIHist).isFailure())
    msg() << MSG::WARNING << "Can't book" << path+m_CaloTrackdPHIHist->GetName() << endreq;
  if ( rootHistSvc->regHist(path+m_CaloTrackdEoverPHist->GetName(),m_CaloTrackdEoverPHist).isFailure())
    msg() << MSG::WARNING << "Can't book" << path+m_CaloTrackdEoverPHist->GetName() << endreq;
  if ( rootHistSvc->regHist(path+m_dETACLUSTERHist->GetName(),m_dETACLUSTERHist).isFailure())
    msg() << MSG::WARNING << "Can't book" << path+m_dETACLUSTERHist->GetName() << endreq;
  if ( rootHistSvc->regHist(path+m_dPHICLUSTERHist->GetName(),m_dPHICLUSTERHist).isFailure())
    msg() << MSG::WARNING << "Can't book" << path+m_dPHICLUSTERHist->GetName() << endreq;
  
  msg() << MSG::DEBUG << "Histograms are booked" << endreq;

  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Initialization completed successfully:" << endreq;
    // default=True (for now...May06)
    msg() << MSG::DEBUG << "AcceptAll            = " 
	<< (m_acceptAll==true ? "True" : "False") << endreq; 
    msg() << MSG::DEBUG << "TrackPt              = " << m_trackPtthr           << endreq; 
    msg() << MSG::DEBUG << "EtaBins              = " << m_etabin               << endreq; 
    msg() << MSG::DEBUG << "CaloTrackdETA        = " << m_calotrackdeta        << endreq; 
    msg() << MSG::DEBUG << "CaloTrackdPHI        = " << m_calotrackdphi        << endreq; 
    msg() << MSG::DEBUG << "CaloTrackdEoverPLow  = " << m_calotrackdeoverp_low << endreq; 
    msg() << MSG::DEBUG << "CaloTrackdEoverPHigh = " << m_calotrackdeoverp_high<< endreq; 
    msg() << MSG::DEBUG << "TrackAlgoId          = " << m_trackalgoID          << endreq; 
    msg() << MSG::DEBUG << "dETACLUSTERthr       = " << m_detacluster          << endreq; 
    msg() << MSG::DEBUG << "dPHICLUSTERthr       = " << m_dphicluster          << endreq; 

  }
  
  return HLT::OK;
}



HLT::ErrorCode TrigL2ElectronHypoMoni::hltFinalize()
{
  if ( msgLvl() <= MSG::INFO ){
    msg() << MSG::INFO << "in finalize()" << endreq;

  }

  return HLT::OK;
}

HLT::ErrorCode TrigL2ElectronHypoMoni::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{
  
  // Accept-All mode: temporary patch; should be done with force-accept 
    if (m_acceptAll) {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "AcceptAll property is set: taking all events" 
	  << endreq;

    pass = true;
    return HLT::OK;
  } 
  else {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "AcceptAll property not set: applying selection" 
	  << endreq;
  }

  // get RoI descriptor
  const TrigRoiDescriptor* roiDescriptor = 0;
  if (getFeature(outputTE, roiDescriptor) != HLT::OK) roiDescriptor = 0;

  if ( !roiDescriptor ) {
    if ( msgLvl() <= MSG::WARNING) {
      msg() <<  MSG::WARNING << "No RoI for this Trigger Element! " << endreq;
    }    
    
    return HLT::NAV_ERROR;
  }

  if ( msgLvl() <= MSG::DEBUG ){
    msg() << MSG::DEBUG 
	<< "Using outputTE("<< outputTE <<")->getId(): " << outputTE->getId()
	<< "; RoI ID = "   << roiDescriptor->roiId()
	<< ": Eta = "      << roiDescriptor->eta()
	<< ", Phi = "      << roiDescriptor->phi()
	<< endreq;
  }

  // RoI reference position
  double etaRef = roiDescriptor->eta();
  double phiRef = roiDescriptor->phi();
  
   const xAOD::TrigElectronContainer* trigElecColl = 0;
  HLT::ErrorCode stat = getFeature(outputTE, trigElecColl, "L2ElectronFex");

  if ( stat != HLT::OK ) {
    if ( msgLvl() <= MSG::WARNING) {
      msg() << MSG::WARNING << "Failed to get TrigElectron collection" << endreq;
    }

    return HLT::OK;
  }

  if ( msgLvl() <= MSG::DEBUG ) {
    msg() << MSG::DEBUG << "Got collection with " << trigElecColl->size() 
	<< " TrigElectrons" << endreq;
  }

  // if no electrons were found, just leave TrigElectronColl. empty and leave
  if ( trigElecColl->size() == 0 ) {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "No electrons to analyse, leaving!" << endreq;
 
    return HLT::OK;
  }

  // Now loop over electrons, see if at least one passes all cuts

  for (xAOD::TrigElectronContainer::const_iterator elecIter = trigElecColl->begin();
       elecIter !=  trigElecColl->end(); ++elecIter) {    
    
    // Retrieve all quantities
    float dPhi =  (*elecIter)->phi() - phiRef;
    dPhi = ( fabs(dPhi) < M_PI ? dPhi : 2*M_PI - dPhi );
    
    float dEta = fabs( etaRef - (*elecIter)->eta() );

    float dPhiCalo  = (*elecIter)->trkClusDphi();
    float dEtaCalo  = (*elecIter)->trkClusDeta();
    float absEta    = fabs( (*elecIter)->eta() );
    float eT_T2Calo = (*elecIter)->emCluster()->et();	  
    float trkPt     = fabs( (*elecIter)->pt() );
    float eTOverPt  = fabs( eT_T2Calo/trkPt );

    // figure out what eta bin this cluster is in
    int etaBin = 0;
    for (int iBin = 0; iBin < 4; iBin++ ) 
      if ( absEta > m_etabin[iBin] && absEta < m_etabin[iBin+1] ) etaBin = iBin; 
    
    if ( msgLvl() <= MSG::DEBUG ) {
      msg() << MSG::DEBUG << "absEta    = " << absEta << " ==> etaBin = " << etaBin << endreq;
      msg() << MSG::DEBUG << "eT_T2Calo = " << eT_T2Calo << endreq;
      msg() << MSG::DEBUG << "dEta      = " << dEta     << ", cut = " << m_dphicluster << endreq;
      msg() << MSG::DEBUG << "dPhi      = " << dPhi     << ", cut = " << m_dphicluster << endreq;
      msg() << MSG::DEBUG << "trkPt     = " << trkPt    << ", cut = " << m_dphicluster << endreq;
      msg() << MSG::DEBUG << "dEtaCalo  = " << dEtaCalo << ", cut = " << m_calotrackdeta[etaBin] << endreq;
      msg() << MSG::DEBUG << "dPhiCalo  = " << dPhiCalo << ", cut = " << m_calotrackdphi[etaBin] << endreq;
      msg() << MSG::DEBUG << "eTOverPt  = " << eTOverPt << ", cuts = [" 
	  <<  m_calotrackdeoverp_low[etaBin] << " , " << m_calotrackdeoverp_high[etaBin] << " ]" << endreq;
    }
    
    m_dETACLUSTERHist->Fill(dEta);
    m_dPHICLUSTERHist->Fill(dPhi);
    m_trackPtHist->Fill(trkPt*1.e-3);
    m_CaloTrackdETAHist->Fill(dEtaCalo);
    m_CaloTrackdPHIHist->Fill(dPhiCalo);
    m_CaloTrackdEoverPHist->Fill(fabs(eTOverPt));
   

    if ( dEta < m_detacluster && dPhi < m_dphicluster && trkPt > m_trackPtthr &&
	 dEtaCalo < m_calotrackdeta[etaBin] && dPhiCalo < m_calotrackdphi[etaBin] &&
	 eTOverPt >  m_calotrackdeoverp_low[etaBin] && 
	 eTOverPt < m_calotrackdeoverp_high[etaBin] ) {
      
      if ( msgLvl() <= MSG::DEBUG ) {
	msg() << MSG::DEBUG << "Event accepted !" << endreq;	      
      }
      
      pass = true;
    }
  }

 

      
  
  return HLT::OK;
}
