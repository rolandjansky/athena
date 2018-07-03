// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaMuonCombHypo/TrigEFElectronMuonAngleHypo.h
 **
 **   Description: - Hypothesis algorithm: searc for pairs of electron and muons with 
 **                delta phi in some interval + some additional cuts; intended for a1->tautau->emu
 **                - Modified from TrigEFDielectronMassHypo and TrigEFDielectronMassFex by R Goncalo
 **
 **   Author: Pavel Jez <pavel.jez@cern.ch>
 **
 **   Created:   Jan 30, 2011
 **************************************************************************/ 


#include "TrigEgammaMuonCombHypo/TrigEFElectronMuonAngleFexAlgo.h"

#include "TrigEgammaMuonCombHypo/TrigEFElectronMuonAngleHypo.h"


class ISvcLocator;

TrigEFElectronMuonAngleHypo::TrigEFElectronMuonAngleHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator)
{
  // Read cuts
  declareProperty("AcceptAll",    m_acceptAll);
  declareProperty("CommonVertex", m_commonVertex=true);
  declareProperty("OppositeSign", m_oppositeCharge=true);
  declareProperty("MaxDRCut",  	  m_MaxDR=4.0); 
  declareProperty("MinDRCut",  	  m_MinDR=0.1); 
  declareProperty("MaxDPhiCut",   m_MaxDPhi=3.0); 
  declareProperty("LowerMassCut", m_lowerMassCut=5000.0);
  declareProperty("UpperMassCut", m_upperMassCut=15000.0);
    declareProperty("inputLabel", m_inputLabel="EF_EgMuTopoFEX");

  // monitoring
  declareMonitoredVariable("massOfAccepted", m_monMassAccepted);
  declareMonitoredVariable("massOfAll", m_monMassAll);  
  declareMonitoredVariable("cut", m_monCut);
  declareMonitoredVariable("dPhiOfAccepted", m_monDPhiAccepted);
  declareMonitoredVariable("dPhiOfAll", m_monDPhiAll);
  declareMonitoredVariable("dROfAccepted", m_monDRAccepted);
  declareMonitoredVariable("dROfAll", m_monDRAll);
  declareMonitoredVariable("Vx_state", m_monVxState);
  


}

TrigEFElectronMuonAngleHypo::~TrigEFElectronMuonAngleHypo()
{ }

HLT::ErrorCode TrigEFElectronMuonAngleHypo::hltInitialize()
{
  
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Initialization:" << endreq;
  }

  
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Initialization completed successfully:" << endreq;
    msg() << MSG::DEBUG << "Input collection label = <" << m_inputLabel  << endreq;
    msg() << MSG::DEBUG << "AcceptAll            = " 
	<< (m_acceptAll==true ? "True" : "False") << endreq; 
     msg() << MSG::DEBUG << "OppositeCharge       = " 
	  << (m_oppositeCharge==true ? "True" : "False") << endreq; 
	 msg() << MSG::DEBUG << "CommonVertex         = " 
	  << (m_commonVertex==true ? "True" : "False") << endreq; 
	msg() << MSG::DEBUG << "DeltaPhiCut         = " << m_MaxDPhi << endreq;
    msg() << MSG::DEBUG << "DeltaRRange         = <" << m_MinDR << " - " << m_MaxDR << " >" << endreq;
    msg() << MSG::DEBUG << "LowerMassCut         = " << m_lowerMassCut << endreq;
    msg() << MSG::DEBUG << "UpperMassCut         = " << m_upperMassCut << endreq;
   }
  
  return HLT::OK;
}


HLT::ErrorCode TrigEFElectronMuonAngleHypo::hltFinalize()
{
 if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endreq;

  return HLT::OK;
}

