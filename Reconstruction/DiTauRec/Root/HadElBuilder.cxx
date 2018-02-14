/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "DiTauRec/HadElBuilder.h"
#include "xAODTau/DiTauJetContainer.h"
#include "xAODTau/DiTauJetAuxContainer.h"
#include "xAODEgamma/ElectronContainer.h"

using namespace DiTauRec;

HadElBuilder::HadElBuilder( const std::string& name ) :
  AsgTool(name),
  m_sHadHadDiTauContainerName("DiTauJets"),
  m_sDiTauHadElContainerName("DiTauJetsHadEl"),
  m_sDiTauHadElAuxContainerName("DiTauJetsHadElAux."),
  m_sElectronContainerName("Electrons"),
  m_iElectronMinPt(0)
{
  declareProperty("HadHadDiTauContainer", m_sHadHadDiTauContainerName);
  declareProperty("DiTauHadElContainer", m_sDiTauHadElContainerName);
  declareProperty("DiTauHadElAuxContainer", m_sDiTauHadElAuxContainerName);
  declareProperty("ElectronContainer", m_sElectronContainerName);
  declareProperty("ElectronMinPt", m_iElectronMinPt);
}

HadElBuilder::~HadElBuilder()
{
  
}

StatusCode HadElBuilder::initialize()
{
  return StatusCode::SUCCESS;
}

