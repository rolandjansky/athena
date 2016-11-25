// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 ** File: Trigger/TrigHypothesis/TrigTauHypo/T2TauTauCombFexAlgo.cxx
 **
 ** Description: FEX algorithm: search for pairs of taus with delta eta in some interval +
 **              some additional cuts; intended for H-> tau tau SM and MSSM
 **
 ** Author: Phillip Urquijo <Phillip.Urquijo@cern.ch>
 ** Created: July 27
 **************************************************************************/

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigTauHypo/T2TauTauCombFexAlgo.h"

#include <math.h>

T2TauTauCombFexAlgo::T2TauTauCombFexAlgo(const std::string & name, ISvcLocator* pSvcLocator) 
  : HLT::ComboAlgo(name, pSvcLocator), 
    m_tautauTopoColl(0)
{

  //Read cuts
  declareProperty("AcceptAll",    m_acceptAll=false);
  declareProperty("ValidTau",     m_tauValid=true);
  declareProperty("MaxDRCut",     m_MaxDR=5.0); 
  declareProperty("MaxDPhiCut",   m_MaxDPhi=5.0);
  declareProperty("MaxDEtaCut",   m_MaxDEta=5.0);
  declareProperty("LowerMassCut", m_lowerMassCut=0.0);
  declareProperty("UpperMassCut", m_upperMassCut=500000.0);

 // monitoring
  declareMonitoredVariable("massOfAccepted", m_monMassAccepted);
  declareMonitoredVariable("massOfAll",      m_monMassAll); 
  declareMonitoredVariable("cut",            m_monCut);
  declareMonitoredVariable("dPhiOfAccepted", m_monDPhiAccepted);
  declareMonitoredVariable("dPhiOfAll",      m_monDPhiAll);
  declareMonitoredVariable("dROfAccepted",   m_monDRAccepted);
  declareMonitoredVariable("dROfAll",        m_monDRAll);
  declareMonitoredVariable("dEtaOfAccepted", m_monDEtaAccepted);
  declareMonitoredVariable("dEtaOfAll",      m_monDEtaAll);
  declareMonitoredVariable("combtype",           m_moncombtype);


  m_DEta = m_DPhi = m_DR = 100.;
  m_mass=-1.;
}

T2TauTauCombFexAlgo::~T2TauTauCombFexAlgo()
{ }

HLT::ErrorCode T2TauTauCombFexAlgo::hltInitialize()
{
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Initialization:" << endmsg;
  }
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Initialization completed successfully:" << endmsg;
    msg() << MSG::DEBUG << "AcceptAll            = " 
          << (m_acceptAll==true ? "True" : "False") << endmsg; 
//     msg() << MSG::DEBUG << "ValidTau        = " 
//           << (m_tauValid==true ? "True" : "False") << endmsg; 
    msg() << MSG::DEBUG << "DeltaPhiCut         = " << m_MaxDPhi << endmsg;
    msg() << MSG::DEBUG << "DeltaPhiCut         = " << m_MaxDPhi << endmsg;
    msg() << MSG::DEBUG << "MaxDeltaRCut         = " << m_MaxDR << endmsg;
    msg() << MSG::DEBUG << "LowerMassCut         = " << m_lowerMassCut << endmsg;
    msg() << MSG::DEBUG << "UpperMassCut         = " << m_upperMassCut << endmsg;
  }
  return HLT::OK;
}

HLT::ErrorCode T2TauTauCombFexAlgo::hltFinalize()
{
  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endmsg;
  return HLT::OK;
}

