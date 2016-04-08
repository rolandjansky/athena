// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaMuonCombHypo/TrigL2PhotonMuonAngleFexAlgo.h
 **
 **   Description: - FEX algorithm: searc for pairs of photon and muons with 
 **                delta phi in some interval + some additional cuts; intended for a1->tautau->emu
 **                - Modified from TrigEFDiphotonMassHypo and TrigEFDiphotonMassFex by R Goncalo
			and TrigL2DimuonMassHypo by C. Schiavi
 **
 **   Author: Pavel Jez <pavel.jez@cern.ch>
 **
 **   Created:   Jan 30, 2011
 **
 **
 **************************************************************************/ 


// trigger EDM
#include "TrigParticle/TrigPhoton.h"
#include "TrigParticle/TrigPhotonContainer.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "TrigTopoEvent/ElectronMuonTopoInfo.h"
#include "TrigTopoEvent/ElectronMuonTopoInfoContainer.h"


#include "TrigEgammaMuonCombHypo/TrigL2PhotonMuonAngleFexAlgo.h"

#include <math.h>
#include <string>
#include <cmath> 
// general athena stuff
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"


//class ISvcLocator;

TrigL2PhotonMuonAngleFexAlgo::TrigL2PhotonMuonAngleFexAlgo(const std::string & name, ISvcLocator* pSvcLocator)
  : HLT::ComboAlgo(name, pSvcLocator)
  {

    // monitoring
  declareMonitoredVariable("mass", m_monMass);
  declareMonitoredVariable("dPhi", m_monDPhi);
  declareMonitoredVariable("dR", m_monDR);
}

TrigL2PhotonMuonAngleFexAlgo::~TrigL2PhotonMuonAngleFexAlgo()
{ }

HLT::ErrorCode TrigL2PhotonMuonAngleFexAlgo::hltInitialize()
{
  return HLT::OK;
}


HLT::ErrorCode TrigL2PhotonMuonAngleFexAlgo::hltFinalize()
{
  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endreq;

  return HLT::OK;
}



