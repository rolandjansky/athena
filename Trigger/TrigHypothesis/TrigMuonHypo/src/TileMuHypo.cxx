/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include <vector>
#include <math.h>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "CLHEP/Units/SystemOfUnits.h"

// --> 
#include "TrigMuonEvent/TileMuFeature.h"
#include "TrigMuonEvent/TileMuFeatureContainer.h"
#include "TrigMuonEvent/TileTrackMuFeature.h"
#include "TrigMuonEvent/TileTrackMuFeatureContainer.h"
#include "TrigMuonHypo/TileMuHypo.h"

//#include "TrigSteeringEvent/TrigRoiDescriptor.h"  // For RoI Check
// <--

class ISvcLocator;

TileMuHypo::TileMuHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator){
	declareProperty("AcceptAll", m_acceptAll=true);
	//declareProperty("UseIDScan", m_UseIDScan=false);
        declareProperty("IOptIDScan", m_IOptIDScan = 1);
        //declareProperty("DelPhi_Cut", m_DelPhi_Cut = 0.1);
        //declareProperty("DelEta_Cut", m_DelEta_Cut = 0.1);
        declareProperty("Pt_Cut",     m_Pt_Cut = 2000.0);	// MeV
 
	// --> For Histogram <--
	declareMonitoredVariable("Eta", m_hypoEta = -9999.9);
        declareMonitoredVariable("Phi", m_hypoPhi = -9999.9);
        declareMonitoredVariable("EffwTrk", m_hypoEffwTrk = -9999.9);
        declareMonitoredVariable("EtaTrk",  m_hypoEtaTrk = -9999.9); 
        declareMonitoredVariable("PhiTrk",  m_hypoPhiTrk = -9999.9);  
	declareMonitoredVariable("PtTrk",  m_hypoPtTrk = -9999.9);
        declareMonitoredVariable("absPtTrk", m_hypo_absPtTrk = -9999.9);
	declareMonitoredVariable("DelEtaTrk", m_hypoDelEtaTrk = -9999.9);
        declareMonitoredVariable("DelEtaTrkTR", m_hypoDelEtaTrkTR = -9999.9);
        declareMonitoredVariable("DelPhiTrk", m_hypoDelPhiTrk = -9999.9);
	declareMonitoredVariable("DelPhiTrkTR", m_hypoDelPhiTrkTR = -9999.9);
        declareMonitoredVariable("absPtTrkPre", m_hypo_absPtTrk_Pre = -9999.9);

	m_storeGate = 0;
}

TileMuHypo::~TileMuHypo(){
}

HLT::ErrorCode TileMuHypo::hltInitialize(){

  if(m_acceptAll) {
      msg() << MSG::INFO
            << "Accepting all the events with not cut!"
	    << endreq;
  } else {
      // initialize the selection here
  }
 
  msg() << MSG::INFO 
        << "Initialization completed successfully!" 
        << " Pt_Cut=" << m_Pt_Cut << endreq;
  
  return HLT::OK;
}


HLT::ErrorCode TileMuHypo::hltFinalize(){
  msg() << MSG::INFO << "in finalize()" << endreq;
  return HLT::OK;
}

