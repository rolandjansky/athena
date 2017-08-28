/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Filename: ParticleLevelLoader.cxx
// Description:
// Author: Fabian Wilk
// Created: Sun Feb 22 13:24:02 2015

#include "TopParticleLevel/ParticleLevelLoader.h"

#include <list>
#include <cassert>
#include <iomanip>

#include "TopEvent/EventTools.h"

#include "TopParticleLevel/ParticleLevelEvent.h"

#include "TopParticleLevel/ParticleLevelElectronObjectSelector.h"
#include "TopParticleLevel/ParticleLevelMuonObjectSelector.h"
#include "TopParticleLevel/ParticleLevelPhotonObjectSelector.h"
#include "TopParticleLevel/ParticleLevelJetObjectSelector.h"

#include "TopConfiguration/TopConfig.h"

// #define TOP_PARTICLE_LEVEL_DEBUG_OVERLAP_REMOVAL 1

namespace top {

    ParticleLevelLoader::ParticleLevelLoader( const std::shared_ptr<top::TopConfig> & cfg )
        : asg::AsgTool( "ParticleLevelLoader" ),
          m_config( cfg ),
          // Don't create them here because construction is a bit long-ish
          m_objectSelector_Electron( nullptr ),
          m_objectSelector_Muon( nullptr ),
          m_objectSelector_Photon( nullptr ),
          m_objectSelector_Jet( nullptr ),
          m_objectSelector_LargeRJet( nullptr ),
          // Tool is inactive on non-MC data and whenever particle level is not requested
          m_active( m_config->doTopParticleLevel() &&
                    m_config->isMC() &&
                    ( m_config->useTruthElectrons() ||
                      m_config->useTruthMuons() ||
                      m_config->useTruthPhotons() ||
                      m_config->useTruthJets() ||
                      m_config->useTruthLargeRJets() ||
                      m_config->useTruthMET() ) ) {

        // Configure and create electron object selector
        auto optEl = ParticleLevelElectronObjectSelector::Options{
            m_config->truth_electron_PtCut(),
            m_config->truth_electron_EtaCut(),
            m_config->truth_electron_NotFromHadron(),
            m_config->truth_electron_TauIsHadron()
        };

        m_objectSelector_Electron.reset( new ParticleLevelElectronObjectSelector( optEl ) );

        // Configure and create muon object selector
        auto optMu = ParticleLevelMuonObjectSelector::Options{
            m_config->truth_muon_PtCut(),
            m_config->truth_muon_EtaCut(),
            m_config->truth_muon_NotFromHadron(),
            m_config->truth_muon_TauIsHadron()
        };

        m_objectSelector_Muon.reset( new ParticleLevelMuonObjectSelector( optMu ) );

        // Configure and create photon object selector
        auto optPhoton = ParticleLevelPhotonObjectSelector::Options{
            m_config->truth_photon_PtCut(),
            m_config->truth_photon_EtaCut(),
            m_config->truth_photon_Origin(),
            m_config->truth_photon_Isolation()
        };

        m_objectSelector_Photon.reset( new ParticleLevelPhotonObjectSelector( optPhoton ) );


        // Configure and create jet object selector
        auto optJet = ParticleLevelJetObjectSelector::Options{
            m_config->truth_jet_PtCut(),
            m_config->truth_jet_EtaCut()
        };

        m_objectSelector_Jet.reset( new ParticleLevelJetObjectSelector( optJet ) );

        // Configure and create jet object selector
        auto optLargeRJet = ParticleLevelJetObjectSelector::Options{
	    m_config->truth_jet_largeR_PtCut(),
            m_config->truth_jet_largeR_EtaCut()
        };

        m_objectSelector_LargeRJet.reset( new ParticleLevelJetObjectSelector( optLargeRJet ) );


        if ( m_active ){
            std::cout << "Particle level reconstruction is enabled; telling you how I am configured:" << '\n';
            std::cout << "   " << std::setw( 20 ) << "UseElectrons? " << std::setw( 5 ) << std::boolalpha << m_config->useTruthElectrons();
            if ( m_config->useTruthElectrons() ){
                std::cout << " [" << m_config->sgKeyTruthElectrons() << "]" << '\n'
                          << "     --- Pt           > " << m_config->truth_electron_PtCut() << '\n'
                          << "     --- |eta|        < " << m_config->truth_electron_EtaCut() << '\n'
                          << "     --- notFromHadron? " << std::boolalpha << m_config->truth_electron_NotFromHadron() << '\n'
                          << "     --- tauIsHadron?   " << std::boolalpha << m_config->truth_electron_TauIsHadron() << '\n';
            } else {
                std::cout << '\n';
            }
            std::cout << "   " << std::setw( 20 ) << "UseMuons? " << std::setw( 5 ) << std::boolalpha << m_config->useTruthMuons();
            if ( m_config->useTruthMuons() ){
                std::cout << " [" << m_config->sgKeyTruthMuons() << "]" << '\n'
                          << "     --- Pt           > " << m_config->truth_muon_PtCut() << '\n'
                          << "     --- |eta|        < " << m_config->truth_muon_EtaCut() << '\n'
                          << "     --- notFromHadron? " << std::boolalpha << m_config->truth_muon_NotFromHadron() << '\n'
                          << "     --- tauIsHadron?   " << std::boolalpha << m_config->truth_muon_TauIsHadron() << '\n';
            } else {
                std::cout << '\n';
            }
            std::cout << "   " << std::setw( 20 ) << "UsePhotons? " << std::setw( 5 ) << std::boolalpha << m_config->useTruthPhotons();
            if ( m_config->useTruthPhotons() ){
                std::cout << " [" << m_config->sgKeyTruthPhotons() << "]" << '\n'
                          << "     --- Pt           > " << m_config->truth_photon_PtCut() << '\n'
                          << "     --- |eta|        < " << m_config->truth_photon_EtaCut() << '\n'
                          << "     --- Origin       = " << m_config->truth_photon_Origin() << '\n'
                          << "     --- Isolation    = " << m_config->truth_photon_Isolation() << '\n';
            } else {
                std::cout << '\n';
            }
            std::cout << "   " << std::setw( 20 ) << "UseJets? " << std::setw( 5 ) << std::boolalpha << m_config->useTruthJets();
            if ( m_config->useTruthJets() ){
                std::cout << " [" << m_config->sgKeyTruthJets() << "]" << '\n'
                          << "     --- Pt           > " << m_config->truth_jet_PtCut() << '\n'
                          << "     --- |eta|        < " << m_config->truth_jet_EtaCut() << '\n';
            } else {
                std::cout << '\n';
            }
            std::cout << "   " << std::setw( 20 ) << "UseLargeRJets? " << std::setw( 5 ) << std::boolalpha << m_config->useTruthLargeRJets();
            if ( m_config->useTruthJets() ){
                std::cout << " [" << m_config->sgKeyTruthLargeRJets() << "]" << '\n'
                          << "     --- Pt           > " << m_config->truth_jet_largeR_PtCut() << '\n'
                          << "     --- |eta|        < " << m_config->truth_jet_largeR_EtaCut() << '\n';
            } else {
                std::cout << '\n';
            }
            std::cout << "   " << std::setw( 20 ) << "UseMET? " << std::setw( 5 ) << std::boolalpha << m_config->useTruthMET();
            if ( m_config->useTruthMET() ){
                std::cout << " [" << m_config->sgKeyTruthMET() << "]" << '\n';
            } else {
                std::cout << '\n';
            }
            std::cout << "   " << std::setw( 20 ) << "DoOverlapRemoval Mu-Jet? " << std::setw( 5 ) << std::boolalpha << m_config->doParticleLevelOverlapRemovalMuJet() << '\n';
            std::cout << "   " << std::setw( 20 ) << "DoOverlapRemoval El-Jet? " << std::setw( 5 ) << std::boolalpha << m_config->doParticleLevelOverlapRemovalElJet() << '\n';
            std::cout << "   " << std::setw( 20 ) << "DoOverlapRemoval Jet-Photon? " << std::setw( 5 ) << std::boolalpha << m_config->doParticleLevelOverlapRemovalJetPhoton() << '\n';

        } else {
            std::cout << "Particle level reconstruction is disabled." << '\n';
        }
    }

