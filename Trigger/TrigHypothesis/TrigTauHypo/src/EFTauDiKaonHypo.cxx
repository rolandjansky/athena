/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     EFTauDiKaonHypo.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigTauHypo
 *
 * AUTHOR:   D. Zanzi based on EFTauMVHypo
 * CREATED:  Aug 17, 2015
 *
  *********************************************************************/

#include <list>
#include <iterator>
#include <sstream>
//
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"
//
#include "TrigTauHypo/EFTauDiKaonHypo.h"
//
#include "xAODTau/TauJetContainer.h"

#include "TFile.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include <PathResolver/PathResolver.h>
class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
EFTauDiKaonHypo::EFTauDiKaonHypo(const std::string& name, 
		     ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator)
{
// set unreasonable values to make sure that initialization is performed from outside
// however we assume if one parameter is set from outside then they all set ok.
  declareProperty("massTrkSysMin", m_massTrkSysMin = 0.);
  declareProperty("massTrkSysMax", m_massTrkSysMax = 1000000000.);
  declareProperty("leadTrkPtMin",  m_leadTrkPtMin  = 0.);
  declareProperty("EtCalibMin",    m_EtCalibMin  = 0.);
  declareProperty("nTrackMin",     m_nTrackMin  = 1);
  declareProperty("nTrackMax",     m_nTrackMax  = 2);
  declareProperty("nWideTrackMax", m_nWideTrackMax  = 10);
  declareProperty("dRmaxMax",      m_dRmaxMax  = 10.);
  declareProperty("etOverPtLeadTrkMin",    m_etOverPtLeadTrkMin  = 0.);
  declareProperty("etOverPtLeadTrkMax",    m_etOverPtLeadTrkMax  = 10.);
  declareProperty("EMPOverTrkSysPMax",     m_EMPOverTrkSysPMax  = 5.);

  declareMonitoredVariable("CutCounter",m_cutCounter=0);
}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
EFTauDiKaonHypo::~EFTauDiKaonHypo()
{  
}

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
/////////////////////////////////////////////////////////////////////
//

// ----------------------------------------------------------------------
HLT::ErrorCode EFTauDiKaonHypo::hltInitialize()
  // ----------------------------------------------------------------------
{
  
  msg() << MSG::INFO << "in initialize()" << endreq;
  
  msg() << MSG::INFO << " REGTEST: EFTauDiKaonHypo will cut on "<<endreq;
  msg() << MSG::INFO << " REGTEST: param massTrkSysMin " << m_massTrkSysMin <<endreq;
  msg() << MSG::INFO << " REGTEST: param massTrkSysMax " << m_massTrkSysMax <<endreq;
  msg() << MSG::INFO << " REGTEST: param leadTrkPtMin " << m_leadTrkPtMin <<endreq;
  msg() << MSG::INFO << " REGTEST: param EtCalibMin " << m_EtCalibMin <<endreq;
  msg() << MSG::INFO << " REGTEST: param nTrackMin (included) " << m_nTrackMin <<endreq;
  msg() << MSG::INFO << " REGTEST: param nTrackMax (included) " << m_nTrackMax <<endreq;
  msg() << MSG::INFO << " REGTEST: param nWideTrackMax (included) " << m_nWideTrackMax <<endreq;
  msg() << MSG::INFO << " REGTEST: param EMPOverTrkSysPMax " << m_EMPOverTrkSysPMax <<endreq;
  msg() << MSG::INFO << " REGTEST: param dRmaxMax " << m_dRmaxMax <<endreq;
  msg() << MSG::INFO << " REGTEST: param etOverPtLeadTrkMin " << m_etOverPtLeadTrkMin <<endreq;
  msg() << MSG::INFO << " REGTEST: param etOverPtLeadTrkMax " << m_etOverPtLeadTrkMax <<endreq;

  msg() << MSG::INFO << " REGTEST: ------ "<<endreq;
  
  if( ( m_massTrkSysMin >  m_massTrkSysMax ) || ( m_nTrackMin > m_nTrackMax )  || (m_etOverPtLeadTrkMin > m_etOverPtLeadTrkMax) )
    {
      msg() << MSG::ERROR << "EFTauDiKaonHypo is uninitialized! " << endreq;
      return HLT::BAD_JOB_SETUP;
    }
  
  msg() << MSG::INFO
	<< "Initialization of EFTauDiKaonHypo completed successfully"
	<< endreq;
  
  return HLT::OK;
}

// ----------------------------------------------------------------------
HLT::ErrorCode EFTauDiKaonHypo::hltFinalize(){
  // ----------------------------------------------------------------------
  
  msg() << MSG::INFO << "in finalize()" << endreq;
  return HLT::OK;
}


