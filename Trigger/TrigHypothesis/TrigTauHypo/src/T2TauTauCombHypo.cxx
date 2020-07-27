/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 ** File: Trigger/TrigHypothesis/TrigTauHypo/TrigL2TauTauCombHypo.h
 **
 ** Description: HYPO algorithm: search for pairs of taus with delta eta in some interval +
 **              some additional cuts; intended for H-> tau tau SM and MSSM
 **
 ** Author: Phillip Urquijo <Phillip.Urquijo@cern.ch>
 ** Created: August 1
 **************************************************************************/

#include "T2TauTauCombFexAlgo.h"
#include "T2TauTauCombHypo.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"


// class ISvcLocator;

T2TauTauCombHypo::T2TauTauCombHypo(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator),
  m_MaxDR(4.0),
  m_MinDR(0),
  m_MaxDPhi(1.0),
  m_lowerMassCut(0),
  m_upperMassCut(10.),
  m_inputLabel("TauTauTopoFEX"),
  m_acceptAll(false),
  m_monMassAccepted(0),
  m_monMassAll(0),
  m_monCut(0),
  m_monDPhiAccepted(0),
  m_monDPhiAll(0),
  m_monDRAccepted(0),
  m_monDRAll(0),
  m_oppositeCharge(0),
  m_commonVertex(0), 
  m_moncombtype(0)
{

  //Read cuts
  declareProperty("AcceptAll",    m_acceptAll=false);
  declareProperty("MaxDRCut",     m_MaxDR=4.0); 
  declareProperty("MinDRCut",     m_MinDR=0.0); 
  declareProperty("MaxDPhiCut",   m_MaxDPhi=1.0); 
  declareProperty("MaxDEtaCut",   m_MaxDEta=2.5); 
  declareProperty("LowerMassCut", m_lowerMassCut=2000.0);
  declareProperty("UpperMassCut", m_upperMassCut=20000.0);
  declareProperty("OppositeSign", m_oppositeCharge=true);
  declareProperty("CommonVertex", m_commonVertex=true);
  declareProperty("inputLabel",   m_inputLabel="TauTauTopoFEX");
  // monitoring
  declareMonitoredVariable("massOfAccepted", m_monMassAccepted);
  declareMonitoredVariable("massOfAll", m_monMassAll); 
  declareMonitoredVariable("cut", m_monCut);
  declareMonitoredVariable("dPhiOfAccepted", m_monDPhiAccepted);
  declareMonitoredVariable("dEtaOfAll", m_monDEtaAll);
  declareMonitoredVariable("dEtaOfAccepted", m_monDEtaAccepted);
  declareMonitoredVariable("dPhiOfAll", m_monDPhiAll);
  declareMonitoredVariable("dROfAccepted", m_monDRAccepted);
  declareMonitoredVariable("dROfAll", m_monDRAll);
  declareMonitoredVariable("combtype",           m_moncombtype);

}

T2TauTauCombHypo::~T2TauTauCombHypo()
{  }

HLT::ErrorCode T2TauTauCombHypo::hltInitialize()
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
//     msg() << MSG::DEBUG << "OppositeCharge       = " 
//           << (m_oppositeCharge==true ? "True" : "False") << endmsg; 
//     msg() << MSG::DEBUG << "CommonVertex         = " 
//           << (m_commonVertex==true ? "True" : "False") << endmsg; 
  }

  return HLT::OK;

}

HLT::ErrorCode T2TauTauCombHypo::hltFinalize()
{
  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endmsg;
  
  return HLT::OK;
}

HLT::ErrorCode T2TauTauCombHypo::hltExecute(const HLT::TriggerElement* inputTE, bool& pass) {
  
  const ElectronMuonTopoInfoContainer * TauTauTopoInfoColl = 0;
  
  
  m_monCut = 0;
  
  float mass=0.;
  float dphi=0.;
  float deta=0.;
  float dr = 0.; 
  // bool  oppositeCharge = false;
  int  VX_state = -1;
  
  
  if ( getFeature(inputTE, TauTauTopoInfoColl, m_inputLabel ) != HLT::OK || TauTauTopoInfoColl==0) {
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
    msg() << MSG::VERBOSE << "Got electronMuonTopoInfoCollection of the length " <<  TauTauTopoInfoColl->size() << endmsg;
  } 
  
  ElectronMuonTopoInfoContainer::const_iterator tautauTopoInfo;
  for ( tautauTopoInfo = TauTauTopoInfoColl->begin(); tautauTopoInfo != TauTauTopoInfoColl->end() && !pass; ++tautauTopoInfo ) {
    if((*tautauTopoInfo)==0) continue;
    mass=(*tautauTopoInfo)->InvMass(); // retrieve the stored info
    dphi=(*tautauTopoInfo)->DeltaPhi(); 
    dr = (*tautauTopoInfo)->DeltaR();
    deta = sqrt( fabs(dr*dr - dphi*dphi));

    // oppositeCharge = (*tautauTopoInfo)->OppositeCharge();
    VX_state = (*tautauTopoInfo)->VertexState(); 
    if ( msgLvl() <= MSG::DEBUG ){
      msg() << MSG::DEBUG  << "Object no. " << tautauTopoInfo - TauTauTopoInfoColl->begin() << ": Reading in mass and distance information from address " << (*tautauTopoInfo) << "; " << *(*tautauTopoInfo) << endmsg;
    }       
    
    m_monDPhiAll=dphi;
    m_monDRAll=dr;
    m_monDEtaAll=deta;
    m_monMassAll = mass;
    m_moncombtype=0;
    if(VX_state==1515)
      m_moncombtype=1;   
    if(VX_state==1513)
      m_moncombtype=2;       
    if(VX_state==1511)
      m_moncombtype=3;       


//     if(m_oppositeCharge && !oppositeCharge)
//       {
// 	if(msgLvl() <= MSG::VERBOSE) {
// 	  msg() << MSG::VERBOSE << "Combination did not pass opposite charge cut" 
// 		<<  endmsg;
// 	}
// 	continue;       
//       }else{
//       if(msgLvl() <= MSG::VERBOSE) {
// 	msg() << MSG::VERBOSE << "Combination passed opposite charge cut" 
// 	      <<  endmsg;
//       }
      
//     }
    
    m_monCut = 2;
    
    if(deta > m_MaxDEta) {
      if(msgLvl() <= MSG::VERBOSE) {
	msg() << MSG::VERBOSE << "Combination failed dEta cut: " 
	      << deta << " larger than " << m_MaxDEta <<  endmsg;
      }
      continue;
    }else{
      if(msgLvl() <= MSG::VERBOSE) {
	msg() << MSG::DEBUG << "Combination passed delta Eta cut: " 
	      << deta << " less than or equal to " << m_MaxDEta <<  endmsg;
      }
      m_monDEtaAccepted=deta;
      
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
		<< dr << " inside < " << m_MinDR << "," << m_MaxDR << ">" << endmsg;              }
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
    
  }//end of loop over tautauTopoInfo objects
  
  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "pass = " << pass << endmsg;
  return HLT::OK;   
  
}