HLT::ErrorCode TrigEFElectronMuonAngleHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  
  const ElectronMuonTopoInfoContainer * EgMuTopoInfoColl = 0;
  
  
  m_monCut = 0;
  float mass=0.;
  float dphi=0.;
  float dr = 0.;  
  int VX_state = -1;
  bool  oppositeCharge = false;
  
  
  
   if ( getFeature(outputTE, EgMuTopoInfoColl) != HLT::OK || EgMuTopoInfoColl==0) {
    if ( msgLvl() <= MSG::WARNING) {
      msg() <<  MSG::WARNING << "No mass information found for this trigger element! " << endreq;
    }
    return HLT::MISSING_FEATURE;  
  } else {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG  << "ElectronMuonTopoInfoContainer successfully retrieved" << endreq;
  } 
  
   if(m_acceptAll){
	pass = true;  
	if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Force accept! pass = " << pass << endreq;
	return HLT::OK;    
   }
 
  
  
  pass=false;
  
      m_monCut = 1;
      
      if(msgLvl() <= MSG::DEBUG) {
	  msg() << MSG::DEBUG << "Got electronMuonTopoInfoCollection of the length " <<  EgMuTopoInfoColl->size() << endreq;
	}
      
      ElectronMuonTopoInfoContainer::const_iterator egMuTopoInfo;
	for ( egMuTopoInfo = EgMuTopoInfoColl->begin(); egMuTopoInfo != EgMuTopoInfoColl->end() && !pass; ++egMuTopoInfo ) {
		if((*egMuTopoInfo)==0) continue;
		mass=(*egMuTopoInfo)->InvMass(); // retrieve the stored info
		dphi=(*egMuTopoInfo)->DeltaPhi(); 
		dr = (*egMuTopoInfo)->DeltaR();
		VX_state = (*egMuTopoInfo)->VertexState();
		oppositeCharge = (*egMuTopoInfo)->OppositeCharge();
		
		if ( msgLvl() <= MSG::DEBUG ){
			msg() << MSG::DEBUG  << "Object no. " << egMuTopoInfo - EgMuTopoInfoColl->begin() << ": Reading in mass and distance information from address " << (*egMuTopoInfo) << "; " << *(*egMuTopoInfo) << endreq;
		}
      
       m_monDPhiAll=dphi;
	   m_monDRAll=dr;
	   m_monMassAll = mass;
	   m_monVxState = VX_state;
	   
	    if(m_oppositeCharge && !oppositeCharge)
	  {
			if(msgLvl() <= MSG::DEBUG) {
				msg() << MSG::DEBUG << "Combination did not pass opposite charge cut" 
				<<  endreq;
			}
			continue;	
	  }else{
			if(msgLvl() <= MSG::DEBUG) {
				msg() << MSG::DEBUG << "Combination passed opposite charge cut" 
				<<  endreq;
			}
	  
	  }
	   m_monCut = 2;
	   
	   if(m_commonVertex)
	   {
		  if(VX_state==1)
		  {
			if(msgLvl() <= MSG::DEBUG) {
				msg() << MSG::DEBUG << "Combination does not form common vertex." 
				<<  endreq;
			}
			continue;
		  }else{
			if(msgLvl() <= MSG::DEBUG) {
				msg() << MSG::DEBUG << "Combination passed common vertex cut with state " << VX_state
				<< endreq; 
			}
		  }	
	   
	   }
	   
	   m_monCut = 3;
	   
	   
	    // apply distance cut
       if(dphi > m_MaxDPhi) {
		  if(msgLvl() <= MSG::DEBUG) {
	    msg() << MSG::DEBUG << "Combination failed dPhi cut: " 
	    	<< dphi << " larger than " << m_MaxDPhi <<  endreq;
	  }
	    continue;
     }else{
		if(msgLvl() <= MSG::DEBUG) {
		msg() << MSG::DEBUG << "Combination passed delta phi cut: " 
			<< dphi << " less than or equal to " << m_MaxDPhi <<  endreq;
		  }
	 m_monDPhiAccepted=dphi;
	
	 }
        m_monCut = 4;
      
        if(m_MaxDR > 1e-11){ 
      if(dr > m_MaxDR || dr < m_MinDR) {
		  if(msgLvl() <= MSG::DEBUG) {
	    msg() << MSG::DEBUG << "Combination failed dR cut: " 
	    	<< dr << " outside < " << m_MinDR << "," << m_MaxDR << ">" << endreq;
	  }
		continue;
     }else{
		if(msgLvl() <= MSG::DEBUG) {
		msg() << MSG::DEBUG << "Combination passed delta R cut: " 
		<< dr << " inside < " << m_MinDR << "," << m_MaxDR << ">" << endreq;		  }
	 m_monDRAccepted=dr;
	
	 }
    }
      
      m_monCut = 5;
      
       
      // apply cut on mass
      if(mass<m_lowerMassCut || mass>m_upperMassCut) {
	if(msgLvl() <= MSG::DEBUG) {
	  msg() << MSG::DEBUG << "Combination failed mass cut: " 
		<< mass << " not in [" << m_lowerMassCut << "," 
		<< m_upperMassCut << "]" << endreq;
	}
      } else {
	// good combination found
	pass = true;
	m_monCut = 6;    
	m_monMassAccepted = mass;
	if(msgLvl() <= MSG::DEBUG) {
	  msg() << MSG::DEBUG << "Combination passed mass cut: " 
		<< m_lowerMassCut << " < " << mass << " < " 
		<< m_upperMassCut << endreq;	     
	  msg() << MSG::DEBUG << "Good combination found! Memu=" 
		<< mass << " CLHEP::MeV" << endreq;
	}
      }
      

		
	}//end of loop over ElectronMuonInfo objects
	
	// set output TriggerElement true if good combination
		if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "pass = " << pass << endreq;
  return HLT::OK;    
}