    ParticleLevelLoader::~ParticleLevelLoader(){ /* Deliberately Empty */ }

    ParticleLevelEvent ParticleLevelLoader::load() {

        // If the ParticleLevelLoader is not active, return an empty object
        if ( ! m_active ){ return {}; }

        // Create the result object.
        ParticleLevelEvent plEvent;

        // Load event info object directly into the plEvent
        top::check( evtStore()->retrieve( plEvent.m_info, m_config->sgKeyEventInfo() ),
                    "xAOD::TEvent::retrieve failed for EventInfo" );

        // Load the info for electrons, muons, jets, and MET into local objects:
        // They need to be selected / modified / etc
        const xAOD::TruthParticleContainer * electrons{ nullptr };
        const xAOD::TruthParticleContainer * muons{ nullptr };
        const xAOD::TruthParticleContainer * photons{ nullptr };
        const xAOD::JetContainer * jets{ nullptr };
        const xAOD::JetContainer * largeRJets{ nullptr };
        const xAOD::MissingETContainer * mets{ nullptr };

        if ( m_config->useTruthElectrons() ){
            top::check( evtStore()->retrieve( electrons, m_config->sgKeyTruthElectrons() ),
                        "xAOD::TEvent::retrieve failed for Truth Electrons" );
        }

        if ( m_config->useTruthMuons() ){
            top::check( evtStore()->retrieve( muons, m_config->sgKeyTruthMuons() ),
                        "xAOD::TEvent::retrieve failed for Truth Muons" );
        }

        if ( m_config->useTruthPhotons() ){
            top::check( evtStore()->retrieve( photons, m_config->sgKeyTruthPhotons() ),
                        "xAOD::TEvent::retrieve failed for Truth Photons" );
        }

        if ( m_config->useTruthJets() ){
            top::check( evtStore()->retrieve( jets, m_config->sgKeyTruthJets() ),
                        "xAOD::TEvent::retrieve failed for Truth Jets" );
        }

        if ( m_config->useTruthLargeRJets() ){
            top::check( evtStore()->retrieve( largeRJets, m_config->sgKeyTruthLargeRJets() ),
                        "xAOD::TEvent::retrieve failed for Truth Jets Large R" );
        }

        if ( m_config->useTruthMET() ){
            top::check( evtStore()->retrieve( mets, m_config->sgKeyTruthMET() ),
                        "xAOD::TEvent::retrieve failed for Truth Missing ET" );
        }

        // ======================================================================
        //                      DRESSING
        // We want to put the dressed leptons into the Event container, however,
        // after reading from the TOPQ file, their dressed kinematics are stored
        // as decorations. Lets (1) create a shallow copied container now, (2)
        // set the kinematics for the shallow copies to the dressed values, (3)
        // create decorations for the bare (non-dressed) kinematics, and (4)
        // deep-copy the selection electrons from that container later.

        // ELECTRONS
        if ( m_config->useTruthElectrons() ) {
            top::check(loadDressedLeptons(* electrons,
                                          m_electronsDressed,
                                          m_electronsDressedAux),
                       "Failure to load dressed electrons.");
        }

        // MUONS
        if ( m_config->useTruthMuons() ){
            top::check(loadDressedLeptons(* muons,
                                          m_muonsDressed,
                                          m_muonsDressedAux),
                       "Failure to load dressed muons.");
        }

        // ======================================================================
        //                            OBJECT SELECTION
        // This yields index collections, one for each
        //    { electrons, muons, photons, jets, largeRJets }.
        std::list<std::size_t> idx_electrons;     // -> relative to `electrons`
        std::list<std::size_t> idx_muons;         // -> relative to `muons`
        std::list<std::size_t> idx_photons;       // -> relative to `photons`
        std::list<std::size_t> idx_jets;          // -> relative to `jets`
        std::list<std::size_t> idx_largeRJets;    // -> relative to `largeRJets`

        // Electrons
        if ( m_config->useTruthElectrons() ) {
            for ( std::size_t i = 0; i < electrons->size(); ++i ){
                const auto * electron = electrons->at(i);

                // Allow use of mixed-particle input container.
                if ( electron->absPdgId() != 11 ){ continue;  }

                // FIXME: Is this still needed?
                if ( electron->barcode() >= 2e5 ){ continue; }

                if ( m_objectSelector_Electron->apply(* electron) ){
                    idx_electrons.push_back( i );
                }
            }
        }

        // Muons
        if ( m_config->useTruthMuons() ){
            for ( std::size_t i = 0; i < muons->size(); ++i ){
                const auto * muon = muons->at(i);

                // Allow use of mixed-particle input container.
                if ( muon->absPdgId() != 13 ){ continue;  }

                // FIXME: Is this still needed?
                if ( muon->barcode() >= 2e5 ){ continue; }

                if ( m_objectSelector_Muon->apply(* muon) ){
                    idx_muons.push_back( i );
                }
            }
        }

        // Photons
        if ( m_config->useTruthPhotons() ){
            for ( std::size_t i = 0; i < photons->size(); ++i ){
                const auto * photon = photons->at(i);

                // Allow use of mixed-particle input container.
                if ( photon->absPdgId() != 22 ){ continue;  }

                // FIXME: Is this still needed?
                if ( photon->barcode() >= 2e5 ){ continue;  }

                if ( not m_objectSelector_Photon->apply(* photon) ){
                    continue;
                }

                // Reject photons used for electron dressing.
                if (m_config->useTruthElectrons()){
                    if (isDressingPhoton(* photon, * electrons)){
                        continue;
                    }
                }

                // Reject photons used for muon dressing.
                if (m_config->useTruthMuons()){
                    if (isDressingPhoton(* photon, * muons)){
                        continue;
                    }
                }
                idx_photons.push_back( i );
            }
        }

        // Jets
        if ( m_config->useTruthJets() ){
            for ( std::size_t i = 0; i < jets->size(); ++i ){
                if ( m_objectSelector_Jet->apply( * jets->at( i ) ) ){
                    idx_jets.push_back( i );
                }
            }
        }

        // Large-R-Jets
        if ( m_config->useTruthLargeRJets() ){
            for ( std::size_t i = 0; i < largeRJets->size(); ++i ){
                if ( m_objectSelector_LargeRJet->apply( * largeRJets->at( i ) ) ){
                    idx_largeRJets.push_back( i );
                }
            }
        }

        // ======================================================================
        //                             OVERLAP REMOVAL
        // Removal Steps:
        //   1. Jets & Muons:
        //      Remove Muons with dR < 0.4
        //   2. Jets & Electrons:
        //      Remove Electrons with dR < 0.4
        //   3. Photons & Jets:
        //      Remove Jets with dR < 0.4

        // Jets and Muons: Remove Muon with dR < 0.4
        if ( m_config->useTruthMuons() && m_config->useTruthJets() && m_config->doParticleLevelOverlapRemovalMuJet() ) {

#ifdef TOP_PARTICLE_LEVEL_DEBUG_OVERLAP_REMOVAL
            const std::size_t nMuonsPreOR{idx_muons.size()};
#endif

            idx_muons.remove_if([& idx_jets, & jets, this](std::size_t m){
                    for( auto j : idx_jets ){
                        if ( jets->at( j )->p4().DeltaR( m_muonsDressed->at( m )->p4() ) < 0.4 ){
                            return true;
                        }
                    }
                    return false;
                });

#ifdef TOP_PARTICLE_LEVEL_DEBUG_OVERLAP_REMOVAL
            std::cout << "[top::ParticleLevelLoader] Muon-In-Jet OR: "
                      << nMuonsPreOR << " -> " << idx_muons.size() << std::endl;
#endif
        }

        // Jets and Electrons: Remove Electron with dR < 0.4
        if ( m_config->useTruthElectrons() && m_config->useTruthJets() && m_config->doParticleLevelOverlapRemovalElJet() ) {

#ifdef TOP_PARTICLE_LEVEL_DEBUG_OVERLAP_REMOVAL
            const std::size_t nElectronsPreOR{idx_electrons.size()};
#endif

            idx_electrons.remove_if([& idx_jets, & jets, this](std::size_t e){
                    for( auto j : idx_jets ){
                        if ( jets->at( j )->p4().DeltaR( m_electronsDressed->at( e )->p4() ) < 0.4 ){
                            return true;
                        }
                    }
                    return false;
                });

#ifdef TOP_PARTICLE_LEVEL_DEBUG_OVERLAP_REMOVAL
            std::cout << "[top::ParticleLevelLoader] Electron-In-Jet OR: "
                      << nElectronsPreOR << " -> " << idx_electrons.size() << std::endl;
#endif
        }

        // Photons and Jets: Remove Jet with dR < 0.4
        if ( m_config->useTruthPhotons() && m_config->useTruthJets() && m_config->doParticleLevelOverlapRemovalJetPhoton() ) {

#ifdef TOP_PARTICLE_LEVEL_DEBUG_OVERLAP_REMOVAL
            const std::size_t nJetPreOR{idx_jets.size()};
#endif

            idx_jets.remove_if([& idx_photons, & photons, & jets](std::size_t j){
                    for( auto ph : idx_photons ){
                        if ( photons->at( ph )->p4().DeltaR( jets->at( j )->p4() ) < 0.4 ){
                            return true;
                        }
                    }
                    return false;
                });

#ifdef TOP_PARTICLE_LEVEL_DEBUG_OVERLAP_REMOVAL
            std::cout << "[top::ParticleLevelLoader] Jet-In-Photon OR: "
                      << nJetsPreOR << " -> " << idx_jets.size() << std::endl;
#endif
        }


        // ======================================================================
        //                             WRITE TO LOADER
        // 1. Loop index lists for electrons / muons / photons / jets
        // 2. Create a deep copy of the item and insert it into the
        //    appropriate container
        // 3. Create a shallow copy of the containers and apply corrections
        //    if needed (dressing, etc.)

        // Create New Containers holding the "Good" Electrons / Muons / Jets
        // and the MET
        if ( m_config->useTruthElectrons() ) {
            xAOD::TruthParticleContainer * goodElectrons = new xAOD::TruthParticleContainer();
            xAOD::TruthParticleAuxContainer * goodElectronsAux = new xAOD::TruthParticleAuxContainer();
            goodElectrons->setStore( goodElectronsAux ); //< Connect the two

            m_goodElectrons.reset( goodElectrons );
            m_goodElectronsAux.reset( goodElectronsAux );

            for ( auto e : idx_electrons ){
                const auto & elPtr = m_electronsDressed->at( e );
                xAOD::TruthParticle * electron = new xAOD::TruthParticle();
                electron->makePrivateStore( * elPtr );
                m_goodElectrons->push_back( electron );
            }
        }

        if ( m_config->useTruthMuons() ){
            xAOD::TruthParticleContainer * goodMuons = new xAOD::TruthParticleContainer();
            xAOD::TruthParticleAuxContainer * goodMuonsAux = new xAOD::TruthParticleAuxContainer();
            goodMuons->setStore( goodMuonsAux ); //< Connect the two

            m_goodMuons.reset( goodMuons );
            m_goodMuonsAux.reset( goodMuonsAux );

            for ( auto m : idx_muons ){
                const auto & muPtr = m_muonsDressed->at( m );
                xAOD::TruthParticle * muon = new xAOD::TruthParticle();
                muon->makePrivateStore( * muPtr );
                m_goodMuons->push_back( muon );
            }
        }

        if ( m_config->useTruthPhotons() ){
            xAOD::TruthParticleContainer * goodPhotons = new xAOD::TruthParticleContainer();
            xAOD::TruthParticleAuxContainer * goodPhotonsAux = new xAOD::TruthParticleAuxContainer();
            goodPhotons->setStore( goodPhotonsAux ); //< Connect the two

            m_goodPhotons.reset( goodPhotons );
            m_goodPhotonsAux.reset( goodPhotonsAux );

            for ( auto ph : idx_photons ){
                const auto & phPtr = photons->at( ph );
                xAOD::TruthParticle * photon = new xAOD::TruthParticle();
                photon->makePrivateStore( * phPtr );
                m_goodPhotons->push_back( photon );
            }
        }

        if ( m_config->useTruthJets() ){
            xAOD::JetContainer * goodJets = new xAOD::JetContainer();
            xAOD::JetAuxContainer * goodJetsAux = new xAOD::JetAuxContainer();
            goodJets->setStore( goodJetsAux ); //< Connect the two

            m_goodJets.reset( goodJets );
            m_goodJetsAux.reset( goodJetsAux );

            for ( auto j : idx_jets ){
                const auto & jetPtr = jets->at( j );
                xAOD::Jet * jet = new xAOD::Jet();
                jet->makePrivateStore( * jetPtr );
                m_goodJets->push_back( jet );
            }
        }

        if ( m_config->useTruthLargeRJets() ){
            xAOD::JetContainer * goodLargeRJets = new xAOD::JetContainer();
            xAOD::JetAuxContainer * goodLargeRJetsAux = new xAOD::JetAuxContainer();
            goodLargeRJets->setStore( goodLargeRJetsAux ); //< Connect the two

            m_goodLargeRJets.reset( goodLargeRJets );
            m_goodLargeRJetsAux.reset( goodLargeRJetsAux );

            for ( auto j : idx_largeRJets ){
                const auto & jetPtr = largeRJets->at( j );
                xAOD::Jet * jet = new xAOD::Jet();
                jet->makePrivateStore( * jetPtr );
                m_goodLargeRJets->push_back( jet );
            }
        }

        // ======================================================================
        //                         INSERTION INTO STORAGE
        // Put everything into storage, i.e. into the ParticleLevel.event object
        plEvent.m_electrons = m_config->useTruthElectrons() ? m_goodElectrons.get() : nullptr;
        plEvent.m_muons = m_config->useTruthMuons() ? m_goodMuons.get() : nullptr;
        plEvent.m_photons = m_config->useTruthPhotons() ? m_goodPhotons.get() : nullptr;
        plEvent.m_jets = m_config->useTruthJets() ? m_goodJets.get() : nullptr;
        plEvent.m_largeRJets = m_config->useTruthLargeRJets() ? m_goodLargeRJets.get() : nullptr;
        plEvent.m_met = m_config->useTruthMET() ? (* mets)[ "NonInt" ] : nullptr;

        return plEvent;
    }

