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
  declareProperty("NTrackMin", m_numTrackMin = -999);
  declareProperty("NTrackMax", m_numTrackMax = 0);
  declareProperty("NWideTrackMax", m_numWideTrackMax = 999);
  declareProperty("EtCalibMin",m_EtCalibMin  = -10000.);
  declareProperty("Level",     m_level       = -1);
  declareProperty("Method",    m_method      = 0);
  declareProperty("Highpt",    m_highpt      = false);
  declareProperty("HighptTrkThr", m_highpttrkthr      = 250000.);
  declareProperty("HighptIDThr", m_highptidthr      = 330000.);
  declareProperty("HighptJetThr", m_highptjetthr      = 410000.); 

  declareMonitoredVariable("CutCounter",m_cutCounter=0);
  OneProngGraph=0;
  MultiProngGraph=0;
}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
EFTauMVHypo::~EFTauMVHypo()
{  
  delete OneProngGraph;
  delete MultiProngGraph;
}

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
/////////////////////////////////////////////////////////////////////
//

// ----------------------------------------------------------------------
HLT::ErrorCode EFTauMVHypo::hltInitialize()
  // ----------------------------------------------------------------------
{
  
  msg() << MSG::INFO << "in initialize()" << endreq;
  
  msg() << MSG::INFO << " REGTEST: EFTauMVHypo will cut on "<<endreq;
  msg() << MSG::INFO << " REGTEST: param NTrackMin " << m_numTrackMin <<endreq;
  msg() << MSG::INFO << " REGTEST: param NTrackMax " << m_numTrackMax <<endreq;
  msg() << MSG::INFO << " REGTEST: param NWideTrackMax " << m_numWideTrackMax <<endreq;
  msg() << MSG::INFO << " REGTEST: param EtCalib " << m_EtCalibMin <<endreq;
  msg() << MSG::INFO << " REGTEST: param Level " << m_level <<endreq;
  msg() << MSG::INFO << " REGTEST: param Method " << m_method <<endreq;
  msg() << MSG::INFO << " REGTEST: param Highpt with thrs " << m_highpt << " " << m_highpttrkthr <<  " " << m_highptidthr << " " << m_highptjetthr <<endreq;
  msg() << MSG::INFO << " REGTEST: ------ "<<endreq;
  
  if( (m_numTrackMin >  m_numTrackMax) || m_level == -1 || (m_highptidthr > m_highptjetthr))
    {
      msg() << MSG::ERROR << "EFTauMVHypo is uninitialized! " << endreq;
      return HLT::BAD_JOB_SETUP;
    }
  
  std::string s_llh_cuts_file = PathResolver::find_file("LMTCutsLLHTrigger.root", "DATAPATH");
  
  msg() << MSG::DEBUG << "Try to open root file containing cuts: " << s_llh_cuts_file << endreq;
 
  TFile* _llhcuts = TFile::Open(s_llh_cuts_file.c_str());
  
  if(!_llhcuts)
    {
      msg() << MSG::ERROR << "Could not find file containing cut values. EFTauMVHypo is uninitialized! " << endreq;
      return HLT::BAD_JOB_SETUP;
    }

  if (m_level == 1)
    {
      s_cut_level      = "medium";
      OneProngGraph    = (TGraph*)((_llhcuts->Get("1prong/medium"))->Clone());
      MultiProngGraph  = (TGraph*)((_llhcuts->Get("3prong/medium"))->Clone());
    }
  else if (m_level == 2)
    {
      s_cut_level      = "medium1";
      OneProngGraph    = (TGraph*)((_llhcuts->Get("1prong/medium1"))->Clone());
      MultiProngGraph  = (TGraph*)((_llhcuts->Get("3prong/medium1"))->Clone());
    }
  else if (m_level == 3)
    {
      s_cut_level      = "tight";
      OneProngGraph    = (TGraph*)((_llhcuts->Get("1prong/tight"))->Clone());
      MultiProngGraph  = (TGraph*)((_llhcuts->Get("3prong/tight"))->Clone());
    }
  else if (m_level == -1111)
    {
      s_cut_level = "none";
    }
  else
    {
      msg() << MSG::ERROR << "Did not configure valid level. EFTauMVHypo is uninitialized! " << endreq;
      return HLT::BAD_JOB_SETUP;
    }
  
  msg() << MSG::INFO
	<< "Initialization of EFTauMVHypo completed successfully"
	<< endreq;
  
  if (_llhcuts) 
    {
      _llhcuts->Close();
      delete _llhcuts;
    }
  return HLT::OK;
}

