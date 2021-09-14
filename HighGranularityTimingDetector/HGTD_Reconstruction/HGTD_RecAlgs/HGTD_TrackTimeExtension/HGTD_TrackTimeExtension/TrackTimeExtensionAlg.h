/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_TrackTimeExtension/TrackTimeExtensionAlg.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @author
 * @date September, 2021
 *
 * @brief Calls the track extension tool and adds the output in addition to
 * truth information as decorations to the track particles.
 * In future iterations, the output of the extension can be a dedicated object
 * written to file, with the TrackParticle linking to its exension (if it has
 * one).
 *
 * TODO:
 * - add all relevant decorations
 * - access the truth information to classify extensions
 */

#ifndef HGTD_RECALGS_TRACKTIMEEXTENSIONALG_H
#define HGTD_RECALGS_TRACKTIMEEXTENSIONALG_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ToolHandle.h"
#include "GeneratorObjects/McEventCollection.h"
#include "HGTD_PrepRawData/HGTD_ClusterContainer.h"
#include "HGTD_RecToolInterfaces/IHGTD_ClusterTruthTool.h"
#include "HGTD_RecToolInterfaces/IHGTD_TrackTimeExtensionTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "InDetSimData/InDetSimDataCollection.h"

#include <string>

namespace HGTD {

using HGTDExtension_t =
    std::array<std::unique_ptr<const Trk::TrackStateOnSurface>, 4>;

class TrackTimeExtensionAlg : public AthAlgorithm {

public:
  TrackTimeExtensionAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TrackTimeExtensionAlg() {}
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;

private:
  StatusCode decorateTrackParticle(const xAOD::TrackParticle* track_ptkl,
                                   const HGTDExtension_t& extension,
                                   const InDetSimDataCollection* sdo_collection,
                                   const HepMC::GenEvent* hs_event,
                                   bool skip_deco = false);

  ToolHandle<HGTD::IHGTD_TrackTimeExtensionTool> m_extension_tool;

  SG::ReadHandleKey<HGTD::HGTD_ClusterContainer> m_clustercont_rh_key;
  SG::ReadHandleKey<InDetSimDataCollection> m_sdo_coll_rh_key;
  SG::ReadHandleKey<McEventCollection> m_mc_coll_rh_key;
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trk_ptkl_rh_key;

  std::string m_deco_prefix;

  float m_eta_cut;

  ToolHandle<HGTD::IHGTD_ClusterTruthTool> m_truth_tool;

  std::unique_ptr<SG::AuxElement::Decorator<std::vector<bool>>>
      m_dec_layer_has_ext;
  std::unique_ptr<SG::AuxElement::Decorator<std::vector<float>>>
      m_dec_layer_ext_chi2;
  std::unique_ptr<SG::AuxElement::Decorator<std::vector<float>>>
      m_dec_layer_cluster_raw_time;
  std::unique_ptr<SG::AuxElement::Decorator<std::vector<float>>>
      m_dec_layer_cluster_time;
  std::unique_ptr<SG::AuxElement::Decorator<std::vector<int>>>
      m_dec_layer_cluster_truth_class;
  std::unique_ptr<SG::AuxElement::Decorator<std::vector<bool>>>
      m_dec_layer_cluster_cluster_shadowed;
  std::unique_ptr<SG::AuxElement::Decorator<std::vector<bool>>>
      m_dec_layer_cluster_cluster_merged;
  // to look for primary hits that have been missed, additional info is needed.
  // this will go in in a next iteration
  // std::unique_ptr<SG::AuxElement::Decorator<std::vector<bool>>>
  //     m_dec_layer_cluster_cluster_expected;
};

} // namespace HGTD

#endif // HGTD_RECALGS_TRACKTIMEEXTENSIONALG_H
