// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaMuonCombHypo/TrigL2ElectronMuonAngleHypo.h
 **
 **   Description: - Hypo algorithm: searc for pairs of electron and muons with 
 **                delta phi in some interval + some additional cuts; intended for a1->tautau->emu
 **                - Modified from TrigEFDielectronMassHypo and TrigEFDielectronMassFex by R Goncalo
			and TrigL2DimuonMassHypo by C. Schiavi
 **
 **   Author: Pavel Jez <pavel.jez@cern.ch>
 **
 **   Created:   Jan 30, 2011
 **
 **
 **************************************************************************/ 

#include "TrigEgammaMuonCombHypo/TrigL2ElectronMuonAngleFexAlgo.h"
#include "TrigEgammaMuonCombHypo/TrigL2ElectronMuonAngleHypo.h"



class ISvcLocator;

TrigL2ElectronMuonAngleHypo::TrigL2ElectronMuonAngleHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator),
  m_MaxDR(4.0),
  m_MinDR(0),
  m_MaxDPhi(1.0),
  m_lowerMassCut(0),
  m_upperMassCut(10.),
  m_inputLabel("EgMuTopoFEX"),
  m_acceptAll(false),
  m_monMassAccepted(0),
  m_monMassAll(0),
  m_monCut(0),
  m_monDPhiAccepted(0),
  m_monDPhiAll(0),
  m_monDRAccepted(0),
  m_monDRAll(0),
  m_oppositeCharge(0),
  m_commonVertex(0)  
{

  // Read cuts
  declareProperty("AcceptAll",    m_acceptAll);
  declareProperty("MaxDRCut",  	  m_MaxDR=4.0); 
  declareProperty("MinDRCut",  	  m_MinDR=0.0); 
  declareProperty("MaxDPhiCut",   m_MaxDPhi=1.0); 
  declareProperty("LowerMassCut", m_lowerMassCut=2000.0);
  declareProperty("UpperMassCut", m_upperMassCut=20000.0);
  declareProperty("OppositeSign", m_oppositeCharge=true);
  declareProperty("CommonVertex", m_commonVertex=true);
  declareProperty("inputLabel", m_inputLabel);
 

  // monitoring
  declareMonitoredVariable("massOfAccepted", m_monMassAccepted);
  declareMonitoredVariable("massOfAll", m_monMassAll);  
  declareMonitoredVariable("cut", m_monCut);
  declareMonitoredVariable("dPhiOfAccepted", m_monDPhiAccepted);
  declareMonitoredVariable("dPhiOfAll", m_monDPhiAll);
  declareMonitoredVariable("dROfAccepted", m_monDRAccepted);
  declareMonitoredVariable("dROfAll", m_monDRAll);
  

}

TrigL2ElectronMuonAngleHypo::~TrigL2ElectronMuonAngleHypo()
{ }

HLT::ErrorCode TrigL2ElectronMuonAngleHypo::hltInitialize()
{
  
  if (msgLvl() <= MSG::VERBOSE) {
    msg() << MSG::DEBUG << "Initialization:" << endmsg;
  }

  
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Initialization completed successfully:" << endmsg;
    msg() << MSG::DEBUG << "AcceptAll            = " 
	<< (m_acceptAll==true ? "True" : "False") << endmsg; 
    msg() << MSG::DEBUG << "Input Collection has label " << m_inputLabel << endmsg;
    msg() << MSG::DEBUG << "DeltaPhiCut         = " << m_MaxDPhi << endmsg;
    msg() << MSG::DEBUG << "DeltaRRange         = <" << m_MinDR << " - " << m_MaxDR << " >" << endmsg;
    msg() << MSG::DEBUG << "LowerMassCut         = " << m_lowerMassCut << endmsg;
    msg() << MSG::DEBUG << "UpperMassCut         = " << m_upperMassCut << endmsg;
    msg() << MSG::DEBUG << "OppositeCharge       = " 
	  << (m_oppositeCharge==true ? "True" : "False") << endmsg; 
    msg() << MSG::DEBUG << "CommonVertex         = " 
	  << (m_commonVertex==true ? "True" : "False") << endmsg; 
    
  }
  
  return HLT::OK;
}


HLT::ErrorCode TrigL2ElectronMuonAngleHypo::hltFinalize()
{
 if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endmsg;

  return HLT::OK;
}

