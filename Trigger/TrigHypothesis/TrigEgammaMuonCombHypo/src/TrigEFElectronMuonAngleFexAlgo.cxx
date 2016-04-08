// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaMuonCombHypo/TrigEFElectronMuonAngleFexAlgo.h
 **
 **   Description: - FEX algorithm: searc for pairs of electron and muons with 
 **                delta phi in some interval + some additional cuts; intended for a1->tautau->emu
 **                - Modified from TrigEFDielectronMassHypo and TrigEFDielectronMassFex by R Goncalo 
			and TrigEFDiMuFex by E. Reinherz-Aronis and A. Kreisel
 **
 **   Author: Pavel Jez <pavel.jez@cern.ch>
 **
 **   Created:   Jan 30, 2011
 **
 **************************************************************************/ 


#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigEgammaMuonCombHypo/TrigEFElectronMuonAngleFexAlgo.h"


#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "MuidEvent/MuidTrackContainer.h"

#include "VxVertex/RecVertex.h"
#include "ITrackToVertex/ITrackToVertex.h"


#include <math.h>


TrigEFElectronMuonAngleFexAlgo::TrigEFElectronMuonAngleFexAlgo(const std::string & name, ISvcLocator* pSvcLocator)
  : HLT::ComboAlgo(name, pSvcLocator), 
    m_primaryVertex(Amg::Vector3D()),
   m_trackToVertexTool("Reco::TrackToVertex"),
   m_electron(0), m_muon(0),
   m_egMuTopoColl(0)
  {

  // Read cuts
  declareProperty("AcceptAll",    m_acceptAll);
  declareProperty("UseRecommended", m_useRecommended=true);
  declareProperty("MaxDRCut",  	  m_MaxDR=4.0); 
  declareProperty("MaxDPhiCut",   m_MaxDPhi=4.0); 
  
  declareProperty("LowerMassCut", m_lowerMassCut=0.0);
  declareProperty("UpperMassCut", m_upperMassCut=50000.0);
  
  declareProperty("trackToVertexTool", m_trackToVertexTool,  "Tool for track extrapolation to vertex"); 

  
  
  // monitoring
  declareMonitoredVariable("massOfAccepted", m_monMassAccepted);
  declareMonitoredVariable("massOfAll", m_monMassAll);  
  declareMonitoredVariable("cut", m_monCut);
  declareMonitoredVariable("dPhiOfAccepted", m_monDPhiAccepted);
  declareMonitoredVariable("dPhiOfAll", m_monDPhiAll);
  declareMonitoredVariable("dROfAccepted", m_monDRAccepted);
  declareMonitoredVariable("dROfAll", m_monDRAll);
  declareMonitoredVariable("Z0pull", m_monPull);
  declareMonitoredVariable("Vx_state", m_monVxState);
  m_DPhi=m_DR = 100.;  
  m_mass=-1;
  
  
    
}

TrigEFElectronMuonAngleFexAlgo::~TrigEFElectronMuonAngleFexAlgo()
{ }

HLT::ErrorCode TrigEFElectronMuonAngleFexAlgo::hltInitialize()
{
  
  if (msgLvl() <= MSG::VERBOSE) {
    msg() << MSG::DEBUG << "Initialization:" << endreq;
  }

  
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Initialization completed successfully:" << endreq;
    msg() << MSG::DEBUG << "AcceptAll            = " 
	  << (m_acceptAll==true ? "True" : "False") << endreq; 
    msg() << MSG::DEBUG << "Using Egamma 2010 recommendations   = " 
	  << (m_useRecommended==true ? "True" : "False") << endreq; 
	    
    msg() << MSG::DEBUG << "DeltaPhiCut         = " << m_MaxDPhi << endreq;
    msg() << MSG::DEBUG << "MaxDeltaRCut         = " << m_MaxDR << endreq;
    msg() << MSG::DEBUG << "LowerMassCut         = " << m_lowerMassCut << endreq;
    msg() << MSG::DEBUG << "UpperMassCut         = " << m_upperMassCut << endreq;
   }
  
  return HLT::OK;
}


