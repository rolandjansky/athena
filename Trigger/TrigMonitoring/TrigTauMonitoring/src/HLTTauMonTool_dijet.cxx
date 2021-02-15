/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HLTTauMonTool.h"
#include "TProfile.h"
#include "AthenaKernel/Units.h"

using Athena::Units::GeV;

/*
STRATEGY:
Measure high pT tau trigger fake rate using dijet events.
1. select events that pass jet trigger (e.g. j400).
2. select leading and subleading offline jets.
3. check if it is a dijet event: 
   leading and subleading jets are back to back in phi, pt is balanced.
4. check if an offline tau matches the subleading jet.
5. check if HLT tau matches the offline tau.
6. efficiency (fake rate) is (HLT taus matched to offline tau)/(offline taus)
*/

//--------------------------------------------------------------------------------------
StatusCode HLTTauMonTool::dijetFakeTausEfficiency()
{
  ATH_MSG_DEBUG("dijet Fake Taus Efficiency"); 

  /* CONFIGURATION */
  const float 		leadingJetPtCut = 450000.; //MeV
  const float 		leadingJetEtaCut = 3.2;
  const std::string 	singleJetTrigger = "j400";
  const std::string     offlineJetContainer = "AntiKt4LCTopoJets";
  const float 		dRCut = 0.2;
  const float 		dRL1Cut = 0.3;
  const float 		dPhiCut = 2.5;
  const float 		dPtBalancingCut = 999.;
  const float 		offlineTauPtCut = 150000.;
  const float 		offlineTauEtaCut = 2.5;
  const std::string	offlineTauIDCut = "";//"loose" "medium" "tight"
  const std::vector<unsigned int>	offlineTauTrackCut = {1,2,3};

  StatusCode sc = StatusCode::SUCCESS;
  const xAOD::JetContainer* jet_cont = 0;
  sc = evtStore()->retrieve(jet_cont, offlineJetContainer.c_str() );
  if(!sc.isSuccess())
    {
      ATH_MSG_WARNING("Failed to retrieve offline Jet container. Exiting.");
      return sc;
    }

//  const xAOD::TauJetContainer* tau_reco_cont = 0;
//  if( evtStore()->retrieve(tau_reco_cont, "TauJets").isFailure() )
//    {
//      ATH_MSG_WARNING("Failed to retrieve  TauJets container. Exiting.");
//      return StatusCode::FAILURE;
//    }


  /* select events that pass the required trigger */
  if(!getTDT()->isPassed(Form("HLT_%s",singleJetTrigger.c_str())))
    {
      ATH_MSG_DEBUG("Event does not pass single jet trigger. Exiting");
      return StatusCode::SUCCESS;
    }


  TLorentzVector leadingJetTLV(0.,0.,0.,0.);
  TLorentzVector subleadingJetTLV(0.,0.,0.,0.);
  const xAOD::TauJet* theOfflineTau = 0;

  /* require offline leading jet and subleading jet */
  for(auto aJet : *jet_cont)                                                                                               
    {
    if( aJet->pt() > leadingJetTLV.Pt() )
      {
        subleadingJetTLV = leadingJetTLV;
        leadingJetTLV = aJet->p4();
      }
    else if( aJet->pt() > subleadingJetTLV.Pt() && aJet->pt() < leadingJetTLV.Pt())
      {
        subleadingJetTLV = aJet->p4(); 
      }
    }

  /* check if leading jet passes pt and eta cut and subleading jet exists */
  if( leadingJetTLV.Pt() < leadingJetPtCut || 
      TMath::Abs(leadingJetTLV.Eta()) > leadingJetEtaCut || 
      subleadingJetTLV.Pt()==0. || 
      TMath::Abs(subleadingJetTLV.Eta()) > offlineTauEtaCut) 
    {
      ATH_MSG_DEBUG("No leading or subleading jet found. Exiting");
      return StatusCode::SUCCESS;
    }

  /* check if leading and subleading jets are back to back + pt balanced */
  if( leadingJetTLV.DeltaPhi(subleadingJetTLV) < dPhiCut || 
      (leadingJetTLV.Pt()-subleadingJetTLV.Pt())/(leadingJetTLV.Pt()+subleadingJetTLV.Pt()) > dPtBalancingCut ) 
    {
      ATH_MSG_DEBUG("No good dijet found. Exiting");
      return StatusCode::SUCCESS;
    }

  /* match offline tau to subleading jet */
  float dR = 666;
  for(auto aTau : m_taus_BDT)
  //  for(auto aTau : m_truth)
    {
      //const xAOD::TauJet *aTau = m_taus.at(t);
      /* check offline tau quality */
      if( aTau->pt() < offlineTauPtCut ) continue;
      if( TMath::Abs(aTau->eta()) > offlineTauEtaCut ) continue;
      //if( TMath::Abs(aTau->charge()) != 1.0 ) continue;
  
      bool trackMatch = false;
      for(auto i : offlineTauTrackCut) if( aTau->nTracks()==i) {trackMatch=true; break;}
      if(!trackMatch) continue;

      if( offlineTauIDCut=="") ;
      else if( offlineTauIDCut=="loose")  {if( !aTau->isTau(xAOD::TauJetParameters::JetRNNSigLoose) ) continue;}
      else if( offlineTauIDCut=="medium") {if( !aTau->isTau(xAOD::TauJetParameters::JetRNNSigMedium) ) continue;}
      else if( offlineTauIDCut=="tight")  {if( !aTau->isTau(xAOD::TauJetParameters::JetRNNSigTight) ) continue;}

      /* match to subleading jet*/
      float temp_dR=subleadingJetTLV.DeltaR(aTau->p4());
      if( temp_dR < dR ) 
        {
          theOfflineTau = aTau;
          dR=temp_dR;
        }
    }
  if( dR > dRCut || theOfflineTau==0 ) 
    {
      ATH_MSG_DEBUG("No matching tau found. Exiting");
      return StatusCode::SUCCESS;
    }

 /* check HLT tau */
  for(auto aHighPtChain : m_trigItemsHighPt)
    {
      setCurrentMonGroup("HLT/TauMon/Expert/dijetFakeTausEff/" + aHighPtChain);

      std::string l1_chain(LowerChain("HLT_" + aHighPtChain));
     /* match offline tau with HLT taus and fill nominators */
      if(L1TauMatching(l1_chain, theOfflineTau->p4(), dRL1Cut))  	
        {
	  profile("TProfDijetFakeTausL1MuEfficiency")->Fill(m_mu_offline,1);
          profile("TProfDijetFakeTausL1PtEfficiency")->Fill(theOfflineTau->pt()/GeV,1);
          profile("TProfDijetFakeTausL1EtaEfficiency")->Fill(theOfflineTau->eta(),1);
          profile("TProfDijetFakeTausL1NTracksEfficiency")->Fill(theOfflineTau->nTracks(),1);
        }
       else
        {
          profile("TProfDijetFakeTausL1PtEfficiency")->Fill(theOfflineTau->pt()/GeV,0);
          profile("TProfDijetFakeTausL1EtaEfficiency")->Fill(theOfflineTau->eta(),0);
	  profile("TProfDijetFakeTausL1MuEfficiency")->Fill(m_mu_offline,0);
          profile("TProfDijetFakeTausL1NTracksEfficiency")->Fill(theOfflineTau->nTracks(),0);
        }

      if(HLTTauMatching(aHighPtChain, theOfflineTau->p4(), dRCut)) 	
        {
          profile("TProfDijetFakeTausHLTPtEfficiency")->Fill(theOfflineTau->pt()/GeV,1);
          profile("TProfDijetFakeTausHLTEtaEfficiency")->Fill(theOfflineTau->eta(),1);
          profile("TProfDijetFakeTausHLTMuEfficiency")->Fill(m_mu_offline,1);
          profile("TProfDijetFakeTausHLTNTracksEfficiency")->Fill(theOfflineTau->nTracks(),1);
        }
      else
        {
          profile("TProfDijetFakeTausHLTPtEfficiency")->Fill(theOfflineTau->pt()/GeV,0);
          profile("TProfDijetFakeTausHLTEtaEfficiency")->Fill(theOfflineTau->eta(),0);
          profile("TProfDijetFakeTausHLTMuEfficiency")->Fill(m_mu_offline,0);
          profile("TProfDijetFakeTausHLTNTracksEfficiency")->Fill(theOfflineTau->nTracks(),0);
        }
    }
  
  return StatusCode::SUCCESS;
  
}
