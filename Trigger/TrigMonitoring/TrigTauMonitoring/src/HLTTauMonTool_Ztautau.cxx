/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*    @file HLTTauMonTool_Ztautau.cxx
 *    
 *    Real Ztauau L1 & HLT trigger efficiency 
 *    created by Babar Ali (babar.ali@cern.ch)
 */

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"
#include "TProfile.h"
#include "HLTTauMonTool.h"
#include "AthenaKernel/Units.h"

using namespace std;
using Athena::Units::GeV;

//--------------------------------------------------------------------------------------
StatusCode HLTTauMonTool::RealZTauTauEfficiency(const std::string & goodTauRefType)
{
  ATH_MSG_DEBUG("Real ZTauTau Efficiency");  

  std::vector<const xAOD::TauJet *> taus_here;
  std::vector<std::string> trigItemsZtt_here;
  if (goodTauRefType == "RNN") {
    taus_here = m_taus_RNN;
    trigItemsZtt_here = m_trigItemsZtt_RNN;
  } else {
    taus_here = m_taus_BDT;
    trigItemsZtt_here = m_trigItemsZtt_BDT;
  }

  // compute eff only for events passing single muon triggers
  if(! ( getTDT()->isPassed("HLT_mu20_iloose_L1MU15") 
	|| getTDT()->isPassed("HLT_mu20_ivarloose_L1MU15") 
	|| getTDT()->isPassed("HLT_mu24_ivarloose") 
	|| getTDT()->isPassed("HLT_mu24_iloose") 
	|| getTDT()->isPassed("HLT_mu24_imedium") 
	|| getTDT()->isPassed("HLT_mu24_ivarmedium") 
	|| getTDT()->isPassed("HLT_mu26_imedium") 
	|| getTDT()->isPassed("HLT_mu26_ivarmedium") 
	|| getTDT()->isPassed("HLT_mu50")
	) ) return StatusCode::SUCCESS;
 
  StatusCode sc = StatusCode::SUCCESS;

//  const xAOD::TauJetContainer    * reco_cont      = 0;
  const xAOD::MuonContainer      * muon_cont      = 0;
  const xAOD::MissingETContainer * off_met_cont = 0;
    
//  if( evtStore()->retrieve(reco_cont, "TauJets").isFailure() )
//    {
//      ATH_MSG_WARNING("Failed to retrieve  TauJets container. Exiting.");
//      return StatusCode::FAILURE;
//    } 
//
  sc = evtStore()->retrieve(muon_cont, "Muons");
  if(!sc.isSuccess())
    {
      ATH_MSG_WARNING("Failed to retrieve Muons container. Exiting.");
      return sc;
    }
  sc = evtStore()->retrieve(off_met_cont, "MET_Reference_AntiKt4LCTopo");
  if (!sc.isSuccess() || !off_met_cont) 
    {
      ATH_MSG_WARNING("Could not retrieve Reconstructed MET term with Key MET_Reference_AntiKt4LCTopo");
      return sc;
    }
  
  TLorentzVector Tau_TLV (0.,0.,0.,0.);
  TLorentzVector Muon_TLV(0.,0.,0.,0.);
  TLorentzVector MET_TLV (0.,0.,0.,0.);

  double tauPt_dum  = -1.  , muPt_dum    = -1.;
  float  Tau_charge = -99.0, Muon_charge = -99.0;
  bool   lead_tau   = false, single_mu   = false;
  
  //Muon Selection
  xAOD::MuonContainer::const_iterator muonItr, muon_cont_end = muon_cont->end();
  for(muonItr=muon_cont->begin(); muonItr!=muon_cont_end; ++muonItr)
    {
      const xAOD::Muon& muon = **muonItr;
      TLorentzVector MuonTLV = (*muonItr)->p4();
      
      double pt_mu  = MuonTLV.Pt();
      double eta_mu = MuonTLV.Eta();
      float  etcone = 0.0, ptcone = 0.0, etcone40Rel = 0.0, ptcone40Rel = 0.0;

      if (!muon.isolation(etcone,xAOD::Iso::etcone40))
	{
	  ATH_MSG_WARNING("No EtCone defined for muon.");
	}
      else
	{
	  etcone40Rel = etcone/pt_mu;
	}
      if (!muon.isolation(ptcone,xAOD::Iso::ptcone40))
	{
	  ATH_MSG_WARNING("No ptCone defined for muon.");
	}
      else
	{
	  ptcone40Rel = ptcone/pt_mu;
	}
      
      if(pt_mu<27000.)			             continue;
      if(fabs(eta_mu)>2.5)                           continue;
      if((*muonItr)->author() != xAOD::Muon::MuidCo) continue;
      if(! muon.passesIDCuts() )                     continue;
      if(etcone40Rel > 0.1)                          continue;
      if(ptcone40Rel > 0.2)                          continue;

      if(single_mu)
	{
	  single_mu = false;
	  Muon_TLV.Clear();
	  Muon_charge = -99;
	  break;
	}
      if(muPt_dum < pt_mu)
	{
	  muPt_dum = pt_mu;
	  Muon_TLV.SetPtEtaPhiM(pt_mu, eta_mu, MuonTLV.Phi(), MuonTLV.M());
	  Muon_charge = (*muonItr)->charge();
	  single_mu = true;
	}
    }

  //Tau Selection
  for(auto recoTau : taus_here)
    {
    TLorentzVector TauTLV = recoTau->p4();
    double pt_Tau     = TauTLV.Pt();
    double eta_Tau    = TauTLV.Eta();
    int    ntrack_Tau = recoTau->nTracks();
    bool   good_Tau(false);
    if (goodTauRefType == "RNN") {
      good_Tau = recoTau->isTau(xAOD::TauJetParameters::JetRNNSigLoose);
    } else {
      good_Tau = recoTau->isTau(xAOD::TauJetParameters::JetBDTSigLoose);
    }
    float  charge_Tau = recoTau->charge();
        
    if(pt_Tau<20000.)                  continue;
    if(fabs(eta_Tau) > 2.47)           continue;
    if(fabs(charge_Tau) != 1.0)        continue;
    if(ntrack_Tau!=1 && ntrack_Tau!=3) continue;
    if(!good_Tau)                      continue;
    
    //Over lap removal between Muon and Tau. Keep Muon remove Tau!!
    if(single_mu)
      {
	if(Muon_TLV.DeltaR(TauTLV) < 0.3) continue;
      }
    else
      {
	break;
      }
    
    if(tauPt_dum < pt_Tau) //Check for lead Tau after OLR
      {
	tauPt_dum = pt_Tau;
	Tau_TLV.SetPtEtaPhiM(pt_Tau,eta_Tau,TauTLV.Phi(),TauTLV.M());
	Tau_charge = charge_Tau;
	lead_tau = true;
      }
    }

  //Offline MET 
  const xAOD::MissingET  *off_met_ptr = 0;
  float off_ex    = -9e9;
  float off_ey    = -9e9;
  float off_met   = -9e9;
  //float off_sumet = -9e9;
  //float off_phi   = -9e9;
  
  if (off_met_cont && off_met_cont->size())
    {
      off_met_ptr = (*off_met_cont)["FinalClus"];
      if(off_met_ptr){ 
       off_ex = (off_met_ptr->mpx());
       off_ey = (off_met_ptr->mpy());
      }
    }
 
  if(off_ex!=0. || off_ey!=0.) off_met = sqrt(off_ex*off_ex+off_ey+off_ey);
  if(off_met==-9e9 || off_met==0.) return StatusCode::SUCCESS; 
      //off_sumet = ((*off_met_cont)["FinalClus"]->sumet());
      //off_phi = atan2(off_ey, off_ex);
     
  MET_TLV.SetPxPyPzE(off_ex,off_ey,0.,off_met);
  ATH_MSG_DEBUG("off_met:" << off_met );
     
  float  ltau_charge  = -99.0;
  double ltau_vismass = -99.0;
  double cos_dphi     = -99.0;
  double mt           = -99.0;
  double ltau_deta    =  99.0;
  //double ltau_dR      =  99.0;
  
  //Events variables with Lead Tau and Single Muon
  if(lead_tau && single_mu)
    {
      ltau_charge  = Tau_charge + Muon_charge;
      ltau_vismass = (Tau_TLV + Muon_TLV).M();
      cos_dphi     = cos(Muon_TLV.DeltaPhi(MET_TLV)) + cos(Tau_TLV.DeltaPhi(MET_TLV)) ;
      mt           = sqrt(2 * Muon_TLV.Pt() * off_met * (1 - cos(Muon_TLV.DeltaPhi(MET_TLV)) ) );
      ltau_deta    = deltaEta(Tau_TLV.Eta(), Muon_TLV.Eta());
      //ltau_dR      = Tau_TLV.DeltaR(Muon_TLV);
    }
 
  //Event Selection
  if(ltau_charge == 0.     &&  
     mt < 60000.           && 
     cos_dphi > -0.5       && 
     fabs(ltau_deta) < 1.5 && 
     //ltau_dR > 2.9         &&
     ltau_vismass > 45000. && ltau_vismass < 85000.)
    {
      for ( const auto& item: m_trigItemsZtt ) {
	  std::string l1_chain(LowerChain("HLT_"+item));
	  std::string hlt_chain = "HLT_"+item;

	  setCurrentMonGroup("HLT/TauMon/Expert/RealZtautauEff/"+item);
	  //hist("hRealZttPtDenom")->Fill(Tau_TLV.Pt()/GeV);

	  //L1
	  if(getTDT()->isPassed(l1_chain , TrigDefs::Physics | TrigDefs::allowResurrectedDecision))
	    {
	      //hist("hRealZttL1PtNum")->Fill(Tau_TLV.Pt()/GeV);
	      profile("TProfRealZttL1PtEfficiency")->Fill(Tau_TLV.Pt()/GeV,1);
	    }
	  else profile("TProfRealZttL1PtEfficiency")->Fill(Tau_TLV.Pt()/GeV,0);

	  //HLT
	  if(getTDT()->isPassed(hlt_chain, TrigDefs::Physics | TrigDefs::allowResurrectedDecision))
	    {
	      //hist("hRealZttHLTPtNum")->Fill(Tau_TLV.Pt()/GeV);
	      profile("TProfRealZttHLTPtEfficiency")->Fill(Tau_TLV.Pt()/GeV,1);
	    }
	  else profile("TProfRealZttHLTPtEfficiency")->Fill(Tau_TLV.Pt()/GeV,0);
	}
    }
  return StatusCode::SUCCESS;
}