HLT::ErrorCode TrigL2PhotonMuonAngleFexAlgo::hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement* outputTE)
{
  //HLT::ErrorCode TrigL2PhotonMuonAngleFexAlgo::acceptInputs(HLT::TEConstVec& inputTE, bool& pass ) {
  
  
  // sanity checks
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "Running TrigL2PhotonMuonAngleFexAlgo::hltExecute" << endreq;

if ( inputTE.size() != 2 ) {
    msg() << MSG::ERROR << "Got diferent than 2 number of input TEs: " <<  inputTE.size() << " job badly configured" << endreq;
    return HLT::BAD_JOB_SETUP;
  }

  // this are 2 TEs which we eventually will combine
 const HLT::TriggerElement* te1 = inputTE[0];
 const HLT::TriggerElement* te2 = inputTE[1];

  
  // retrieve TrigPhotonContainers from this TE
  const TrigPhotonContainer* photonContainer1(0);
  std::vector<const CombinedMuonFeature*> muonContainer1;
  ElectronMuonTopoInfoContainer * egMuTopoColl = new ElectronMuonTopoInfoContainer();

 // const TrigPhotonContainer* photonContainer2(0);
  if ( getFeature(te1, photonContainer1) != HLT::OK || photonContainer1 == 0)
    { 
      if(getFeature(te2, photonContainer1) != HLT::OK  || photonContainer1 == 0) 
        {
	  
	  if ( msgLvl() <= MSG::WARNING) {
	    msg() << MSG::WARNING << "Failed to get TrigPhoton collection" << endreq;
	  }
	  return HLT::MISSING_FEATURE;
	}
      else{
	if ( msgLvl() <= MSG::DEBUG )
	  msg() << MSG::DEBUG  << "TrigPhoton collection successfully retrieved" << endreq; 	
			} 
      
    }else{
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG  << "TrigPhoton collection successfully retrieved" << endreq; 	
  }
  // retrieving muons
  if ( getFeatures(te1, muonContainer1) != HLT::OK || muonContainer1.size() == 0)
    { 
      if(getFeatures(te2, muonContainer1) != HLT::OK  || muonContainer1.size() == 0) 
        {
	  
	  if ( msgLvl() <= MSG::WARNING) {
	    msg() << MSG::WARNING << "Failed to get MuonCombinedFeature collection" << endreq;
	  }
	  return HLT::MISSING_FEATURE;
	}
      else{
	if ( msgLvl() <= MSG::DEBUG )
	  msg() << MSG::DEBUG  << "MuonCombinedFeature collection successfully retrieved" << endreq; 	
      } 
      
    }else{
	if ( msgLvl() <= MSG::DEBUG )
	  msg() << MSG::DEBUG  << "MuonCombinedFeature collection successfully retrieved" << endreq; 	
  }

  // now we have 2 bunches of leptons and we need to find out whether they can form topological combination
  // loop over all combinations
  TrigPhotonContainer::const_iterator photon1;
  std::vector<const CombinedMuonFeature*>::iterator muon1;
  for ( photon1 = photonContainer1->begin(); photon1 != photonContainer1->end(); ++photon1 ) {
    if((*photon1)==0)
      {
	msg() << MSG::WARNING << "Null pointer in TrigPhotonContainer. Skipping." << endreq;
	continue;	
      }
    if ( !(*photon1)->isValid() ) 
      {
	msg() << MSG::DEBUG << "Photon is not valid" << endreq;
	continue;	
      }
    for ( muon1 = muonContainer1.begin(); muon1 != muonContainer1.end(); ++muon1 ) {
      if((*muon1)==0)
	{
	  msg() << MSG::WARNING << "Null pointer in CombinedMuonFeature vector. Skipping." << endreq;
	  continue;	
	}
      
      if((*muon1)->IDTrack()==0)
	{
	  if(msgLvl() <= MSG::DEBUG) {
	    msg() << MSG::DEBUG << "Null pointer to IDTrack from muon. Skipping." << endreq;
	  }
	  continue;	
	}
      
      ElectronMuonTopoInfo* EgMuTopoInfo = new ElectronMuonTopoInfo();
  
      
      if(msgLvl() <= MSG::VERBOSE) {
	msg() << MSG::VERBOSE << "New combination:" << endreq; 
	msg() << MSG::VERBOSE << "1st TrigPhoton: addr=" << (*photon1)
	      << ((*photon1)->isValid() ? "(valid" : "(not valid")
	      << ") RoI id="<< (*photon1)->roiId()
	      << "; pt="    << (*photon1)->pt()  
	      << "; eta="   << (*photon1)->eta() 
	      << "; phi="   << (*photon1)->phi() 
	      << endreq;
	msg() << MSG::VERBOSE << "1st CombinedMuonFeature: addr=" << (*muon1)
          << " pt=" << 1/(*muon1)->ptq() << " trkAddr=" << (*muon1)->IDTrack() 
          << " " << *(*muon1)
          << "; Zvtx =" << (*muon1)->IDTrack()->param()->z0() << "+-" << (*muon1)->IDTrack()->param()->ez0()
          << endreq;
      }
      EgMuTopoInfo->SetRoiWord((*photon1)->roiId());
      EgMuTopoInfo->SetElecValid(true);    

  
      double dPhi = (*photon1)->phi()-(*muon1)->IDTrack()->param()->phi0();
      if (dPhi < -M_PI) dPhi += 2.*M_PI;
      if (dPhi > M_PI) dPhi -= 2.*M_PI;
      m_monDPhi =  fabs(dPhi);
      
      double dEta = (*photon1)->eta() - (*muon1)->IDTrack()->param()->eta();
      m_monDR = sqrt(dEta*dEta + dPhi*dPhi);

      m_monMass = EgMuTopoInfo->invariantMass(fabs((*photon1)->pt()),(*photon1)->eta(), (*photon1)->phi(), 0.,
					      fabs((*muon1)->pt()),(*muon1)->IDTrack()->param()->eta(), 
					      (*muon1)->IDTrack()->param()->phi0(),  105.6);
      EgMuTopoInfo->SetDeltaPhi(m_monDPhi);
      EgMuTopoInfo->SetDeltaR(m_monDR);
      EgMuTopoInfo->SetInvMass(m_monMass);

      if(msgLvl() <= MSG::DEBUG){
	msg() << MSG::DEBUG << "Created following object: "
	      << (*EgMuTopoInfo)
	      << endreq;
      }
	
     
      egMuTopoColl->push_back(EgMuTopoInfo);
    } // photons2 container loop end
  } // photons1 container loop end


  HLT::ErrorCode hltStatus =  attachFeature(outputTE, egMuTopoColl, "L2_PhotonMuonTopoFEX"); 
  if (hltStatus != HLT::OK ){
    msg() << MSG::WARNING << "Write of ElectronMuonTopoInfo container to  outputTE failed" << endreq;
    for(ElectronMuonTopoInfoContainer::iterator itr = egMuTopoColl->begin(); itr!=egMuTopoColl->end() ; ++itr)
      delete *itr;
    delete egMuTopoColl;
    return hltStatus;
  }else{
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "successfully recorded L2_PhotonMuonTopoFEX" << endreq;
  }

  // set output TriggerElement true if good combination
  return HLT::OK;    
}  

