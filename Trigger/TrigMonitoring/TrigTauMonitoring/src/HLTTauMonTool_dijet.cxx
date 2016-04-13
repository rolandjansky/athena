/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/IJobOptionsSvc.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"

#include "AnalysisUtils/AnalysisMisc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "EventInfo/TriggerInfo.h"
#include "TrigSteeringEvent/HLTResult.h"
#include "EventInfo/EventInfo.h"
#include <EventInfo/EventID.h>
#include "xAODEventInfo/EventInfo.h"

#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/Feature.h"
#include "TrigDecisionTool/ChainGroup.h"
#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"

#include "xAODTau/TauJet.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauDefs.h"

#include "xAODTrigger/EmTauRoI.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "xAODTrigger/EnergySumRoI.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/TruthVertexContainer.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"

#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetConstituentVector.h"

#include "VxVertex/VxContainer.h"

#include "TROOT.h"
#include "TH1I.h"
#include "TH1F.h"
#include "TH2I.h"
#include "TH2F.h"
#include "TEfficiency.h"

#include <vector>
#include <iostream>
#include <fstream>
//#define _USE_MATH_DEFINES
#include <math.h>


#include "TrigHLTMonitoring/IHLTMonTool.h"
#include "TrigConfHLTData/HLTChain.h"
#include "HLTTauMonTool.h"

using namespace std;

