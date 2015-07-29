/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*    @file HLTTauMonTool_Ztautauemulation.cxx
 *    
 *    Real Ztauau L1 & HLT trigger efficiency 
 *    created by Babar Ali (babar.ali@cern.ch)
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
StatusCode HLTTauMonTool::RealZTauTauEfficiency()
{
  ATH_MSG_DEBUG("Real ZTauTau Efficiency");  
  setCurrentMonGroup("HLT/TauMon/Expert/RealZtautauEff");

  const xAOD::TauJetContainer * reco_cont = 0;
  const xAOD::MuonContainer * muon_cont = 0;
  const xAOD::MissingETContainer *m_off_met_cont = 0;
  const xAOD::TruthParticleContainer* truth_cont = 0;
  
  if( evtStore()->retrieve(reco_cont, "TauJets").isFailure() )
    {
      ATH_MSG_WARNING("Failed to retrieve  TauJets container. Exiting.");
      return StatusCode::FAILURE;
    } 
  if(evtStore()->retrieve(muon_cont, "Muons").isFailure())
    {
      ATH_MSG_WARNING("Failed to retrieve Muons container. Exiting.");
      return StatusCode::FAILURE;
    }
  StatusCode sc = m_storeGate->retrieve(m_off_met_cont, "MET_Reference_AntiKt4LCTopo");
  if (sc.isFailure() || !m_off_met_cont) 
    {
      ATH_MSG_WARNING("Could not retrieve Reconstructed MET term with Key MET_Reference_AntiKt4LCTopo : m_off_met_cont = 0");
    }
  
  if(evtStore()->retrieve(truth_cont, "TruthParticles" ).isFailure())
    {
      ATH_MSG_WARNING("Failed to retrieve TruthParticle container. Exiting.");
      return StatusCode::FAILURE;
    }
  
  hist("hCutFlow")->Fill(CutItems.at(0).c_str(),1.);
  hist("hTauCutFlow")->Fill(TauCutFlow.at(0).c_str(),1.);
  hist("hMuCutFlow")->Fill(MuCutFlow.at(0).c_str(),1.);

  TLorentzVector Tau_TLV(0.,0.,0.,0.);
  TLorentzVector Muon_TLV(0.,0.,0.,0.);
  TLorentzVector MET_TLV(0.,0.,0.,0.);

  double tauPt_dum = -1., muPt_dum = -1.;
  float Tau_charge = -99.0, Muon_charge = -99.0;
  bool selected_tau = false, selected_mu = false;
  int Tau_ntrack = -1, count_selected_tau = 0, count_selected_mu = 0, count_cand_tau = 0, count_cand_mu = 0;
 
  xAOD::TauJetContainer::const_iterator recoItr, reco_cont_end = reco_cont->end();
  for(recoItr=reco_cont->begin(); recoItr!=reco_cont_end; ++recoItr)
    {
    TLorentzVector TauTLV = (*recoItr)->p4();
    double pt_Tau = TauTLV.Pt();
    double eta_Tau = TauTLV.Eta();
    double phi_Tau = TauTLV.Phi();
    int ntrack_Tau = (*recoItr)->nTracks();
    bool good_Tau = (*recoItr)->isTau(xAOD::TauJetParameters::JetBDTSigMedium);
    float charge_Tau = (*recoItr)->charge();

    hist("hRealBSTauPt")->Fill(pt_Tau/1000.);
    hist("hRealBSTauEta")->Fill(eta_Tau);
    hist("hRealBSTauPhi")->Fill(phi_Tau);
    hist("hRealBSTauNTrack")->Fill(ntrack_Tau);
  
    count_cand_tau++;
    if(pt_Tau<20000.) continue;
    hist("hTauCutFlow")->Fill(TauCutFlow.at(1).c_str(),1.);
    if(fabs(eta_Tau) > 2.47) continue;
    hist("hTauCutFlow")->Fill(TauCutFlow.at(2).c_str(),1.);
    if(fabs(charge_Tau) != 1.0) continue;
    hist("hTauCutFlow")->Fill(TauCutFlow.at(3).c_str(),1.);
    if(ntrack_Tau!=1 && ntrack_Tau!=3) continue;
    hist("hTauCutFlow")->Fill(TauCutFlow.at(4).c_str(),1.);
    if(!good_Tau) continue;
    hist("hTauCutFlow")->Fill(TauCutFlow.at(5).c_str(),1.);
    count_selected_tau++;
    if(tauPt_dum < pt_Tau)
      {
	tauPt_dum = pt_Tau;
	Tau_TLV.SetPtEtaPhiM(pt_Tau,eta_Tau,TauTLV.Phi(),TauTLV.M());
	Tau_charge = charge_Tau;
	Tau_ntrack = ntrack_Tau;
	selected_tau = true;
      }
    }

  hist("hRealSelectedTau")->Fill(count_selected_tau);
  hist("hRealCandTau")->Fill(count_cand_tau);
  hist("hRealASTauPt")->Fill(Tau_TLV.Pt()/1000.);
  hist("hRealASTauEta")->Fill(Tau_TLV.Eta());
  hist("hRealASTauPhi")->Fill(Tau_TLV.Phi());
  hist("hRealASTauNTrack")->Fill(Tau_ntrack);

  xAOD::MuonContainer::const_iterator muonItr, muon_cont_end = muon_cont->end();
  for(muonItr=muon_cont->begin(); muonItr!=muon_cont_end; ++muonItr)
    {
      const xAOD::Muon& muon = **muonItr;
      TLorentzVector MuonTLV = (*muonItr)->p4();
      double pt_mu = MuonTLV.Pt();
      double eta_mu = MuonTLV.Eta();
      double phi_mu = MuonTLV.Phi();
      
      ATH_MSG_DEBUG("Muon Type " << (*muonItr)->muonType());
      ATH_MSG_DEBUG("Muon quality " << (*muonItr)->quality());
      ATH_MSG_DEBUG("MuonAuthor " << (*muonItr)->author());
      
      hist("hRealBSMuPt")->Fill(pt_mu/1000.);
      hist("hRealBSMuEta")->Fill(eta_mu);
      hist("hRealBSMuPhi")->Fill(phi_mu);

      float etcone = 0.0, ptcone = 0.0, etcone20Rel = 0.0, ptcone20Rel = 0.0;
      if (!muon.isolation(etcone,xAOD::Iso::etcone20))
	{
	  ATH_MSG_WARNING("No EtCone defined for muon.");
	}
      else
	{
	  etcone20Rel = etcone/pt_mu;
	}
      if (!muon.isolation(ptcone,xAOD::Iso::ptcone20))
	{
	  ATH_MSG_WARNING("No ptCone defined for muon.");
	}
      else
	{
	  ptcone20Rel = ptcone/pt_mu;
	}
      
      count_cand_mu++;
      if(pt_mu<22000.) continue;
      hist("hMuCutFlow")->Fill(MuCutFlow.at(1).c_str(),1.);
      if(fabs(eta_mu)>2.4) continue;
      hist("hMuCutFlow")->Fill(MuCutFlow.at(2).c_str(),1.);
      if((*muonItr)->author() != xAOD::Muon::MuidCo) continue;
      hist("hMuCutFlow")->Fill(MuCutFlow.at(3).c_str(),1.);
      if(! muon.passesIDCuts() ) continue;
      hist("hMuCutFlow")->Fill(MuCutFlow.at(4).c_str(),1.);
      if(etcone20Rel > 0.2) continue;
      hist("hMuCutFlow")->Fill(MuCutFlow.at(5).c_str(),1.);
      if(ptcone20Rel > 0.06) continue;
      hist("hMuCutFlow")->Fill(MuCutFlow.at(6).c_str(),1.);
      count_selected_mu++;      
      if(selected_mu)
	{
	  selected_mu = false;
	  count_selected_mu = 0;
	  Muon_TLV.Clear();
	  Muon_charge = -99;
	  break;
	  cout << " Found 2nd MUON : " << endl;
	}
      if(muPt_dum < pt_mu)
	{
	  muPt_dum = pt_mu;
	  Muon_TLV.SetPtEtaPhiM(pt_mu, eta_mu, MuonTLV.Phi(), MuonTLV.M());
	  Muon_charge = (*muonItr)->charge();
	  selected_mu = true;
	}
    }
  hist("hRealSelectedMu")->Fill(count_selected_mu);
  hist("hRealCandMu")->Fill(count_cand_mu);
  hist("hRealASMuPt")->Fill(Muon_TLV.Pt()/1000.);
  hist("hRealASMuEta")->Fill(Muon_TLV.Eta());
  hist("hRealASMuPhi")->Fill(Muon_TLV.Phi());

  const xAOD::MissingET  *m_off_met = 0;
  float off_met = -9e9;
  //float off_met_phi = -9e9;
  if (m_off_met_cont && m_off_met_cont->size())
    {
      m_off_met = m_off_met_cont->at(0);
      float off_ex = (m_off_met_cont->at(0)->mpx());
      float off_ey = (m_off_met_cont->at(0)->mpy());
      //off_met_phi =  (m_off_met_cont->at(0)->phi());
      off_met = sqrt(off_ex*off_ex+off_ey+off_ey);
      ATH_MSG_DEBUG("m_off_met:" << m_off_met );
      MET_TLV.SetPxPyPzE(off_ex,off_ey,0,off_met);
    }
  //MET
  hist("hRealMET")->Fill(off_met/1000.);
  
  float ltau_charge = -99.0;
  double mltau_vis = -99.0;
  double cos_dphi = -99.0;
  double mt = -99.0;
  double ltau_dphi = -99.0;

  if(selected_tau) 
    {
      hist("hRealMuTau")->Fill(count_selected_mu); 
      hist("hCutFlow")->Fill(CutItems.at(1).c_str(),1.);
    }
  if(selected_mu)
    {
      hist("hRealTauMu")->Fill(count_selected_tau);
      hist("hCutFlow")->Fill(CutItems.at(2).c_str(),1.);
    }
  if(selected_tau && selected_mu)
    {
      hist("hCutFlow")->Fill(CutItems.at(3).c_str(),1.);   
      
      ltau_charge = Tau_charge + Muon_charge;
      mltau_vis = (Tau_TLV + Muon_TLV).M();
      cos_dphi = cos(Muon_TLV.DeltaPhi(MET_TLV)) + cos(Tau_TLV.DeltaPhi(MET_TLV)) ;
      mt = sqrt(2 * Muon_TLV.Pt() * off_met * (1 - cos(Muon_TLV.DeltaPhi(MET_TLV)) ) );
      ltau_dphi = Tau_TLV.DeltaPhi(Muon_TLV);
     
      //Tau vars
      hist("hRealTauPt")->Fill(Tau_TLV.Pt()/1000.);
      hist("hRealTauEta")->Fill(Tau_TLV.Eta());
      hist("hRealTauPhi")->Fill(Tau_TLV.Phi());
      hist("hRealTauNTrack")->Fill(Tau_ntrack);
      hist("hRealTauCharge")->Fill(Tau_charge);
      //Muon vars
      hist("hRealMuPt")->Fill(Muon_TLV.Pt()/1000.);
      hist("hRealMuEta")->Fill(Muon_TLV.Eta());
      hist("hRealMuPhi")->Fill(Muon_TLV.Phi());
      hist("hRealMuCharge")->Fill(Muon_charge);
      //Cut vars
      hist("hRealTauMuCosdPhi")->Fill(cos_dphi);	
      hist("hRealMETMuTransMass")->Fill(mt/1000.);
      hist("hRealTauMuVisMass")->Fill(mltau_vis/1000.);
      hist("hRealTauMuDPhi")->Fill(ltau_dphi);
      hist("hRealTauMuCharge")->Fill(ltau_charge);
    }
    
  if(ltau_charge == 0) hist("hCutFlow")->Fill(CutItems.at(4).c_str(),1.);
  if(mltau_vis > 42000 && mltau_vis < 82000) hist("hCutFlow")->Fill(CutItems.at(5).c_str(),1.);
  if(ltau_dphi > 2.4) hist("hCutFlow")->Fill(CutItems.at(6).c_str(),1.);
      
  //selection
  if(ltau_charge == 0. &&
     mltau_vis > 42000. && mltau_vis < 82000. &&
     cos_dphi > -0.15 &&
     mt < 50000. &&
     ltau_dphi > 2.4)
    {
      for(unsigned int i=0;i<m_trigItems.size();++i)
	{
	  std::string l1_chain(LowerChain("HLT_"+m_trigItems[i]));
	  std::string hlt_chain = "HLT_"+m_trigItems[i];
	  setCurrentMonGroup("HLT/TauMon/Expert/RealZtautauEff/"+m_trigItems[i]);
	  hist("hRealZttPtDenom")->Fill(Tau_TLV.Pt()/1000.);
	  if(getTDT()->isPassed(l1_chain))  hist("hRealZttL1PtNum")->Fill(Tau_TLV.Pt()/1000.);
	  if(getTDT()->isPassed(hlt_chain)) hist("hRealZttHLTPtNum")->Fill(Tau_TLV.Pt()/1000.);
	}
    }
     
  return StatusCode::SUCCESS;
}
