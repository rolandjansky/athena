/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     EFTauInvHypo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigTauHypo
//
// AUTHOR:   Richard Soluk - based on TrigEFEgammaHypo.cxx 
//           Apply cuts based on calo and tracking
//
// co-editor:  Stefania Xella 
// ********************************************************************
//
#include <list>
#include <iterator>
#include <sstream>
//
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/SystemOfUnits.h"
//
#include "EFTauInvHypo.h"
//
#include "tauEvent/TauJetContainer.h"
#include "tauEvent/TauJet.h"
#include "tauEvent/TauDetailsContainer.h"
#include "tauEvent/TauCommonDetails.h"
#include "tauEvent/TauCommonExtraDetails.h"
//#include "tauEvent/TauRecDetails.h"
//#include "tauEvent/TauRecExtraDetails.h"
//#include "tauEvent/Tau1P3PDetails.h"
//#include "tauEvent/Tau1P3PExtraDetails.h"
#include "tauEvent/TauPID.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
EFTauInvHypo::EFTauInvHypo(const std::string& name, 
			   ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator)
{
  // set unreasonable values to make sure that initialization is performed from outside
  // however we assume if one parameter is set from outside then they all set ok.
  declareProperty("NTrackMin",m_numTrackMin = 999); 
  declareProperty("NTrackMax",m_numTrackMax = 0); 
  declareProperty("NTrkOnly1or3",m_numTrkOnly1or3 = false); 
  declareProperty("EMRadiusMax_1P",m_EMRadiusMax_1P = 100.);
  declareProperty("EMRadiusMax_MP",m_EMRadiusMax_MP = 100.);
  declareProperty("EtCalibMin",m_EtCalibMin = -10.0*Gaudi::Units::GeV);
  declareProperty("DrTrkAvgMax_1P",m_DrTrkAvgMax_1P = 100.);
  declareProperty("DrTrkAvgMax_MP",m_DrTrkAvgMax_MP = 100.);
  declareProperty("EtovPtLeadMax_1P",m_EtovPtLeadMax_1P = 100.);
  declareProperty("EtovPtLeadMax_MP",m_EtovPtLeadMax_MP = 100.);
  declareProperty("emrad_OP_a0", m_emrad_OP_a0= 100.);
  declareProperty("emrad_OP_a1", m_emrad_OP_a1= 100.);
  declareProperty("emrad_OP_a2", m_emrad_OP_a2= 100.);
  declareProperty("emrad_MP_a0", m_emrad_MP_a0= 100.);
  declareProperty("emrad_MP_a1", m_emrad_MP_a1= 100.);
  declareProperty("emrad_MP_a2", m_emrad_MP_a2= 100.);
  declareProperty("trkrad_OP_a0", m_trkrad_OP_a0= 100.);
  declareProperty("trkrad_OP_a1", m_trkrad_OP_a1= 100.);
  declareProperty("trkrad_OP_a2", m_trkrad_OP_a2= 100.);
  declareProperty("trkrad_MP_a0", m_trkrad_MP_a0= 100.);
  declareProperty("trkrad_MP_a1", m_trkrad_MP_a1= 100.);
  declareProperty("trkrad_MP_a2", m_trkrad_MP_a2= 100.);
  
  declareMonitoredVariable("CutCounter",m_cutCounter=0);

  m_numTrack = -100;
  m_EMRadius = 999.;
  m_EtovPtLead = 999.;
  m_DrTrkAvg= 999.;  

}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
EFTauInvHypo::~EFTauInvHypo()
{  
}

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
/////////////////////////////////////////////////////////////////////
//