HLT::ErrorCode TrigL2ElectronMuonAngleHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {

  const ElectronMuonTopoInfoContainer * EgMuTopoInfoColl = 0;
  
  
   m_monCut = 0;
  
  float mass=0.;
  float dphi=0.;
  float dr = 0.;  
  bool  oppositeCharge = false;
  int  VX_state = -1;
  
  
  if ( getFeature(outputTE, EgMuTopoInfoColl, m_inputLabel ) != HLT::OK || EgMuTopoInfoColl==0) {
    if ( msgLvl() <= MSG::WARNING) {
      msg() <<  MSG::WARNING << "No mass information found for this trigger element woth label " << m_inputLabel << endmsg;
    }
    return HLT::MISSING_FEATURE;  
  } else {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG  << "ElectronMuonTopoInfoContainer successfully retrieved" << endmsg;
  }
  
    
  
   if(m_acceptAll){
	pass = true;  
	if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Force accept! pass = " << pass << endmsg;
	return HLT::OK;    
   }
 
      pass=false;
  
      m_monCut = 1;
      
    if(msgLvl() <= MSG::VERBOSE) {
	  msg() << MSG::VERBOSE << "Got electronMuonTopoInfoCollection of the length " <<  EgMuTopoInfoColl->size() << endmsg;
	}  
  
	ElectronMuonTopoInfoContainer::const_iterator egMuTopoInfo;
	for ( egMuTopoInfo = EgMuTopoInfoColl->begin(); egMuTopoInfo != EgMuTopoInfoColl->end() && !pass; ++egMuTopoInfo ) {
		if((*egMuTopoInfo)==0) continue;
		mass=(*egMuTopoInfo)->InvMass(); // retrieve the stored info
		dphi=(*egMuTopoInfo)->DeltaPhi(); 
		dr = (*egMuTopoInfo)->DeltaR();
		oppositeCharge = (*egMuTopoInfo)->OppositeCharge();
		VX_state = (*egMuTopoInfo)->VertexState(); 
		if ( msgLvl() <= MSG::DEBUG ){
			msg() << MSG::DEBUG  << "Object no. " << egMuTopoInfo - EgMuTopoInfoColl->begin() << ": Reading in mass and distance information from address " << (*egMuTopoInfo) << "; " << *(*egMuTopoInfo) << endmsg;
		}	
    
	
  
  
	  
	   m_monDPhiAll=dphi;
	   m_monDRAll=dr;
	   m_monMassAll = mass;
	   
	  if(m_oppositeCharge && !oppositeCharge)
	  {
			if(msgLvl() <= MSG::VERBOSE) {
				msg() << MSG::VERBOSE << "Combination did not pass opposite charge cut" 
				<<  endmsg;
			}
			continue;	
	  }else{
			if(msgLvl() <= MSG::VERBOSE) {
				msg() << MSG::VERBOSE << "Combination passed opposite charge cut" 
				<<  endmsg;
			}
	  
	  }
	  
	  m_monCut = 2;
  
	  
	  if(m_commonVertex)
	   {
		  if(VX_state==1)
		  {
			if(msgLvl() <= MSG::VERBOSE) {
				msg() << MSG::VERBOSE << "Combination does not form common vertex." 
				<<  endmsg;
			}
			continue;
		  }else{
			if(msgLvl() <= MSG::VERBOSE) {
				msg() << MSG::DEBUG << "Combination passed common vertex cut with state " << VX_state
				<< endmsg; 
			}
		  }	
	   
	   } 
	  m_monCut = 3;
  
	  if(dphi > m_MaxDPhi) {
		  if(msgLvl() <= MSG::VERBOSE) {
	    msg() << MSG::VERBOSE << "Combination failed dPhi cut: " 
	    	<< dphi << " larger than " << m_MaxDPhi <<  endmsg;
	  }
		continue;
     }else{
		if(msgLvl() <= MSG::VERBOSE) {
		msg() << MSG::DEBUG << "Combination passed delta phi cut: " 
			<< dphi << " less than or equal to " << m_MaxDPhi <<  endmsg;
		  }
	 m_monDPhiAccepted=dphi;
	
	 }

	  
      m_monCut = 4;
      if(m_MaxDR > 1e-11){ 
      if(dr > m_MaxDR || dr < m_MinDR) {
		  if(msgLvl() <= MSG::VERBOSE) {
	    msg() << MSG::VERBOSE << "Combination failed dR cut: " 
	    	<< dr << " outside < " << m_MinDR << "," << m_MaxDR << ">" << endmsg;
	  }
		continue;
     }else{
		if(msgLvl() <= MSG::VERBOSE) {
		msg() << MSG::DEBUG << "Combination passed delta R cut: " 
		<< dr << " inside < " << m_MinDR << "," << m_MaxDR << ">" << endmsg;		  }
	 m_monDRAccepted=dr;
	
	 }
    }
      
      m_monCut = 5;
      // apply cut on mass
      if(mass<m_lowerMassCut || mass>m_upperMassCut) {

	if(msgLvl() <= MSG::VERBOSE) {
	  msg() << MSG::VERBOSE << "Combination failed mass cut: " 
		<< mass << " not in [" << m_lowerMassCut << "," 
		<< m_upperMassCut << "]" << endmsg;
	}
	continue;
	 } else {
	// good combination found
	pass = true;
	m_monCut = 6;    
	m_monMassAccepted = mass;
	if(msgLvl() <= MSG::VERBOSE) {
	  msg() << MSG::VERBOSE << "Combination passed mass cut: " 
		<< m_lowerMassCut << " < " << mass << " < " 
		<< m_upperMassCut << endmsg;	     
	  msg() << MSG::DEBUG << "Good combination found! Memu=" 
		<< mass << " CLHEP::MeV" << endmsg;
	}
      }
      
  }//end of loop over egMuTopoInfo objects
  
  
  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "pass = " << pass << endmsg;
  return HLT::OK;    

}