// ----------------------------------------------------------------------
HLT::ErrorCode EFTauMVHypo::hltFinalize(){
  // ----------------------------------------------------------------------
  
  msg() << MSG::INFO << "in finalize()" << endreq;
  return HLT::OK;
}


// ----------------------------------------------------------------------
HLT::ErrorCode EFTauMVHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass){
  // ----------------------------------------------------------------------
  
  // Get the messaging service, print where you are
  
  if( msgLvl() <= MSG::DEBUG )  msg() << MSG::DEBUG <<"REGTEST:"<< name() << ": in execute()" << endreq;
  
  // general reset
  pass=false;
  
  m_cutCounter = 0;
  m_numTrack = -100;
  m_numWideTrack = -100;
  m_LLHScore = -1111.;
  m_BDTScore = -1111.;  
  
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
    
    double EFet = (*tauIt)->pt()*1e-3;
    
    if( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << " REGTEST: Et Calib "<<EFet<<endreq;
    
    if(!( EFet > m_EtCalibMin*1e-3)) continue;
    m_cutCounter++;
    
    m_numTrack = (*tauIt)->nTracks();
    m_numWideTrack = (*tauIt)->nWideTracks();

    if( msgLvl() <= MSG::DEBUG ){
      msg() << MSG::DEBUG << " REGTEST: Track size "<<m_numTrack <<endreq;	
      msg() << MSG::DEBUG << " REGTEST: Wide Track size "<<m_numWideTrack <<endreq;
    }    

    // turn off track selection at highpt
    bool applyTrkSel(true);
    if(m_highpt && (EFet > m_highpttrkthr*1e-3) ) applyTrkSel = false;

    if(!(m_numTrack <= m_numTrackMax)) continue;
    if(applyTrkSel) if( !( m_numTrack >= m_numTrackMin ) )  continue;
    if(applyTrkSel) if( !(m_numWideTrack <= m_numWideTrackMax)  ) continue;
   
    m_cutCounter++;
   
    //loosen and turn off ID cut at highpt
    if(m_highpt && (EFet > m_highptidthr*1e-3) && m_level>1) m_level = 1; //works only for BDT, not llh
    if(m_highpt && (EFet > m_highptjetthr*1e-3) ) m_level = -1111;
 
    if(m_method == 1 || m_method == 0)
      {
	double llh_cut = 11111.;     
	std::string prong;
	m_numTrack==1 ?  prong = "1P" : prong = "3P";
	
	if(m_level == -1111){ //noCut, accept this TE
	  pass = true;
	  m_cutCounter++;
	  continue;
	}
	
	std::string s_cut_llh = "llh_" + s_cut_level +"_" + prong;
	msg() << MSG::DEBUG << "REGTEST: cut string: " << s_cut_llh << endreq;
	
	if(m_numTrack == 1)
	  {
	    llh_cut=OneProngGraph->Eval(EFet);
	  }
	else
	  {
	    llh_cut=MultiProngGraph->Eval(EFet);	  
	  }
	
	msg() << MSG::DEBUG<<" REGTEST: will cut on llh score at "<< llh_cut <<endreq;
	
	m_LLHScore = (*tauIt)->discriminant(xAOD::TauJetParameters::Likelihood);
	
	msg() << MSG::DEBUG<<" REGTEST: LLHScore "<<m_LLHScore<<endreq;
	
	if (m_LLHScore < llh_cut) continue;
	
	m_cutCounter++;
      }
    
    else if(m_method == 2 || m_method == 0)
      {
	m_BDTScore = (*tauIt)->discriminant(xAOD::TauJetParameters::BDTJetScore);
	
	msg() << MSG::DEBUG<<" REGTEST: BDTScore "<<m_BDTScore<<endreq;
	
	if(m_level == -1111)
	  { //noCut, accept this TE
	    pass = true;
	    m_cutCounter++;
	    continue;
	  }
	
	if (m_level == 1 && (*tauIt)->isTau(xAOD::TauJetParameters::JetBDTSigLoose) == 0)
	  continue;
	else if (m_level == 2 && (*tauIt)->isTau(xAOD::TauJetParameters::JetBDTSigMedium) == 0)
	  continue;
	else if (m_level ==3  && (*tauIt)->isTau(xAOD::TauJetParameters::JetBDTSigTight) == 0)
	  continue;
	
	m_cutCounter++;
      }
    else
      {
	msg() << MSG::ERROR << " no valid method defined "<<endreq;	
	continue;
      }
    
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