// ----------------------------------------------------------------------
HLT::ErrorCode EFTauInvHypo::hltInitialize()
// ----------------------------------------------------------------------
{
  
  msg() << MSG::INFO << "in initialize()" << endmsg;
  
  msg() << MSG::INFO << " REGTEST: EFTauInvHypo will cut on "<<endmsg;
  msg() << MSG::INFO << " REGTEST: ntracks "<<m_numTrackMin <<"-"<< m_numTrackMax <<endmsg; 
  msg() << MSG::INFO << " REGTEST: NTrkOnly1or3: "<<m_numTrkOnly1or3 <<endmsg; 
  msg() << MSG::INFO << " REGTEST: emradius 1P < "<<m_EMRadiusMax_1P <<endmsg;
  msg() << MSG::INFO << " REGTEST: emradius MP < "<<m_EMRadiusMax_MP <<endmsg;
  msg() << MSG::INFO << " REGTEST: Et calib > "<< m_EtCalibMin  <<endmsg;
  msg() << MSG::INFO << " REGTEST: DrTrkAvg 1P < "<<m_DrTrkAvgMax_1P <<endmsg;
  msg() << MSG::INFO << " REGTEST: DrTrkAvg MP < "<<m_DrTrkAvgMax_MP <<endmsg;
  msg() << MSG::INFO << " REGTEST: Et ov Pt 1P < "<<m_EtovPtLeadMax_1P <<endmsg;
  msg() << MSG::INFO << " REGTEST: Et ov Pt MP < "<<m_EtovPtLeadMax_MP <<endmsg;
  msg() << MSG::INFO << " REGTEST: param emrad OP "<<m_emrad_OP_a0<<","<<m_emrad_OP_a1<<","<<m_emrad_OP_a2<<endmsg;
  msg() << MSG::INFO << " REGTEST: param emrad MP "<<m_emrad_MP_a0<<","<<m_emrad_MP_a1<<","<<m_emrad_MP_a2<<endmsg;
  msg() << MSG::INFO << " REGTEST: param trkrad OP "<<m_trkrad_OP_a0<<","<<m_trkrad_OP_a1<<","<<m_trkrad_OP_a2<<endmsg;
  msg() << MSG::INFO << " REGTEST: param trkrad MP "<<m_trkrad_MP_a0<<","<<m_trkrad_MP_a1<<","<<m_trkrad_MP_a2<<endmsg;
  msg() << MSG::INFO << " REGTEST: ------ "<<endmsg;
  
  
  if( m_numTrackMin >  m_numTrackMax  && ! m_numTrkOnly1or3) //only check consistency of Max/Min if  numTrkOnly1or3==false
    {
      msg() << MSG::ERROR << "EFTauInvHypo is uninitialized! " << endmsg;
      return HLT::BAD_JOB_SETUP;
    }
  
  msg() << MSG::INFO << "Initialization of EFTauInvHypo completed successfully"<< endmsg;
  return HLT::OK;
}

// ----------------------------------------------------------------------
HLT::ErrorCode EFTauInvHypo::hltFinalize(){
  // ----------------------------------------------------------------------
  
  msg() << MSG::INFO << "in finalize()" << endmsg;
  return HLT::OK;
}


