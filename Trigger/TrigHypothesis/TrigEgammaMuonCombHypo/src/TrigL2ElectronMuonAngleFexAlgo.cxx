// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaMuonCombHypo/TrigL2ElectronMuonAngleFexAlgo.h
 **
 **   Description: - FEX algorithm: searc for pairs of electron and muons with 
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


#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigEgammaMuonCombHypo/TrigL2ElectronMuonAngleFexAlgo.h"
#include "CxxUtils/sincos.h"

#include <math.h>


//class ISvcLocator;

TrigL2ElectronMuonAngleFexAlgo::TrigL2ElectronMuonAngleFexAlgo(const std::string & name, ISvcLocator* pSvcLocator)
  : HLT::ComboAlgo(name, pSvcLocator),
    m_egMuTopoColl(0)

  {

  // Read cuts
  declareProperty("AcceptAll",    m_acceptAll);
  declareProperty("ValidElectron",m_electronValid=true);
  declareProperty("MaxDRCut",  	  m_MaxDR=4.0); 
  declareProperty("MaxDPhiCut",   m_MaxDPhi=4.0); 
  
  declareProperty("LowerMassCut", m_lowerMassCut=0.0);
  declareProperty("UpperMassCut", m_upperMassCut=50000.0);
  
  // monitoring
  declareMonitoredVariable("massOfAccepted", m_monMassAccepted);
  declareMonitoredVariable("massOfAll", m_monMassAll);  
  declareMonitoredVariable("cut", m_monCut);
  declareMonitoredVariable("dPhiOfAccepted", m_monDPhiAccepted);
  declareMonitoredVariable("dPhiOfAll", m_monDPhiAll);
  declareMonitoredVariable("dROfAccepted", m_monDRAccepted);
  declareMonitoredVariable("dROfAll", m_monDRAll);
  m_DPhi=m_DR = 100.;  
  m_mass=-1;
  
}

TrigL2ElectronMuonAngleFexAlgo::~TrigL2ElectronMuonAngleFexAlgo()
{ }

HLT::ErrorCode TrigL2ElectronMuonAngleFexAlgo::hltInitialize()
{
  
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Initialization:" << endmsg;
  }
   
  
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Initialization completed successfully:" << endmsg;
    msg() << MSG::DEBUG << "AcceptAll            = " 
	  << (m_acceptAll==true ? "True" : "False") << endmsg; 
    msg() << MSG::DEBUG << "ValidElectron        = " 
	  << (m_electronValid==true ? "True" : "False") << endmsg; 
    msg() << MSG::DEBUG << "DeltaPhiCut         = " << m_MaxDPhi << endmsg;
    msg() << MSG::DEBUG << "DeltaPhiCut         = " << m_MaxDPhi << endmsg;
    msg() << MSG::DEBUG << "MaxDeltaRCut         = " << m_MaxDR << endmsg;
    msg() << MSG::DEBUG << "LowerMassCut         = " << m_lowerMassCut << endmsg;
    msg() << MSG::DEBUG << "UpperMassCut         = " << m_upperMassCut << endmsg;
  }
  
  return HLT::OK;
}


HLT::ErrorCode TrigL2ElectronMuonAngleFexAlgo::hltFinalize()
{
  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endmsg;

  return HLT::OK;
}

