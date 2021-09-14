/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_TrackTimeExtension/src/TrackTimeExtensionAlg.cxx
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @author
 * @date July, 2021
 * @brief
 */

#include "HGTD_TrackTimeExtension/TrackTimeExtensionAlg.h"

#include "HGTD_RIO_OnTrack/HGTD_ClusterOnTrack.h"
#include "StoreGate/WriteHandle.h"
#include "xAODTruth/TruthParticleContainer.h"

namespace {
static constexpr unsigned short n_hgtd_layers = 4;
}

namespace HGTD {

TrackTimeExtensionAlg::TrackTimeExtensionAlg(const std::string& name,
                                             ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator),
      m_extension_tool(
          "HGTD_IterativeExtensionTool/HGTD_IterativeExtensionTool"),
      m_deco_prefix("HGTD"),
      m_eta_cut(2.38),
      m_truth_tool("ClusterTruthTool/ClusterTruthTool") {
  declareProperty("TimeExtensionTool", m_extension_tool);
  declareProperty("HGTDClusterContainerName",
                  m_clustercont_rh_key = "HGTD_Cluster");
  declareProperty("HGTD_SDOCollectionName", m_sdo_coll_rh_key = "HGTD_SDO_Map");
  declareProperty("MC_CollectionName", m_mc_coll_rh_key = "TruthEvent");
  declareProperty("TrackParticleContainerName",
                  m_trk_ptkl_rh_key = "InDetTrackParticles");
  declareProperty("DecorationPrefix", m_deco_prefix);
  declareProperty("MinEtaAcceptance", m_eta_cut);
  declareProperty("TruthTool", m_truth_tool);
}

StatusCode TrackTimeExtensionAlg::initialize() {

  ATH_CHECK(m_extension_tool.retrieve());

  ATH_CHECK(m_clustercont_rh_key.initialize());
  ATH_CHECK(m_sdo_coll_rh_key.initialize());
  ATH_CHECK(m_mc_coll_rh_key.initialize());
  ATH_CHECK(m_trk_ptkl_rh_key.initialize());

  m_dec_layer_has_ext =
      std::make_unique<SG::AuxElement::Decorator<std::vector<bool>>>(
          m_deco_prefix + "_has_extension");
  m_dec_layer_ext_chi2 =
      std::make_unique<SG::AuxElement::Decorator<std::vector<float>>>(
          m_deco_prefix + "_extension_chi2");
  m_dec_layer_cluster_raw_time =
      std::make_unique<SG::AuxElement::Decorator<std::vector<float>>>(
          m_deco_prefix + "_cluster_raw_time");
  m_dec_layer_cluster_time =
      std::make_unique<SG::AuxElement::Decorator<std::vector<float>>>(
          m_deco_prefix + "_cluster_time");
  m_dec_layer_cluster_truth_class =
      std::make_unique<SG::AuxElement::Decorator<std::vector<int>>>(
          m_deco_prefix + "_cluster_truth_class");
  m_dec_layer_cluster_cluster_shadowed =
      std::make_unique<SG::AuxElement::Decorator<std::vector<bool>>>(
          m_deco_prefix + "_cluster_shadowed");
  m_dec_layer_cluster_cluster_merged =
      std::make_unique<SG::AuxElement::Decorator<std::vector<bool>>>(
          m_deco_prefix + "_cluster_merged");
  // m_dec_layer_cluster_cluster_expected =
  //     std::make_unique<SG::AuxElement::Decorator<std::vector<bool>>>(
  //         m_deco_prefix + "_cluster_expected");

  return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

StatusCode TrackTimeExtensionAlg::execute() {

  ATH_MSG_DEBUG("Start event");

  SG::ReadHandle<HGTD::HGTD_ClusterContainer> cluster_container_handle(
      m_clustercont_rh_key);
  const HGTD::HGTD_ClusterContainer* cluster_container =
      cluster_container_handle.cptr();
  if (not cluster_container) {
    ATH_MSG_ERROR("[TrackTimeExtensionAlg] HGTD_ClusterContainer not found, "
                  "aborting execute!");
    return StatusCode::FAILURE;
  }

  SG::ReadHandle<InDetSimDataCollection> sdo_collection_handle(
      m_sdo_coll_rh_key);
  const InDetSimDataCollection* sdo_collection = sdo_collection_handle.cptr();
  if (not sdo_collection) {
    ATH_MSG_ERROR(
        "[TrackTimeExtensionAlg] SDO Collection not found, aborting execute!");
    return StatusCode::FAILURE;
  }

  SG::ReadHandle<McEventCollection> mc_collection_handle(m_mc_coll_rh_key);
  const McEventCollection* mc_coll = mc_collection_handle.cptr();
  if (not mc_coll) {
    ATH_MSG_ERROR("[TrackTimeExtensionAlg] McEventCollection not found, "
                  "aborting execute!");
    return StatusCode::FAILURE;
  }

  const HepMC::GenEvent* hs_event = nullptr;
  if (mc_coll->size() > 0) {
    hs_event = mc_coll->at(0);
  } else {
    hs_event = nullptr;
  }

  // for each track, run the extension if the track is in HGTD acceptance
  SG::ReadHandle<xAOD::TrackParticleContainer> trk_ptkl_container_handle(
      m_trk_ptkl_rh_key);
  const xAOD::TrackParticleContainer* track_particles =
      trk_ptkl_container_handle.cptr();
  if (not track_particles) {
    ATH_MSG_ERROR("[TrackTimeExtensionAlg] TrackParticleContainer not found, "
                  "aborting execute!");
    return StatusCode::FAILURE;
  }

  for (const auto* track_ptkl : *track_particles) {

    ATH_MSG_DEBUG("Track eta: " << track_ptkl->eta()
                                << " pt: " << track_ptkl->pt());

    HGTDExtension_t extension{nullptr, nullptr, nullptr, nullptr};

    if (std::abs(track_ptkl->eta()) < m_eta_cut) {
      ATH_MSG_DEBUG("Track out of acceptance");
      // decorate all track particle objects to avoid issues with the
      // decorations
      ATH_CHECK(decorateTrackParticle(track_ptkl, extension, sdo_collection,
                                      hs_event, true));
      continue;
    }

    // this should not happen?
    if (track_ptkl->track() == nullptr) {
      ATH_MSG_DEBUG("There is no Trk::Track");
      ATH_CHECK(decorateTrackParticle(track_ptkl, extension, sdo_collection,
                                      hs_event, true));
      continue;
    }

    // return 4 track states on surface objects as a result of the extension
    extension = m_extension_tool->extendTrackToHGTD(*(track_ptkl->track()),
                                                    cluster_container);

    // TODO here:
    // retrieve truth info for associated and non-associated HGTD hits, merging
    // or shadowing info

    // decorate the track
    ATH_CHECK(decorateTrackParticle(track_ptkl, extension, sdo_collection,
                                    hs_event, false));

  } // END LOOP over tracks

  return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

StatusCode TrackTimeExtensionAlg::decorateTrackParticle(
    const xAOD::TrackParticle* track_ptkl, const HGTDExtension_t& extension,
    const InDetSimDataCollection* sdo_collection,
    const HepMC::GenEvent* hs_event, bool skip_deco) {

  std::vector<bool> has_cluster_vec;
  has_cluster_vec.reserve(n_hgtd_layers);
  std::vector<float> chi2_vec;
  chi2_vec.reserve(n_hgtd_layers);
  std::vector<float> raw_time_vec;
  raw_time_vec.reserve(n_hgtd_layers);
  std::vector<float> time_vec;
  time_vec.reserve(n_hgtd_layers);
  std::vector<int> truth_vec;
  truth_vec.reserve(n_hgtd_layers);
  std::vector<bool> is_shadowed_vec;
  is_shadowed_vec.reserve(n_hgtd_layers);
  std::vector<bool> is_merged_vec;
  is_merged_vec.reserve(n_hgtd_layers);

  for (const std::unique_ptr<const Trk::TrackStateOnSurface>& trk_state :
       extension) {

    if (trk_state) {
      has_cluster_vec.emplace_back(true);

      chi2_vec.emplace_back(
          trk_state->fitQualityOnSurface()->chiSquared() /
          trk_state->fitQualityOnSurface()->doubleNumberDoF());

      const HGTD::HGTD_ClusterOnTrack* cot =
          dynamic_cast<const HGTD::HGTD_ClusterOnTrack*>(
              trk_state->measurementOnTrack());

      time_vec.emplace_back(cot->time());

      // get the cluster
      const HGTD::HGTD_Cluster* cluster = cot->prepRawData();

      raw_time_vec.emplace_back(cluster->time());

      // get the truth particle
      static SG::AuxElement::Accessor<ElementLink<xAOD::TruthParticleContainer>>
          acc_tpl("truthParticleLink");
      const xAOD::TruthParticle* truth_particle = nullptr;
      if (acc_tpl.isAvailable(*track_ptkl)) {
        auto truth_match_link = acc_tpl(*track_ptkl);
        if (truth_match_link.isValid()) {
          truth_particle = *truth_match_link;
        }
      }

      HGTD::ClusterTruthOrigin truth_origin = ClusterTruthOrigin::UNIDENTIFIED;
      bool is_shadowed = false;
      bool is_merged = false;

      if (truth_particle) {
        auto truth_info = m_truth_tool->classifyCluster(
            cluster, truth_particle, sdo_collection, hs_event);
        truth_origin = truth_info.origin;
        is_shadowed = truth_info.is_shadowed;
        is_merged = truth_info.is_merged;
      }

      truth_vec.emplace_back(truth_origin);
      is_shadowed_vec.emplace_back(is_shadowed);
      is_merged_vec.emplace_back(is_merged);
    } else {
      has_cluster_vec.emplace_back(false);
      if (not skip_deco) {
        chi2_vec.emplace_back(-1.);
        raw_time_vec.emplace_back(-1.);
        time_vec.emplace_back(-1.);
        truth_vec.emplace_back(-1);
        is_shadowed_vec.emplace_back(false);
        is_merged_vec.emplace_back(false);
      }
    }
  } // END LOOP over TrackStateOnSurface

  m_dec_layer_has_ext->set(*track_ptkl, has_cluster_vec);
  m_dec_layer_ext_chi2->set(*track_ptkl, chi2_vec);
  m_dec_layer_cluster_raw_time->set(*track_ptkl, raw_time_vec);
  m_dec_layer_cluster_time->set(*track_ptkl, time_vec);
  m_dec_layer_cluster_truth_class->set(*track_ptkl, truth_vec);
  m_dec_layer_cluster_cluster_shadowed->set(*track_ptkl, is_shadowed_vec);
  m_dec_layer_cluster_cluster_merged->set(*track_ptkl, is_merged_vec);

  return StatusCode::SUCCESS;
}

} // namespace HGTD
