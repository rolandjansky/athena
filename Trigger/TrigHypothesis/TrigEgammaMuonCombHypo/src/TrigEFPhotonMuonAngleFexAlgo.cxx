// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaMuonCombHypo/TrigEFPhotonMuonAngleFexAlgo.h
 **
 **   Description: search for pairs of photon and muons with 
 **               delta phi in some interval + some additional cuts; 
 **   Author: Olya Igonkina,
 **
 **   Created:  Nov 24 , 2011
 **
 **************************************************************************/ 


#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigEgammaMuonCombHypo/TrigEFPhotonMuonAngleFexAlgo.h"

#include "TrigTopoEvent/ElectronMuonTopoInfo.h"
#include "TrigTopoEvent/ElectronMuonTopoInfoContainer.h"


#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "MuidEvent/MuidTrackContainer.h"

#include <math.h>


TrigEFPhotonMuonAngleFexAlgo::TrigEFPhotonMuonAngleFexAlgo(const std::string & name, ISvcLocator* pSvcLocator)
  : HLT::ComboAlgo(name, pSvcLocator)
  {

  
  // monitoring
  declareMonitoredVariable("mass", m_monMass);
  declareMonitoredVariable("dPhi", m_monDPhi);
  declareMonitoredVariable("dR", m_monDR);
  
  
    
}

TrigEFPhotonMuonAngleFexAlgo::~TrigEFPhotonMuonAngleFexAlgo()
{ }

HLT::ErrorCode TrigEFPhotonMuonAngleFexAlgo::hltInitialize()
{
  
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Initialization:" << endreq;
  }

 
  return HLT::OK;
}


HLT::ErrorCode TrigEFPhotonMuonAngleFexAlgo::hltFinalize()
{
  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endreq;

  return HLT::OK;
}

