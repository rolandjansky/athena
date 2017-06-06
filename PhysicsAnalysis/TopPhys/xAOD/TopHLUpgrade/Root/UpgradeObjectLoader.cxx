/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopHLUpgrade/UpgradeObjectLoader.h"

#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"
#include "TopHLUpgrade/UpgradeLeptonObjectSelector.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODCore/ShallowCopy.h"

#include <algorithm>

using namespace top;

bool ptSort(const xAOD::IParticle* p1, const xAOD::IParticle* p2) {
  return p1->pt() > p2->pt();
}

UpgradeObjectLoader::UpgradeObjectLoader( const std::shared_ptr<top::TopConfig> & cfg ) :
  asg::AsgTool( "UpgradeObjectLoader" ),
  m_config(cfg),
  m_upgrade(),
  // tool is active depending on config settings
  m_active(m_config->isMC() && m_config->HLLHC()),
  // object selection tools setup below
  m_objectSelector_Electron( nullptr ),
  m_objectSelector_Muon( nullptr )
 {

  if ( m_active ){
    std::cout << "Upgrade level reconstruction is enabled; telling you how I am configured:" << '\n';
    m_upgrade.reset(new UpgradePerformanceFunctionsxAOD(UpgradePerformanceFunctions::gold, 200.0)); //layout & mu hard coded for now
    m_upgrade->setElectronWorkingPoint(UpgradePerformanceFunctions::looseElectron);
    m_upgrade->setElectronRandomSeed(171);
    m_upgrade->setMuonWorkingPoint(UpgradePerformanceFunctions::tightMuon);
    m_upgrade->setMETRandomSeed(986);
    m_upgrade->loadMETHistograms("UpgradePerformanceFunctions/sumetPU_mu200_ttbar_gold.root");
    m_upgrade->setPileupRandomSeed(121);
    m_upgrade->setPileupRandomSeed(771);
    m_upgrade->setPileupJetPtThresholdMeV(30000.);
    m_upgrade->setPileupEfficiencyScheme(UpgradePerformanceFunctions::PileupEff::HS);
    m_upgrade->setPileupEff(0.9);
    m_upgrade->setPileupTemplatesPath("/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/UpgradePerformanceFunctions/");
    m_upgrade->setFlavourTaggingCalibrationFilename("UpgradePerformanceFunctions/flavor_tags_v1.1.root");

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

  // container to put the selected electrons in - use view container to avoid deep copies
  m_selectedElectrons.reset( new xAOD::TruthParticleContainer(SG::VIEW_ELEMENTS));
  
  // Smear & efficiency correct electrons
  for(auto electron : *electrons) {
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
    if( m_objectSelector_Electron->apply( *electron ) ) m_selectedElectrons->push_back( electron );
  }

  // sort the electrons by pT
  std::sort(m_selectedElectrons->begin(), m_selectedElectrons->end(), ptSort);

  // keep hold of the pointers in our private variables (also clears last event)
  m_electrons.reset(electrons);
  m_electronsShallowAux.reset(electrons_shallowCopy.second);

  // pass the electrons to the ParticleLevelEvent object
  particleEvent.m_electrons = m_selectedElectrons.get();

  
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

      // now get b-tagging efficiency
      const double MV1tagEff = m_upgrade->getFlavourTagEfficiency(jet->pt(), jet->eta(), type, "mv1", 70, m_upgrade->getPileupTrackConfSetting());
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
    
    const float trackEff = m_upgrade->getTrackJetConfirmEff(puJet.Pt(),puJet.Eta(),"PU");

    if(m_upgrade->getRandom3()->Uniform() > trackEff) continue; // ignore jets failing track confirmation

    // jet 4 momentum stored as PtEtaPhiM (see xAODJet/JetTypes.h)
    xAOD::JetFourMom_t mom( puJet.Pt(), puJet.Eta(), puJet.Phi(), puJet.M() );
    xAOD::Jet* jet = new xAOD::Jet();
    selectedJets->push_back( jet );
    selectedJets->back()->setJetP4( mom );
    selectedJets->back()->auxdata<int>("pileUp") = 1; // so we know this jet is pileup later on
    // now get b-tagging efficiency
    const double MV1tagEff = m_upgrade->getFlavourTagEfficiency(jet->pt(), jet->eta(), 'P', "mv1", 70, m_upgrade->getPileupTrackConfSetting());
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


  /* ----------------------------- MET ------------------------------------------*/
  const xAOD::MissingETContainer * origmetcont(0);
  top::check( evtStore()->retrieve( origmetcont, m_config->sgKeyTruthMET() ),
	    "xAOD::TEvent::retrieve failed for Truth MET" );  

  ATH_MSG_DEBUG("N(MET) = " << origmetcont->size());

  // Maybe we shouldn't hard code this, but probably it will not change....
  const xAOD::MissingET* origmet = (*origmetcont)["NonInt"];

  // the MET type is just std::pair<double,double>, representing the smeared METx and METy
  UpgradePerformanceFunctions::MET smearedMET = m_upgrade->getMETSmeared(origmet->sumet(), origmet->mpx(), origmet->mpy());
  
  // Shallow copy 
  std::pair< xAOD::MissingETContainer*, xAOD::ShallowAuxContainer* > metcont_shallowCopy = xAOD::shallowCopyContainer( *origmetcont );
  xAOD::MissingETContainer* metcont = metcont_shallowCopy.first;
  xAOD::MissingET* met = (*metcont)["NonInt"];
  met->setMpx(smearedMET.first);
  met->setMpx(smearedMET.second);

  // keep hold of the pointers in our private variables (also clears last event)
  m_metCont.reset(metcont);
  m_metShallowAux.reset(metcont_shallowCopy.second);

  // pass the MET pointer to the ParticleLevelEvent object
  particleEvent.m_met = met;
  
  return particleEvent;
}