// ----------------------------------------------------------------------
HLT::ErrorCode EFTauDiKaonHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass){
  // ----------------------------------------------------------------------
  
  // Get the messaging service, print where you are
  
  if( msgLvl() <= MSG::DEBUG )  msg() << MSG::DEBUG <<"REGTEST:"<< name() << ": in execute()" << endreq;
  
  // general reset
  pass=false;

  m_cutCounter = 0;
  
  m_massTrkSys = 0.;
  m_leadTrkPt = 0.;
  m_nTrack = 0;
  m_nWideTrack = 0;
  m_EtCalib = 0.;  
  m_EMPOverTrkSysP = 0.;
  m_dRmax = 0.;
  m_etOverPtLeadTrk = 0.;

  // get the trigger element and extract the RoI information
  //---------------------------------------------------------
  
  const TrigRoiDescriptor* roiDescriptor = 0;
  HLT::ErrorCode status = getFeature(outputTE, roiDescriptor);
  
  if ( roiDescriptor ) 
    {
      if( msgLvl() <= MSG::DEBUG ) 
	msg() << MSG::DEBUG << "REGTEST: RoI id " << roiDescriptor->roiId()
	      << " located at   phi = " <<  roiDescriptor->phi()
	      << ", eta = " << roiDescriptor->eta() << endreq;
    } 
  else 
    {
      if( msgLvl() <= MSG::DEBUG ) 
	msg() <<  MSG::DEBUG << "Failed to find RoiDescriptor " << endreq;
    }
  
  // get tau objects from the trigger element:
  //--------------------------------------------------
  
  std::vector<const xAOD::TauJetContainer*> vectorTauContainers;
  
  status = getFeatures(outputTE, vectorTauContainers);
 
  if(status!=HLT::OK) 
    {
      msg() << MSG::INFO
	    << " REGTEST: Failed to get tauContainer's from the trigger element" 
	    << endreq;
      return HLT::OK;
    } 
  
  if( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << " Got " << vectorTauContainers.size() 
	  << " tauContainers's associated to the TE " << endreq;
  
  if(vectorTauContainers.size() == 0)
    {
      if( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG << " REGTEST: Received 0 taucontainers  "
	      << "This algorithm is designed to work with  one tau container per TE."
	      << endreq;
      return HLT::OK;
    }
  
  const xAOD::TauJetContainer *TauContainer = vectorTauContainers.back();
  
  msg() << MSG::DEBUG << " REGTEST: number of tau in container "<< TauContainer->size() << endreq;
  
  for(xAOD::TauJetContainer::const_iterator tauIt = TauContainer->begin();
      tauIt != TauContainer->end(); tauIt++){ 
    
    if( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << " tauRec candidate "<<endreq;
    
    m_cutCounter++;
    
    // cut on calibrated pt:
    double EFet = (*tauIt)->pt()*1e-3;
    
    if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " REGTEST: Et Calib "<< EFet <<endreq;
    
    if(!( EFet > m_EtCalibMin*1e-3)) continue;
    m_cutCounter++;
    
    // cut on core and wide tracks:
    m_nTrack = (*tauIt)->nTracks();
    m_nWideTrack = (*tauIt)->nWideTracks();
    if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " REGTEST: Track size "<< m_nTrack <<endreq;	
    if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " REGTEST: Wide Track size "<< m_nWideTrack <<endreq;
 
    if (!( (m_nTrack >= m_nTrackMin) && (m_nTrack <= m_nTrackMax)))  continue;
    if (!( m_nWideTrack <= m_nWideTrackMax ))  continue;
    m_cutCounter++;

    // cut on leading track pt:     
    (*tauIt)->detail(xAOD::TauJetParameters::leadTrkPt, m_leadTrkPt);
    if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " REGTEST: leadTrkPt "<< m_leadTrkPt <<endreq;

    if(!( m_leadTrkPt > m_leadTrkPtMin)) continue;
    m_cutCounter++;

    // cut on massTrkSys:     
    (*tauIt)->detail(xAOD::TauJetParameters::massTrkSys, m_massTrkSys);
    if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " REGTEST: massTrkSys "<< m_massTrkSys <<endreq;
    
    if (!( (m_massTrkSys > m_massTrkSysMin) && (m_massTrkSys < m_massTrkSysMax) ) )  continue;
    m_cutCounter++;

    // cut on EMPOverTrkSysP:     
    (*tauIt)->detail(xAOD::TauJetParameters::EMPOverTrkSysP, m_EMPOverTrkSysP);
    if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " REGTEST: EMPOverTrkSysP "<< m_EMPOverTrkSysP <<endreq;
               
    if ( !(m_EMPOverTrkSysP < m_EMPOverTrkSysPMax) )  continue;
    m_cutCounter++;

    // cut on etOverPtLeadTrk:
    (*tauIt)->detail(xAOD::TauJetParameters::etOverPtLeadTrk, m_etOverPtLeadTrk);
    if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " REGTEST: etOverPtLeadTrk "<< m_etOverPtLeadTrk <<endreq;

    if( !( (m_etOverPtLeadTrk > m_etOverPtLeadTrkMin) && (m_etOverPtLeadTrk < m_etOverPtLeadTrkMax)  ) ) continue;
    m_cutCounter++;

    // cut on dRmax:
    (*tauIt)->detail(xAOD::TauJetParameters::dRmax, m_dRmax);
    if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " REGTEST: dRmax "<< m_dRmax <<endreq;

    if( !( m_dRmax < m_dRmaxMax ) ) continue;
    m_cutCounter++;    
 
    //-------------------------------------------------
    // At least one Tau matching passed all cuts.
    // Accept the event!
    //-------------------------------------------------
    
    pass=true;
    
    if( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << " REGTEST: pass taurec is "<<pass<<endreq;
    
  } // end of loop in tau objects.
  
  if(pass)
    {
      if( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG
	      << " REGTEST: TE accepted !! "
	      << endreq;
      // activate Trigger Element.
    }
  else
    {
      if( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG
	      << " REGTEST: No good tau found !! TE rejected "
	      << endreq;
    }
  
  return HLT::OK;
  
}