HLT::ErrorCode TrigEFPhotonMuonAngleFexAlgo::hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement* outputTE){
  
  m_monMass=-1;
  m_monDPhi= 3.14;
  m_monDR= 5;

  // sanity checks
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "Running TrigEFPhotonMuonAngleFexAlgo" << endreq;
  
  if ( inputTE.size() != 2 ) {
    msg() << MSG::ERROR << "Got diferent than 2 number of input TEs: " <<  inputTE.size() << " job badly configured" << endreq;
    return HLT::BAD_JOB_SETUP;
  }

  // this are 2 TEs which we eventually will combine
  const HLT::TriggerElement* te1 = inputTE[0];
  const HLT::TriggerElement* te2 = inputTE[1];

  // for debugging purpose look into RoIDescriptors
  if ( msgLvl() <= MSG::DEBUG ){
    const TrigRoiDescriptor* roiDescriptor1 = 0;
    const TrigRoiDescriptor* roiDescriptor2 = 0;
    if ( getFeature(te1, roiDescriptor1) != HLT::OK || getFeature(te2, roiDescriptor2) != HLT::OK || roiDescriptor1==0 || roiDescriptor2==0) {
      if ( msgLvl() <= MSG::DEBUG) {
	msg() <<  MSG::DEBUG << "No RoIDescriptors for this Trigger Elements! " << endreq;
      }
    } else {
      if ( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG  << "Trying to combine 2 RoIs: " << *roiDescriptor1 << " & " << *roiDescriptor2 << endreq;
    }
  }
  
  // retrieve TrigPhotonContainers from this TE
  const egammaContainer* photonContainer1(0);
  const MuidTrackContainer* MuEFTracksEF(0);

  
  if ( getFeature(te1, photonContainer1) != HLT::OK || photonContainer1 == 0)
    { 
      if(getFeature(te2, photonContainer1) != HLT::OK  || photonContainer1 == 0) 
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
  
  


  // now we have bunch of photons and muons and we need to find out whether they can form topological combination
  // loop over all combinations
  egammaContainer::const_iterator photon1;
  MuidTrackContainer::const_iterator tr;
  if(msgLvl() <= MSG::VERBOSE) {
	msg() << MSG::VERBOSE << "Size of photon container: " << photonContainer1->size() << " size of muon container " << MuEFTracksEF->size() << endreq; 
   }
  int mu_count,el_count;
  mu_count=el_count=0; 
  ElectronMuonTopoInfoContainer * egMuTopoColl = new ElectronMuonTopoInfoContainer();

  for ( photon1 = photonContainer1->begin(); photon1 != photonContainer1->end(); ++photon1 ) {
    
    if(msgLvl() <= MSG::VERBOSE) {
      el_count++;
      mu_count=0;
    }
    
    if((*photon1)==0)
      {
	msg() << MSG::WARNING << "Null pointer in egammaContainer. Skipping." << endreq;
	continue;	
      }
    const CaloCluster* clus = (*photon1)->cluster();
    if(!clus) {
      if(msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "REGTEST no cluster pointer in egamma object " << endreq;
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
	msg() << MSG::VERBOSE << "Processing photon no. " << el_count << " and muon no. " << mu_count << endreq; 
      }
      

      const Trk::Perigee* muon1 = (*tr)->indetTrack()->perigeeParameters();            

      // selection is done here
      //
      // debug dump 
      
      ElectronMuonTopoInfo* EgMuTopoInfo = new ElectronMuonTopoInfo();
      
      float mu_phi =  muon1->parameters()[Trk::phi];
      
      if(msgLvl() <= MSG::VERBOSE) {
	msg() << MSG::VERBOSE << "New combination:" << endreq; 
	msg() << MSG::VERBOSE << "egammaPhoton: addr=" << (*photon1) 
	      << "  et="    << clus->e()/cosh(clus->eta())  
	      << "; eta="   << clus->eta() 
	      << "; phi="   << clus->phi() 
	      << endreq;	      
	msg() << MSG::VERBOSE << "muon: addr=" << muon1 
	      << "  et="    << muon1->pT()  
	      << "; eta="   << muon1->eta()                 
	      << "; phi="   << mu_phi
	      << "; charge=" << muon1->charge()
	      << endreq;
      }

       
      // evaluate distance
      float gphi = clus->phi();
      float geta = clus->eta();
      float gpt = clus->e()/cosh(clus->eta()) ;
      
      double dPhi = gphi - mu_phi;
      if (dPhi < -M_PI) dPhi += 2.*M_PI;
      if (dPhi > M_PI) dPhi -= 2.*M_PI;
      m_monDPhi =  fabs(dPhi);
      
      double dEta = geta - muon1->eta();
      m_monDR = sqrt(dEta*dEta + dPhi*dPhi);
      m_monMass = EgMuTopoInfo->invariantMass( fabs(gpt), geta, gphi, 0.,
					       fabs(muon1->pT()),muon1->eta(), mu_phi,105.6);
      
      EgMuTopoInfo->SetDeltaPhi(m_monDPhi);
      EgMuTopoInfo->SetDeltaR(m_monDR); 		
      EgMuTopoInfo->SetInvMass(m_monMass);
    
      if(msgLvl() <= MSG::DEBUG){
	msg() << MSG::DEBUG << "Created following object: "
	      << (*EgMuTopoInfo)
	      << endreq;
      }

      egMuTopoColl->push_back(EgMuTopoInfo);
    
    } // muons container loop end
  } // photons1 container loop end

  
  HLT::ErrorCode hltStatus =  attachFeature(outputTE, egMuTopoColl, "EF_PhotonMuonTopoFEX"); 
  if (hltStatus != HLT::OK ){
    msg() << MSG::WARNING << "Write of ElectronMuonTopoInfo container to  outputTE failed" << endreq;
    for(ElectronMuonTopoInfoContainer::iterator itr = egMuTopoColl->begin(); itr!=egMuTopoColl->end() ; ++itr)
      delete *itr;
    delete egMuTopoColl;
    return hltStatus;
  }else{
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "successfully recorded EF_PhotonMuonTopoFEX" << endreq;
  }
  
  return HLT::OK;    
}  
