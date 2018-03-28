/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     EFTauMVHypo.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigTauHypo
 *
 * AUTHOR:   M. Morgenstern based on EFTauInvHypo
 * CREATED:  Jun 20, 2011
 *
  *********************************************************************/

#include <list>
#include <iterator>
#include <sstream>
//
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"
//
#include "TrigTauHypo/EFTauMVHypo.h"
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
EFTauMVHypo::EFTauMVHypo(const std::string& name, 
		     ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator)
{
  // set unreasonable values to make sure that initialization is performed from outside
  // however we assume if one parameter is set from outside then they all set ok.
  declareProperty("NTrackMin",     m_numTrackMin       = -999);
  declareProperty("NTrackMax",     m_numTrackMax       = 0);
  declareProperty("NWideTrackMax", m_numWideTrackMax   = 999);
  declareProperty("EtCalibMin",    m_EtCalibMin        = -10000.);
  declareProperty("Level",         m_level             = -1);
  declareProperty("Method",        m_method            = 0);
  declareProperty("Highpt",        m_highpt            = true);
  declareProperty("HighptTrkThr",  m_highpttrkthr      = 200000.);
  declareProperty("HighptIDThr",   m_highptidthr       = 330000.);
  declareProperty("HighptJetThr",  m_highptjetthr      = 440000.); 
  declareProperty("ApplyIDon0p",   m_applyIDon0p       = true);

  declareMonitoredVariable("CutCounter",m_cutCounter=0);
  declareMonitoredVariable("NTrack",m_mon_nTrackAccepted=0);
  declareMonitoredVariable("NWideTrack",m_mon_nWideTrackAccepted=0);
  declareMonitoredVariable("EtCalib",m_mon_ptAccepted=-10.);
  declareMonitoredVariable("NInputTaus",m_inputTaus=-1.);

  m_numTrack = -100;
  m_numWideTrack = -100;
  m_LLHScore = -1111.;
  m_BDTScore = -1111.;
  m_OneProngGraph=0;
  m_MultiProngGraph=0;
}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
EFTauMVHypo::~EFTauMVHypo()
{  
  delete m_OneProngGraph;
  delete m_MultiProngGraph;
}

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
/////////////////////////////////////////////////////////////////////
//

// ----------------------------------------------------------------------
HLT::ErrorCode EFTauMVHypo::hltInitialize()
  // ----------------------------------------------------------------------
{
  
  msg() << MSG::INFO << "in initialize()" << endmsg;
  
  msg() << MSG::INFO << " REGTEST: EFTauMVHypo will cut on "<<endmsg;
  msg() << MSG::INFO << " REGTEST: param NTrackMin " << m_numTrackMin <<endmsg;
  msg() << MSG::INFO << " REGTEST: param NTrackMax " << m_numTrackMax <<endmsg;
  msg() << MSG::INFO << " REGTEST: param NWideTrackMax " << m_numWideTrackMax <<endmsg;
  msg() << MSG::INFO << " REGTEST: param EtCalib " << m_EtCalibMin <<endmsg;
  msg() << MSG::INFO << " REGTEST: param Level " << m_level <<endmsg;
  msg() << MSG::INFO << " REGTEST: param Method " << m_method <<endmsg;
  msg() << MSG::INFO << " REGTEST: param Highpt with thrs " << m_highpt << " " << m_highpttrkthr <<  " " << m_highptidthr << " " << m_highptjetthr <<endmsg;
  msg() << MSG::INFO << " REGTEST: param ApplyIDon0p " << m_applyIDon0p <<endmsg;
  msg() << MSG::INFO << " REGTEST: ------ "<<endmsg;
  
  if( (m_numTrackMin >  m_numTrackMax) || m_level == -1 || (m_highptidthr > m_highptjetthr) )
    {
      msg() << MSG::ERROR << "EFTauMVHypo is uninitialized! " << endmsg;
      return HLT::BAD_JOB_SETUP;
    }
  
  // this should be improved
  std::string s_llh_cuts_file = PathResolverFindCalibFile("TrigTauRec/00-11-02/LMTCutsLLHTrigger.root");  
  msg() << MSG::DEBUG << "Try to open root file containing cuts: " << s_llh_cuts_file << endmsg;
 
  TFile* llhcuts = TFile::Open(s_llh_cuts_file.c_str());
  
  if(!llhcuts)
    {
      msg() << MSG::ERROR << "Could not find file containing cut values. EFTauMVHypo is uninitialized! " << endmsg;
      return HLT::BAD_JOB_SETUP;
    }

  // allow level 0 for RNN
  if (m_level == 0 && m_method==3)
    {
      m_cut_level      = "veryloose";
    }
  else if (m_level == 1)
    {
      m_cut_level      = "medium";
      m_OneProngGraph    = (TGraph*)((llhcuts->Get("1prong/medium"))->Clone());
      m_MultiProngGraph  = (TGraph*)((llhcuts->Get("3prong/medium"))->Clone());
    }
  else if (m_level == 2)
    {
      m_cut_level      = "medium1";
      m_OneProngGraph    = (TGraph*)((llhcuts->Get("1prong/medium1"))->Clone());
      m_MultiProngGraph  = (TGraph*)((llhcuts->Get("3prong/medium1"))->Clone());
    }
  else if (m_level == 3)
    {
      m_cut_level      = "tight";
      m_OneProngGraph    = (TGraph*)((llhcuts->Get("1prong/tight"))->Clone());
      m_MultiProngGraph  = (TGraph*)((llhcuts->Get("3prong/tight"))->Clone());
    }
  else if (m_level == -1111)
    {
      m_cut_level = "none";
    }
  else
    {
      msg() << MSG::ERROR << "Did not configure valid level. EFTauMVHypo is uninitialized! " << endmsg;
      return HLT::BAD_JOB_SETUP;
    }
  
  msg() << MSG::INFO
	<< "Initialization of EFTauMVHypo completed successfully"
	<< endmsg;
  
  if (llhcuts) 
    {
      llhcuts->Close();
      delete llhcuts;
    }
  return HLT::OK;
}

