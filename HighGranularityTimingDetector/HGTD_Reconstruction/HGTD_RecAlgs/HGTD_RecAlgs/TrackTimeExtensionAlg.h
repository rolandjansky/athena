/**
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_RecAlgs/TrackTimeExtensionAlg.h
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

class TrackTimeExtensionAlg : public AthAlgorithm {

public:
  TrackTimeExtensionAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TrackTimeExtensionAlg() {}
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;

private:
  StatusCode execute_r (const EventContext& ctx);

  StatusCode decorateTrackParticle(const xAOD::TrackParticle* track_ptkl,
                                   const HGTD::ExtensionObject& extension,
                                   const InDetSimDataCollection* sdo_collection,
                                   const HepMC::GenEvent* hs_event,
                                   bool skip_deco = false) const;

  ToolHandle<IHGTD_TrackTimeExtensionTool> m_extension_tool{this, "TimeExtensionTool", "HGTD_IterativeExtensionTool/HGTD_IterativeExtensionTool", "Tool for extending ID tracks into the HGTD"};

  SG::ReadHandleKey<HGTD_ClusterContainer> m_clustercont_rh_key{this, "HGTDClusterContainerName", "HGTD_Cluster", "Name of the HGTD_Cluster container"};
  SG::ReadHandleKey<InDetSimDataCollection> m_sdo_coll_rh_key{this, "HGTD_SDOCollectionName", "HGTD_SDO_Map", "Name of the SDO container"};
  SG::ReadHandleKey<McEventCollection> m_mc_coll_rh_key{this, "MC_CollectionName", "TruthEvent", "Name of the McEventCollection containing the truth information"};
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trk_ptkl_rh_key{this, "TrackParticleContainerName", "InDetTrackParticles", "Name of the TrackParticle container"};

  StringProperty m_deco_prefix{this, "DecorationPrefix", "HGTD", "Prefix to better identify decorations"};

  FloatProperty m_eta_cut{this, "MinEtaAcceptance", 2.38, "Minimum eta to consider a track for extension"};

  ToolHandle<IHGTD_ClusterTruthTool> m_truth_tool{this, "TruthTool", "ClusterTruthTool/ClusterTruthTool", "Tool for classifying HGTD clusters with truth information"};

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
      m_dec_layer_cluster_shadowed;
  std::unique_ptr<SG::AuxElement::Decorator<std::vector<bool>>>
      m_dec_layer_cluster_merged;
  std::unique_ptr<SG::AuxElement::Decorator<std::vector<bool>>>
      m_dec_layer_primary_expected;
  std::unique_ptr<SG::AuxElement::Decorator<float>> m_dec_extrap_x;
  std::unique_ptr<SG::AuxElement::Decorator<float>> m_dec_extrap_y;
};

} // namespace HGTD

#endif // HGTD_RECALGS_TRACKTIMEEXTENSIONALG_H