HLT::ErrorCode T2TauTauCombFexAlgo::acceptInputs(HLT::TEConstVec& inputTE, bool& pass ) {
  m_monMassAccepted = -999.;
  m_monCut=0;
  m_tautauTopoColl=0;
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "Running T2TauTauCombFexAlgo::acceptInputs" << endmsg;
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

  // The 2 TEs which we will eventually combine
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
    //  return HLT::MISSING_FEATURE; 
    } else {
      if ( msgLvl() <= MSG::DEBUG )
        msg() << MSG::DEBUG  << "Trying to combine 2 RoIs: " << *roiDescriptor1 << " & " << *roiDescriptor2 << endmsg;
    }
  }

  // retrieve TrigTauContainers from this TE
  bool tautau=false;
  bool tauelec=false;
  bool taumu=false;
  bool hasmu=false;
  bool haselec=false;
  bool hastau=false;

  //////////////////////////////////
  //TauTau combinations
  const TrigTau* tau1(0);
  const TrigTau* tau2(0);

  if(m_acceptAll){
    pass=true;
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Force accept! pass = " << pass << endmsg;
    return HLT::OK;
  }
  
  if ( getFeature(te1, tau1) == HLT::OK){
    if( getFeature(te2, tau2) == HLT::OK){
      if ( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG  << "TrigTauContainers 1&2 collection successfully retrieved" << endmsg;     
      tautau=true;
       
    }
    else{
      if ( msgLvl() <= MSG::DEBUG) 
	msg() << MSG::DEBUG << "Failed to get TrigTauContainer 2" << endmsg;
    }
  }
  else{
    if ( msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "Failed to get TrigTauContainer 1" << endmsg;
  }
  
  pass =false;

  //now we have 2 containers of taus and we can find out whether they can form topological combinations
  
  m_monCut = 1;
  if(tau1==0){
    msg()<<MSG::DEBUG << "Null point in TrigTauContainer 1."<<endmsg;
    tautau=false;
  }

  if(tau2==0){
    msg() << MSG::DEBUG << "Null pointer in TrigTauContainer 2."<<endmsg;
    tautau=false;
  }

  
  /////////////////////////////////////
  //Tau Muon combinations
  if(!tautau){
    if ( getFeature(te1, tau1) != HLT::OK || tau1 == 0)
      {
	if(getFeature(te2, tau1) != HLT::OK  || tau1 == 0)
	  {
	    if ( msgLvl() <= MSG::DEBUG) 
	      msg() << MSG::DEBUG << "Failed to get TrigTauContainer" << endmsg;
	    
	  }
	else{
	  if ( msgLvl() <= MSG::DEBUG )
	    msg() << MSG::DEBUG  << "TrigTauContainer successfully retrieved" << endmsg;     
	  hastau=true;
	}
	
      }else{
      if ( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG  << "TrigTauContainer successfully retrieved" << endmsg;     
      hastau=true;
    }
  }

  std::vector<const CombinedMuonFeature*> muonContainer1;
  if ( getFeatures(te1, muonContainer1) != HLT::OK || muonContainer1.size() == 0)
    {
      if(getFeatures(te2, muonContainer1) != HLT::OK  || muonContainer1.size() == 0)
	{
	  if ( msgLvl() <= MSG::DEBUG) 
	    msg() << MSG::DEBUG << "Failed to get MuonCombinedFeature collection" << endmsg;
	  
	}
      else{
	if ( msgLvl() <= MSG::DEBUG )
	  msg() << MSG::DEBUG  << "MuonCombinedFeature collection successfully retrieved" << endmsg;     
	hasmu=true;
      }
      
    }else{
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG  << "MuonCombinedFeature collection successfully retrieved" << endmsg;     
    hasmu=true;
  }
  
  const TrigElectronContainer* electronContainer1(0);
  if ( getFeature(te1, electronContainer1) != HLT::OK || electronContainer1 == 0)
    {
      if(getFeature(te2, electronContainer1) != HLT::OK  || electronContainer1 == 0)
	{
	  if ( msgLvl() <= MSG::DEBUG) 
	    msg() << MSG::DEBUG << "Failed to get TrigElectron collection" << endmsg;
	  
	}
      else{
	if ( msgLvl() <= MSG::DEBUG )
	  msg() << MSG::DEBUG  << " TrigElectron collection successfully retrieved" << endmsg;     
	haselec=true;
      }
      
    }else{
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG  << " TrigElectron collection collection successfully retrieved" << endmsg;     
    haselec=true;
  }

  if(hastau&&hasmu)taumu=true;
  if(hastau&&haselec)tauelec=true;

  if(!tautau&&!taumu&&!tauelec)
    if ( msgLvl() <= MSG::WARNING ){
      msg() << MSG::WARNING  << " No topo candidate features" << endmsg;     
      return HLT::MISSING_FEATURE;
    }
  
  if(tautau){
    ///TauTau combinations first
    ElectronMuonTopoInfo* TauTauTopoInfo = new ElectronMuonTopoInfo();
    
    if(msgLvl() <= MSG::DEBUG){
      msg() << MSG::DEBUG << "New combination:" << endmsg; 
      msg() << MSG::DEBUG << "1st TrigTau: addr=" << tau1
	    << "RoI id="  << tau1->roiId()
	    << "; pt="    << tau1->pt() 
	    << "; eta="   << tau1->eta() 
	    << "; phi="   << tau1->phi() 
	    << "; Zvtx="  << tau1->Zvtx()<<"+-"<< tau1->err_Zvtx() 
	    << endmsg;
      msg() << MSG::DEBUG << "2nd TrigTau: addr=" << (*tau2)
	    << " RoI id=" << tau2->roiId()
	    << " pt="     << tau2->pt() 
	    << "; eta="   << tau2->eta()  
	    << "; phi="   << tau2->phi()  
	    << "; Zvtx="  << tau2->Zvtx()<<"+-"<< tau2->err_Zvtx() 
	    << endmsg;
    }
    
    TauTauTopoInfo->SetRoiWord(tau1->roiId());
    TauTauTopoInfo->SetElecValid(true);
    m_DPhi=50., m_DR = 100.; // setting value which can never pass
    m_DPhi = TauTauTopoInfo->deltaPhi(tau1,tau2);
    m_DR   = TauTauTopoInfo->deltaR(tau1,tau2);
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
    TauTauTopoInfo->SetDeltaPhi(m_DPhi);
    TauTauTopoInfo->SetDeltaR(m_DR);
    TauTauTopoInfo->SetOppositeCharge(true);
    TauTauTopoInfo->SetVertexState(1515);
    m_mass = TauTauTopoInfo->invariantMass(tau1,tau2);
    m_monMassAll = m_mass;
    TauTauTopoInfo->SetInvMass(m_mass);
    pass = true;
    
    if(msgLvl() <= MSG::DEBUG){
      msg() <<MSG::DEBUG << "Created following object: "
	    <<(*TauTauTopoInfo)
	    <<endmsg;
    }
    
    if ( !m_tautauTopoColl ) m_tautauTopoColl = new ElectronMuonTopoInfoContainer();
    m_tautauTopoColl->push_back(new ElectronMuonTopoInfo(*TauTauTopoInfo));
    delete TauTauTopoInfo;
  }

    ///TauTau combinations first
  TrigElectronContainer::const_iterator electron1;
  if(tauelec){
    for ( electron1 = electronContainer1->begin(); electron1 != electronContainer1->end(); ++electron1 ) {
      if((*electron1)==0)
	{
	  msg() << MSG::DEBUG << "Null pointer in TrigElectronContainer. Skipping." << endmsg;
	  continue;
	}
      ElectronMuonTopoInfo* TauTauTopoInfo = new ElectronMuonTopoInfo();
      
      if(msgLvl() <= MSG::DEBUG){
	msg() << MSG::DEBUG << "New combination:" << endmsg; 
	msg() << MSG::DEBUG << "1st TrigTau: addr=" << tau1
	      << "RoI id="  << tau1->roiId()
	      << "; pt="    << tau1->pt() 
	      << "; eta="   << tau1->eta() 
	      << "; phi="   << tau1->phi() 
	      << "; Zvtx="  << tau1->Zvtx()<<"+-"<< tau1->err_Zvtx() 
	      << endmsg;
	msg() << MSG::DEBUG << "1st TrigElectron: addr=" << (*electron1)
	      << ((*electron1)->isValid() ? "(valid" : "(not valid")
	      << ") RoI id="<< (*electron1)->roiId()
	      << "; pt="    << (*electron1)->pt() 
	      << "; eta="   << (*electron1)->eta() <<"+-"<< (*electron1)->err_eta()
	      << "; phi="   << (*electron1)->phi() <<"+-"<< (*electron1)->err_phi()
	      << "; Zvtx="  << (*electron1)->Zvtx()<<"+-"<< (*electron1)->err_Zvtx()
	      <<endmsg;
	  }
      TauTauTopoInfo->SetRoiWord(tau1->roiId());
      TauTauTopoInfo->SetElecValid(true);
      m_DPhi=50., m_DR = 100.; // setting value which can never pass
      m_DPhi = TauTauTopoInfo->deltaPhi(tau1,(*electron1));
      m_DR   = TauTauTopoInfo->deltaR(tau1,(*electron1));
      m_DEta=sqrt(fabs(m_DR*m_DR - m_DPhi*m_DPhi));  
      if(msgLvl() <= MSG::VERBOSE) {
	msg() << MSG::VERBOSE 
	      << "Delta Phi between tau1 and electron1 is " << m_DPhi 
	      <<" Delta Eta is " << m_DEta 
	      <<" Radial distance delta R is " << m_DR << endmsg;
      }
      
      m_monDPhiAll=m_DPhi;
      m_monDRAll=m_DR;
      m_monDEtaAll=m_DEta;
      TauTauTopoInfo->SetDeltaPhi(m_DPhi);
      TauTauTopoInfo->SetDeltaR(m_DR);
      TauTauTopoInfo->SetOppositeCharge(true);
      TauTauTopoInfo->SetVertexState(1511);
      m_mass = TauTauTopoInfo->invariantMass(tau1,(*electron1));
      m_monMassAll = m_mass;
      TauTauTopoInfo->SetInvMass(m_mass);
      pass = true;
      
      if(msgLvl() <= MSG::DEBUG){
	msg() <<MSG::DEBUG << "Created following object: "
	      <<(*TauTauTopoInfo)
	      <<endmsg;
      }
      
      if ( !m_tautauTopoColl ) m_tautauTopoColl = new ElectronMuonTopoInfoContainer();
      m_tautauTopoColl->push_back(new ElectronMuonTopoInfo(*TauTauTopoInfo));
      delete TauTauTopoInfo;
      
    }
  }
    
    std::vector<const CombinedMuonFeature*>::iterator muon1;
  if(taumu){
    for ( muon1 = muonContainer1.begin(); muon1 != muonContainer1.end(); ++muon1 ) {
      if((*muon1)==0)
	{
	  msg() << MSG::DEBUG << "Null pointer in CombinedMuonFeature vector. Skipping." << endmsg;
	  continue;
	}
      if((*muon1)->IDTrack()==0)
	{
	  if(msgLvl() <= MSG::DEBUG) {
	    msg() << MSG::DEBUG << "Null pointer to IDTrack from muon. Skipping." << endmsg;
	  }
	  continue;       
	}
      
      ElectronMuonTopoInfo* TauTauTopoInfo = new ElectronMuonTopoInfo();
      
      if(msgLvl() <= MSG::DEBUG){
	msg() << MSG::DEBUG << "New combination:" << endmsg; 
	msg() << MSG::DEBUG << "1st TrigTau: addr=" << tau1
	      << "RoI id="  << tau1->roiId()
	      << "; pt="    << tau1->pt() 
	      << "; eta="   << tau1->eta() 
	      << "; phi="   << tau1->phi() 
	      << "; Zvtx="  << tau1->Zvtx()<<"+-"<< tau1->err_Zvtx() 
	      << endmsg;
	msg() << MSG::DEBUG << "1st CombinedMuonFeature: addr=" << (*muon1)
	      << " pt=" << 1/(*muon1)->ptq() << " trkAddr=" << (*muon1)->IDTrack()
	      << " " << *(*muon1)
	      << "; Zvtx =" << (*muon1)->IDTrack()->param()->z0() 
	      << "+-" << (*muon1)->IDTrack()->param()->ez0()
	      << endmsg;
      }
      
      TauTauTopoInfo->SetRoiWord(tau1->roiId());
      TauTauTopoInfo->SetElecValid(true);
      m_DPhi=50., m_DR = 100.; // setting value which can never pass
      m_DPhi = TauTauTopoInfo->deltaPhi(tau1,(*muon1));
      m_DR   = TauTauTopoInfo->deltaR(tau1,(*muon1));
      m_DEta=sqrt(fabs(m_DR*m_DR - m_DPhi*m_DPhi));  
      if(msgLvl() <= MSG::VERBOSE) {
	msg() << MSG::VERBOSE 
	      << "Delta Phi between tau1 and electron1 is " << m_DPhi 
	      <<" Delta Eta is " << m_DEta 
	      <<" Radial distance delta R is " << m_DR << endmsg;
      }
      
      m_monDPhiAll=m_DPhi;
      m_monDRAll=m_DR;
      m_monDEtaAll=m_DEta;
      TauTauTopoInfo->SetDeltaPhi(m_DPhi);
      TauTauTopoInfo->SetDeltaR(m_DR);
      TauTauTopoInfo->SetOppositeCharge(true);
      TauTauTopoInfo->SetVertexState(1513);
      m_mass = TauTauTopoInfo->invariantMass(tau1,(*muon1));
      m_monMassAll = m_mass;
      TauTauTopoInfo->SetInvMass(m_mass);
      pass = true;
      
      if(msgLvl() <= MSG::DEBUG){
	msg() <<MSG::DEBUG << "Created following object: "
	      <<(*TauTauTopoInfo)
	      <<endmsg;
      }
      
      if ( !m_tautauTopoColl ) m_tautauTopoColl = new ElectronMuonTopoInfoContainer();
      m_tautauTopoColl->push_back(new ElectronMuonTopoInfo(*TauTauTopoInfo));
      delete TauTauTopoInfo;
    }
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
T2TauTauCombFexAlgo::hltExecute(HLT::TEConstVec& /*inputTE*/, HLT::TriggerElement* outputTE){
  if(msgLvl() <= MSG::VERBOSE) {
    msg() << MSG::VERBOSE << "Attaching TauTauTopoInfoCollection of the length " <<  m_tautauTopoColl->size() << endmsg;
  }
  if(m_tautauTopoColl!=0) attachFeature(outputTE, m_tautauTopoColl, "TauTauTopoFEX"); 
  return HLT::OK;
}