HLT::ErrorCode TileMuHypo::hltExecute(const HLT::TriggerElement* outputTE, 
                                      bool& pass){

  m_storeGate = store();

  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "in execute()" << endreq;

  if(m_acceptAll) {
      pass = true;
      if(msgLvl() <= MSG::DEBUG) {
          msg() << MSG::DEBUG
                << "Accept property is set: taking all the events"
                << endreq;
      }
      return HLT::OK;
  }

  if(msgLvl() <= MSG::DEBUG) {
       msg() << MSG::DEBUG << "Accept property not set: applying selection!"
             << endreq;
  }

  bool result = false;
  //bool OK_Match = false;

  float here_hypoEta = -9999.9;
  float here_hypoPhi = -9999.9;
  float here_hypoEffwTrk = -9999.9;
  float here_hypoEtaTrk = -9999.9;
  float here_hypoPhiTrk = -9999.9;
  float here_hypoPtTrk = -9999.9;
  float here_hypo_absPtTrk = -9999.9;
  float here_hypoDelEtaTrk = -9999.9;
  float here_hypoDelEtaTrkTR = -9999.9;
  float here_hypoDelPhiTrk = -9999.9;
  float here_hypoDelPhiTrkTR = -9999.9;
  float here_hypo_absPtTrk_Pre = -9999.9;

  // ----------------------------------------------------------------
  // Determination of trigger decision 
  // ----------------------------------------------------------------

  if ( m_IOptIDScan == 0) {	  // muons from only TileCal
    ElementLink<TileMuFeatureContainer> TileMuFeatureEL;
    if ( HLT::OK != getFeatureLink<TileMuFeatureContainer, TileMuFeature>
                                (outputTE, TileMuFeatureEL) )
    {
      msg() << MSG::ERROR << " getFeatureLink fails to get TileMuFeature "
                          << endreq;
      return HLT::ERROR;                                                    
    }
    if ( !TileMuFeatureEL.isValid() )
    {                               
      msg() << MSG::ERROR            
            <<  " getFeatureLink finds no TileMuFeature (EL invalid)"
            << endreq;
      return HLT::NAV_ERROR;
    }
   
    const TileMuFeature* muon = *TileMuFeatureEL;
    if (!muon) {
      msg() << MSG::ERROR << "Retrieval of TileMuFeature from vector failed"                << endreq;
      return HLT::NAV_ERROR;
    }
  
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << " REGTEST Eta=" << muon->eta()
            << " Phi=" << muon->phi() << endreq;
    } 
    here_hypoEta = muon->eta();
    here_hypoPhi = muon->phi();
    here_hypoEffwTrk = 0.0;

    result    = true;

  } else if (m_IOptIDScan >= 1 && m_IOptIDScan <= 4) { 
				  // muos from TileCal + InnerDetector
				  // from "TileTrackMuFeature" from Fex

    ElementLink<TileTrackMuFeatureContainer> TileTrackMuFeatureEL;
    if ( HLT::OK != getFeatureLink<TileTrackMuFeatureContainer,
                    TileTrackMuFeature>(outputTE, TileTrackMuFeatureEL) )        
    {
      msg() << MSG::ERROR
            << " getFeatureLink fails to get the TileTrackMuFeature " <<endreq;
      return HLT::ERROR;
    }            
  
    if ( !TileTrackMuFeatureEL.isValid() )
    {
      msg() << MSG::ERROR
            << " getFeatureLink finds no TileTrackMuFeature (EL invalid)"
            << endreq;
      return HLT::NAV_ERROR;
    }
  
    const TileTrackMuFeature*  TileTrackMu = *TileTrackMuFeatureEL;
    if (!TileTrackMu) {
      msg() << MSG::ERROR <<"Retrieval of TileTrackMuFeature from vector failed"
            << endreq;
      return HLT::NAV_ERROR;
    } 
   
    //get the pointer to the TileMuFeature 
    ElementLink<TileMuFeatureContainer> TileMuFeatureEL = 
					TileTrackMu->TileMuLink();
    const TileMuFeature* TileMu;

    if ( !TileMuFeatureEL.isValid() )
    { 
      msg() << MSG::DEBUG
            << " TileTrackMuFeature has no valid TileMuFeature (EL invalid)"
            << endreq;
      TileMu = 0; 
    } else {
      TileMu = *TileMuFeatureEL; 
    }

    //get the pointer to the ID track
    ElementLink<TrigInDetTrackCollection> IDScanEL = TileTrackMu->IDScanLink();
    const TrigInDetTrack* Track;

    if ( !IDScanEL.isValid() )
    {
      msg() << MSG::DEBUG 
            << " TileTrackMuFeature has no valid IDtracks (EL invalid)"
            << endreq;
      Track = 0;
    } else {
      Track = *IDScanEL;
    }

    if (TileMu != 0) {					// Have TileMuFeature
      here_hypoEta      = TileMu->eta();
      here_hypoPhi      = TileMu->phi();
      here_hypoEffwTrk  = 0.0;
    } 
      
    if (TileMu != 0 && Track != 0) {			
      if ( TileTrackMu->EtaTR_Trk() != -9999.9 &&
           TileTrackMu->PhiTR_Trk() != -9999.9 )	// Matched Track!
      {
        here_hypoEffwTrk = 1.0;         
 
        float DelEta = TileMu->eta() - TileTrackMu->EtaTR_Trk();
    
        const float User_Pi = 3.141592654;  
        float DelPhi = TileMu->phi() - Track->param()->phi0();
        if (DelPhi >      User_Pi) DelPhi -= 2*User_Pi;
        if (DelPhi < (-1)*User_Pi) DelPhi += 2*User_Pi;

        float DelPhiTR = TileMu->phi() - TileTrackMu->PhiTR_Trk();
        if (DelPhiTR >      User_Pi) DelPhiTR -= 2*User_Pi;
        if (DelPhiTR < (-1)*User_Pi) DelPhiTR += 2*User_Pi;

        //if ( fabs(DelEta) < m_DelEta_Cut && fabs(DelPhiTR) < m_DelPhi_Cut )
        //  OK_Match = true;
	if (std::abs( Track->param()->pT()) > m_Pt_Cut ) result = true; 
        //if ( OK_Match && fabs( Track->param()->pT()) > m_Pt_Cut ) result = true;

        // --- For Mornitoring ---

        const float MeV2GeV=1./1000.;
	here_hypo_absPtTrk_Pre = fabs( Track->param()->pT()) * MeV2GeV;

	/*
        if (OK_Match) {
          m_hypo_absPtTrk_Pre = fabs( Track->param()->pT()) * MeV2GeV;
        } else {
          m_hypo_absPtTrk_Pre = -9999.9;
        } 
        */

        if ( result ) {
          here_hypoEtaTrk = TileMu->eta();
          here_hypoPhiTrk = TileMu->phi();
          here_hypo_absPtTrk = fabs( Track->param()->pT()) * MeV2GeV; 
          here_hypoPtTrk = Track->param()->pT() * MeV2GeV;
          here_hypoDelEtaTrk = TileMu->eta() - Track->param()->eta();
          here_hypoDelEtaTrkTR = DelEta;
          here_hypoDelPhiTrk = DelPhi;
          here_hypoDelPhiTrkTR = DelPhiTR; 
 
          /*
          m_hypoEtaTrk = TileMu->eta();
          m_hypoPhiTrk = TileMu->phi(); 
          m_hypo_absPtTrk = fabs( Track->param()->pT()) * MeV2GeV; 
          m_hypoPtTrk = Track->param()->pT() * MeV2GeV;
          m_hypoDelEtaTrk = TileMu->eta() - Track->param()->eta();
          m_hypoDelEtaTrkTR = DelEta;
          m_hypoDelPhiTrk = DelPhi;
          m_hypoDelPhiTrkTR = DelPhiTR; 
          */

          msg() << MSG::DEBUG << " REGTEST Tile::Eta=" << TileMu->eta()
                << " Phi="        << TileMu->phi()
                << " Track::Eta=" << Track->param()->eta()
                << " Phi="        << Track->param()->phi0() 
                << " PhiTR="      << TileTrackMu->PhiTR_Trk() 
                << " Pt="         << Track->param()->pT() 
                << " Typ_IDTrk="  << TileTrackMu->Typ_IDTrk() << endreq;
          msg() << MSG::DEBUG << " REGTEST Pt_Cut=" << m_Pt_Cut << endreq;
  
        } else {

          msg() << MSG::DEBUG << " REGTEST !Failed due to PtCut! Tile::Eta=" 
	        << TileMu->eta()
                << " Phi="        << TileMu->phi()
                << " Track::Eta=" << Track->param()->eta()
                << " Phi="        << Track->param()->phi0()
                << " PhiTR="      << TileTrackMu->PhiTR_Trk()
                << " Pt="         << Track->param()->pT()
                << " Typ_IDTrk="  << TileTrackMu->Typ_IDTrk() << endreq;
        }
      } else { // No Matched Track

        msg() << MSG::DEBUG << " REGTEST No-matched Track! Tile::Eta=" 
                << TileMu->eta()
                << " Phi="        << TileMu->phi()
		<< " Track::Eta=" << Track->param()->eta()
                << " Phi="        << Track->param()->phi0()
		<< endreq;
      }
    }
  }
  // -----------------------------------------
 
  //store the result
  m_hypoEta	= here_hypoEta;
  m_hypoPhi	= here_hypoPhi;
  m_hypoEffwTrk	= here_hypoEffwTrk;
  m_hypoEtaTrk  = here_hypoEtaTrk;
  m_hypoPhiTrk  = here_hypoPhiTrk;
  m_hypoPtTrk   = here_hypoPtTrk;
  m_hypo_absPtTrk	= here_hypo_absPtTrk;
  m_hypoDelEtaTrk	= here_hypoDelEtaTrk;	
  m_hypoDelEtaTrkTR	= here_hypoDelEtaTrkTR;
  m_hypoDelPhiTrk	= here_hypoDelPhiTrk;
  m_hypoDelPhiTrkTR	= here_hypoDelPhiTrkTR;
  m_hypo_absPtTrk_Pre	= here_hypo_absPtTrk_Pre;;

  pass = result;
  
  return HLT::OK;
}
