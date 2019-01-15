// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaMuonCombHypo/TrigEFElectronMuonAngleFexRun2Algo.h
 **
 **   Description: search for pairs of electron and muons with 
 **               delta phi in some interval + some additional cuts; 
 **   Author: Olya Igonkina,
 **
 **   Created:  Nov 24 , 2011
 **   Migrated to xAOD Nov 28, 2014
 **
 **************************************************************************/ 


#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigEgammaMuonCombHypo/TrigEFElectronMuonAngleFexRun2Algo.h"

// Notice that the only function from ElectronMuonTopoInfo used is invariantMass(floats..)
// so old TrigTopoEvent is just fine..

#include "TrigTopoEvent/ElectronMuonTopoInfo.h"
#include "TrigTopoEvent/ElectronMuonTopoInfoContainer.h"

// trigger EDM
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"


#include <math.h>


TrigEFElectronMuonAngleFexRun2Algo::TrigEFElectronMuonAngleFexRun2Algo(const std::string & name, ISvcLocator* pSvcLocator)
  : HLT::ComboAlgo(name, pSvcLocator)
  {

  
  // monitoring
  declareMonitoredVariable("mass", m_monMass);
  declareMonitoredVariable("dPhi", m_monDPhi);
  declareMonitoredVariable("dR", m_monDR);
  
  
    
}

TrigEFElectronMuonAngleFexRun2Algo::~TrigEFElectronMuonAngleFexRun2Algo()
{ }

HLT::ErrorCode TrigEFElectronMuonAngleFexRun2Algo::hltInitialize()
{
  
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Initialization:" << endmsg;
  }

 
  return HLT::OK;
}


HLT::ErrorCode TrigEFElectronMuonAngleFexRun2Algo::hltFinalize()
{
  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endmsg;

  return HLT::OK;
}