// ----------------------------------------------------------------------
HLT::ErrorCode EFTauMVHypo::hltFinalize(){
  // ----------------------------------------------------------------------
  
  msg() << MSG::INFO << "in finalize()" << endmsg;
  return HLT::OK;
}


// ----------------------------------------------------------------------
HLT::ErrorCode EFTauMVHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass){
  // ----------------------------------------------------------------------
  
  // Get the messaging service, print where you are
  
  if( msgLvl() <= MSG::DEBUG )  msg() << MSG::DEBUG <<"REGTEST:"<< name() << ": in execute()" << endmsg;
  
  // general reset
  pass=false;
  
  m_cutCounter = 0;
  m_numTrack = -100;
  m_numWideTrack = -100;
  m_LLHScore = -1111.;
  m_BDTScore = -1111.;  

  m_mon_ptAccepted = -10.;
  m_mon_nTrackAccepted = -1;
  m_mon_nWideTrackAccepted = -1;

  // get the trigger element and extract the RoI information
  //---------------------------------------------------------
  
  const TrigRoiDescriptor* roiDescriptor = 0;
  HLT::ErrorCode status = getFeature(outputTE, roiDescriptor);
  
  if ( roiDescriptor ) 
    {
      if( msgLvl() <= MSG::DEBUG ) 
	msg() << MSG::DEBUG << "REGTEST: RoI id " << roiDescriptor->roiId()
	      << " located at   phi = " <<  roiDescriptor->phi()
	      << ", eta = " << roiDescriptor->eta() << endmsg;
    } 
  else 
    {
      if( msgLvl() <= MSG::DEBUG ) 
	msg() <<  MSG::DEBUG << "Failed to find RoiDescriptor " << endmsg;
    }
  
  // get tau objects from the trigger element:
  //--------------------------------------------------
  
  std::vector<const xAOD::TauJetContainer*> vectorTauContainers;
  
  status = getFeatures(outputTE, vectorTauContainers);
 
  if(status!=HLT::OK) 
    {
      msg() << MSG::INFO
	    << " REGTEST: Failed to get tauContainer's from the trigger element" 
	    << endmsg;
      return HLT::OK;
    } 
  
  if( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << " Got " << vectorTauContainers.size() 
	  << " tauContainers's associated to the TE " << endmsg;
  
  if(vectorTauContainers.size() == 0)
    {
      if( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG << " REGTEST: Received 0 taucontainers  "
	      << "This algorithm is designed to work with  one tau container per TE."
	      << endmsg;
      return HLT::OK;
    }
  
  const xAOD::TauJetContainer *TauContainer = vectorTauContainers.back();
  
  msg() << MSG::DEBUG << " REGTEST: number of tau in container "<< TauContainer->size() << endmsg;
  m_inputTaus = TauContainer->size(); 
 
  for(xAOD::TauJetContainer::const_iterator tauIt = TauContainer->begin();
      tauIt != TauContainer->end(); tauIt++){ 
    
    if( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << " tauRec candidate "<<endmsg;
    
    m_cutCounter++;
    
    double EFet = (*tauIt)->pt()*1e-3;

    if( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << " REGTEST: Et Calib "<<EFet<<endmsg;
    
    if(!( EFet > m_EtCalibMin*1e-3)) continue;
    m_cutCounter++;
    m_mon_ptAccepted = EFet;
    
    // handled transparently whether using standard or MVA track counting
    m_numTrack = (*tauIt)->nTracks();
#ifndef XAODTAU_VERSIONS_TAUJET_V3_H
    m_numWideTrack = (*tauIt)->nWideTracks();
#else
    m_numWideTrack = (*tauIt)->nTracksIsolation();
#endif
    
    if( msgLvl() <= MSG::DEBUG ){
      msg() << MSG::DEBUG << " REGTEST: Track size "<<m_numTrack <<endmsg;	
      msg() << MSG::DEBUG << " REGTEST: Wide Track size "<<m_numWideTrack <<endmsg;
    }    
    
    // turn off track selection at highpt
    bool applyTrkSel(true);
    bool applyMaxTrkSel(true);
    if(m_highpt && (EFet > m_highpttrkthr*1e-3) ) applyTrkSel = false;
    if(m_highpt && (EFet > m_highptjetthr*1e-3) ) applyMaxTrkSel = false;

    if(applyMaxTrkSel) if( !(m_numTrack <= m_numTrackMax) ) continue;
    if(applyTrkSel)    if( !(m_numTrack >= m_numTrackMin) ) continue;
    if(applyTrkSel)    if( !(m_numWideTrack <= m_numWideTrackMax)  ) continue;      
   
    m_cutCounter++;
    m_mon_nTrackAccepted = m_numTrack;
    m_mon_nWideTrackAccepted = m_numWideTrack;  

    auto local_level = m_level;
    //loosen and turn off ID cut at highpt
    if(m_highpt && (EFet > m_highptidthr*1e-3) && m_level>1) local_level = 1; //works only for BDT, not llh
    if(m_highpt && (EFet > m_highptjetthr*1e-3) ) local_level = -1111;
    if(!m_applyIDon0p && m_numTrack==0) local_level = -1111;
 
    // likelihood
    if(m_method == 1 || m_method == 0)
      {
	double llh_cut = 11111.;     
	std::string prong;
	m_numTrack==1 ?  prong = "1P" : prong = "3P";
	
	if(local_level == -1111){ //noCut, accept this TE
	  pass = true;
	  m_cutCounter++;
	  continue;
	}
	
	std::string s_cut_llh = "llh_" + m_cut_level +"_" + prong;
	msg() << MSG::DEBUG << "REGTEST: cut string: " << s_cut_llh << endmsg;
	
	if(m_numTrack == 1)
	  {
	    llh_cut=m_OneProngGraph->Eval(EFet);
	  }
	else
	  {
	    llh_cut=m_MultiProngGraph->Eval(EFet);	  
	  }
	
	msg() << MSG::DEBUG<<" REGTEST: will cut on llh score at "<< llh_cut <<endmsg;
	
	m_LLHScore = (*tauIt)->discriminant(xAOD::TauJetParameters::Likelihood);
	
	msg() << MSG::DEBUG<<" REGTEST: LLHScore "<<m_LLHScore<<endmsg;
	
	if (m_LLHScore < llh_cut) continue;
	
	m_cutCounter++;
      }
    // BDT
    else if(m_method == 2)
      {
	m_BDTScore = (*tauIt)->discriminant(xAOD::TauJetParameters::BDTJetScore);
	
	msg() << MSG::DEBUG<<" REGTEST: BDTScore "<<m_BDTScore<<endmsg;
	
	if(local_level == -1111)
	  { //noCut, accept this TE
	    pass = true;
	    m_cutCounter++;
	    continue;
	  }
	
	if (local_level == 1 && (*tauIt)->isTau(xAOD::TauJetParameters::JetBDTSigLoose) == 0)
	  continue;
	else if (local_level == 2 && (*tauIt)->isTau(xAOD::TauJetParameters::JetBDTSigMedium) == 0)
	  continue;
	else if (local_level == 3  && (*tauIt)->isTau(xAOD::TauJetParameters::JetBDTSigTight) == 0)
	  continue;
	
	m_cutCounter++;
      }
    // RNN
    else if(m_method == 3)
      {
	if(!(*tauIt)->hasDiscriminant(xAOD::TauJetParameters::RNNJetScoreSigTrans))
	  msg() << MSG::WARNING <<" RNNJetScoreSigTrans not available. Make sure TauWPDecorator is run for RNN!"<<endmsg;
	
	msg() << MSG::DEBUG << " REGTEST: RNNJetScoreSigTrans "<< (*tauIt)->discriminant(xAOD::TauJetParameters::RNNJetScoreSigTrans) << endmsg;
	
	if(local_level == -1111)
	  { //noCut, accept this TE
	    pass = true;
	    m_cutCounter++;
	    continue;
	  }
	if (local_level == 0 && (*tauIt)->isTau(xAOD::TauJetParameters::JetRNNSigVeryLoose) == 0)  
	  continue;
	else if (local_level == 1 && (*tauIt)->isTau(xAOD::TauJetParameters::JetRNNSigLoose) == 0)
	  continue;
	else if (local_level == 2 && (*tauIt)->isTau(xAOD::TauJetParameters::JetRNNSigMedium) == 0)
	  continue;
	else if (local_level == 3  && (*tauIt)->isTau(xAOD::TauJetParameters::JetRNNSigTight) == 0)
	  continue;
	
	m_cutCounter++;
      }
    
    else
      {
	msg() << MSG::ERROR << " no valid method defined "<<endmsg;	
	continue;
      }
    
    //-------------------------------------------------
    // At least one Tau matching passed all cuts.
    // Accept the event!
    //-------------------------------------------------
    
    pass=true;
    
    if( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << " REGTEST: pass taurec is "<<pass<<endmsg;
    
  } // end of loop in tau objects.
  
  if(pass)
    {
      if( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG
	      << " REGTEST: TE accepted !! "
	      << endmsg;
      // activate Trigger Element.
    }
  else
    {
      if( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG
	      << " REGTEST: No good tau found !! TE rejected "
	      << endmsg;
    }
  
  return HLT::OK;
  
}