    bool ParticleLevelLoader::loadDressedLeptons(const xAOD::TruthParticleContainer & input,
                                                 std::unique_ptr<xAOD::TruthParticleContainer> & store,
                                                 std::unique_ptr<xAOD::ShallowAuxContainer> & storeAux) const {
        auto dressed = xAOD::shallowCopyContainer(input);
        store.reset(dressed.first);
        storeAux.reset(dressed.second);

        for (auto pl : * store.get()){
            TLorentzVector fvDressed;
            fvDressed.SetPtEtaPhiE(pl->auxdata<float>("pt_dressed"),
                    pl->auxdata<float>("eta_dressed"),
                    pl->auxdata<float>("phi_dressed"),
                    pl->auxdata<float>("e_dressed"));

            // Store original kinematics as decoration
            // m->auxdata<int>( "nPhotons_dressed" ) = m->auxdata<int>( "nPhotons_dressed" );

            pl->auxdata<float>( "pt_bare" ) = pl->pt();
            pl->auxdata<float>( "eta_bare" ) = pl->eta();
            pl->auxdata<float>( "phi_bare" ) = pl->phi();
            pl->auxdata<float>( "e_bare" ) = pl->e();

            pl->setPx( fvDressed.Px() );
            pl->setPy( fvDressed.Py() );
            pl->setPz( fvDressed.Pz() );
            pl->setE( fvDressed.E() );
        }

        return true;
    }

    bool ParticleLevelLoader::isDressingPhoton(const xAOD::TruthParticle & photon,
                                               const xAOD::TruthParticleContainer & dressedParticles,
                                               const float dressingCone /* = 0.1 */) const {
        // We do not check whether the original truth particle decoration exists
        // and / or is valid because at this point we assume that it was already
        // used by the lepton loading function.
        static const std::string decoName{"originalTruthParticle"};

        for (const auto & particle : dressedParticles){
            auto truthProxy = particle->auxdata<ElementLink<xAOD::TruthParticleContainer> >("originalTruthParticle");

            if ((* truthProxy)->p4().DeltaR(photon.p4()) <= dressingCone){
                return true;
            }

        }

        return false;
    }
}
