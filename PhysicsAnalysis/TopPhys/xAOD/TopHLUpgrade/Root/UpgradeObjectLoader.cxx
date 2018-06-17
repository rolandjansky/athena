/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopHLUpgrade/UpgradeObjectLoader.h"

#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"
#include "TopHLUpgrade/UpgradeLeptonObjectSelector.h"
#include "TopHLUpgrade/UpgradePhotonObjectSelector.h"
#include "TopParticleLevel/ParticleLevelPhotonObjectSelector.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODCore/ShallowCopy.h"

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
    std::cout << "Upgrade level reconstruction is enabled; telling you how I am configured:" << '\n';
    //asg::setProperty(m_upgrade, "Layout", UpgradePerformanceFunctionsxAOD::Step1p6) , "Failed to setProperty" );
    m_upgrade.reset( new UpgradePerformanceFunctionsxAOD("UpgradePerformanceFunctionsxAOD"));
    std::cout << "Upgrade level reconstruction is enabled; telling you how I am configured:" << '\n';
    top::check(m_upgrade->setProperty("Layout", UpgradePerformanceFunctionsxAOD::Step1p6) , "Failed to setProperty" );
    top::check(m_upgrade->setProperty("AvgMu", 200) , "Failed to setProperty" );
    top::check(m_upgrade->setProperty("ElectronWorkingPoint", UpgradePerformanceFunctionsxAOD::mediumElectron) , "Failed to setProperty" );
    top::check(m_upgrade->setProperty("ElectronRadomSeed", 171) , "Failed to setProperty" );
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
  
  // Load event info object directly into the particleEvent
  top::check( evtStore()->retrieve( particleEvent.m_info, m_config->sgKeyEventInfo() ),
	      "xAOD::TEvent::retrieve failed for EventInfo" );

  /* ------------------------------ Muons -----------------------------------------------------------*/
  // Get muons
  const xAOD::TruthParticleContainer * origmuons(0);
  top::check( evtStore()->retrieve( origmuons, m_config->sgKeyTruthMuons() ),
	    "xAOD::TEvent::retrieve failed for Truth Muons" );

  // Shallow copy 
  std::pair< xAOD::TruthParticleContainer*, xAOD::ShallowAuxContainer* > muons_shallowCopy = xAOD::shallowCopyContainer( *origmuons );
  xAOD::TruthParticleContainer* muons = muons_shallowCopy.first;

  // container to put the selected muons in - use view container to avoid deep copies
  m_selectedMuons.reset( new xAOD::TruthParticleContainer(SG::VIEW_ELEMENTS));
  
  // Smear & efficiency correct muons
  for(auto muon : *muons) {
    // efficiency (assume pass unsmeared pT here)
    const double eff = m_upgrade->getMuonEfficiency( muon->pt(), muon->eta());
    //std::cout << "Muon with pT " << muon->pt() << " has efficiency = " << eff << std::endl;

    // smear
    m_upgrade->smearMuon( *muon );
    //std::cout << "\t smeared pT " << muon->pt() << std::endl;

    // store efficiency correction
    if(m_upgrade->getRandom3()->Uniform() < eff) {
      muon->auxdata<int>("passReco") = 1;
    }
    else muon->auxdata<int>("passReco") = 0;

    // apply all cuts
    if( m_objectSelector_Muon->apply( *muon ) ) m_selectedMuons->push_back( muon );

  }

  // sort the muons by pT
  std::sort(m_selectedMuons->begin(), m_selectedMuons->end(), ptSort);

  // keep hold of the pointers in our private variables (also clears last event)
  m_muons.reset(muons);
  m_muonsShallowAux.reset(muons_shallowCopy.second);

  // pass the muons to the ParticleLevelEvent object
  particleEvent.m_muons = m_selectedMuons.get();

  /* ------------------------------ Electrons -----------------------------------------------------------*/
  // Get electrons
  const xAOD::TruthParticleContainer * origelectrons(0);
  top::check( evtStore()->retrieve( origelectrons, m_config->sgKeyTruthElectrons() ),
	    "xAOD::TEvent::retrieve failed for Truth Electrons" );
  
  // Shallow copy 
  std::pair< xAOD::TruthParticleContainer*, xAOD::ShallowAuxContainer* > electrons_shallowCopy = xAOD::shallowCopyContainer( *origelectrons );
  xAOD::TruthParticleContainer* electrons = electrons_shallowCopy.first;

  // we are going to put selected 'good' electrons in to a xAOD::TruthParticleContainer
  m_selectedElectrons.reset();
  m_selectedElectrons = std::make_shared<xAOD::TruthParticleContainer>();
  m_auxElectronCont.reset();
  m_auxElectronCont = std::make_shared<xAOD::TruthParticleAuxContainer>();
  m_selectedElectrons->setStore(m_auxElectronCont.get()); // connect container & store
  //fake electrons container
  xAOD::TruthParticleContainer* fakeElectrons = new xAOD::TruthParticleContainer();
  xAOD::TruthParticleAuxContainer* auxfakeElectronsCont = new xAOD::TruthParticleAuxContainer();
  fakeElectrons->setStore(auxfakeElectronsCont); // connect container & store  
    
  //fake photons container
  xAOD::TruthParticleContainer* fakePhotons = NULL;
  xAOD::TruthParticleAuxContainer* auxfakePhotonsCont = NULL;
  if( m_dofakes && m_config->useTruthPhotons() ){
    fakePhotons = new xAOD::TruthParticleContainer();
    auxfakePhotonsCont = new xAOD::TruthParticleAuxContainer();
    fakePhotons->setStore(auxfakePhotonsCont); // connect container & store  
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
    //std::cout << "Electron with pT " << electron->pt() << " has efficiency = " << eff << std::endl;

    // smear
    m_upgrade->smearElectron( *electron );
    //std::cout << "\t smeared pT " << electron->pt() << std::endl;

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

  delete electrons_shallowCopy.first;
  delete electrons_shallowCopy.second;
  /* ------------------------------ Jets-----------------------------------------------------------*/
  // Get jets
  const xAOD::JetContainer * origjets(0);
  top::check( evtStore()->retrieve( origjets, m_config->sgKeyTruthJets() ),
	    "xAOD::TEvent::retrieve failed for Truth Jets" );

  // Shallow copy 
  std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > jets_shallowCopy = xAOD::shallowCopyContainer( *origjets );
  xAOD::JetContainer* jets = jets_shallowCopy.first;

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
      if(jet->auxdata<int>("GhostBQuarksFinalCount") > 0) type = 'B';
      else if(jet->auxdata<int>("GhostCQuarksFinalCount") > 0) type = 'C';

      // now get b-tagging efficiency - update to mv2c10 but name remains same (mv1TagEff)
      const double MV1tagEff = m_upgrade->getFlavourTagEfficiency(jet->pt(), jet->eta(), type, "mv2c10", 70, m_upgrade->getPileupTrackConfSetting());
      jet->auxdata<float>("mv1TagEff") = MV1tagEff;

      // put jet in to selectedJets container, have to deep copy the jet
      xAOD::Jet* newjet = new xAOD::Jet(*jet);
      selectedJets->push_back( newjet );
      
    }
    else jet->auxdata<int>("passTrackConf") = 0;

    
    //std::cout << "\t smeared pT " << jet->pt() << std::endl;
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
        const float fakeEff2 = m_upgrade->getPhotonFakeRate(puJet.Pt());
        if(m_upgrade->getRandom3()->Uniform() < fakeEff2) {//TODO: are we going to use uniform?
	  xAOD::TruthParticle *fakePhoton = new xAOD::TruthParticle();
          fakePhotons->push_back( fakePhoton );
          m_upgrade->smearFakePhoton(*fakePhoton, puJet);
          fakePhoton->auxdata<int>("passReco") = 1;
	  fakePhoton->auxdata<int>("FakeType") = 2; // faketype 0:true, 1:e->y, 2:j->y
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
  
  // clean up memory from the shallow copy of jets that is finished with now
  delete jets;
  delete jets_shallowCopy.second;

  /* -------------------------- Fat Jets-----------------------------------------------------------*/
  if ( m_config->useTruthLargeRJets() ){
     // pass the fat jets to the ParticleLevelEvent object
    const xAOD::JetContainer * origfatjets(0);
    top::check( evtStore()->retrieve( origfatjets, m_config->sgKeyTruthLargeRJets() ),
                "xAOD::TEvent::retrieve failed for Truth Jets" );
 
    //std::cout << "is large r jet" << std::endl;
    // Shallow copy 
    std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > fatjets_shallowCopy = xAOD::shallowCopyContainer( *origfatjets );
    xAOD::JetContainer* fatjets = fatjets_shallowCopy.first;

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

    delete fatjets;
    delete fatjets_shallowCopy.second;
  }

  /* ------------------------------ photons -----------------------------------------------------------*/
  if( m_config->useTruthPhotons() ){
    // Get photons
    const xAOD::TruthParticleContainer * origphotons(0);
    top::check( evtStore()->retrieve( origphotons, m_config->sgKeyTruthPhotons() ),
              "xAOD::TEvent::retrieve failed for Truth Photons" );
    
    // Shallow copy 
    std::pair< xAOD::TruthParticleContainer*, xAOD::ShallowAuxContainer* > photons_shallowCopy = xAOD::shallowCopyContainer( *origphotons );
    xAOD::TruthParticleContainer* photons = photons_shallowCopy.first;

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

      // apply all cuts
      if( m_objectSelector_Photon->apply( *photon ) ){
	if (m_dofakes) photon->auxdata<int>("FakeType") = 0; // faketype 0:true, 1:e->y, 2:j->y
        xAOD::TruthParticle *newPh = new xAOD::TruthParticle();
        m_selectedPhotons->push_back( newPh ); //particle acquires the selectedPhotons auxStore
        *newPh = *photon; //deep copy
      }
    }
    delete photons_shallowCopy.first;
    delete photons_shallowCopy.second;

    if(m_dofakes){
       for(auto f_ph : *fakePhotons) {
         if( m_objectSelector_Photon->apply( *f_ph ) ) m_selectedPhotons->push_back( f_ph );
       }
       delete auxfakePhotonsCont;
    }

    // sort the photons by pT
    std::sort(m_selectedPhotons->begin(), m_selectedPhotons->end(), ptSort);

    // keep hold of the pointers in our private variables (also clears last event)
    //m_photons.reset(photons);
    //m_photonsShallowAux.reset(photons_shallowCopy.second);

    // pass the photons to the ParticleLevelEvent object
    particleEvent.m_photons = m_selectedPhotons.get();
  }


  /* ---------- Electron finalization (after fakes treatment) -------------------*/
  if(m_dofakes){
     for(auto f_el : *fakeElectrons) {
       if( m_objectSelector_Electron->apply( *f_el ) ) m_selectedElectrons->push_back( f_el );
     }
     delete auxfakeElectronsCont;
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