//--------------------------------------------------------------------------------------
StatusCode HLTTauMonTool::dijetFakeTausEfficiency()
{
  ATH_MSG_DEBUG("dijet Fake Taus Efficiency");  
  setCurrentMonGroup("HLT/TauMon/Expert/dijetFakeTausEff");

  
  const xAOD::JetContainer* jet_cont = 0;
  if(evtStore()->retrieve(jet_cont, "AntiKt4EMTopoJets" ).isFailure())
    {
      ATH_MSG_WARNING("Failed to retrieve offline Jet container. Exiting.");
      return StatusCode::FAILURE;
    }
  
  const xAOD::JetContainer* HLTjet_cont = 0;
  if(evtStore()->retrieve(HLTjet_cont, "HLT_xAOD__JetContainer_a4tcemsubjesFS" ).isFailure())
    {
      ATH_MSG_WARNING("Failed to retrieve HLT Jet container. Exiting.");
      return StatusCode::FAILURE;
    }

  const xAOD::TauJetContainer * reco_cont = 0;
  if( evtStore()->retrieve(reco_cont, "TauJets").isFailure() )
    {
      ATH_MSG_WARNING("Failed to retrieve  TauJets container. Exiting.");
      return StatusCode::FAILURE;
    }

  
  double pt_lead = -1;
  double pt_sub = -1;
  double pt_cut = 380000.;
  bool off_match = false;
  //  bool tau_matching = false;
  TLorentzVector LeadJetTLV(0.,0.,0.,0.);
  TLorentzVector SubLeadJetTLV(0.,0.,0.,0.);
  const std::string theChain = "j360";
  const std::string theContainer = "a4tcemsubjesFS";
  bool leadjet = false;
  bool subleadjet = false;

  if(!getTDT()->isPassed(Form("HLT_%s",theChain.c_str())))
    {
      ATH_MSG_WARNING("Failed to pass single jet trigger. Exiting.");
      return StatusCode::FAILURE;
    }

  if(getTDT()->isPassed(Form("HLT_%s",theChain.c_str()))){ //events fired by single jet trigger
    auto cg = getTDT()->getChainGroup(Form("HLT_%s",theChain.c_str())); 
    auto fc = cg->features();
    
    auto JetFeatureContainers = fc.get<xAOD::JetContainer>(theContainer.c_str()); //get features container
    
    for(auto jcont : JetFeatureContainers) {
      for (auto j : *jcont.cptr()) {
	// selection of leading and sub leading jet                                                                                                
	double pt_jet = j->pt();
	double eta_jet = j->eta();
	double phi_jet = j->phi();
	
	if(pt_jet < pt_cut) continue;
	if(fabs(eta_jet) > 2.5) continue;
	
        if (pt_jet > pt_lead)
          {
	    pt_sub = pt_lead;
	    SubLeadJetTLV = LeadJetTLV;
            pt_lead = pt_jet;
            LeadJetTLV.SetPtEtaPhiE(pt_lead,eta_jet,phi_jet,j->e());
	    leadjet = true;
	    
          }
        if(pt_jet < pt_lead && pt_jet > pt_sub)
          {
            pt_sub = pt_jet;
            SubLeadJetTLV.SetPtEtaPhiE(pt_sub,eta_jet,phi_jet,j->e());
	    subleadjet = true;
          }
      }
    }//jet features
    
    // check if leading and sub leading jets exist + are back to back + pt balanced
    if(leadjet && subleadjet && deltaPhi(LeadJetTLV.Phi(),SubLeadJetTLV.Phi()) >= 2.5 && (LeadJetTLV.Pt()-SubLeadJetTLV.Pt())/(LeadJetTLV.Pt()+SubLeadJetTLV.Pt()) < 0.3) {
      
      xAOD::JetContainer::const_iterator jetItr, jet_cont_end = jet_cont->end();
      
      double dr_lim = 0.2;
      
      // to check if leading jet matches offline jet                                
      for(jetItr=jet_cont->begin(); jetItr!=jet_cont_end; ++jetItr)
	{
	  if ((*jetItr)->pt() < 20000 || TMath::Abs((*jetItr)->eta()) > 2.5) continue;
	  double dr_hlt = deltaR((*jetItr)->eta(),LeadJetTLV.Eta(),(*jetItr)->phi(),LeadJetTLV.Phi());
	  if(dr_hlt>dr_lim) continue;
	  off_match = true;
	}
      
      if(off_match)
	{
	  
	  /*	  xAOD::TauJetContainer::const_iterator recoItr, reco_cont_end = reco_cont->end();
	  //to check if leading jets matches an  offline tau
	  for(recoItr=reco_cont->begin(); recoItr!=reco_cont_end; ++recoItr)
	    {
	      TLorentzVector TauTLV = (*recoItr)->p4();
	      double pt_Tau = TauTLV.Pt();
	      double eta_Tau = TauTLV.Eta();
	      double phi_Tau = TauTLV.Phi();
	      int ntrack_Tau = (*recoItr)->nTracks();
	      bool good_Tau = (*recoItr)->isTau(xAOD::TauJetParameters::JetBDTSigMedium);
	      float charge_Tau = (*recoItr)->charge();
	      if(pt_Tau<20000.) continue;
	      if(fabs(eta_Tau) > 2.5) continue;
	      if(fabs(charge_Tau) != 1.0) continue;
	      if(ntrack_Tau!=1 && ntrack_Tau!=3) continue;
	      if(!good_Tau) continue;
	      double dR = deltaR(LeadJetTLV.Eta(),eta_Tau,LeadJetTLV.Phi(),phi_Tau);                                                                
	      if(dR > dr_lim) continue;                                                                                                             
	      tau_matching = true;
	    }
	  
	  if(tau_matching){
	  */
	    for(unsigned int i=0;i<m_trigItemsHighPt.size();++i)
	      {
		std::string l1_chain(LowerChain("HLT_"+m_trigItemsHighPt[i]));
		setCurrentMonGroup("HLT/TauMon/Expert/dijetFakeTausEff/"+m_trigItemsHighPt[i]);
		hist("hdijetFakeTausPtDenom")->Fill(SubLeadJetTLV.Pt()/1000.);
		if(L1TauMatching(l1_chain, SubLeadJetTLV, 0.3))  hist("hdijetFakeTausL1PtNum")->Fill(SubLeadJetTLV.Pt()/1000.);
		if(HLTTauMatching(m_trigItemsHighPt[i], SubLeadJetTLV, 0.2)) hist("hdijetFakeTausHLTPtNum")->Fill(SubLeadJetTLV.Pt()/1000.);
	      }
	  }
      //}
    }
    
  }
  
  return StatusCode::SUCCESS;
  
}
