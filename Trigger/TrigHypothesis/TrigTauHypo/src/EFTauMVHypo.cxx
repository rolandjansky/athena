/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "EFTauMVHypo.h"
//
#include "xAODTau/TauJetContainer.h"

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
  m_BDTScore = -1111.;
}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
EFTauMVHypo::~EFTauMVHypo()
{  
}

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
/////////////////////////////////////////////////////////////////////
//

// ----------------------------------------------------------------------
HLT::ErrorCode EFTauMVHypo::hltInitialize()
  // ----------------------------------------------------------------------
{
  
  ATH_MSG_INFO( "In initialize()" );
  
  ATH_MSG_INFO( "REGTEST: EFTauMVHypo will cut on" );
  ATH_MSG_INFO( "REGTEST: param NTrackMin " << m_numTrackMin );
  ATH_MSG_INFO( "REGTEST: param NTrackMax " << m_numTrackMax );
  ATH_MSG_INFO( "REGTEST: param NWideTrackMax " << m_numWideTrackMax );
  ATH_MSG_INFO( "REGTEST: param EtCalib " << m_EtCalibMin );
  ATH_MSG_INFO( "REGTEST: param Level " << m_level );
  ATH_MSG_INFO( "REGTEST: param Method " << m_method );
  ATH_MSG_INFO( "REGTEST: param Highpt with thrs " << m_highpt << " " << m_highpttrkthr <<  " " << m_highptidthr << " " << m_highptjetthr );
  ATH_MSG_INFO( "REGTEST: param ApplyIDon0p " << m_applyIDon0p );
  ATH_MSG_INFO( "REGTEST: ------ " );
  
  if( (m_numTrackMin >  m_numTrackMax) || m_level == -1 || (m_highptidthr > m_highptjetthr) )
    {
      ATH_MSG_ERROR( "EFTauMVHypo is uninitialized! " );
      return HLT::BAD_JOB_SETUP;
    }
  
  // Likelihood ID no longer supported
  if( m_method == 1)
    {
      ATH_MSG_ERROR( "Likelihood identification discontinued!" );
      return HLT::BAD_ALGO_CONFIG;
    }
  else if( m_method == 0 && m_level != -1111)
    {
      ATH_MSG_ERROR( "Incorrect combination of Method and Level." );
      return HLT::BAD_ALGO_CONFIG;
    }

  // allow level 0 for RNN
  if (m_level == 0 && m_method==3)
    {
      m_cut_level      = "veryloose";
    }
  else if (m_level == 1)
    {
      m_cut_level      = "medium";
    }
  else if (m_level == 2)
    {
      m_cut_level      = "medium1";
    }
  else if (m_level == 3)
    {
      m_cut_level      = "tight";
    }
  else if (m_level == -1111)
    {
      m_cut_level = "none";
    }
  else
    {
      ATH_MSG_ERROR( "Did not configure valid level. EFTauMVHypo is uninitialized!" );
      return HLT::BAD_JOB_SETUP;
    }
  
  ATH_MSG_INFO( "Initialization of EFTauMVHypo completed successfully." );
  
  return HLT::OK;
}

// ----------------------------------------------------------------------
HLT::ErrorCode EFTauMVHypo::hltFinalize(){
  // ----------------------------------------------------------------------
  
  ATH_MSG_INFO( "In finalize()" );
  return HLT::OK;
}


// ----------------------------------------------------------------------
HLT::ErrorCode EFTauMVHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass){
  // ----------------------------------------------------------------------
  
  ATH_MSG_DEBUG( "REGTEST:" << name() << ": in execute()");
  
  // general reset
  pass=false;
  
  m_cutCounter = 0;
  m_numTrack = -100;
  m_numWideTrack = -100;
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
      ATH_MSG_DEBUG( "REGTEST: RoI id " << roiDescriptor->roiId()
		     << " located at phi = " <<  roiDescriptor->phi()
		     << ", eta = " << roiDescriptor->eta() );
    } 
  else 
    {
      ATH_MSG_DEBUG( "Failed to find RoiDescriptor" );
    }
  
  // get tau objects from the trigger element:
  //--------------------------------------------------
  
  std::vector<const xAOD::TauJetContainer*> vectorTauContainers;
  
  status = getFeatures(outputTE, vectorTauContainers);
 
  if(status!=HLT::OK) 
    {
      ATH_MSG_INFO( "REGTEST: Failed to get tauContainer's from the trigger element" );
      return HLT::OK;
    } 
  
  ATH_MSG_DEBUG( "Got " << vectorTauContainers.size() << " tauContainers's associated to the TE" );
  
  if(vectorTauContainers.size() == 0)
    {
      ATH_MSG_DEBUG( "REGTEST: Received 0 taucontainers. "
		     << "This algorithm is designed to work with  one tau container per TE." );
      return HLT::OK;
    }
  
  const xAOD::TauJetContainer *TauContainer = vectorTauContainers.back();
  
  ATH_MSG_DEBUG( "REGTEST: number of tau in container "<< TauContainer->size() );
  m_inputTaus = TauContainer->size(); 
 
  for(xAOD::TauJetContainer::const_iterator tauIt = TauContainer->begin();
      tauIt != TauContainer->end(); tauIt++){ 
    
    ATH_MSG_DEBUG( "tauRec candidate" );
    
    m_cutCounter++;
    
    double EFet = (*tauIt)->pt()*1e-3;

    ATH_MSG_DEBUG( "REGTEST: Et Calib " << EFet );
    
    if(!( EFet > m_EtCalibMin*1e-3)) continue;
    m_cutCounter++;
    m_mon_ptAccepted = EFet;
    
    // handled transparently whether using standard or MVA track counting
    m_numTrack = (*tauIt)->nTracks();
    m_numWideTrack = (*tauIt)->nTracksIsolation();
    
    ATH_MSG_DEBUG( "REGTEST: Track size " << m_numTrack );
    ATH_MSG_DEBUG( "REGTEST: Wide Track size " << m_numWideTrack );
    
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

    // No tau ID
    if(m_method == 0)
      {
	pass = true;
	m_cutCounter++;
	break;      
      }
    // BDT
    else if(m_method == 2)
      {
	m_BDTScore = (*tauIt)->discriminant(xAOD::TauJetParameters::BDTJetScore);
	
	ATH_MSG_DEBUG( "REGTEST: BDTScore " << m_BDTScore );
	
	if(local_level == -1111)
	  { //noCut, accept this TE
	    pass = true;
	    m_cutCounter++;
	    break;
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
	  ATH_MSG_WARNING( "RNNJetScoreSigTrans not available. Make sure TauWPDecorator is run for RNN!" );
	
	ATH_MSG_DEBUG( "REGTEST: RNNJetScoreSigTrans "<< (*tauIt)->discriminant(xAOD::TauJetParameters::RNNJetScoreSigTrans) );
	
	if(local_level == -1111)
	  { //noCut, accept this TE
	    pass = true;
	    m_cutCounter++;
	    break;
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
	ATH_MSG_ERROR( "No valid method defined." );
	continue;
      }
    
    //-------------------------------------------------
    // At least one Tau matching passed all cuts.
    // Accept the event!
    //-------------------------------------------------
    
    pass=true;
    
    ATH_MSG_DEBUG( "REGTEST: pass taurec is " << pass );

    break;
    
  } // end of loop in tau objects.
  
  if(pass)
    {
      ATH_MSG_DEBUG( "REGTEST: TE accepted !!" );
      // activate Trigger Element.
    }
  else
    {
      ATH_MSG_DEBUG( "REGTEST: No good tau found !! TE rejected" );
    }
  
  return HLT::OK;
  
}
