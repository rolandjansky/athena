/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*    @file HLTTauMonTool_Ztautauemulation.cxx
 *    
 *    L1 and HLT Real Ztauau trigger efficiency 
 *    created by bali
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
StatusCode HLTTauMonTool::RealZTauTauEfficiency(const std::string & trigItem)
{
  ATH_MSG_DEBUG("Real ZTauTau Efficiency for trigItem" << trigItem);
  if(trigItem == "Dump") {ATH_MSG_DEBUG("Not computing Real Tau efficiencies for Dump"); return StatusCode::SUCCESS;};
  
  std::string l1_chain(LowerChain("HLT_"+trigItem));
  std::string hlt_chain = "HLT_"+trigItem;

  const xAOD::TauJetContainer * reco_cont = 0;
  const xAOD::MuonContainer * muon_cont = 0;
  const xAOD::MissingETContainer *m_off_met_cont = 0;
  
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
    
  TLorentzVector Tau_TLV(0.,0.,0.,0.);
  TLorentzVector Muon_TLV(0.,0.,0.,0.);
  TLorentzVector MET_TLV(0.,0.,0.,0.);

  double tauPt_dum = -1., muPt_dum = -1., BDTJetscore = 0.0;
  float tau_charge = 10, muon_charge = 10;

  xAOD::TauJetContainer::const_iterator recoItr, reco_cont_end = reco_cont->end();
  for(recoItr=reco_cont->begin(); recoItr!=reco_cont_end; ++recoItr)
    {
    TLorentzVector TauTLV = (*recoItr)->p4();
    double pt_Tau = TauTLV.Pt();
    double eta_Tau = TauTLV.Eta();
    int ntrack_TAU = (*recoItr)->nTracks();
    bool good_tau = (*recoItr)->isTau(xAOD::TauJetParameters::JetBDTSigMedium);
    bool not_a_electron = !( (*recoItr)->isTau(xAOD::TauJetParameters::EleBDTMedium) );
    bool not_a_muon = !( (*recoItr)->isTau(xAOD::TauJetParameters::MuonVeto) );
    bool best_tau = good_tau && not_a_electron && not_a_muon;
    
    if(pt_Tau<20000.) continue;
    if(fabs(eta_Tau) > 2.47) continue;
    if(ntrack_TAU!=1 && ntrack_TAU!=3) continue;
    if(!best_tau) continue;      
    if(tauPt_dum < pt_Tau)
      {
	tauPt_dum = pt_Tau;
	Tau_TLV.SetPtEtaPhiM(pt_Tau,eta_Tau,TauTLV.Phi(),TauTLV.M());
	tau_charge = (*recoItr)->charge();
	BDTJetscore = (*recoItr)->discriminant(xAOD::TauJetParameters::BDTJetScore);
      }
    }
  
  xAOD::MuonContainer::const_iterator muonItr, muon_cont_end = muon_cont->end();
  for(muonItr=muon_cont->begin(); muonItr!=muon_cont_end; ++muonItr)
    {
      TLorentzVector MuonTLV = (*muonItr)->p4();
      double pt_mu = MuonTLV.Pt();
      double eta_mu = MuonTLV.Eta();
      if(pt_mu<22000.) continue;
      if(fabs(eta_mu)>2.4) continue;
      //if( !( (*muonItr)->setQuality(xAOD::Muon::Tight) ) ) continue;
      if(muPt_dum < pt_mu)
	{
	  muPt_dum = pt_mu;
	  Muon_TLV.SetPtEtaPhiM(pt_mu, eta_mu, MuonTLV.Phi(), MuonTLV.M());
	  muon_charge = (*muonItr)->charge();
	}
    }
 
  const xAOD::MissingET  *m_off_met = 0;
  float off_met = -9e9;
  if (m_off_met_cont && m_off_met_cont->size())
    {
      m_off_met = m_off_met_cont->at(0);
      float off_ex = (m_off_met_cont->at(0)->mpx())/CLHEP::GeV;
      float off_ey = (m_off_met_cont->at(0)->mpy())/CLHEP::GeV;
      off_met = sqrt(off_ex*off_ex+off_ey+off_ey);
      ATH_MSG_DEBUG("m_off_met:" << m_off_met );
      MET_TLV.SetPxPyPzE(off_ex,off_ey,0,off_met);
    }
  
  setCurrentMonGroup("HLT/TauMon/Expert/"+trigItem+"/RealZtautauEff");

  float ltau_charge = tau_charge + muon_charge;
  double mltau_vis = (Tau_TLV + Muon_TLV).M();
  double cos_dphi = cos(Muon_TLV.DeltaPhi(MET_TLV)) + cos(Tau_TLV.DeltaPhi(MET_TLV)) ;
  double mt = sqrt(2 * Muon_TLV.Pt() * off_met * (1 - cos(Muon_TLV.DeltaPhi(MET_TLV)) ) );
  double ltau_dphi = Tau_TLV.DeltaPhi(Muon_TLV);

  if(ltau_charge == 0. &&
     Muon_TLV.Pt() < 40000. &&
     BDTJetscore > 0.3 &&
     mltau_vis > 42000. && mltau_vis < 82000. &&
     cos_dphi > -0.15 &&
     mt < 50000. &&
     ltau_dphi > 2.4)
    {
      hist("hRealZttPtDenom")->Fill(Tau_TLV.Pt()/1000.);
      if(getTDT()->isPassed(l1_chain))  hist("hRealZttL1PtNum")->Fill(Tau_TLV.Pt()/1000.);
      if(getTDT()->isPassed(hlt_chain))	hist("hRealZttHLTPtNum")->Fill(Tau_TLV.Pt()/1000.);
    }
     
     
  return StatusCode::SUCCESS;
}