// ----------------------------------------------------------------------
HLT::ErrorCode EFTauInvHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass){
  // ----------------------------------------------------------------------
  
  // Get the messaging service, print where you are
  
  if( msgLvl() <= MSG::DEBUG )  msg() << MSG::DEBUG <<"REGTEST:"<< name() << ": in execute()" << endmsg;
  
  // general reset
  pass=false;
  
  m_cutCounter = 0;
  m_numTrack = -100;
  m_EMRadius = 999.;
  m_EtovPtLead = 999.;
  m_DrTrkAvg= 999.;
  
  
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
  
  //bool accepted=false;
  std::vector<const Analysis::TauJetContainer*> vectorTauContainers;
  
  status = getFeatures(outputTE, vectorTauContainers);
  if(status!=HLT::OK) {
    msg() << MSG::INFO
	  << " REGTEST: Failed to get tauContainer's from the trigger element" 
	  << endmsg;
    return HLT::OK;
  } 
  
  if( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << " Got " << vectorTauContainers.size() 
	  << " tauContainers's associated to the TE " << endmsg;
  
  if(vectorTauContainers.size() == 0){
    if( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << " REGTEST: Received 0 taucontainers  "
	    << "This algorithm is designed to work with  one tau container per TE."
	    << endmsg;
    return HLT::OK;
  }
  
  const Analysis::TauJetContainer *TauContainer = vectorTauContainers.back();
  
  msg() << MSG::DEBUG << " REGTEST: number of tau in container "<< TauContainer->size() << endmsg;
  
  for(Analysis::TauJetContainer::const_iterator tauIt = TauContainer->begin();
      tauIt != TauContainer->end(); tauIt++){                
    
    if ((*tauIt)->hasAuthor( TauJetParameters::tau1P3P) && !( (*tauIt)->hasAuthor( TauJetParameters::tauRec)) ) continue;
    
    if ((*tauIt)->hasAuthor( TauJetParameters::tauRec)) {
      
      
      const Analysis::TauCommonDetails*  TauDetails = (*tauIt)->details<const Analysis::TauCommonDetails>();
      
      if (TauDetails == NULL) 
	continue;
      
      if( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " tauRec candidate "<<endmsg;
      
      m_cutCounter++;
      
      // Cut on EtCalib
      
      double EFet = (*tauIt)->et()*1e-3;
      
      if( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG << " REGTEST: Et Calib "<<EFet<<endmsg;
      
      if(!( EFet > m_EtCalibMin*1e-3)) continue;
      m_cutCounter++;
      
      // Cut on number of tracks
      
      m_numTrack = (*tauIt)->numTrack(); //changed from seedCalo_numTrack() to numTrack() to syncronize with tauRec04-00-04
      
      if( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG << " REGTEST: Track size "<<m_numTrack <<endmsg;	
      
      if (m_numTrkOnly1or3) 
	{
	  if ( ! ( m_numTrack==1 || m_numTrack==3 ) ) continue;
	} 
      else 
	{
	  if (!( (m_numTrack >= m_numTrackMin) && (m_numTrack <= m_numTrackMax)))  continue;
	}
      m_cutCounter++;
      
      // Cut on EM radius of cluster
      m_EMRadius = TauDetails->seedCalo_EMRadius();
      
      if( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG << " REGTEST: EMRadius  "<< m_EMRadius <<endmsg;
      
      double emrad_cut=999.;
      if( EFet!=0 && EFet<=60. && m_numTrack==1 ) 
	emrad_cut = m_emrad_OP_a0+m_emrad_OP_a1*EFet+m_emrad_OP_a2/EFet;

      if( EFet!=0 && EFet<=60. && m_numTrack>1 ) 
	emrad_cut = m_emrad_MP_a0+m_emrad_MP_a1*EFet+m_emrad_MP_a2/EFet;
      
      if( EFet>60. && m_numTrack==1 ) emrad_cut = m_EMRadiusMax_1P;
      if( EFet>60. && m_numTrack>1 )  emrad_cut = m_EMRadiusMax_MP;
      
      if( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG << " REGTEST: EMRadius cut "<< emrad_cut<<endmsg;
      
      if( m_EMRadius > emrad_cut ) continue;
      m_cutCounter++;
      
      // Cut on track average distance
      
      m_DrTrkAvg = TauDetails->seedCalo_trkAvgDist();
      msg() << MSG::DEBUG<<" REGTEST: Track avg distance "<<m_DrTrkAvg<<endmsg;
      
      double trkrad_cut=999.;
      if( EFet!=0 && EFet<=60. && m_numTrack==1 ) 
	trkrad_cut = m_trkrad_OP_a0+m_trkrad_OP_a1*EFet+m_trkrad_OP_a2/EFet;

      if( EFet!=0 && EFet<=60. && m_numTrack>1 ) 
	trkrad_cut = m_trkrad_MP_a0+m_trkrad_MP_a1*EFet+m_trkrad_MP_a2/EFet;

      if( EFet>60. && m_numTrack==1 ) trkrad_cut = m_DrTrkAvgMax_1P;
      if( EFet>60. && m_numTrack>1 )  trkrad_cut = m_DrTrkAvgMax_MP;
      
      if( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG << " REGTEST: TrkRadius cut "<< trkrad_cut<<endmsg;
      
      if( m_DrTrkAvg > trkrad_cut ) continue;      
      m_cutCounter++;
      
      // Cut on Et over pT leading 
      m_EtovPtLead = TauDetails->etOverPtLeadTrk();
      
      if( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG << " REGTEST: Et over pTLead  "<< m_EtovPtLead <<endmsg;
      
      if(m_numTrack==1 && m_EtovPtLead > m_EtovPtLeadMax_1P) continue;
      if(m_numTrack>1  && m_EtovPtLead > m_EtovPtLeadMax_MP) continue;
      
      m_cutCounter++;
      //-------------------------------------------------
      // At least one Tau matching passed all cuts.
      // Accept the event!
      //-------------------------------------------------
      
      pass=true;
      
      if( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG << " REGTEST: pass taurec is "<<pass<<endmsg;
      
    }
    
  } // end of loop in tau objects.
  
  if(pass){
    
    if( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << " REGTEST: TE accepted !! " << endmsg;
    // activate Trigger Element.
  }
  else{
    if( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << " REGTEST: No good tau found !! TE rejected " << endmsg;
  }
  
  return HLT::OK;
  
}
