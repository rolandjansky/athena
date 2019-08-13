/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopHLUpgrade/UpgradeObjectLoader.h"

#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"
#include "TopHLUpgrade/UpgradeLeptonObjectSelector.h"
#include "TopHLUpgrade/UpgradePhotonObjectSelector.h"
#include "TopParticleLevel/ParticleLevelPhotonObjectSelector.h"

#include "xAODTruth/TruthVertexContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODCore/ShallowCopy.h"

#include "TruthUtils/PIDHelpers.h"

#include <algorithm>
#include <memory>

using namespace top;

bool ptSort(const xAOD::IParticle* p1, const xAOD::IParticle* p2) {
  return p1->pt() > p2->pt();
}

UpgradeObjectLoader::UpgradeObjectLoader( const std::shared_ptr<top::TopConfig> & cfg ) :
  asg::AsgTool( "UpgradeObjectLoader" ),
  m_config(cfg),
  m_upgrade(nullptr),
  // tool is active depending on config settings
  m_active(m_config->isMC() && m_config->HLLHC()),
  m_dofakes(m_config->isMC() && m_config->HLLHCFakes()),
  // object selection tools setup below
  m_objectSelector_Electron( nullptr ),
  m_objectSelector_Muon( nullptr ),
  m_objectSelector_Photon( nullptr )
 {

  if ( m_active ){
    ATH_MSG_INFO("Upgrade level reconstruction is enabled; telling you how I am configured:");
    //asg::setProperty(m_upgrade, "Layout", UpgradePerformanceFunctionsxAOD::Step1p6) , "Failed to setProperty" );
    m_upgrade.reset( new UpgradePerformanceFunctionsxAOD("UpgradePerformanceFunctionsxAOD"));
    top::check(m_upgrade->setProperty("Layout", UpgradePerformanceFunctionsxAOD::Step1p6) , "Failed to setProperty" );
    top::check(m_upgrade->setProperty("AvgMu", 200) , "Failed to setProperty" );
    top::check(m_upgrade->setProperty("ElectronWorkingPoint", UpgradePerformanceFunctionsxAOD::mediumElectron) , "Failed to setProperty" );
    top::check(m_upgrade->setProperty("ElectronRandomSeed", 171) , "Failed to setProperty" );
    top::check(m_upgrade->setProperty("MuonWorkingPoint", UpgradePerformanceFunctionsxAOD::tightMuon), "Failed to setProperty" );
    top::check(m_upgrade->setProperty("METRandomSeed", 986), "Failed to setProperty" );
    top::check(m_upgrade->setProperty("METFile", "UpgradePerformanceFunctions/CalibArea-00-01/sumetPU_mu200_ttbar_gold.root"), "Failed to setProperty" );
    top::check(m_upgrade->setProperty("UseTrackConfirmation", true), "Failed to setProperty" ); 
    top::check(m_upgrade->setProperty("JetRandomSeed", 121), "Failed to setProperty" );
    top::check(m_upgrade->setProperty("PileupRandomSeed", 771), "Failed to setProperty" );
    top::check(m_upgrade->setProperty("PileupJetThresholdMeV", 30000.), "Failed to setProperty" );
    top::check(m_upgrade->setProperty("PileupEfficiency", UpgradePerformanceFunctionsxAOD::PU), "Failed to setProperty" );
    top::check(m_upgrade->setProperty("JVT_PU_Efficiency", 0.02), "Failed to setProperty" );
    top::check(m_upgrade->setProperty("PileupPath", "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/UpgradePerformanceFunctions/"), "Failed to setProperty" );
    top::check(m_upgrade->setProperty("FlavourTaggingCalibrationFile", "UpgradePerformanceFunctions/CalibArea-00-01/flavor_tags_v2.0.root"), "Failed to setProperty" );
    top::check(m_upgrade->setProperty("PhotonWorkingPoint", UpgradePerformanceFunctionsxAOD::tightPhoton), "Failed to setProperty" );
    top::check(m_upgrade->setProperty("PhotonNoiseScaling", 0.375), "Failed to setProperty" ); //recommended value
    top::check(m_upgrade->setProperty("PhotonRandomSeed", 1), "Failed to setProperty" );
    top::check(m_upgrade->setProperty("PhotonFakeFile", "UpgradePerformanceFunctions/CalibArea-00-01/PhotonFakes.root"), "Failed to setProperty" );
    top::check(m_upgrade->setProperty("UseHGTD0", false), "Failed to setProperty" );

    top::check(m_upgrade->initialize(), "Failed to initialize upgrade loader");

    // configure muon selector
    auto optMu = UpgradeLeptonObjectSelector::Options{
      m_config->truth_muon_PtCut(),
      m_config->truth_muon_EtaCut()
    };

    m_objectSelector_Muon.reset( new UpgradeLeptonObjectSelector( optMu ) );

    // configure electron selector
    auto optEl = UpgradeLeptonObjectSelector::Options{
      m_config->truth_electron_PtCut(),
      m_config->truth_electron_EtaCut()
    };

    m_objectSelector_Electron.reset( new UpgradeLeptonObjectSelector( optEl ) );
  }

  if (m_config->useTruthPhotons()) {
    auto optPh = UpgradePhotonObjectSelector::Options{
      m_config->truth_photon_PtCut(),
      m_config->truth_photon_EtaCut()
    };

    m_objectSelector_Photon.reset( new UpgradePhotonObjectSelector( optPh ) );
  }
    //Configure and create photon object selector
    //auto optPhoton = ParticleLevelPhotonObjectSelector::Options{
    //m_config->truth_photon_PtCut(),
    //m_config->truth_photon_EtaCut(),
    //m_config->truth_photon_Origin(),
    //m_config->truth_photon_Isolation() };

    //m_objectSelector_Photon.reset( new ParticleLevelPhotonObjectSelector( optPhoton ) );


}//UpgradeObjectLoader constructor

