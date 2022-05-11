/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   PseudoTrackSelector
//   Author: Sebastien Rettie, sebastien.rettie@cern.ch
//   This is a modified version of exisitng TrackParticleMerger
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// Pseudotrack selector to be used mainly for creating new track
// collections with a subset of input pseudotracks
///////////////////////////////////////////////////////////////////

#include "DerivationFrameworkInDet/PseudoTrackSelector.h"
#include "GaudiKernel/MsgStream.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "InDetTrackSystematicsTools/InDetTrackTruthOriginDefs.h"
///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

namespace DerivationFramework {

  PseudoTrackSelector::PseudoTrackSelector(const std::string& t,
                                           const std::string& n,
                                           const IInterface* p) : AthAlgTool(t,n,p),
                                           m_trackOriginTool("InDet::InDetTrackTruthOriginTool")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    // The default goal of this selector is to create a track collection from a subset of pseudo tracks
    declareProperty("RecoTrackParticleLocation",            m_in_recoTrackParticleLocation);       /** Reco track collection.   */
    declareProperty("PseudoTrackParticleLocation",          m_in_pseudoTrackParticleLocation);     /** Pseudo track collection. */
    declareProperty("OutputRecoReplacedWithPseudo",         m_out_recoReplacedWithPseudo);         /** Output track collection. */
    declareProperty("OutputRecoReplacedWithPseudoFromB",    m_out_recoReplacedWithPseudoFromB);    /** Output track collection. */
    declareProperty("OutputRecoReplacedWithPseudoNotFromB", m_out_recoReplacedWithPseudoNotFromB); /** Output track collection. */
    declareProperty("OutputRecoPlusPseudo",                 m_out_recoPlusPseudo);                 /** Output track collection. */
    declareProperty("OutputRecoPlusPseudoFromB",            m_out_recoPlusPseudoFromB);            /** Output track collection. */
    declareProperty("OutputRecoPlusPseudoNotFromB",         m_out_recoPlusPseudoNotFromB);         /** Output track collection. */
    declareProperty("OutputRecoNoFakes",                    m_out_recoNoFakes);                    /** Output track collection. */
    declareProperty("OutputRecoNoFakesFromB",               m_out_recoNoFakesFromB);               /** Output track collection. */
    declareProperty("OutputRecoNoFakesNotFromB",            m_out_recoNoFakesNotFromB);            /** Output track collection. */
  }

  ///////////////////////////////////////////////////////////////////
  // Initialisation
  ///////////////////////////////////////////////////////////////////

  StatusCode PseudoTrackSelector::initialize()
  {
    ATH_MSG_DEBUG("Initializing PseudoTrackSelector");
    ATH_CHECK( m_in_recoTrackParticleLocation.initialize() );
    ATH_CHECK( m_in_pseudoTrackParticleLocation.initialize() );
    ATH_CHECK( m_out_recoReplacedWithPseudo.initialize() );
    ATH_CHECK( m_out_recoReplacedWithPseudoFromB.initialize() );
    ATH_CHECK( m_out_recoReplacedWithPseudoNotFromB.initialize() );
    ATH_CHECK( m_out_recoPlusPseudo.initialize() );
    ATH_CHECK( m_out_recoPlusPseudoFromB.initialize() );
    ATH_CHECK( m_out_recoPlusPseudoNotFromB.initialize() );
    ATH_CHECK( m_out_recoNoFakes.initialize() );
    ATH_CHECK( m_out_recoNoFakesFromB.initialize() );
    ATH_CHECK( m_out_recoNoFakesNotFromB.initialize() );
    ATH_CHECK( m_trackOriginTool.retrieve() );
    return StatusCode::SUCCESS;
  }

  ///////////////////////////////////////////////////////////////////
  // Finalize
  ///////////////////////////////////////////////////////////////////

  StatusCode PseudoTrackSelector::finalize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode PseudoTrackSelector::addBranches() const
  {
    const EventContext& ctx = Gaudi::Hive::currentContext();
    std::unique_ptr<xAOD::TrackParticleContainer> outputRecoReplacedWithPseudo = std::make_unique<xAOD::TrackParticleContainer>();
    std::unique_ptr<xAOD::TrackParticleAuxContainer> outputRecoReplacedWithPseudoAux = std::make_unique<xAOD::TrackParticleAuxContainer>();
    outputRecoReplacedWithPseudo->setStore(outputRecoReplacedWithPseudoAux.get());
    std::unique_ptr<xAOD::TrackParticleContainer> outputRecoReplacedWithPseudoFromB = std::make_unique<xAOD::TrackParticleContainer>();
    std::unique_ptr<xAOD::TrackParticleAuxContainer> outputRecoReplacedWithPseudoFromBAux = std::make_unique<xAOD::TrackParticleAuxContainer>();
    outputRecoReplacedWithPseudoFromB->setStore(outputRecoReplacedWithPseudoFromBAux.get());
    std::unique_ptr<xAOD::TrackParticleContainer> outputRecoReplacedWithPseudoNotFromB = std::make_unique<xAOD::TrackParticleContainer>();
    std::unique_ptr<xAOD::TrackParticleAuxContainer> outputRecoReplacedWithPseudoNotFromBAux = std::make_unique<xAOD::TrackParticleAuxContainer>();
    outputRecoReplacedWithPseudoNotFromB->setStore(outputRecoReplacedWithPseudoNotFromBAux.get());
    std::unique_ptr<xAOD::TrackParticleContainer> outputRecoPlusPseudo = std::make_unique<xAOD::TrackParticleContainer>();
    std::unique_ptr<xAOD::TrackParticleAuxContainer> outputRecoPlusPseudoAux = std::make_unique<xAOD::TrackParticleAuxContainer>();
    outputRecoPlusPseudo->setStore(outputRecoPlusPseudoAux.get());
    std::unique_ptr<xAOD::TrackParticleContainer> outputRecoPlusPseudoFromB = std::make_unique<xAOD::TrackParticleContainer>();
    std::unique_ptr<xAOD::TrackParticleAuxContainer> outputRecoPlusPseudoFromBAux = std::make_unique<xAOD::TrackParticleAuxContainer>();
    outputRecoPlusPseudoFromB->setStore(outputRecoPlusPseudoFromBAux.get());
    std::unique_ptr<xAOD::TrackParticleContainer> outputRecoPlusPseudoNotFromB = std::make_unique<xAOD::TrackParticleContainer>();
    std::unique_ptr<xAOD::TrackParticleAuxContainer> outputRecoPlusPseudoNotFromBAux = std::make_unique<xAOD::TrackParticleAuxContainer>();
    outputRecoPlusPseudoNotFromB->setStore(outputRecoPlusPseudoNotFromBAux.get());
    std::unique_ptr<xAOD::TrackParticleContainer> outputRecoNoFakes = std::make_unique<xAOD::TrackParticleContainer>();
    std::unique_ptr<xAOD::TrackParticleAuxContainer> outputRecoNoFakesAux = std::make_unique<xAOD::TrackParticleAuxContainer>();
    outputRecoNoFakes->setStore(outputRecoNoFakesAux.get());
    std::unique_ptr<xAOD::TrackParticleContainer> outputRecoNoFakesFromB = std::make_unique<xAOD::TrackParticleContainer>();
    std::unique_ptr<xAOD::TrackParticleAuxContainer> outputRecoNoFakesFromBAux = std::make_unique<xAOD::TrackParticleAuxContainer>();
    outputRecoNoFakesFromB->setStore(outputRecoNoFakesFromBAux.get());
    std::unique_ptr<xAOD::TrackParticleContainer> outputRecoNoFakesNotFromB = std::make_unique<xAOD::TrackParticleContainer>();
    std::unique_ptr<xAOD::TrackParticleAuxContainer> outputRecoNoFakesNotFromBAux = std::make_unique<xAOD::TrackParticleAuxContainer>();
    outputRecoNoFakesNotFromB->setStore(outputRecoNoFakesNotFromBAux.get());
    
    // retrieve reco and pseudo tracks
    SG::ReadHandle<xAOD::TrackParticleContainer> recoTrackParticleCol (m_in_recoTrackParticleLocation, ctx);
    SG::ReadHandle<xAOD::TrackParticleContainer> pseudoTrackParticleCol (m_in_pseudoTrackParticleLocation, ctx);

    // fail if containers not available
    if (!recoTrackParticleCol.isValid()){
      ATH_MSG_ERROR( m_in_recoTrackParticleLocation.key() << " not valid!" );
      return StatusCode::FAILURE;
    }
    if (!pseudoTrackParticleCol.isValid()){
      ATH_MSG_ERROR( m_in_pseudoTrackParticleLocation.key() << " not valid!" );
      return StatusCode::FAILURE;
    }
    
    // select subset of tracks
    fillRecoReplacedWithPseudo(recoTrackParticleCol.cptr(), pseudoTrackParticleCol.cptr(), outputRecoReplacedWithPseudo.get());
    fillRecoReplacedWithPseudo(recoTrackParticleCol.cptr(), pseudoTrackParticleCol.cptr(), outputRecoReplacedWithPseudoFromB.get(), true, false);
    fillRecoReplacedWithPseudo(recoTrackParticleCol.cptr(), pseudoTrackParticleCol.cptr(), outputRecoReplacedWithPseudoNotFromB.get(), false, true);
    fillRecoPlusPseudo(recoTrackParticleCol.cptr(), pseudoTrackParticleCol.cptr(), outputRecoPlusPseudo.get());
    fillRecoPlusPseudo(recoTrackParticleCol.cptr(), pseudoTrackParticleCol.cptr(), outputRecoPlusPseudoFromB.get(), true, false);
    fillRecoPlusPseudo(recoTrackParticleCol.cptr(), pseudoTrackParticleCol.cptr(), outputRecoPlusPseudoNotFromB.get(), false, true);
    fillRecoNoFakes(recoTrackParticleCol.cptr(), outputRecoNoFakes.get());
    fillRecoNoFakes(recoTrackParticleCol.cptr(), outputRecoNoFakesFromB.get(), true, false);
    fillRecoNoFakes(recoTrackParticleCol.cptr(), outputRecoNoFakesNotFromB.get(), false, true);

    // Write tracks to store
    ATH_MSG_DEBUG("outputRecoReplacedWithPseudo has "<<outputRecoReplacedWithPseudo.get()->size()<<" tracks");
    SG::WriteHandle<xAOD::TrackParticleContainer> h_write_recoReplacedWithPseudo(m_out_recoReplacedWithPseudo, ctx);
    ATH_CHECK(h_write_recoReplacedWithPseudo.record(std::move(outputRecoReplacedWithPseudo), std::move(outputRecoReplacedWithPseudoAux)));
    ATH_MSG_DEBUG("wrote "<<m_out_recoReplacedWithPseudo<<" to store");

    ATH_MSG_DEBUG("outputRecoReplacedWithPseudoFromB has "<<outputRecoReplacedWithPseudoFromB.get()->size()<<" tracks");
    SG::WriteHandle<xAOD::TrackParticleContainer> h_write_recoReplacedWithPseudoFromB(m_out_recoReplacedWithPseudoFromB, ctx);
    ATH_CHECK(h_write_recoReplacedWithPseudoFromB.record(std::move(outputRecoReplacedWithPseudoFromB), std::move(outputRecoReplacedWithPseudoFromBAux)));
    ATH_MSG_DEBUG("wrote "<<m_out_recoReplacedWithPseudoFromB<<" to store");

    ATH_MSG_DEBUG("outputRecoReplacedWithPseudoNotFromB has "<<outputRecoReplacedWithPseudoNotFromB.get()->size()<<" tracks");
    SG::WriteHandle<xAOD::TrackParticleContainer> h_write_recoReplacedWithPseudoNotFromB(m_out_recoReplacedWithPseudoNotFromB, ctx);
    ATH_CHECK(h_write_recoReplacedWithPseudoNotFromB.record(std::move(outputRecoReplacedWithPseudoNotFromB), std::move(outputRecoReplacedWithPseudoNotFromBAux)));
    ATH_MSG_DEBUG("wrote "<<m_out_recoReplacedWithPseudoNotFromB<<" to store");

    ATH_MSG_DEBUG("outputRecoPlusPseudo has "<<outputRecoPlusPseudo.get()->size()<<" tracks");
    SG::WriteHandle<xAOD::TrackParticleContainer> h_write_recoPlusPseudo(m_out_recoPlusPseudo, ctx);
    ATH_CHECK(h_write_recoPlusPseudo.record(std::move(outputRecoPlusPseudo), std::move(outputRecoPlusPseudoAux)));
    ATH_MSG_DEBUG("wrote "<<m_out_recoPlusPseudo<<" to store");

    ATH_MSG_DEBUG("outputRecoPlusPseudoFromB has "<<outputRecoPlusPseudoFromB.get()->size()<<" tracks");
    SG::WriteHandle<xAOD::TrackParticleContainer> h_write_recoPlusPseudoFromB(m_out_recoPlusPseudoFromB, ctx);
    ATH_CHECK(h_write_recoPlusPseudoFromB.record(std::move(outputRecoPlusPseudoFromB), std::move(outputRecoPlusPseudoFromBAux)));
    ATH_MSG_DEBUG("wrote "<<m_out_recoPlusPseudoFromB<<" to store");

    ATH_MSG_DEBUG("outputRecoPlusPseudoNotFromB has "<<outputRecoPlusPseudoNotFromB.get()->size()<<" tracks");
    SG::WriteHandle<xAOD::TrackParticleContainer> h_write_recoPlusPseudoNotFromB(m_out_recoPlusPseudoNotFromB, ctx);
    ATH_CHECK(h_write_recoPlusPseudoNotFromB.record(std::move(outputRecoPlusPseudoNotFromB), std::move(outputRecoPlusPseudoNotFromBAux)));
    ATH_MSG_DEBUG("wrote "<<m_out_recoPlusPseudoNotFromB<<" to store");

    ATH_MSG_DEBUG("outputRecoNoFakes has "<<outputRecoNoFakes.get()->size()<<" tracks");
    SG::WriteHandle<xAOD::TrackParticleContainer> h_write_recoNoFakes(m_out_recoNoFakes, ctx);
    ATH_CHECK(h_write_recoNoFakes.record(std::move(outputRecoNoFakes), std::move(outputRecoNoFakesAux)));
    ATH_MSG_DEBUG("wrote "<<m_out_recoNoFakes<<" to store");
    
    ATH_MSG_DEBUG("outputRecoNoFakesFromB has "<<outputRecoNoFakesFromB.get()->size()<<" tracks");
    SG::WriteHandle<xAOD::TrackParticleContainer> h_write_recoNoFakesFromB(m_out_recoNoFakesFromB, ctx);
    ATH_CHECK(h_write_recoNoFakesFromB.record(std::move(outputRecoNoFakesFromB), std::move(outputRecoNoFakesFromBAux)));
    ATH_MSG_DEBUG("wrote "<<m_out_recoNoFakesFromB<<" to store");

    ATH_MSG_DEBUG("outputRecoNoFakesNotFromB has "<<outputRecoNoFakesNotFromB.get()->size()<<" tracks");
    SG::WriteHandle<xAOD::TrackParticleContainer> h_write_recoNoFakesNotFromB(m_out_recoNoFakesNotFromB, ctx);
    ATH_CHECK(h_write_recoNoFakesNotFromB.record(std::move(outputRecoNoFakesNotFromB), std::move(outputRecoNoFakesNotFromBAux)));
    ATH_MSG_DEBUG("wrote "<<m_out_recoNoFakesNotFromB<<" to store");

    return StatusCode::SUCCESS;
  }
  
  ///////////////////////////////////////////////////////////////////
  // Select relevant pseudo tracks
  ///////////////////////////////////////////////////////////////////

  void PseudoTrackSelector::fillRecoReplacedWithPseudo(const xAOD::TrackParticleContainer* recoTrackParticleCol,
                                                       const xAOD::TrackParticleContainer* pseudoTrackParticleCol,
                                                       xAOD::TrackParticleContainer* outputCol,
                                                       bool onlyFromB /*= false*/,
                                                       bool onlyNotFromB /*= false*/) const
  {
    ATH_MSG_DEBUG("Start fillRecoReplacedWithPseudo with onlyFromB = "<<onlyFromB<<" and onlyNotFromB = "<<onlyNotFromB);
    // loop over tracks, accept them and add them into output collection
    if((recoTrackParticleCol && !recoTrackParticleCol->empty()) &&
       (pseudoTrackParticleCol && !pseudoTrackParticleCol->empty())) {
      ATH_MSG_DEBUG("Size of reco track particle collection " << recoTrackParticleCol->size());
      ATH_MSG_DEBUG("Size of pseudo track particle collection " << pseudoTrackParticleCol->size());
      int n_saved = 0;
      // loop over reco tracks
      for(const auto *const tp_reco: *recoTrackParticleCol){
        // Get truth particle
        const xAOD::TruthParticle* truth_reco = getTruth(tp_reco);
        if (truth_reco){
          bool match_found = false;
          // loop over pseudo tracks to look for a match
          for(const auto *const tp_pseudo: *pseudoTrackParticleCol){
            const xAOD::TruthParticle* truth_pseudo = getTruth(tp_pseudo);
            if (truth_reco == truth_pseudo){
              ATH_MSG_DEBUG("reco track has truth-matched pseudo track!");
              match_found = true;
              // add pseudo track into output
              if (!onlyFromB && !onlyNotFromB){
                xAOD::TrackParticle* newTrackParticle = new xAOD::TrackParticle(*tp_pseudo);
                outputCol->push_back(newTrackParticle);
                n_saved++;
                ATH_MSG_DEBUG("saving pseudo track");
              }
              // Check origin of track
              int origin = m_trackOriginTool->getTrackOrigin(tp_pseudo);
              ATH_MSG_DEBUG("fromB: "<<InDet::TrkOrigin::isFromB(origin));
              if (onlyFromB){
                if (InDet::TrkOrigin::isFromB(origin)){
                  // Track is from B hadron, replace with pseudo track
                  xAOD::TrackParticle* newTrackParticle = new xAOD::TrackParticle(*tp_pseudo);
                  outputCol->push_back(newTrackParticle);
                  n_saved++;
                  ATH_MSG_DEBUG("saving fromB pseudo track");
                }
                else{
                  // Track is not from B hadron, keep reco track
                  xAOD::TrackParticle* newTrackParticle = new xAOD::TrackParticle(*tp_reco);
                  outputCol->push_back(newTrackParticle);
                  n_saved++;
                  ATH_MSG_DEBUG("keeping notFromB reco track");
                }
              }
              if (onlyNotFromB){
                if (!InDet::TrkOrigin::isFromB(origin)){
                  // Track is not from B hadron, replace with pseudo track
                  xAOD::TrackParticle* newTrackParticle = new xAOD::TrackParticle(*tp_pseudo);
                  outputCol->push_back(newTrackParticle);
                  n_saved++;
                  ATH_MSG_DEBUG("saving notFromB pseudo track");
                }
                else{
                  // Track is from B hadron, keep reco track
                  xAOD::TrackParticle* newTrackParticle = new xAOD::TrackParticle(*tp_reco);
                  outputCol->push_back(newTrackParticle);
                  n_saved++;
                  ATH_MSG_DEBUG("keeping FromB reco track");
                }
              }
              // Found match and replaced relevant track
              break;
            }
            else{
              ATH_MSG_VERBOSE("truth-matched pseudo track not found yet...");
            }
          }
          if(!match_found){
            ATH_MSG_DEBUG("reco track that has truth link DOES NOT have truth-matched pseudo track! :(");
          }
        }
        else{
          ATH_MSG_DEBUG("no truth particle for reco track!");
        }
      }
      ATH_MSG_DEBUG("Size of output track particle collection " << outputCol->size() <<" (sanity: "<<n_saved<<")");
    }
  }

  void PseudoTrackSelector::fillRecoPlusPseudo(const xAOD::TrackParticleContainer* recoTrackParticleCol,
                                               const xAOD::TrackParticleContainer* pseudoTrackParticleCol,
                                               xAOD::TrackParticleContainer* outputCol,
                                               bool onlyFromB /*= false*/,
                                               bool onlyNotFromB /*= false*/) const
  {
    ATH_MSG_DEBUG("Start fillRecoPlusPseudo with onlyFromB = "<<onlyFromB<<" and onlyNotFromB = "<<onlyNotFromB);
    // loop over tracks, accept them and add them into output collection
    if((recoTrackParticleCol && !recoTrackParticleCol->empty()) &&
       (pseudoTrackParticleCol && !pseudoTrackParticleCol->empty())) {
      ATH_MSG_DEBUG("Size of reco track particle collection " << recoTrackParticleCol->size());
      ATH_MSG_DEBUG("Size of pseudo track particle collection " << pseudoTrackParticleCol->size());
      int n_saved = 0;
      // Start by keeping reco tracks
      for(const auto *const tp_reco: *recoTrackParticleCol){
        xAOD::TrackParticle* newTrackParticle = new xAOD::TrackParticle(*tp_reco);
        outputCol->push_back(newTrackParticle);
        n_saved++;
      }
      // Now add pseudo tracks that are not matched to reco tracks
      for(const auto *const tp_pseudo: *pseudoTrackParticleCol){
        bool match_found = false;
        const xAOD::TruthParticle* truth_pseudo = getTruth(tp_pseudo);
        for(const auto *const tp_reco: *recoTrackParticleCol){
          // Get truth particle
          const xAOD::TruthParticle* truth_reco = getTruth(tp_reco);
          if (truth_reco){
            if (truth_reco == truth_pseudo){
              ATH_MSG_DEBUG("reco track has truth-matched pseudo track!");
              match_found = true;
              break;
            }
          }
          else{
            ATH_MSG_DEBUG("no truth particle for reco track!");
          }
        }
        if(!match_found){
          ATH_MSG_DEBUG("no match found, so adding pseudo track to output");
          if(!onlyFromB && !onlyNotFromB){
            xAOD::TrackParticle* newTrackParticle = new xAOD::TrackParticle(*tp_pseudo);
            outputCol->push_back(newTrackParticle);
            n_saved++;
            ATH_MSG_DEBUG("saving pseudo track");
          }
          int origin = m_trackOriginTool->getTrackOrigin(tp_pseudo);
          ATH_MSG_DEBUG("fromB: "<<InDet::TrkOrigin::isFromB(origin));
          if(onlyFromB && InDet::TrkOrigin::isFromB(origin)){
            xAOD::TrackParticle* newTrackParticle = new xAOD::TrackParticle(*tp_pseudo);
            outputCol->push_back(newTrackParticle);
            n_saved++;
            ATH_MSG_DEBUG("saving fromB pseudo track");
          }
          if(onlyNotFromB && !InDet::TrkOrigin::isFromB(origin)){
            xAOD::TrackParticle* newTrackParticle = new xAOD::TrackParticle(*tp_pseudo);
            outputCol->push_back(newTrackParticle);
            n_saved++;
            ATH_MSG_DEBUG("saving notFromB pseudo track");
          }
        }
        else{
          ATH_MSG_DEBUG("match found so not adding pseudo track to output");
        }
      }
      ATH_MSG_DEBUG("Size of output track particle collection " << outputCol->size() <<" (sanity: "<<n_saved<<")");
    }
  }

  void PseudoTrackSelector::fillRecoNoFakes(const xAOD::TrackParticleContainer* recoTrackParticleCol,
                                            xAOD::TrackParticleContainer* outputCol,
                                            bool onlyFromB /*= false*/,
                                            bool onlyNotFromB /*= false*/) const
  {
    ATH_MSG_DEBUG("Start fillRecoNoFakes with onlyFromB = "<<onlyFromB<<" and onlyNotFromB = "<<onlyNotFromB);
    // loop over tracks, accept them and add them into output collection
    if((recoTrackParticleCol && !recoTrackParticleCol->empty())){
      ATH_MSG_DEBUG("Size of reco track particle collection " << recoTrackParticleCol->size());
      int n_saved = 0;
      static const SG::AuxElement::ConstAccessor< float > acc_truthProb("truthMatchProbability");

      // loop over tracks
      for(const auto *const tp_reco: *recoTrackParticleCol){
        // place cut on truth match probability
        if (!onlyFromB && !onlyNotFromB){
          if( acc_truthProb( *tp_reco ) < 0.5) {
            ATH_MSG_DEBUG("Fake track!");
            continue;
          }
        }
        int origin = m_trackOriginTool->getTrackOrigin(tp_reco);
        ATH_MSG_DEBUG("fromB: "<<InDet::TrkOrigin::isFromB(origin));
        if (onlyFromB && InDet::TrkOrigin::isFromB(origin)){
          if( acc_truthProb( *tp_reco ) < 0.5) {
            ATH_MSG_DEBUG("Fake track fromB!");
            continue;
          }
        }
        if (onlyNotFromB && !InDet::TrkOrigin::isFromB(origin)){
          if( acc_truthProb( *tp_reco ) < 0.5) {
            ATH_MSG_DEBUG("Fake track notFromB!");
            continue;
          }
        }
        // add track into output
        xAOD::TrackParticle* newTrackParticle = new xAOD::TrackParticle(*tp_reco);
        outputCol->push_back(newTrackParticle);
        n_saved++;
      }
      ATH_MSG_DEBUG("Size of output track particle collection " << outputCol->size() <<" (sanity: "<<n_saved<<")");
    }
  }

  const xAOD::TruthParticle* PseudoTrackSelector::getTruth( const xAOD::TrackParticle* track ) const {
    // create a pointer to a truth particle which will correspond to this track
    const xAOD::TruthParticle* linkedTruthParticle = nullptr;

    // if the track doesnt't have a valid truth link, skip to the next track
    // in practice, all tracks seem to have a truth link, but we need to also
    // check whether it's valid
    if ( !track->isAvailable<TruthLink>("truthParticleLink") ) {
      return nullptr;
    }

    // retrieve the link and check its validity
    const TruthLink &link = track->auxdata<TruthLink>("truthParticleLink");

    // a missing or invalid link implies truth particle has been dropped from
    // the truth record at some stage - probably it was a pileup or low pT GEANT
    // particle.
    if(!link or !link.isValid()) {
      return nullptr;
    }

    // seems safe to access and return the linked truth particle
    linkedTruthParticle = (*link);
    return linkedTruthParticle;
  }
}