StatusCode HadElBuilder::execute()
{
  ATH_MSG_DEBUG("execute HadElBuilder");
  xAOD::DiTauJetContainer* xDiTauJetContainerHadEl        = new xAOD::DiTauJetContainer();
  xAOD::DiTauJetAuxContainer* xDiTauJetAuxContainerHadEl  = new xAOD::DiTauJetAuxContainer();
  xDiTauJetContainerHadEl->setStore(xDiTauJetAuxContainerHadEl);
  
  ANA_CHECK(evtStore()->record(xDiTauJetContainerHadEl, m_sDiTauHadElContainerName));
  ANA_CHECK(evtStore()->record(xDiTauJetAuxContainerHadEl, m_sDiTauHadElAuxContainerName));
    
  const xAOD::ElectronContainer* electronContainer = evtStore()->retrieve<const xAOD::ElectronContainer>(m_sElectronContainerName);
  const xAOD::DiTauJetContainer* xDiTauContainer = evtStore()->retrieve<const xAOD::DiTauJetContainer>(m_sHadHadDiTauContainerName);

  static SG::AuxElement::Accessor<int> acc_n_subjets ("n_subjets");
  static SG::AuxElement::Accessor<float> acc_R_core ("R_core");
  static SG::AuxElement::Accessor<float> acc_R_subjet ("R_subjet");
  static SG::AuxElement::Accessor<ElementLink<xAOD::DiTauJetContainer>> acc_origDiTauLink ("origDiTauLink");
  static SG::AuxElement::Accessor<ElementLink<xAOD::ElectronContainer>> acc_electronLink ("elLink");
  static SG::AuxElement::Accessor<int> acc_origSubjetIndex ("origSubjetIndex");

  static SG::AuxElement::Accessor<char>   acc_isHadHad  ("isHadHad");
  static SG::AuxElement::Accessor<char>   acc_isHadMu   ("isHadMu" );
  static SG::AuxElement::Accessor<char>   acc_isHadEl   ("isHadEl" );

  static SG::AuxElement::Accessor<float> acc_tau_pt    ("tau_pt" );
  static SG::AuxElement::Accessor<float> acc_tau_eta   ("tau_eta");
  static SG::AuxElement::Accessor<float> acc_tau_phi   ("tau_phi");
  static SG::AuxElement::Accessor<float> acc_tau_E     ("tau_E"  );
                                                                
  static SG::AuxElement::Accessor<float> acc_el_pt     ("electron_pt"  );
  static SG::AuxElement::Accessor<float> acc_el_eta    ("electron_eta" );
  static SG::AuxElement::Accessor<float> acc_el_phi    ("electron_phi" );
  static SG::AuxElement::Accessor<float> acc_el_E      ("electron_E"   );

  static SG::AuxElement::Accessor<float> acc_DeltaR      ("DeltaR"   );
  
  
  for(auto xHadHadDiTau : *xDiTauContainer){
    for(const auto* electron : *electronContainer){
      TLorentzVector p4_electron = electron->p4();
      // Search for electrons in anti-kt-10 region of original DiTau:
      if(p4_electron.DeltaR(xHadHadDiTau->p4()) > 1.0)
	continue;
      if(electron->pt() < m_iElectronMinPt)
        continue;
      xAOD::DiTauJet* DiTauHadEl= 0;
      TLorentzVector p4_subjet;
      TLorentzVector p4_hadel;
      int newDiTaus = 0;
      // Create new DiTauObject for every electron subjet combination
      // take only leading two subjets into account and demand minimum
      // distance of 0.1 between electron and subjet:
      for(int i = 0; i<xHadHadDiTau->nSubjets(); i++){
        float subjet_pt  = xHadHadDiTau->subjetPt(i);
        float subjet_eta = xHadHadDiTau->subjetEta(i);
        float subjet_phi = xHadHadDiTau->subjetPhi(i);
        float subjet_E   = xHadHadDiTau->subjetE(i);
        p4_subjet.SetPtEtaPhiE(subjet_pt, subjet_eta, subjet_phi, subjet_E);
        if(p4_subjet.DeltaR(p4_electron) < 0.1)
          continue;
        DiTauHadEl = new xAOD::DiTauJet();
        // Save Object in new HadEl Container:
        xDiTauJetContainerHadEl->push_back(DiTauHadEl);
        // Set some basic properties for new HadEl Ditau:
        acc_n_subjets(*DiTauHadEl) = 1;
        p4_hadel = p4_subjet + p4_electron;
        DiTauHadEl->setP4(p4_hadel.Pt(), p4_hadel.Eta(), p4_hadel.Phi(), p4_hadel.M());
        DiTauHadEl->setSubjetPtEtaPhiE(0, subjet_pt, subjet_eta, subjet_phi, subjet_E);
        
        acc_isHadEl (*DiTauHadEl) = true;
        acc_isHadHad(*DiTauHadEl) = false;
        acc_isHadMu (*DiTauHadEl) = false;
          
        acc_origSubjetIndex(*DiTauHadEl) = i;

        acc_R_core   (*DiTauHadEl) = 0.1;
        acc_R_subjet (*DiTauHadEl) = 0.2;
       
        ElementLink<xAOD::DiTauJetContainer> orig_ditaulink(xHadHadDiTau, *xDiTauContainer);
        acc_origDiTauLink(*DiTauHadEl) = orig_ditaulink;
	  
        ElementLink<xAOD::ElectronContainer> reco_electron_link(electron, *electronContainer);
        acc_electronLink(*DiTauHadEl) = reco_electron_link;
	  
        acc_tau_pt  (*DiTauHadEl) = subjet_pt;
        acc_tau_eta (*DiTauHadEl) = subjet_eta;
        acc_tau_phi (*DiTauHadEl) = subjet_phi;
        acc_tau_E   (*DiTauHadEl) = subjet_E;
                     
        acc_el_pt   (*DiTauHadEl) = p4_electron.Pt();
        acc_el_eta  (*DiTauHadEl) = p4_electron.Eta();
        acc_el_phi  (*DiTauHadEl) = p4_electron.Phi();
        acc_el_E    (*DiTauHadEl) = p4_electron.E();

        acc_DeltaR  (*DiTauHadEl) = p4_electron.DeltaR(p4_subjet);

        DiTauHadEl->setfCore(0, xHadHadDiTau->fCore(i));
        
        // set track-links, remove electron track:
        
        typedef std::vector< ElementLink< xAOD::TrackParticleContainer >> TrackParticleLinks_t;

        TrackParticleLinks_t hadElDiTauTracks;
        TrackParticleLinks_t originalDiTauTracks = xHadHadDiTau->trackLinks();

        for(auto origDiTauTrack : originalDiTauTracks){
          bool elTrack = false;
          if(!origDiTauTrack.isValid()){
            ATH_MSG_WARNING( "invalid DiTauTrack" );
            continue;
          }
          TLorentzVector p4_origDiTauTrack = (*origDiTauTrack)->p4();
          for(unsigned int i = 0; i < electron->nTrackParticles(); i++){
            if(!electron->trackParticle(i))
              continue;
            TLorentzVector p4_electronTrack = electron->trackParticle(i)->p4();
            if(p4_electronTrack.DeltaR(p4_origDiTauTrack) < 0.001){
              elTrack = true;
            }
          }
          if(!elTrack)
            hadElDiTauTracks.push_back(origDiTauTrack);
        }
        DiTauHadEl->setTrackLinks(hadElDiTauTracks);
        newDiTaus++;
        if(newDiTaus == 2) // use only the leading two possible DiTauHadEl Candidates in DiTauJet per electron
          break;
      } // subjet loop
    } // electron loop
  } // DiTau loop

  return StatusCode::SUCCESS;
}