UpgradeObjectLoader::~UpgradeObjectLoader( ) {

}

/*!
 * The main function to do the heavy lifting - get the truth objects, smear them
 * and apply efficiency corrections.
 * @return ParticleLevelEvent object
 */
ParticleLevelEvent UpgradeObjectLoader::load() {

  // Create the ParticleLevelEvent object
  ParticleLevelEvent particleEvent;
 
  //
  // Retrieve the truth collections
  const xAOD::TruthParticleContainer* importedAllTruthParticles(nullptr);
  top::check( evtStore()->retrieve(importedAllTruthParticles, "TruthParticles"), 
      "xAOD::TEvent::retrieve failed for TruthParticles"  );

  // Vectors to store particles which will be dressed
  std::vector<const xAOD::TruthParticle*>  candidateParticlesList;

  //make a list of all candidate particles that could fall inside the cone of the particle of interest from listOfParticlesForIso
  
  std::vector<int> excludeFromCone = {-16,-14,-12,12,14,16};

  int g4BarcodeOffset = 200000;

  std::vector<int> pdgId = {11,13,22};

  for (const xAOD::TruthParticle *particle : *importedAllTruthParticles) {
    if (particle->status() != 1) continue;
                 
    if (find(pdgId.begin(), pdgId.end(), abs(particle->pdgId())) == pdgId.end()) continue;
                 
    //ensure particles are not from GEANT
    if (particle->barcode() >= g4BarcodeOffset) continue;

    candidateParticlesList.push_back(particle);
  }

  // ----------------------------------------------------------


  // Load event info object directly into the particleEvent
  top::check( evtStore()->retrieve( particleEvent.m_info, m_config->sgKeyEventInfo() ),
      "xAOD::TEvent::retrieve failed for EventInfo" );

  /* ------------------------------ Muons -----------------------------------------------------------*/
  // Get muons
  const xAOD::TruthParticleContainer * origmuons(0);
  top::check( evtStore()->retrieve( origmuons, m_config->sgKeyTruthMuons() ),
      "xAOD::TEvent::retrieve failed for Truth Muons" );
  // get truth particles
  const xAOD::TruthParticleContainer * truthpart(0);
  top::check( evtStore()->retrieve( truthpart, m_config->sgKeyMCParticle() ),
      "xAOD::TEvent::retrieve failed for Truth Particles" );

  // Shallow copy 
  auto muons_shallowCopy = xAOD::shallowCopyContainer( *origmuons );
  std::unique_ptr<xAOD::TruthParticleContainer> muons (muons_shallowCopy.first);
  std::unique_ptr<xAOD::ShallowAuxContainer>    muonsAux (muons_shallowCopy.second);

  // container to put the selected muons in - use view container to avoid deep copies
  m_selectedMuons.reset( new xAOD::TruthParticleContainer(SG::VIEW_ELEMENTS));
  
  // Smear & efficiency correct muons
  for(auto muon : *muons) {
    // efficiency (assume pass unsmeared pT here)
    const double eff = m_upgrade->getMuonEfficiency( muon->pt(), muon->eta());

    // smear
    m_upgrade->smearMuon( *muon );

    // store efficiency correction
    if(m_upgrade->getRandom3()->Uniform() < eff) {
      muon->auxdata<int>("passReco") = 1;
    }
    else muon->auxdata<int>("passReco") = 0;

    // apply all cuts
    if( m_objectSelector_Muon->apply( *muon ) ) {

      //find matching truth particle by barcode
      //this is done after selection to spend less time looping all TPs
      for (const auto tp : *truthpart) {
        if (muon->barcode() == tp->barcode()) {
          //Add production vertex info- only truthparticle has a working
          //prodVtxLink
          const ElementLink<xAOD::TruthVertexContainer> prodVtxLink =
            tp->auxdata<ElementLink< xAOD::TruthVertexContainer > >
            ("prodVtxLink");
          if(prodVtxLink.isValid()) {
            const xAOD::TruthVertex *tvtx = *prodVtxLink;
            muon->auxdata<float>("prodVtx_z")    = tvtx->z();
            muon->auxdata<float>("prodVtx_perp") = tvtx->perp();
            muon->auxdata<float>("prodVtx_phi")  = tvtx->phi();
          }
          break;
        }
      }

      m_selectedMuons->push_back( muon );
    }
  }

  // loop over the selected muons
  for (xAOD::TruthParticle *particle : *m_selectedMuons) {
    double ptcone30 = 0;
    double etcone20 = 0;

    TLorentzVector p1;
    p1.SetPtEtaPhiE(particle->pt(),particle->eta(),particle->phi(),particle->e());

    // loop over the truth particles that could be around the muon
    for (const auto& cand_part : candidateParticlesList) {
      if (find(excludeFromCone.begin(), excludeFromCone.end(), cand_part->pdgId()) != excludeFromCone.end()) {
        //skip if we find a particle in the exclude list
        continue;
      }
      if (MC::isNonInteracting(cand_part->pdgId())){
        // Do not include non-interacting particles, and this particle is non-interacting
        continue;
      }
      if (cand_part->barcode() != particle->barcode()) {
        TLorentzVector p2;
        p2.SetPtEtaPhiE(cand_part->pt(),cand_part->eta(),cand_part->phi(),cand_part->e());
        if(p1.DeltaR(p2) < 0.2)
        {
          etcone20 += cand_part->e() / cosh( cand_part->eta() );
        }
	if(p1.DeltaR(p2) < 0.3)
        {
          ptcone30 += cand_part->pt();
        }
      }
    }
    particle->auxdata<float>("ptcone30") = ptcone30;
    particle->auxdata<float>("etcone20") = etcone20;
  }

  // sort the muons by pT
  std::sort(m_selectedMuons->begin(), m_selectedMuons->end(), ptSort);

  // keep hold of the pointers in our private variables (also clears last event)
  m_muons           = std::move(muons);
  m_muonsShallowAux = std::move(muonsAux);

  // pass the muons to the ParticleLevelEvent object
  particleEvent.m_muons = m_selectedMuons.get();

  /* ------------------------------ Electrons -----------------------------------------------------------*/
  // Get electrons
  const xAOD::TruthParticleContainer * origelectrons(0);
  top::check( evtStore()->retrieve( origelectrons, m_config->sgKeyTruthElectrons() ),
      "xAOD::TEvent::retrieve failed for Truth Electrons" );
  
  // Shallow copy 
  auto electrons_shallowCopy = xAOD::shallowCopyContainer( *origelectrons );
  std::unique_ptr<xAOD::TruthParticleContainer> electrons    (electrons_shallowCopy.first);
  std::unique_ptr<xAOD::ShallowAuxContainer>    electronsAux (electrons_shallowCopy.second);

  // we are going to put selected 'good' electrons in to a xAOD::TruthParticleContainer
  m_selectedElectrons.reset();
  m_selectedElectrons = std::make_shared<xAOD::TruthParticleContainer>();
  m_auxElectronCont.reset();
  m_auxElectronCont = std::make_shared<xAOD::AuxContainerBase>();
  m_selectedElectrons->setStore(m_auxElectronCont.get()); // connect container & store
  //fake electrons container
  auto fakeElectrons    = std::make_unique<xAOD::TruthParticleContainer>();
  auto fakeElectronsAux = std::make_unique<xAOD::TruthParticleAuxContainer>();
  fakeElectrons->setStore(fakeElectronsAux.get()); // connect container & store
    
  //fake photons container
  std::unique_ptr<xAOD::TruthParticleContainer>    fakePhotons = nullptr;
  std::unique_ptr<xAOD::TruthParticleAuxContainer> fakePhotonsAux = nullptr;
  if( m_dofakes && m_config->useTruthPhotons() ){
    fakePhotons = std::make_unique<xAOD::TruthParticleContainer>();
    fakePhotonsAux = std::make_unique<xAOD::TruthParticleAuxContainer>();
    fakePhotons->setStore(fakePhotonsAux.get()); // connect container & store
  }
  
  // Smear & efficiency correct electrons
  for(auto electron : *electrons) {
    if(m_dofakes && m_config->useTruthPhotons()){
      //fake photon efficiency
      const float fakeEff = m_upgrade->getElectronToPhotonFakeRate(electron->pt(),electron->eta());
      if(m_upgrade->getRandom3()->Uniform() < fakeEff) {
        xAOD::TruthParticle *fakePhoton = new xAOD::TruthParticle();
        fakePhotons->push_back( fakePhoton );
        *fakePhoton = *electron;
        fakePhoton->auxdata<int>("passReco") = 1;
        fakePhoton->auxdata<int>("FakeType") = 1; // faketype 0:true, 1:e->y, 2:j->y
        electron->auxdata<bool>("FakingPhoton") = true; //add auxdata to identify the electron that is used as fake photon
      } else electron->auxdata<bool>("FakingPhoton") = false; //add auxdata to identify the electron that is used as fake photon
    }

    // efficiency (assume pass unsmeared pT here)
    const double eff = m_upgrade->getElectronEfficiency( electron->pt(), electron->eta());

    // smear
    m_upgrade->smearElectron( *electron );

    // store efficiency result
    if(m_upgrade->getRandom3()->Uniform() < eff) {
      electron->auxdata<int>("passReco") = 1;
    }
    else electron->auxdata<int>("passReco") = 0;

    // apply all cuts
    if( m_objectSelector_Electron->apply( *electron ) ){
      if (m_dofakes) electron->auxdata<int>("FakeType") = 0; // faketype 0:true, 2:j->e
      xAOD::TruthParticle *newElec = new xAOD::TruthParticle();
      m_selectedElectrons->push_back( newElec ); //particle acquires the selectedElectrons auxStore
      *newElec = *electron; //deep copy
    }
  }

  // loop over the selected electrons
  for (xAOD::TruthParticle *particle : *m_selectedElectrons) {
    double ptcone30 = 0;
    double etcone20 = 0;

    TLorentzVector p1;
    p1.SetPtEtaPhiE(particle->pt(),particle->eta(),particle->phi(),particle->e());

    // loop over the truth particles that could	be around the electron
    for (const auto& cand_part : candidateParticlesList) {
      if (find(excludeFromCone.begin(), excludeFromCone.end(), cand_part->pdgId()) != excludeFromCone.end()) {
        //skip if we find a particle in the exclude list
        continue;
      }
      if (MC::isNonInteracting(cand_part->pdgId())){
        // Do not include non-interacting particles, and this particle is non-interacting
        continue;
      }
      if (cand_part->barcode() != particle->barcode()) {
        TLorentzVector p2;
        p2.SetPtEtaPhiE(cand_part->pt(),cand_part->eta(),cand_part->phi(),cand_part->e());
        if(p1.DeltaR(p2) < 0.2)
        {
          etcone20 += cand_part->e() / cosh( cand_part->eta() );
        }
	if(p1.DeltaR(p2) < 0.3)
        {
          ptcone30 += cand_part->pt();
        }
      }
    }
    particle->auxdata<float>("ptcone30") = ptcone30;
    particle->auxdata<float>("etcone20") = etcone20;
  }

  /* ------------------------------ Jets-----------------------------------------------------------*/
  // Get jets
  const xAOD::JetContainer * origjets(0);
  top::check( evtStore()->retrieve( origjets, m_config->sgKeyTruthJets() ),
      "xAOD::TEvent::retrieve failed for Truth Jets" );

  // Shallow copy 
  auto jets_shallowCopy = xAOD::shallowCopyContainer( *origjets );
  std::unique_ptr<xAOD::JetContainer>        jets(jets_shallowCopy.first);
  std::unique_ptr<xAOD::ShallowAuxContainer> jetsAux(jets_shallowCopy.second);

  // we are going to put selected 'good' jets in to a xAOD::JetContainer
  xAOD::JetContainer* selectedJets = new xAOD::JetContainer();
  xAOD::JetAuxContainer* auxJetCont = new xAOD::JetAuxContainer();
  selectedJets->setStore(auxJetCont); // connect container & store
  
  // Smear & efficiency correct jets
  for(auto jet : *jets) {
    if(m_dofakes){
      //fake electron efficiency
      const float fakeEff = m_upgrade->getElectronFakeRate(jet->e(),jet->eta());
      if(m_upgrade->getRandom3()->Uniform() < fakeEff) {
        xAOD::TruthParticle *fakeElectron = new xAOD::TruthParticle();
        fakeElectrons->push_back( fakeElectron );
        m_upgrade->smearFakeElectron(*fakeElectron, *jet);
        fakeElectron->auxdata<int>("passReco") = 1;
        fakeElectron->auxdata<int>("FakeType") = 2; // faketype 0:true, 2:j->e
        continue;
      }
      if(m_config->useTruthPhotons()){
        //fake photon efficiency
        const float fakeEff2 = m_upgrade->getPhotonFakeRate(jet->pt());
        if(m_upgrade->getRandom3()->Uniform() < fakeEff2) {
          xAOD::TruthParticle *fakePhoton = new xAOD::TruthParticle();
          fakePhotons->push_back( fakePhoton );
          m_upgrade->smearFakePhoton(*fakePhoton, *jet);
          fakePhoton->auxdata<int>("passReco") = 1;
          fakePhoton->auxdata<int>("FakeType") = 2; // faketype 0:true, 1:e->y, 2:j->y
          continue;
        }
      }
    }

    // smear
    m_upgrade->smearJet( *jet );

    // jets must be above minimum threshold
    if ( jet->pt() < m_upgrade->getPileupJetPtThresholdMeV()) continue;

    jet->auxdata<int>("pileUp") = 0; // so we know this jet is not pileup later on
    
    const float trackEff = m_upgrade->getTrackJetConfirmEff(jet->pt(),jet->eta(), "HS");

    if(m_upgrade->getRandom3()->Uniform() < trackEff) {
      // mark that jet passed track confirmation
      jet->auxdata<int>("passTrackConf") = 1;

      // store b-tagging efficiency

      // first work out jet flavour
      char type = 'L';
      try {
        if(jet->auxdata<int>("GhostBHadronsFinalCount") > 0) type = 'B';
        else if(jet->auxdata<int>("GhostCHadronsFinalCount") > 0) type = 'C';
      } catch (const SG::ExcBadAuxVar& e) {
        //didn't find any ghost b-hadron info, have to assume it's a light jet
        ATH_MSG_DEBUG("Found a jet with no GhostXHadronFinalCount auxdata");
      }

      // now get b-tagging efficiency - update to mv2c10 but name remains same (mv1TagEff)
      const double MV1tagEff = m_upgrade->getFlavourTagEfficiency(jet->pt(), jet->eta(), type, "mv2c10", 70, m_upgrade->getPileupTrackConfSetting());
      jet->auxdata<float>("mv1TagEff") = MV1tagEff;

      // put jet in to selectedJets container, have to deep copy the jet
      xAOD::Jet* newjet = new xAOD::Jet(*jet);
      selectedJets->push_back( newjet );
      
    }
    else jet->auxdata<int>("passTrackConf") = 0;

  }

  // now get the pileup jets
  std::vector<TLorentzVector> puJets = m_upgrade->getPileupJets();
  // loop over pileup jets
  for(auto puJet : puJets) {
    if(m_dofakes){
      //fake electron efficiency
      const float fakeEff = m_upgrade->getElectronFakeRate(puJet.E(),puJet.Eta());
      if(m_upgrade->getRandom3()->Uniform() < fakeEff) {//TODO: are we going to use uniform?
        xAOD::TruthParticle *fakeElectron = new xAOD::TruthParticle();
        fakeElectrons->push_back( fakeElectron );
        m_upgrade->smearFakeElectron(*fakeElectron, puJet);
        fakeElectron->auxdata<int>("passReco") = 1;
        fakeElectron->auxdata<int>("FakeType") = 2; // faketype 0:true, 2:j->e
        continue;
      }
      if(m_config->useTruthPhotons()){
        //fake photon efficiency
        const float fakeEff2 = m_upgrade->getPhotonPileupFakeRate(puJet.Pt());
        if(m_upgrade->getRandom3()->Uniform() < fakeEff2) {//TODO: are we going to use uniform?
          xAOD::TruthParticle *fakePhoton = new xAOD::TruthParticle();
          fakePhotons->push_back( fakePhoton );
          m_upgrade->smearFakePhoton(*fakePhoton, puJet);
          fakePhoton->auxdata<int>("passReco") = 1;
          fakePhoton->auxdata<int>("FakeType") = 2; // faketype 0:true, 1:e->y, 2:j->y
          top::check(fakePhoton->isAvailable<int>("FakeType"), "missing faketype KF0");
          top::check(fakePhotons->back()->isAvailable<int>("FakeType"), "missing faketype KF1");
          continue;
        }
      }
    }
   
    const float trackEff = m_upgrade->getTrackJetConfirmEff(puJet.Pt(),puJet.Eta(),"PU");

    if(m_upgrade->getRandom3()->Uniform() > trackEff) continue; // ignore jets failing track confirmation

    // jet 4 momentum stored as PtEtaPhiM (see xAODJet/JetTypes.h)
    xAOD::JetFourMom_t mom( puJet.Pt(), puJet.Eta(), puJet.Phi(), puJet.M() );
    xAOD::Jet* jet = new xAOD::Jet();
    selectedJets->push_back( jet );
    selectedJets->back()->setJetP4( mom );
    selectedJets->back()->auxdata<int>("pileUp") = 1; // so we know this jet is pileup later on
    // now get b-tagging efficiency - update to mv2c10 but keep name same (mv1TagEff)
    const double MV1tagEff = m_upgrade->getFlavourTagEfficiency(jet->pt(), jet->eta(), 'P', "mv2c10", 70, m_upgrade->getPileupTrackConfSetting());
    selectedJets->back()->auxdata<float>("mv1TagEff") = MV1tagEff;

  }// pileup jet loop

  // sort jets by pT
  std::sort(selectedJets->begin(), selectedJets->end(), ptSort);

  // keep hold of the pointers in our private variables (also clears last event)
  m_jets.reset(selectedJets);
  m_jetsAux.reset(auxJetCont);
  
  // pass the jets to the ParticleLevelEvent object
  particleEvent.m_jets = selectedJets;

  /* -------------------------- Fat Jets-----------------------------------------------------------*/
  if ( m_config->useTruthLargeRJets() ){
     // pass the fat jets to the ParticleLevelEvent object
    const xAOD::JetContainer * origfatjets(0);
    top::check( evtStore()->retrieve( origfatjets, m_config->sgKeyTruthLargeRJets() ),
                "xAOD::TEvent::retrieve failed for Truth Jets" );
 
    // Shallow copy 
    auto fatjets_shallowCopy = xAOD::shallowCopyContainer( *origfatjets );
    std::unique_ptr<xAOD::JetContainer> fatjets(fatjets_shallowCopy.first);
    std::unique_ptr<xAOD::ShallowAuxContainer> fatjetsAux(fatjets_shallowCopy.second);

    // we are going to put selected 'good' fat jets in to a xAOD::JetContainer
    xAOD::JetContainer* selectedFatJets = new xAOD::JetContainer();
    xAOD::JetAuxContainer* auxFatJetCont = new xAOD::JetAuxContainer();
    selectedFatJets->setStore(auxFatJetCont); // connect container & store

    for(auto fatjet : *fatjets) {
      m_upgrade->smearFatJet( *fatjet );

      // put fat jet in to selectedFatJets container, have to deep copy the jet
      xAOD::Jet* newfatjet = new xAOD::Jet(*fatjet);
      selectedFatJets->push_back( newfatjet );
    }

    //TODO? handle PU, efficiency?
    // sort jets by pT
    std::sort(selectedFatJets->begin(), selectedFatJets->end(), ptSort);

    // keep hold of the pointers in our private variables (also clears last event)
    m_fatjets.reset(selectedFatJets);
    m_fatjetsAux.reset(auxFatJetCont);

    particleEvent.m_largeRJets = selectedFatJets;
  }

  /* ------------------------------ photons -----------------------------------------------------------*/
  if( m_config->useTruthPhotons() ){
    // Get photons
    const xAOD::TruthParticleContainer * origphotons(0);
    top::check( evtStore()->retrieve( origphotons, m_config->sgKeyTruthPhotons() ),
              "xAOD::TEvent::retrieve failed for Truth Photons" );
    
    // Shallow copy 
    std::pair< xAOD::TruthParticleContainer*, xAOD::ShallowAuxContainer* > photons_shallowCopy = xAOD::shallowCopyContainer( *origphotons );
    std::unique_ptr<xAOD::TruthParticleContainer> photons(photons_shallowCopy.first);
    std::unique_ptr<xAOD::ShallowAuxContainer>    photonsAux(photons_shallowCopy.second);

    // we are going to put selected 'good' photons in to a xAOD::TruthParticleContainer
    m_selectedPhotons.reset();
    m_selectedPhotons = std::make_shared<xAOD::TruthParticleContainer>();
    m_auxPhotonCont.reset();
    m_auxPhotonCont = std::make_shared<xAOD::TruthParticleAuxContainer>();
    m_selectedPhotons->setStore(m_auxPhotonCont.get()); // connect container & store

    for(auto photon : *photons) {
      // efficiency (assume pass unsmeared pT here)
      const double eff = m_upgrade->getPhotonEfficiency(photon->pt());

      // smear
      if (!((fabs(photon->eta())>1.37&&fabs(photon->eta())<1.52) || fabs(photon->eta())>2.37)) m_upgrade->smearPhoton( *photon ); // remove "bad eta" photons to avoid large amount of warnings

      // efficiency selection
      if(m_upgrade->getRandom3()->Uniform() < eff) {
        photon->auxdata<int>("passReco") = 1;
      } 
      else photon->auxdata<int>("passReco") = 0;

      if (m_dofakes) photon->auxdata<int>("FakeType") = 0; // faketype 0:true, 1:e->y, 2:j->y
      // apply all cuts
      if( m_objectSelector_Photon->apply( *photon ) ){
        xAOD::TruthParticle *newPh = new xAOD::TruthParticle();
        m_selectedPhotons->push_back( newPh ); //particle acquires the selectedPhotons auxStore
        *newPh = *photon; //deep copy
      }
    }

    if(m_dofakes){
      for(auto f_ph : *fakePhotons) {
        if( m_objectSelector_Photon->apply( *f_ph ) ) {
          xAOD::TruthParticle *newPh = new xAOD::TruthParticle();
          m_selectedPhotons->push_back( newPh );
          *newPh = *f_ph;
        }
      }
    }

    // sort the photons by pT
    std::sort(m_selectedPhotons->begin(), m_selectedPhotons->end(), ptSort);

    // keep hold of the pointers in our private variables (also clears last event)
    //m_photons.reset(photons);
    //m_photonsShallowAux.reset(photons_shallowCopy.second);

    // pass the photons to the ParticleLevelEvent object
    particleEvent.m_photons = m_selectedPhotons.get();
    for (const auto* const ph : * particleEvent.m_photons) {
      top::check(ph->isAvailable<int>("FakeType"), "missing faketype KF3");
    }
  }


  /* ---------- Electron finalization (after fakes treatment) -------------------*/
  if(m_dofakes){
    for(auto f_el : *fakeElectrons) {
      if( m_objectSelector_Electron->apply( *f_el ) ) {
        xAOD::TruthParticle *newEl = new xAOD::TruthParticle();
        m_selectedElectrons->push_back( newEl );
        *newEl = *f_el;
      }
    }
  }
  // sort the electrons by pT
  std::sort(m_selectedElectrons->begin(), m_selectedElectrons->end(), ptSort);

  // keep hold of the pointers in our private variables (also clears last event)
  //m_electrons.reset(electrons);
  //m_electronsShallowAux.reset(electrons_shallowCopy.second);

  // pass the electrons to the ParticleLevelEvent object
  particleEvent.m_electrons = m_selectedElectrons.get();

  /* ----------------------------- MET ------------------------------------------*/
  const xAOD::MissingETContainer * origmetcont(0);
  top::check( evtStore()->retrieve( origmetcont, m_config->sgKeyTruthMET() ),
      "xAOD::TEvent::retrieve failed for Truth MET" );

  ATH_MSG_DEBUG("N(MET) = " << origmetcont->size());

  // Maybe we shouldn't hard code this, but probably it will not change....
  const xAOD::MissingET* origmet = (*origmetcont)["NonInt"];

  // the MET type is just std::pair<double,double>, representing the smeared METx and METy
  UpgradePerformanceFunctionsxAOD::MET smearedMET = m_upgrade->getMETSmeared(origmet->sumet(), origmet->mpx(), origmet->mpy());
  
  // Shallow copy 
  std::pair< xAOD::MissingETContainer*, xAOD::ShallowAuxContainer* > metcont_shallowCopy = xAOD::shallowCopyContainer( *origmetcont );
  xAOD::MissingETContainer* metcont = metcont_shallowCopy.first;
  xAOD::MissingET* met = (*metcont)["NonInt"];
  met->setMpx(smearedMET.first);
  met->setMpy(smearedMET.second);

  // keep hold of the pointers in our private variables (also clears last event)
  m_metCont.reset(metcont);
  m_metShallowAux.reset(metcont_shallowCopy.second);

  // pass the MET pointer to the ParticleLevelEvent object
  particleEvent.m_met = met;
  
  return particleEvent;
}