HLT::ErrorCode TrigEFElectronMuonAngleFexRun2Algo::hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement* outputTE){
  
  m_monMass=-1;
  m_monDPhi= 3.14;
  m_monDR= 5;

  bool debug =  msgLvl() <= MSG::DEBUG;

  // sanity checks
  if ( debug )  msg() << MSG::DEBUG << "Running TrigEFElectronMuonAngleFexRun2Algo" << endmsg;
  
  if ( inputTE.size() != 2 ) {
    msg() << MSG::ERROR << "Got diferent than 2 number of input TEs: " <<  inputTE.size() << " job badly configured" << endmsg;
    return HLT::BAD_JOB_SETUP;
  }

  // this are 2 TEs which we eventually will combine
  const HLT::TriggerElement* te1 = inputTE[0];
  const HLT::TriggerElement* te2 = inputTE[1];

  // for debugging purpose look into RoIDescriptors
  if (debug ){
    const TrigRoiDescriptor* roiDescriptor1 = 0;
    const TrigRoiDescriptor* roiDescriptor2 = 0;
    if ( getFeature(te1, roiDescriptor1) != HLT::OK || getFeature(te2, roiDescriptor2) != HLT::OK || roiDescriptor1==0 || roiDescriptor2==0) {
      if ( msgLvl() <= MSG::DEBUG) {
	msg() <<  MSG::DEBUG << "No RoIDescriptors for this Trigger Elements! " << endmsg;
      }
    } else {
      if ( debug )
	msg() << MSG::DEBUG  << "Trying to combine 2 RoIs: " << *roiDescriptor1 << " & " << *roiDescriptor2 << endmsg;
    }
  }
  
  // retrieve TrigElectronContainers from this TE
  std::vector<const xAOD::ElectronContainer*> vectorEgammaContainers;  

  
  if ( getFeatures(te1, vectorEgammaContainers) != HLT::OK )
    { 
      if(getFeatures(te2, vectorEgammaContainers) != HLT::OK )
        {
	  
	  if ( msgLvl() <= MSG::WARNING) 
	    msg() << MSG::WARNING << "Failed to get xAOD::ElectronContainer collection" << endmsg;	  
	  return HLT::MISSING_FEATURE;
	}
      else{
	if ( debug )
	  msg() << MSG::DEBUG  << "xAOD::ElectronContainer collection successfully retrieved" << endmsg; 	
      } 
      
    }else{
    if ( debug )
      msg() << MSG::DEBUG  << "xAOD::ElectronContainer collection successfully retrieved" << endmsg; 	
  }

  if (vectorEgammaContainers.size() < 1) {
    msg() << MSG::DEBUG << " empty xAOD::ElectronContainer from the trigger element" << endmsg;
    return HLT::OK;
  }  
  const xAOD::ElectronContainer* electronContainer1 = vectorEgammaContainers.back();;


  // Now get muons
  const xAOD::MuonContainer* MuEFTracksEF(0);

  
  if ( getFeature(te1, MuEFTracksEF) != HLT::OK || MuEFTracksEF == 0)
    { 
      if(getFeature(te2, MuEFTracksEF) != HLT::OK  || MuEFTracksEF == 0) 
        {	  
	  if ( msgLvl() <= MSG::WARNING) 
	    msg() << MSG::WARNING << "Failed to get EF MuidTrackContainer" << endmsg;
	  return HLT::MISSING_FEATURE;
	}
      else{
	if ( debug )
	  msg() << MSG::DEBUG  << "EF MuidTrackContainer successfully retrieved with size " << MuEFTracksEF->size() << endmsg; 	
      } 
      
    }else{
    if ( debug )
      msg() << MSG::DEBUG  << "EF MuidTrackContainer successfully retrieved with size " << MuEFTracksEF->size() << endmsg; 	
  }	
  
  


  // now we have bunch of electrons and muons and we need to find out whether they can form topological combination
  // loop over all combinations
  xAOD::ElectronContainer::const_iterator electron1;
  xAOD::MuonContainer::const_iterator muon;
  if(msgLvl() <= MSG::DEBUG) {
	msg() << MSG::DEBUG << "Size of electron container: " << electronContainer1->size() << " size of muon container " << MuEFTracksEF->size() << endmsg; 
   }
  int mu_count,el_count;
  mu_count=el_count=0; 
  ElectronMuonTopoInfoContainer * egMuTopoColl = new ElectronMuonTopoInfoContainer();

  for ( electron1 = electronContainer1->begin(); electron1 != electronContainer1->end(); ++electron1 ) {
    
    if(msgLvl() <= MSG::DEBUG) {
      el_count++;
      mu_count=0;
    }
    
    if((*electron1)==0)
      {
	msg() << MSG::WARNING << "Null pointer in egammaContainer. Skipping." << endmsg;
	continue;	
      }
    const xAOD::CaloCluster* clus = (*electron1)->caloCluster();
    if(!clus) {
      if(debug)
	msg() << MSG::DEBUG << "REGTEST no cluster pointer in egamma object " << endmsg;
      continue;
    }

    for (muon = MuEFTracksEF->begin(); muon != MuEFTracksEF->end(); ++muon ) {
      mu_count++;
      if((*muon)==0)
	{
	  msg() << MSG::WARNING << "Null pointer in MuonContainer. Skipping." << endmsg;
	  continue;	
	}
      const xAOD::TrackParticle* muon1 = (*muon)->trackParticle(xAOD::Muon::CombinedTrackParticle);
      if (!muon1) {
	if (debug) msg() << MSG::DEBUG << "No CombinedTrackParticle found." << endmsg;
	continue;
      }
      if(msgLvl() <= MSG::DEBUG) {
	mu_count++;
	msg() << MSG::DEBUG << "Processing electron no. " << el_count << " and muon no. " << mu_count << endmsg; 
      }
      

      ElectronMuonTopoInfo* EgMuTopoInfo = new ElectronMuonTopoInfo();
      
      float mu_phi =  muon1->phi();
      
      if(msgLvl() <= MSG::DEBUG) {
	msg() << MSG::DEBUG << "New combination:" << endmsg; 
	msg() << MSG::DEBUG << "egammaElectron: addr=" << (*electron1) 
	      << "  et="    << (*electron1)->pt()  
	      << "; eta="   << (*electron1)->eta() 
	      << "; phi="   << (*electron1)->phi() 
	      << "; charge="   << (*electron1)->charge() 
	      << endmsg;	      
	msg() << MSG::DEBUG << "muon: addr=" << muon1 
	      << "  et="    << muon1->pt()  
	      << "; eta="   << muon1->eta()                 
	      << "; phi="   << mu_phi
	      << "; charge=" << muon1->charge()
	      << endmsg;
      }

       
      // evaluate distance
      float gphi = (*electron1)->phi();
      float geta = (*electron1)->eta();
      float gpt = (*electron1)->pt();
      
      double dPhi = gphi - mu_phi;
      if (dPhi < -M_PI) dPhi += 2.*M_PI;
      if (dPhi > M_PI) dPhi -= 2.*M_PI;
      m_monDPhi =  fabs(dPhi);
      
      double dEta = geta - muon1->eta();
      m_monDR = sqrt(dEta*dEta + dPhi*dPhi);
      m_monMass = EgMuTopoInfo->invariantMass( fabs(gpt), geta, gphi, 0.5,
					       fabs(muon1->pt()),muon1->eta(), mu_phi,105.6);
      
      EgMuTopoInfo->SetDeltaPhi(m_monDPhi);
      EgMuTopoInfo->SetDeltaR(m_monDR); 		
      EgMuTopoInfo->SetInvMass(m_monMass);
      EgMuTopoInfo->SetElecValid(true);
      if(muon1->charge()*(*electron1)->charge() < 0 ) EgMuTopoInfo->SetOppositeCharge(true);
      else                                         EgMuTopoInfo->SetOppositeCharge(false);
    
      if(debug){
	msg() << MSG::DEBUG << "Created following object: "
	      << (*EgMuTopoInfo)
	      << endmsg;
      }

      egMuTopoColl->push_back(EgMuTopoInfo);
    
    } // muons container loop end
  } // electrons1 container loop end

  
  HLT::ErrorCode hltStatus =  attachFeature(outputTE, egMuTopoColl, "HLT_ElectronMuonTopoFEX"); 
  if (hltStatus != HLT::OK ){
    msg() << MSG::WARNING << "Write of ElectronMuonTopoInfo container to  outputTE failed" << endmsg;
    for(ElectronMuonTopoInfoContainer::iterator itr = egMuTopoColl->begin(); itr!=egMuTopoColl->end() ; ++itr)
      delete *itr;
    delete egMuTopoColl;
    return hltStatus;
  }else{
    if(debug) msg() << MSG::DEBUG << "successfully recorded EF_ElectronMuonTopoFEX" << endmsg;
  }
  
  return HLT::OK;    
}  
