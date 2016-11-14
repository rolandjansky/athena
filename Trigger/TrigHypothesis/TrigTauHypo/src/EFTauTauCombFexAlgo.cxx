// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 ** File: Trigger/TrigHypothesis/TrigTauHypo/EFTauTauCombFexAlgo.cxx
 **
 ** Description: FEX algorithm: search for pairs of taus with delta eta in some interval +
 **              some additional cuts; intended for H-> tau tau SM and MSSM
 **
 ** Author: Phillip Urquijo <Phillip.Urquijo@cern.ch>
 ** Created: August 1
 **************************************************************************/

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigTauHypo/EFTauTauCombFexAlgo.h"
#include "tauEvent/TauJetContainer.h"
#include "tauEvent/TauJet.h"
#include "tauEvent/TauDetailsContainer.h"
#include "tauEvent/TauCommonDetails.h"
#include "tauEvent/TauCommonExtraDetails.h"
#include "tauEvent/TauPID.h"
#include <math.h>

EFTauTauCombFexAlgo::EFTauTauCombFexAlgo(const std::string & name, ISvcLocator* pSvcLocator) 
  : HLT::ComboAlgo(name, pSvcLocator), 
    m_tautauTopoColl(0)
{
  //Read cuts
  declareProperty("AcceptAll",    m_acceptAll=false);
  //declareProperty("ValidTau",     m_tauValid=true);
  declareProperty("MaxDRCut",     m_MaxDR=5.0); 
  declareProperty("MaxDPhiCut",   m_MaxDPhi=5.0);
  declareProperty("MaxDEtaCut",   m_MaxDEta=5.0);
  declareProperty("LowerMassCut", m_lowerMassCut=0.0);
  declareProperty("UpperMassCut", m_upperMassCut=50000.0);
  // monitoring
  declareMonitoredVariable("massOfAccepted", m_monMassAccepted);
  declareMonitoredVariable("massOfAll",      m_monMassAll); 
  declareMonitoredVariable("cut",            m_monCut);
  declareMonitoredVariable("dPhiOfAccepted", m_monDPhiAccepted);
  declareMonitoredVariable("dPhiOfAll",      m_monDPhiAll);
  declareMonitoredVariable("dEtaOfAccepted", m_monDEtaAccepted);
  declareMonitoredVariable("dEtaOfAll",      m_monDEtaAll);
  declareMonitoredVariable("dROfAccepted",   m_monDRAccepted);
  declareMonitoredVariable("dROfAll",        m_monDRAll);
  declareMonitoredVariable("combtype",           m_moncombtype);

  m_DEta = m_DPhi = m_DR = 100.;
  m_mass=-1.;
}

EFTauTauCombFexAlgo::~EFTauTauCombFexAlgo()
{ }

HLT::ErrorCode EFTauTauCombFexAlgo::hltInitialize()
{
  if (msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "Initialization:" << endmsg;
  
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Initialization completed successfully:" << endmsg;
    msg() << MSG::DEBUG << "AcceptAll            = " 
          << (m_acceptAll==true ? "True" : "False") << endmsg; 
//     msg() << MSG::DEBUG << "ValidTau        = " 
//           << (m_tauValid==true ? "True" : "False") << endmsg; 
    msg() << MSG::DEBUG << "DeltaPhiCut         = " << m_MaxDPhi << endmsg;
    msg() << MSG::DEBUG << "DeltaEtaCut         = " << m_MaxDEta << endmsg;
    msg() << MSG::DEBUG << "MaxDeltaRCut         = " << m_MaxDR << endmsg;
    msg() << MSG::DEBUG << "LowerMassCut         = " << m_lowerMassCut << endmsg;
    msg() << MSG::DEBUG << "UpperMassCut         = " << m_upperMassCut << endmsg;
  }
  return HLT::OK;
}


HLT::ErrorCode EFTauTauCombFexAlgo::hltFinalize()
{
  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endmsg;
  return HLT::OK;
}