HLT::ErrorCode TrigEFElectronMuonAngleFexAlgo::hltFinalize()
{
  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endreq;

  return HLT::OK;
}

//HLT::ErrorCode TrigEFElectronMuonAngleFexAlgo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
HLT::ErrorCode TrigEFElectronMuonAngleFexAlgo::acceptInputs(HLT::TEConstVec& inputTE, bool& pass ) {
  
  m_monMassAccepted = -999.;
  m_monCut=0;
  // reseting pointers
  m_egMuTopoColl=0;
  m_electron=0;
  m_muon=0;
  electronCollection.clear();
  muonCollection.clear();
  // sanity checks
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "Running TrigEFElectronMuonAngleFexAlgo::acceptInputs" << endreq;

  if ( inputTE.size() != 2 ) {
    msg() << MSG::ERROR << "Got diferent than 2 number of input TEs: " <<  inputTE.size() << " job badly configured" << endreq;
    return HLT::BAD_JOB_SETUP;
  }



  // Accept-All mode: temporary patch; should be done with force-accept 
  if (m_acceptAll) {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "AcceptAll property is set: taking all events" 
	    << endreq;
    
    pass = true;
    return HLT::OK;
  } 
  else {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "AcceptAll property not set: applying selection" 
	    << endreq;
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
	msg() <<  MSG::WARNING << "No RoIDescriptors for this Trigger Elements! " << endreq;
      }
      return HLT::MISSING_FEATURE;  
    } else {
      if ( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG  << "Trying to combine 2 RoIs: " << *roiDescriptor1 << " & " << *roiDescriptor2 << endreq;
    }
  }
  
  // retrieve TrigElectronContainers from this TE
  const egammaContainer* electronContainer1(0);
  const MuidTrackContainer* MuEFTracksEF;

  
  if ( getFeature(te1, electronContainer1) != HLT::OK || electronContainer1 == 0)
	{ 
		if(getFeature(te2, electronContainer1) != HLT::OK  || electronContainer1 == 0) 
        {
			
			if ( msgLvl() <= MSG::WARNING) {
			msg() << MSG::WARNING << "Failed to get EF egamma collection" << endreq;
			}
			return HLT::MISSING_FEATURE;
		}
		else{
			if ( msgLvl() <= MSG::DEBUG )
				msg() << MSG::DEBUG  << "EF egamma collection successfully retrieved" << endreq; 	
			} 
    
  }else{
	if ( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG  << "EF egamma collection successfully retrieved" << endreq; 	
  }
 
  
  if ( getFeature(te1, MuEFTracksEF, "MuonCombEF") != HLT::OK || MuEFTracksEF == 0)
	{ 
		if(getFeature(te2, MuEFTracksEF, "MuonCombEF") != HLT::OK  || MuEFTracksEF == 0) 
        {
			
			if ( msgLvl() <= MSG::WARNING) {
			msg() << MSG::WARNING << "Failed to get EF MuidTrackContainer" << endreq;
			}
			return HLT::MISSING_FEATURE;
		}
		else{
			if ( msgLvl() <= MSG::DEBUG )
				msg() << MSG::DEBUG  << "EF MuidTrackContainer successfully retrieved with size " << MuEFTracksEF->size() << endreq; 	
			} 
    
  }else{
	if ( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG  << "EF MuidTrackContainer successfully retrieved with size " << MuEFTracksEF->size() << endreq; 	
  }	
  
  


  pass=false;

  // now we have bunch of electrons and muons and we need to find out whether they can form topological combination
  // loop ver all combinations
  egammaContainer::const_iterator electron1;
  MuidTrackContainer::const_iterator tr;
  if(msgLvl() <= MSG::VERBOSE) {
	msg() << MSG::VERBOSE << "Size of electron container: " << electronContainer1->size() << " size of muon container " << MuEFTracksEF->size() << endreq; 
   }
  int mu_count,el_count;
  mu_count=el_count=0; 
  for ( electron1 = electronContainer1->begin(); electron1 != electronContainer1->end(); ++electron1 ) {
    
    if(msgLvl() <= MSG::VERBOSE) {
		el_count++;
		mu_count=0;
	}
	
	if((*electron1)==0)
      {
		  msg() << MSG::WARNING << "Null pointer in egammaContainer. Skipping." << endreq;
		  continue;	
	  }
	  
	  if((*electron1)->trackParticle()==0)
      {
			if(msgLvl() <= MSG::DEBUG) {
				msg() << MSG::DEBUG << "No trackParticle attached to electron. Skipping" << endreq; 
			}
			continue;
	  }
    
    for ( tr = MuEFTracksEF->begin(); tr != MuEFTracksEF->end(); ++tr ) {
      if((*tr)==0)
      {
		  msg() << MSG::WARNING << "Null pointer in MuidTrackContainer. Skipping." << endreq;
		  continue;	
	  }
	  if((*tr)->indetTrack()==0)
	  {
			if(msgLvl() <= MSG::DEBUG) {
				msg() << MSG::DEBUG << "No indetTrack attached to muon. Skipping" << endreq; 
			}
			continue;
	  }
      
      if(msgLvl() <= MSG::VERBOSE) {
		mu_count++;
		msg() << MSG::VERBOSE << "Processing electron no. " << el_count << " and muon no. " << mu_count << endreq; 
	   }
      
      const Trk::Perigee* muon1 = (*tr)->indetTrack()->perigeeParameters();

     
      m_monCut = 0;
      // selection is done here
      //
      // debug dump 
      
      if(( (*electron1)->trackParticle()->trackSummary()->get( Trk::numberOfPixelHits )+ (*electron1)->trackParticle()->trackSummary()->get( Trk::numberOfSCTHits )) < 4 )
	  {
		 msg() << MSG::WARNING << " Electron has less then 4 Silicon hits! Skipping " << endreq;
		 continue;
	  }
	
	  m_monCut = 1;
      		
	  ElectronMuonTopoInfo* EgMuTopoInfo = new ElectronMuonTopoInfo();
	  EgMuTopoInfo->SetDeltaPhi(float(-999.0));
	  EgMuTopoInfo->SetDeltaR(float(-999.0));
	  EgMuTopoInfo->SetInvMass(float(-999.0));
	  EgMuTopoInfo->SetOppositeCharge(false);
	  EgMuTopoInfo->SetVertexState(2);

       float mu_phi =  muon1->parameters()[Trk::phi];
      
	  if(msgLvl() <= MSG::VERBOSE) {
	msg() << MSG::VERBOSE << "New combination:" << endreq; 
	msg() << MSG::VERBOSE << "egammaElectron: addr=" << (*electron1) 
	      << "  et="    << (*electron1)->cluster()->e()/cosh((*electron1)->trackParticle()->eta())  
	      << "; eta="   << (*electron1)->trackParticle()->eta() 
	      << "; phi="   << (*electron1)->trackParticle()->phi() 
	      << "; charge=" << (*electron1)->trackParticle()->charge()
	      << "; #Silicon hits=" << 
	      (*electron1)->trackParticle()->trackSummary()->get( Trk::numberOfPixelHits )+ (*electron1)->trackParticle()->trackSummary()->get( Trk::numberOfSCTHits )
	      << endreq;	      
	msg() << MSG::VERBOSE << "muon: addr=" << muon1 
	      << "  et="    << muon1->pT()  
	      << "; eta="   << muon1->eta()                 
	      << "; phi="   << mu_phi
	      << "; charge=" << muon1->charge()
	      << endreq;
      }

       
      // evaluate distance
      m_DPhi=m_DR = 100.; // setting value which can never pass
      
      m_DPhi = EgMuTopoInfo->deltaPhi((*electron1),muon1);
      m_DR = EgMuTopoInfo->deltaR((*electron1),muon1);
      
      
      if(msgLvl() <= MSG::VERBOSE) {
	    msg() << MSG::VERBOSE << "Delta Phi between muon and electron is " << m_DPhi << 
	    " and radial distance delta R is " << m_DR << endreq;
	  }
	  
	  m_monDPhiAll=m_DPhi;
	  m_monDRAll=m_DR;
	  
	  
	  if(m_DPhi > m_MaxDPhi) {
		  if(msgLvl() <= MSG::VERBOSE) {
	    msg() << MSG::VERBOSE << "Combination failed Delta Phi cut" << endreq;
	  }
		delete EgMuTopoInfo;
		continue;		
     }else{
		if(msgLvl() <= MSG::VERBOSE) {
	    msg() << MSG::VERBOSE << "Combination passed Delta Phi cut of " << m_MaxDPhi << endreq;
	  }
	 }
	 
	  m_monDPhiAccepted = m_DPhi;
      m_monCut = 2;
      
      EgMuTopoInfo->SetDeltaPhi(m_DPhi);
      
	 
	 if(m_MaxDR > 1e-11)
	 {
		if(m_DR > m_MaxDR) {
		  if(msgLvl() <= MSG::VERBOSE) {
	    msg() << MSG::VERBOSE << "Combination failed Delta R cut" << endreq;
	  }
		delete EgMuTopoInfo;
		continue;
     }else{
		if(msgLvl() <= MSG::VERBOSE) {
	    msg() << MSG::VERBOSE << "Combination passed Delta R cut of " << m_MaxDR << endreq;
	  }
	 }	 
	 }
	 
	  m_monDRAccepted = m_DR;	
      m_monCut = 3;

      EgMuTopoInfo->SetDeltaR(m_DR);
    
		
      if ( ! EgMuTopoInfo->opositeCharge(*electron1, muon1) ) {
			EgMuTopoInfo->SetOppositeCharge(false);

		if(msgLvl() <= MSG::VERBOSE) {
			msg() << MSG::VERBOSE << "Combination failed did not have opposite charge" << endreq;
		}
		} else {
			if(msgLvl() <= MSG::VERBOSE) {
				msg() << MSG::VERBOSE << "Combination had opposite charge" 
				<< endreq;
			}
			EgMuTopoInfo->SetOppositeCharge(true);
  
		}
	
	
	
		
      // evaluate mass
      
    m_mass = EgMuTopoInfo->invariantMass((*electron1),muon1);
     
    m_monMassAll = m_mass;  
      // apply cut on mass
    if(m_mass<m_lowerMassCut || m_mass>m_upperMassCut) {

		if(msgLvl() <= MSG::VERBOSE) {
			msg() << MSG::VERBOSE << "Combination failed mass cut: " 
			<< m_mass << " not in [" << m_lowerMassCut << "," 
			<< m_upperMassCut << "]" << endreq;
		}
		delete EgMuTopoInfo;
		continue;
    } else {
	// good combination found
		pass = true;
		m_monCut = 5;    
		m_monMassAccepted = m_mass;
		EgMuTopoInfo->SetInvMass(m_mass);
		if(msgLvl() <= MSG::VERBOSE) {
			msg() << MSG::VERBOSE << "Combination passed mass cut: " 
			<< m_lowerMassCut << " < " << m_mass << " < " 
			<< m_upperMassCut << endreq;	     
			msg() << MSG::DEBUG << "Good combination found! Memu=" 
			<< m_mass << " CLHEP::MeV" << endreq;
		}
		
		m_electron = (*electron1)->trackParticle();
		
		m_muon = (*tr)->indetTrack();
		
		msg() << MSG::VERBOSE << " emu created " << endreq;
		electronCollection.push_back(m_electron);
		msg() << MSG::VERBOSE << " electron saved " << endreq;
		
		muonCollection.push_back(m_muon);
		
		if(msgLvl() <= MSG::DEBUG){
		msg() << MSG::DEBUG << "Created following object: "
		<< (*EgMuTopoInfo)
		<< " and recorded electron at " << m_electron 
		<< " and muon at " << m_muon
		<< endreq;
		}
	
		if ( !m_egMuTopoColl ) m_egMuTopoColl = new ElectronMuonTopoInfoContainer();
			m_egMuTopoColl->push_back(new ElectronMuonTopoInfo(*EgMuTopoInfo));
			delete EgMuTopoInfo;
	
      }   
	} // muons container loop end
  } // electrons1 container loop end
  // set output TriggerElement true if good combination
  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "pass = " << pass << endreq;
  return HLT::OK;    
}  

HLT::ErrorCode TrigEFElectronMuonAngleFexAlgo::hltExecute(HLT::TEConstVec& /*inputTE*/, HLT::TriggerElement* outputTE){
   // vertex cut
   
   int VX_state = 2;
   double pull = -999.;
   
   //Vertex checking based on 'findImpact' function from egamma offline
   
		std::vector<const Rec::TrackParticleContainer*> vecIDTrakCont;
		const Rec::TrackParticleContainer* idtrackCont=0;
		
		
		
		ElectronMuonTopoInfo* EgMuTopoInfo2 = new ElectronMuonTopoInfo();
  
   
		if(getFeatures(outputTE, vecIDTrakCont) != HLT::OK || vecIDTrakCont.size() == 0) 
        {
			if ( msgLvl() <= MSG::WARNING) {
				msg() << MSG::WARNING << "Failed to get vector of TrkParticleContainer" << endreq;
			}
			return ElectronMuonTopoInfo::Unapplicable;
		}
		else{
			if( vecIDTrakCont.size() != 1)
			{
				if ( msgLvl() <= MSG::DEBUG) {
						msg() << MSG::DEBUG << "TrkParticleContainer does not have size 1! It has size " << vecIDTrakCont.size() << endreq;
				}
			} else { 
				if ( msgLvl() <= MSG::DEBUG )
						msg() << MSG::DEBUG  << "Obtained vector of TrkParticleContainer with size 1" <<  endreq; 	
			}
		}
		int nTrkPartCollection = vecIDTrakCont.size(); 		
	
		bool sizeMatch = false;
	        if(m_egMuTopoColl==0 || electronCollection.size()==0 || muonCollection.size()==0){delete EgMuTopoInfo2; return HLT::OK;}
		if(m_egMuTopoColl->size()== electronCollection.size() && electronCollection.size()==muonCollection.size())
		{
			if(msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "Container sizes match and are " << m_egMuTopoColl->size() << endreq;
			sizeMatch = true;
		}else{
			if(msgLvl() <= MSG::WARNING){ 
		    msg() << MSG::WARNING << "Container sizes do not match and are: EmuTopoInfo (" << m_egMuTopoColl->size() 
			<< "); electronColl(" << electronCollection.size() << "); muonColl(" 
			<< muonCollection.size() << ")."
			<< endreq;
			}
		}
		
		ElectronMuonTopoInfoContainer::const_iterator egMuTopoInfo;
		for ( egMuTopoInfo = m_egMuTopoColl->begin(); egMuTopoInfo != m_egMuTopoColl->end(); ++egMuTopoInfo ) {
			if((*egMuTopoInfo)==0) continue;
			if(!sizeMatch){(*egMuTopoInfo)->SetVertexState(2); continue;} 
			int it = egMuTopoInfo - m_egMuTopoColl->begin();
			m_electron = electronCollection.at(it);
			m_muon = muonCollection.at(it);
			if(m_electron==0 || m_muon ==0) continue;
		
			const Rec::TrackParticle* muonTP = 0;
   
			bool matched = false;
			int iColl = 0;
			for(iColl = 0; iColl < nTrkPartCollection; iColl++)
			{
				if(matched) break;
				idtrackCont  = vecIDTrakCont.at(iColl);
   
				Rec::TrackParticleContainer::const_iterator trPart;
				for ( trPart = idtrackCont->begin(); trPart != idtrackCont->end(); trPart++ ) {
					const Rec::TrackParticle* TPiter = (*trPart);
					const Trk::Track* track = TPiter->originalTrack();
					if(track==m_muon){
						muonTP=TPiter;
						matched = true;
					}
				}
			}
			
			if ( msgLvl() <= MSG::DEBUG )
				msg() << MSG::DEBUG  << "Muon track at " << m_muon << " was matched to TrkParticle from collection no. " << iColl << " at address " << muonTP << endreq;
			
			const Trk::Perigee* perigeeEL =   m_trackToVertexTool->perigeeAtBeamspot(*m_electron);
	   
			const Trk::Perigee* perigeeMU =   m_trackToVertexTool->perigeeAtBeamspot(*muonTP);
		    
		   
			
		
			if (perigeeEL==0) {
				if(msgLvl() <= MSG::WARNING) msg() << MSG::WARNING <<"No electron perigee using beam spot; no z0 calculation"<<endreq;    
				perigeeEL = m_trackToVertexTool->perigeeAtVertex(*m_electron, m_primaryVertex);
			}
		
			if (perigeeMU==0) {
				if(msgLvl() <= MSG::WARNING) msg() << MSG::WARNING <<"No muon perigee using beam spot; no z0 calculation"<<endreq;    
				perigeeMU = m_trackToVertexTool->perigeeAtVertex(*muonTP, m_primaryVertex);
			}
			if ( msgLvl() <= MSG::DEBUG ){
				msg() << MSG::DEBUG  << "Found perigee for electron at " << m_electron << ":  " << perigeeEL 
				<< "; and for muon at " << muonTP << ": " << perigeeMU 
				<< endreq;			
			}
			
			bool doDebug = false;
			if (msgLvl() <= MSG::DEBUG) doDebug = true;
			const ElectronMuonTopoInfo::Vertex vx = EgMuTopoInfo2->commonVertex(perigeeEL, perigeeMU, pull, doDebug);
			
			if ( vx == ElectronMuonTopoInfo::NotCommon ) {
			  if (msgLvl() <= MSG::VERBOSE) {
			    msg() << MSG::VERBOSE 
				  << "Combination failed vertex cut" 
				  << endreq;
			  }
			  
			} else  if ( vx == ElectronMuonTopoInfo::Common ) {
			  if (msgLvl() <= MSG::VERBOSE) {
			    msg() << MSG::VERBOSE 
				  << "Combination passed vertex cut" << endreq;
			  }
			} else {
			  if(msgLvl() <= MSG::VERBOSE) {
			    msg() << MSG::VERBOSE 
				  << "Bad track vertex errors: not applying cut" << endreq;
			  }
			}
			VX_state = vx;
			if (msgLvl() <= MSG::VERBOSE) {
				msg() << MSG::VERBOSE 
				<< "Vertex state is " << VX_state << " and the pull (distance/error) is " << pull << endreq;
			}
			(*egMuTopoInfo)->SetVertexState(VX_state);
			if(msgLvl() <= MSG::DEBUG){
				msg() << MSG::DEBUG << "Updated following object: "
				<< *(*egMuTopoInfo)
				<< " at address " << (*egMuTopoInfo)
				<< endreq;
			}
		}
	
   
   
   
   
   
   m_monPull = pull;
   m_monVxState = VX_state;
   
    if(msgLvl() <= MSG::VERBOSE) {
	  msg() << MSG::VERBOSE << "Attaching electronMuonTopoInfoCollection of the length " <<  m_egMuTopoColl->size() << endreq;
	}
   
   if(m_egMuTopoColl!=0) attachFeature(outputTE, m_egMuTopoColl, "EF_EgMuTopoFEX"); 
  
   delete EgMuTopoInfo2;
   return HLT::OK;
}