//HLT::ErrorCode TrigL2ElectronMuonAngleFexAlgo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
HLT::ErrorCode TrigL2ElectronMuonAngleFexAlgo::acceptInputs(HLT::TEConstVec& inputTE, bool& pass ) {
  
  m_monMassAccepted = -999.;
  m_monCut=0;
  m_egMuTopoColl=0;
  
  // sanity checks
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "Running TrigL2ElectronMuonAngleFexAlgo::acceptInputs" << endmsg;

  if ( inputTE.size() != 2 ) {
    msg() << MSG::ERROR << "Got diferent than 2 number of input TEs: " <<  inputTE.size() << " job badly configured" << endmsg;
    return HLT::BAD_JOB_SETUP;
  }



  // Accept-All mode: temporary patch; should be done with force-accept 
  if (m_acceptAll) {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "AcceptAll property is set: taking all events" 
	    << endmsg;
    
    pass = true;
    return HLT::OK;
  } 
  else {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "AcceptAll property not set: applying selection" 
	    << endmsg;
  }

  // this are 2 TEs which we eventually will combine
  const HLT::TriggerElement* te1 = inputTE[0];
  const HLT::TriggerElement* te2 = inputTE[1];

  // for debugging purpose look into RoIDescriptors
  if ( msgLvl() <= MSG::DEBUG ){
    const TrigRoiDescriptor* roiDescriptor1 = 0;
    const TrigRoiDescriptor* roiDescriptor2 = 0;
    if ( getFeature(te1, roiDescriptor1) != HLT::OK || getFeature(te2, roiDescriptor2) != HLT::OK || roiDescriptor1==0 || roiDescriptor2==0) {
      if ( msgLvl() <= MSG::WARNING) {
	msg() <<  MSG::WARNING << "No RoIDescriptors for this Trigger Elements! " << endmsg;
      }
      return HLT::MISSING_FEATURE;  
    } else {
      if ( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG  << "Trying to combine 2 RoIs: " << *roiDescriptor1 << " & " << *roiDescriptor2 << endmsg;
    }
  }
  
  // retrieve TrigElectronContainers from this TE
  const TrigElectronContainer* electronContainer1(0);
  std::vector<const CombinedMuonFeature*> muonContainer1;

 // const TrigElectronContainer* electronContainer2(0);
  if ( getFeature(te1, electronContainer1) != HLT::OK || electronContainer1 == 0)
	{ 
		if(getFeature(te2, electronContainer1) != HLT::OK  || electronContainer1 == 0) 
        {
			
			if ( msgLvl() <= MSG::WARNING) {
			msg() << MSG::WARNING << "Failed to get TrigElectron collection" << endmsg;
			}
			return HLT::MISSING_FEATURE;
		}
		else{
			if ( msgLvl() <= MSG::DEBUG )
				msg() << MSG::DEBUG  << "TrigElectron collection successfully retrieved" << endmsg; 	
			} 
    
  }else{
	if ( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG  << "TrigElectron collection successfully retrieved" << endmsg; 	
  }
  // retrieving muons
  if ( getFeatures(te1, muonContainer1) != HLT::OK || muonContainer1.size() == 0)
	{ 
		if(getFeatures(te2, muonContainer1) != HLT::OK  || muonContainer1.size() == 0) 
        {
			
			if ( msgLvl() <= MSG::WARNING) {
			msg() << MSG::WARNING << "Failed to get MuonCombinedFeature collection" << endmsg;
			}
			return HLT::MISSING_FEATURE;
		}
		else{
			if ( msgLvl() <= MSG::DEBUG )
				msg() << MSG::DEBUG  << "MuonCombinedFeature collection successfully retrieved" << endmsg; 	
			} 
    
  }else{
	if ( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG  << "MuonCombinedFeature collection successfully retrieved" << endmsg; 	
  }
  
  if(m_acceptAll){
	pass = true;  
	if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Force accept! pass = " << pass << endmsg;
	return HLT::OK;    
   }  


  pass=false;

  // now we have 2 bunches of leptons and we need to find out whether they can form topological combination
  // loop over all combinations
  TrigElectronContainer::const_iterator electron1;
  std::vector<const CombinedMuonFeature*>::iterator muon1;
  for ( electron1 = electronContainer1->begin(); electron1 != electronContainer1->end(); ++electron1 ) {
	  if((*electron1)==0)
      {
		  msg() << MSG::WARNING << "Null pointer in TrigElectronContainer. Skipping." << endmsg;
		  continue;	
	  }
    for ( muon1 = muonContainer1.begin(); muon1 != muonContainer1.end(); ++muon1 ) {
      m_monCut = 1;
      
       if((*muon1)==0)
		{
			msg() << MSG::WARNING << "Null pointer in CombinedMuonFeature vector. Skipping." << endmsg;
			continue;	
		}
      
         if((*muon1)->IDTrack()==0)
		{
			if(msgLvl() <= MSG::DEBUG) {
				msg() << MSG::DEBUG << "Null pointer to IDTrack from muon. Skipping." << endmsg;
			}
			continue;	
		}
      // selection is done here
      //
      // debug dump (both leptons have tracks now)
      
      ElectronMuonTopoInfo* EgMuTopoInfo = new ElectronMuonTopoInfo();
  

      if(msgLvl() <= MSG::VERBOSE) {
	msg() << MSG::VERBOSE << "New combination:" << endmsg; 
	msg() << MSG::VERBOSE << "1st TrigElectron: addr=" << (*electron1)
	      << ((*electron1)->isValid() ? "(valid" : "(not valid")
	      << ") RoI id="<< (*electron1)->roiId()
	      << "; pt="    << (*electron1)->pt()  
	      << "; eta="   << (*electron1)->eta() <<"+-"<< (*electron1)->err_eta() 
	      << "; phi="   << (*electron1)->phi() <<"+-"<< (*electron1)->err_phi() 
	      << "; Zvtx="  << (*electron1)->Zvtx()<<"+-"<< (*electron1)->err_Zvtx() 
	      << endmsg;
	msg() << MSG::VERBOSE << "1st CombinedMuonFeature: addr=" << (*muon1)
          << " pt=" << 1/(*muon1)->ptq() << " trkAddr=" << (*muon1)->IDTrack() 
          << " " << *(*muon1)
          << "; Zvtx =" << (*muon1)->IDTrack()->param()->z0() << "+-" << (*muon1)->IDTrack()->param()->ez0()
          << endmsg;
      }
      EgMuTopoInfo->SetRoiWord((*electron1)->roiId());
       
 

      // check we're starting from valid TrigElectrons (i.e. which 
      // passed all cuts)
      if (m_electronValid) {
	
	if ( !(*electron1)->isValid() ) {
	  
	  if(msgLvl() <= MSG::VERBOSE) {
	    msg() << MSG::VERBOSE << "TrigElectron not valid: rejecting combination" << endmsg;
	  }	    
	  delete EgMuTopoInfo;
	  continue;  // reject non-valid TrigElectrons if cut is on
	}
      }
      m_monCut = 2;
      EgMuTopoInfo->SetElecValid(true);    

      m_DPhi=m_DR = 100.; // setting value which can never pass
      
      m_DPhi = EgMuTopoInfo->deltaPhi((*electron1),(*muon1));
      m_DR = EgMuTopoInfo->deltaR((*electron1),(*muon1));
      
      if(msgLvl() <= MSG::VERBOSE) {
	    msg() << MSG::VERBOSE << "Delta Phi between muon and electron is " << m_DPhi << 
	    " and radial distance delta R is " << m_DR << endmsg;
	  }
	  
	  m_monDPhiAll=m_DPhi;
	  m_monDRAll=m_DR;
	  
	  
	  if(m_DPhi > m_MaxDPhi) {
		  if(msgLvl() <= MSG::VERBOSE) {
	    msg() << MSG::VERBOSE << "Combination failed Delta Phi cut" << endmsg;
	  }
		delete EgMuTopoInfo;
		continue;
     }else{
		if(msgLvl() <= MSG::VERBOSE) {
	    msg() << MSG::VERBOSE << "Combination passed Delta Phi cut of " << m_MaxDPhi << endmsg;
	  }
	 }
	 
	  m_monDPhiAccepted = m_DPhi;
      m_monCut = 3;
      
      EgMuTopoInfo->SetDeltaPhi(m_DPhi);
      
	 
	 if(m_MaxDR > 1e-11)
	 {
		if(m_DR > m_MaxDR) {
		  if(msgLvl() <= MSG::VERBOSE) {
	    msg() << MSG::VERBOSE << "Combination failed Delta R cut" << endmsg;
	  }
		delete EgMuTopoInfo;
		continue;
     }else{
		if(msgLvl() <= MSG::VERBOSE) {
	    msg() << MSG::VERBOSE << "Combination passed Delta R cut of " << m_MaxDR << endmsg;
	  }
	 }	 
	 }
	 
	  m_monDRAccepted = m_DR;
      m_monCut = 4;
      
      EgMuTopoInfo->SetDeltaR(m_DR);
      
	 
      // check charge: track pT is signed according to charge but not electron pT
      // should study how well charge can actually be estimated for high-pT tracks...
      
	// note: track charge can be zero (unknown) so must use > and not >=
	if ( ! EgMuTopoInfo->opositeCharge(*electron1, *muon1) ) {
	  if(msgLvl() <= MSG::VERBOSE) {
	    msg() << MSG::VERBOSE << "Combination does not have opposite charge" << endmsg;
	    EgMuTopoInfo->SetOppositeCharge(false);

	  }
	} else {
	  if(msgLvl() <= MSG::VERBOSE) {
	    msg() << MSG::VERBOSE << "Combination has opposite charge" 
		  << endmsg;
		  EgMuTopoInfo->SetOppositeCharge(true);
	  }
	}
           
      

      // check vertex position: two electrons should come from common 
      // vertex check only Z of vertex, and only ef Z error is not zero
      // take track z at perigee as estimate of vertex z, since lateral 
      // coordinates are small this should be done better with a vertexing
      // algorithm...
   
	const ElectronMuonTopoInfo::Vertex vx = EgMuTopoInfo->commonVertex(*electron1, *muon1);
	
	if ( vx == ElectronMuonTopoInfo::NotCommon ) {
	  if (msgLvl() <= MSG::VERBOSE) {
	    msg() << MSG::VERBOSE 
		  << "Combination failed vertex cut" 
		  << endmsg;
	  }
	  
	} else  if ( vx == ElectronMuonTopoInfo::Common ) {
	  if (msgLvl() <= MSG::VERBOSE) {
	    msg() << MSG::VERBOSE 
		  << "Combination passed vertex cut" << endmsg;
	  }
	} else {
	  if(msgLvl() <= MSG::VERBOSE) {
	    msg() << MSG::VERBOSE 
		  << "Bad track vertex errors: not applying cut" << endmsg;
	  }
	}
	   EgMuTopoInfo->SetVertexState(vx);
   
      
      // evaluate mass
      m_mass = EgMuTopoInfo->invariantMass((*electron1),(*muon1));
      m_monMassAll = m_mass; 
      // apply cut on mass
      if(m_mass<m_lowerMassCut || m_mass>m_upperMassCut) {

	if(msgLvl() <= MSG::VERBOSE) {
	  msg() << MSG::VERBOSE << "Combination failed mass cut: " 
		<< m_mass << " not in [" << m_lowerMassCut << "," 
		<< m_upperMassCut << "]" << endmsg;
	}
	delete EgMuTopoInfo;
	continue;
      } else {
	// good combination found
	pass = true;
	m_monCut = 5;    
	EgMuTopoInfo->SetInvMass(m_mass);
	m_monMassAccepted = m_mass;
	if(msgLvl() <= MSG::VERBOSE) {
	  msg() << MSG::VERBOSE << "Combination passed mass cut: " 
		<< m_lowerMassCut << " < " << m_mass << " < " 
		<< m_upperMassCut << endmsg;	     
	  msg() << MSG::DEBUG << "Good combination found! Memu=" 
		<< m_mass << " CLHEP::MeV" << endmsg;
	}
      }   
	
	if(msgLvl() <= MSG::DEBUG){
		msg() << MSG::DEBUG << "Created following object: "
		<< (*EgMuTopoInfo)
		<< endmsg;
	}
	
	 if ( !m_egMuTopoColl ) m_egMuTopoColl = new ElectronMuonTopoInfoContainer();
		m_egMuTopoColl->push_back(new ElectronMuonTopoInfo(*EgMuTopoInfo));
		delete EgMuTopoInfo;
	
    } // electrons2 container loop end
  } // electrons1 container loop end
  // set output TriggerElement true if good combination
  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "pass = " << pass << endmsg;
  return HLT::OK;    
}  

HLT::ErrorCode TrigL2ElectronMuonAngleFexAlgo::hltExecute(HLT::TEConstVec& /*inputTE*/, HLT::TriggerElement* outputTE){
   if(msgLvl() <= MSG::VERBOSE) {
	  msg() << MSG::VERBOSE << "Attaching electronMuonTopoInfoCollection of the length " <<  m_egMuTopoColl->size() << endmsg;
	}
   if(m_egMuTopoColl!=0) attachFeature(outputTE, m_egMuTopoColl, "EgMuTopoFEX"); 
  
  return HLT::OK;
}