HLT::ErrorCode EFTauTauCombFexAlgo::acceptInputs(HLT::TEConstVec& inputTE, bool& pass ) {

  m_monMassAccepted = -999.;
  m_monCut=0;
  m_tautauTopoColl=0;

  //  tau1Collection.clear();

  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "Running EFTauTauCombFexAlgo::acceptInputs" << endmsg;
  if ( inputTE.size() != 2 ) {
    msg() << MSG::ERROR << "Did not collect 2 input TEs: " <<  inputTE.size() << " job badly configured" << endmsg;
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
      //return HLT::MISSING_FEATURE; 
    } else {
      if ( msgLvl() <= MSG::DEBUG )
        msg() << MSG::DEBUG  << "Trying to combine 2 RoIs: " << *roiDescriptor1 << " & " << *roiDescriptor2 << endmsg;
    }
  }

  bool tautau=false;
  bool tauelec=false;
  bool taumu=false;
  bool hasmu=false;
  bool haselec=false;
  bool hastau=false;

  std::vector<const Analysis::TauJetContainer*> vectorTauContainers1;
  std::vector<const Analysis::TauJetContainer*> vectorTauContainers2;


  if ( getFeatures(te1, vectorTauContainers1) == HLT::OK && vectorTauContainers1.size() != 0)
    {
      if ( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG  << "EF tau collection 1 successfully retrieved" << endmsg;       

      if ( getFeatures(te2, vectorTauContainers2) == HLT::OK && vectorTauContainers2.size() != 0)
	{
	  if ( msgLvl() <= MSG::DEBUG )
	    msg() << MSG::DEBUG  << "EF tau collection 2 successfully retrieved" << endmsg;       
	  tautau=true;
	}
      else    { 
	if ( msgLvl() <= MSG::DEBUG) {
	  msg() << MSG::DEBUG << "Failed to get EF tau collection 2" << endmsg;
	}
	//	return HLT::MISSING_FEATURE;
      }
      
    }
  else    { 
    if ( msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Failed to get EF tau collection 2" << endmsg;
    }
    //    return HLT::MISSING_FEATURE;
  }


  const egammaContainer* electronContainer1(0);
  const MuidTrackContainer* MuEFTracksEF;
  if(!tautau){
    if ( getFeatures(te1, vectorTauContainers1) != HLT::OK || vectorTauContainers1.size() == 0)
      {
	if(getFeatures(te2, vectorTauContainers1) != HLT::OK  || vectorTauContainers1.size() == 0)
	  {
	    if ( msgLvl() <= MSG::DEBUG) 
	      msg() << MSG::DEBUG << "Failed to get EFtau collection" << endmsg;
	    
	  }
	else{
	  if ( msgLvl() <= MSG::DEBUG )
	    msg() << MSG::DEBUG  << "EF tau collection successfully retrieved" << endmsg;       
	  hastau=true;
	}
      }else{
      if ( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG  << "EF tau collection successfully retrieved" << endmsg;       
	  hastau=true;
    }

    if ( getFeature(te1, electronContainer1) != HLT::OK || electronContainer1 == 0)
      {
	if(getFeature(te2, electronContainer1) != HLT::OK  || electronContainer1 == 0)
	  {
	    if ( msgLvl() <= MSG::DEBUG) 
	      msg() << MSG::DEBUG << "Failed to get EF egamma collection" << endmsg;
	    
	  }
	else{
	  if ( msgLvl() <= MSG::DEBUG )
	    msg() << MSG::DEBUG  << "EF egamma collection successfully retrieved" << endmsg;       
	  haselec=true;
	}
      }else{
      if ( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG  << "EF egamma collection successfully retrieved" << endmsg;       
	  haselec=true;
    }
    
    
    if ( getFeature(te1, MuEFTracksEF, "MuonCombEF") != HLT::OK || MuEFTracksEF == 0)
      {
	if(getFeature(te2, MuEFTracksEF, "MuonCombEF") != HLT::OK  || MuEFTracksEF == 0)
	  {
	    if ( msgLvl() <= MSG::DEBUG) 
	      msg() << MSG::DEBUG << "Failed to get EF MuidTrackContainer" << endmsg;
	    
	  }
	else{
	  if ( msgLvl() <= MSG::DEBUG )
	    msg() << MSG::DEBUG  << "EF MuidTrackContainer successfully retrieved with size " << MuEFTracksEF->size() << endmsg;   
	  hasmu=true;
     }
      }else{
      if ( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG  << "EF MuidTrackContainer successfully retrieved with size " << MuEFTracksEF->size() << endmsg;   
      hasmu=true;
    }   
  }
 
  if(hastau&&hasmu)taumu=true;
  if(hastau&&haselec)tauelec=true;
  
  if(!tautau&&!taumu&&!tauelec)
    if ( msgLvl() <= MSG::WARNING ){
      msg() << MSG::WARNING  << " No topo candidate features" << endmsg;     
      return HLT::MISSING_FEATURE;
    }

  pass=false;
  // now we have two bunches of taus and we need to find out whether they can form topological combination

  if(tautau){
    if(vectorTauContainers1.size() == 0 || vectorTauContainers2.size()==0){
      if( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG << " REGTEST: Received 0 taucontainers  "
	      << "This algorithm is designed to work with  one tau container per TE."
	      << endmsg;
      return HLT::OK;
    }
    const Analysis::TauJetContainer *tauContainer1 = vectorTauContainers1.back();
    const Analysis::TauJetContainer *tauContainer2 = vectorTauContainers2.back();
    
    Analysis::TauJetContainer::const_iterator tau1;
    Analysis::TauJetContainer::const_iterator tau2;
    
    if(msgLvl() <= MSG::VERBOSE) {
      msg() << MSG::VERBOSE << "Size of tau1 container: " << tauContainer1->size() << " size of tau2 container " << tauContainer2->size() << endmsg; 
    }
    
    int tau1_count,tau2_count;
    tau1_count=tau2_count=0; 
    
    for ( tau1 = tauContainer1->begin(); tau1 != tauContainer1->end(); tau1++ ) {
      if(msgLvl() <= MSG::VERBOSE) {
	tau1_count++;
	tau2_count=0;
      }
      if((*tau1)==0)
	{
	  msg() << MSG::DEBUG << "Null pointer in tauContainer1. Skipping." << endmsg;
	  continue;
	}
      
      for ( tau2 = tauContainer2->begin(); tau2 != tauContainer2->end(); tau2++ ) {
	if(msgLvl() <= MSG::VERBOSE) {
	  tau2_count++;
	}
	if((*tau2)==0)
	  {
	    msg() << MSG::DEBUG << "Null pointer in tauContainer2. Skipping." << endmsg;
	    continue;
	  }
	
	m_monCut = 1;
	ElectronMuonTopoInfo* TauTauTopoInfo = new ElectronMuonTopoInfo();
	TauTauTopoInfo->SetDeltaPhi(float(-999.0));
	TauTauTopoInfo->SetDeltaR(float(-999.0));
	TauTauTopoInfo->SetInvMass(float(-999.0));
	TauTauTopoInfo->SetOppositeCharge(false);
	TauTauTopoInfo->SetVertexState(1515);
	
	if(msgLvl() <= MSG::DEBUG) {
	  msg() << MSG::DEBUG << "New combination:" << endmsg; 
	  msg() << MSG::DEBUG << "tau1: addr=" << (*tau1) 
		<< "  et="    << (*tau1)->et() 
		<< "; eta="   << (*tau1)->eta() 
		<< "; phi="   << (*tau1)->phi() 
		<< endmsg;             
	  msg() << MSG::DEBUG << "tau2: addr=" << (*tau2) 
		<< "  et="    << (*tau2)->et() 
		<< "; eta="   << (*tau2)->eta() 
		<< "; phi="   << (*tau2)->phi() 
		<< endmsg;             
	}
	
	// evaluate distance
	m_DEta = m_DPhi=m_DR = 100.; // setting value which can never pass
	m_DPhi = TauTauTopoInfo->deltaPhi((*tau1),(*tau2));
	m_DR = TauTauTopoInfo->deltaR((*tau1),(*tau2));
	
	m_DEta=sqrt(fabs(m_DR*m_DR - m_DPhi*m_DPhi));  
	
	if(msgLvl() <= MSG::VERBOSE) {
	  msg() << MSG::VERBOSE 
		<< "Delta Phi between tau1 and tau2 is " << m_DPhi 
		<<" Delta Eta is " << m_DEta 
		<<" Radial distance delta R is " << m_DR << endmsg;
	}
	
	m_monDPhiAll=m_DPhi;
	m_monDRAll=m_DR;
	m_monDEtaAll=m_DEta;
	
	if(m_DPhi > m_MaxDPhi) {
	  if(msgLvl() <= MSG::VERBOSE) 
	    msg() << MSG::VERBOSE << "Combination failed Delta Phi cut" << endmsg;
	  
	  delete TauTauTopoInfo;
	  continue;               
	}else{
	  if(msgLvl() <= MSG::VERBOSE) 
	    msg() << MSG::VERBOSE << "Combination passed Delta Phi cut of " << m_MaxDPhi << endmsg;
	  
	}
	
	m_monDPhiAccepted = m_DPhi;
	m_monCut = 2;
	
	TauTauTopoInfo->SetDeltaPhi(m_DPhi);
	if(m_MaxDR > 1e-11)
	  {
	    if(m_DR > m_MaxDR) {
	      if(msgLvl() <= MSG::VERBOSE) 
		msg() << MSG::VERBOSE << "Combination failed Delta R cut" << endmsg;
	      
	      delete TauTauTopoInfo;
	      continue;
	    }else{
	      if(msgLvl() <= MSG::VERBOSE) {
		msg() << MSG::VERBOSE << "Combination passed Delta R cut of " << m_MaxDR << endmsg;
	      }
	    }       
	  }
	
	m_monDRAccepted = m_DR;       
	m_monCut = 3;
	m_monDEtaAccepted = m_DEta;
	TauTauTopoInfo->SetDeltaR(m_DR);
	TauTauTopoInfo->SetOppositeCharge(false);
	
	m_mass = TauTauTopoInfo->invariantMass((*tau1),(*tau2));
	
	TauTauTopoInfo->SetInvMass(m_mass);
	
	m_monMassAll = m_mass; 
	
	if(msgLvl() <= MSG::DEBUG){
	  msg() << MSG::DEBUG << "Created the following object: "
		<< (*TauTauTopoInfo)
		<< endmsg;
	}
	if ( !m_tautauTopoColl ) m_tautauTopoColl = new ElectronMuonTopoInfoContainer();
	m_tautauTopoColl->push_back(new ElectronMuonTopoInfo(*TauTauTopoInfo));
	delete TauTauTopoInfo;
	pass=true;
	
      } //tau1
    } //tau2
  }
  ///////////////////////////////////////////////////////////////
  if(taumu){
    if(vectorTauContainers1.size() == 0){
      if( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG << " REGTEST: Received 0 taucontainers  "
	      << "This algorithm is designed to work with  one tau container per TE."
	      << endmsg;
      return HLT::OK;
    }

    const Analysis::TauJetContainer *tauContainer1 = vectorTauContainers1.back();
    Analysis::TauJetContainer::const_iterator tau1;
    MuidTrackContainer::const_iterator tr;
    
    if(msgLvl() <= MSG::VERBOSE) {
      msg() << MSG::VERBOSE << "Size of tau1 container: " << tauContainer1->size()<< endmsg; 
    }
    
    int tau1_count,mu1_count;
    tau1_count=mu1_count=0; 
    
    for ( tau1 = tauContainer1->begin(); tau1 != tauContainer1->end(); tau1++ ) {
      if(msgLvl() <= MSG::VERBOSE) {
	tau1_count++;
	mu1_count=0;
      }
      if((*tau1)==0)
	{
	  msg() << MSG::DEBUG << "Null pointer in tauContainer1. Skipping." << endmsg;
	  continue;
	}
    
      for ( tr = MuEFTracksEF->begin(); tr != MuEFTracksEF->end(); ++tr ) {
	if(msgLvl() <= MSG::VERBOSE) {
	  mu1_count++;
	}
	if((*tr)==0)
	  {
	    msg() << MSG::DEBUG << "Null pointer in MuEF. Skipping." << endmsg;
	    continue;
	  }
	if((*tr)->indetTrack()==0)
	  {
	    if(msgLvl() <= MSG::DEBUG) 
	      msg() << MSG::DEBUG << "No indetTrack attached to muon. Skipping" << endmsg;
	    
	    continue;
	  }
	const Trk::Perigee* muon1 = (*tr)->indetTrack()->perigeeParameters();

	m_monCut = 1;
	ElectronMuonTopoInfo* TauTauTopoInfo = new ElectronMuonTopoInfo();
	TauTauTopoInfo->SetDeltaPhi(float(-999.0));
	TauTauTopoInfo->SetDeltaR(float(-999.0));
	TauTauTopoInfo->SetInvMass(float(-999.0));
	TauTauTopoInfo->SetOppositeCharge(false);
	TauTauTopoInfo->SetVertexState(1513);
	float mu_phi = muon1->parameters()[Trk::phi];	

	if(msgLvl() <= MSG::DEBUG) {
	  msg() << MSG::DEBUG << "New combination:" << endmsg; 
	  msg() << MSG::DEBUG << "tau1: addr=" << (*tau1) 
		<< "  et="    << (*tau1)->et() 
		<< "; eta="   << (*tau1)->eta() 
		<< "; phi="   << (*tau1)->phi() 
		<< endmsg;             
	  msg() << MSG::DEBUG << "muon: addr=" << muon1
		<< "  qoverp="  << muon1->parameters()[Trk::qOverP] 
		<< "; theta="   << muon1->parameters()[Trk::theta] 
		<< "; phi="   << mu_phi
		<< endmsg;       
	}
	
	// evaluate distance
	m_DEta = m_DPhi=m_DR = 100.; // setting value which can never pass
	m_DPhi = TauTauTopoInfo->deltaPhi((*tau1),muon1);
	m_DR = TauTauTopoInfo->deltaR((*tau1),muon1);
	
	m_DEta=sqrt(fabs(m_DR*m_DR - m_DPhi*m_DPhi));  
	
	if(msgLvl() <= MSG::VERBOSE) {
	  msg() << MSG::VERBOSE 
		<< "Delta Phi between tau1 and muon1 is " << m_DPhi 
		<<" Delta Eta is " << m_DEta 
		<<" Radial distance delta R is " << m_DR << endmsg;
	}
	
	m_monDPhiAll=m_DPhi;
	m_monDRAll=m_DR;
	m_monDEtaAll=m_DEta;
	
	if(m_DPhi > m_MaxDPhi) {
	  if(msgLvl() <= MSG::VERBOSE) 
	    msg() << MSG::VERBOSE << "Combination failed Delta Phi cut" << endmsg;
	  
	  delete TauTauTopoInfo;
	  continue;               
	}else{
	  if(msgLvl() <= MSG::VERBOSE) 
	    msg() << MSG::VERBOSE << "Combination passed Delta Phi cut of " << m_MaxDPhi << endmsg;
	  
	}
	
	m_monDPhiAccepted = m_DPhi;
	m_monCut = 2;
	
	TauTauTopoInfo->SetDeltaPhi(m_DPhi);
	if(m_MaxDR > 1e-11)
	  {
	    if(m_DR > m_MaxDR) {
	      if(msgLvl() <= MSG::VERBOSE) 
		msg() << MSG::VERBOSE << "Combination failed Delta R cut" << endmsg;
	      
	      delete TauTauTopoInfo;
	      continue;
	    }else{
	      if(msgLvl() <= MSG::VERBOSE) 
		msg() << MSG::VERBOSE << "Combination passed Delta R cut of " << m_MaxDR << endmsg;
	      
	    }       
	  }
	
	m_monDRAccepted = m_DR;       
	m_monCut = 3;
	m_monDEtaAccepted = m_DEta;
	TauTauTopoInfo->SetDeltaR(m_DR);
	TauTauTopoInfo->SetOppositeCharge(false);
	m_mass = TauTauTopoInfo->invariantMass((*tau1),muon1);
	TauTauTopoInfo->SetInvMass(m_mass);
	m_monMassAll = m_mass; 
	if(msgLvl() <= MSG::DEBUG){
	  msg() << MSG::DEBUG << "Created the following object: "
		<< (*TauTauTopoInfo)
		<< endmsg;
	}
	if ( !m_tautauTopoColl ) m_tautauTopoColl = new ElectronMuonTopoInfoContainer();
	m_tautauTopoColl->push_back(new ElectronMuonTopoInfo(*TauTauTopoInfo));
	delete TauTauTopoInfo;
	pass=true;
	
      } //muon1
    } //tau1
  }


  ///////////////////////////////////////////////////////////
  if(tauelec){

    if(vectorTauContainers1.size() == 0){
      if( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG << " REGTEST: Received 0 taucontainers  "
	      << "This algorithm is designed to work with  one tau container per TE."
	      << endmsg;
      return HLT::OK;
    }
    

    const Analysis::TauJetContainer *tauContainer1 = vectorTauContainers1.back();
    Analysis::TauJetContainer::const_iterator tau1;
    egammaContainer::const_iterator electron1;
    
    if(msgLvl() <= MSG::VERBOSE) {
      msg() << MSG::VERBOSE << "Size of tau1 container: " << tauContainer1->size()<< endmsg; 
    }
    
    int tau1_count,e1_count;
    tau1_count=e1_count=0; 
    
    for ( tau1 = tauContainer1->begin(); tau1 != tauContainer1->end(); tau1++ ) {
      if(msgLvl() <= MSG::VERBOSE) {
	tau1_count++;
	e1_count=0;
      }
      if((*tau1)==0)
	{
	  msg() << MSG::DEBUG << "Null pointer in tauContainer1. Skipping." << endmsg;
	  continue;
	}
      for ( electron1 = electronContainer1->begin(); electron1 != electronContainer1->end(); ++electron1 ) {
	if(msgLvl() <= MSG::VERBOSE) {
	  e1_count++;
	}
	if((*electron1)==0)
	  {
	    msg() << MSG::DEBUG << "Null pointer in egammaContainer. Skipping." << endmsg;
	    continue;
	  }
	if((*electron1)->trackParticle()==0)
	  {
	    if(msgLvl() <= MSG::DEBUG) 
	      msg() << MSG::DEBUG << "No trackParticle attached to electron. Skipping" << endmsg;
	    
	    continue;
	  }

	m_monCut = 1;
	ElectronMuonTopoInfo* TauTauTopoInfo = new ElectronMuonTopoInfo();
	TauTauTopoInfo->SetDeltaPhi(float(-999.0));
	TauTauTopoInfo->SetDeltaR(float(-999.0));
	TauTauTopoInfo->SetInvMass(float(-999.0));
	TauTauTopoInfo->SetOppositeCharge(false);
	TauTauTopoInfo->SetVertexState(1511);
	
	if(msgLvl() <= MSG::DEBUG) {
	  msg() << MSG::DEBUG << "New combination:" << endmsg; 
	  msg() << MSG::DEBUG << "tau1: addr=" << (*tau1) 
		<< "  et="    << (*tau1)->et() 
		<< "; eta="   << (*tau1)->eta() 
		<< "; phi="   << (*tau1)->phi() 
		<< endmsg;             
	  msg() << MSG::DEBUG << "egammaElectron: addr=" << (*electron1)
		<< "  et="    << (*electron1)->cluster()->e()/cosh((*electron1)->trackParticle()->eta()) 
		<< "; eta="   << (*electron1)->trackParticle()->eta()
		<< "; phi="   << (*electron1)->trackParticle()->phi()
		<< "; charge=" << (*electron1)->trackParticle()->charge()
		<< "; #Silicon hits=" <<
	    (*electron1)->trackParticle()->trackSummary()->get( Trk::numberOfPixelHits )+ (*electron1)->trackParticle()->trackSummary()->get( Trk::numberOfSCTHits )
		<< endmsg;
	}
	
	// evaluate distance
	m_DEta = m_DPhi=m_DR = 100.; // setting value which can never pass
	m_DPhi = TauTauTopoInfo->deltaPhi((*tau1),(*electron1));
	m_DR = TauTauTopoInfo->deltaR((*tau1),(*electron1));
	
	m_DEta=sqrt(fabs(m_DR*m_DR - m_DPhi*m_DPhi));  
	
	if(msgLvl() <= MSG::VERBOSE) {
	  msg() << MSG::VERBOSE 
		<< "Delta Phi between tau1 and muon1 is " << m_DPhi 
		<<" Delta Eta is " << m_DEta 
		<<" Radial distance delta R is " << m_DR << endmsg;
	}
	
	m_monDPhiAll=m_DPhi;
	m_monDRAll=m_DR;
	m_monDEtaAll=m_DEta;
	
	if(m_DPhi > m_MaxDPhi) {
	  if(msgLvl() <= MSG::VERBOSE) 
	    msg() << MSG::VERBOSE << "Combination failed Delta Phi cut" << endmsg;
	  
	  delete TauTauTopoInfo;
	  continue;               
	}else{
	  if(msgLvl() <= MSG::VERBOSE) 
	    msg() << MSG::VERBOSE << "Combination passed Delta Phi cut of " << m_MaxDPhi << endmsg;
	  
	}
	
	m_monDPhiAccepted = m_DPhi;
	m_monCut = 2;
	
	TauTauTopoInfo->SetDeltaPhi(m_DPhi);
	if(m_MaxDR > 1e-11)
	  {
	    if(m_DR > m_MaxDR) {
	      if(msgLvl() <= MSG::VERBOSE) 
		msg() << MSG::VERBOSE << "Combination failed Delta R cut" << endmsg;
	      
	      delete TauTauTopoInfo;
	      continue;
	    }else{
	      if(msgLvl() <= MSG::VERBOSE) 
		msg() << MSG::VERBOSE << "Combination passed Delta R cut of " << m_MaxDR << endmsg;
	      
	    }       
	  }
	
	m_monDRAccepted = m_DR;       
	m_monCut = 3;
	m_monDEtaAccepted = m_DEta;
	TauTauTopoInfo->SetDeltaR(m_DR);
	TauTauTopoInfo->SetOppositeCharge(false);
	m_mass = TauTauTopoInfo->invariantMass((*tau1),(*electron1));
	TauTauTopoInfo->SetInvMass(m_mass);
	m_monMassAll = m_mass; 
	if(msgLvl() <= MSG::DEBUG){
	  msg() << MSG::DEBUG << "Created the following object: "
		<< (*TauTauTopoInfo)
		<< endmsg;
	}
	if ( !m_tautauTopoColl ) m_tautauTopoColl = new ElectronMuonTopoInfoContainer();
	m_tautauTopoColl->push_back(new ElectronMuonTopoInfo(*TauTauTopoInfo));
	delete TauTauTopoInfo;
	pass=true;
	
      } //muon1
    } //tau1
  }

  
  m_moncombtype=0;
  if(tautau)m_moncombtype=1;
  if(taumu)m_moncombtype=2;
  if(tauelec)m_moncombtype=3;

  // set output TriggerElement true if good combination  
  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "pass = " << pass << endmsg;
  return HLT::OK;


}

HLT::ErrorCode
EFTauTauCombFexAlgo::hltExecute(HLT::TEConstVec& /*inputTE*/, HLT::TriggerElement* outputTE){
  if(msgLvl() <= MSG::VERBOSE) {
    msg() << MSG::VERBOSE << "Attaching TauTauTopoInfoCollection of the length " <<  m_tautauTopoColl->size() << endmsg;
  }
  if(m_tautauTopoColl!=0) attachFeature(outputTE, m_tautauTopoColl, "EF_TauTauTopoFEX"); 
  return HLT::OK;
}






