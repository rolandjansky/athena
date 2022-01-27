/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_FastDigitization/HGTD_SmearedDigitizationTool.h
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @author
 * @date May, 2021
 * @brief Tool to do a fast digitization, meant mainily for standalone reco
 *        developments. Smearing of the SiHit intersection position as well
 *        as the SiHit time can be turned on and off (both on by default).
 *        Per SiHit, only the pixel at the point of intersection, which is the
 *        halfway between start and end, and only its identifier is stored in
 *        rdo_list of the HGTD_Cluster, even if more pixels have been crossed.
 */

#ifndef HGTD_FASTSIDIGITIZATION_HGTD_SMEAREDDIGITIZATIONTOOL_H
#define HGTD_FASTSIDIGITIZATION_HGTD_SMEAREDDIGITIZATIONTOOL_H

#include "AthenaKernel/IAthRNGSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/AlgTool.h"
#include "HGTD_PrepRawData/HGTD_Cluster.h"
#include "HGTD_PrepRawData/HGTD_ClusterCollection.h"
#include "HGTD_PrepRawData/HGTD_ClusterContainer.h"
#include "HitManagement/TimedHitCollection.h"
#include "HitManagement/TimedHitPtr.h"
#include "InDetSimEvent/SiHit.h"
#include "InDetSimEvent/SiHitCollection.h"
#include "PileUpTools/PileUpToolBase.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include <string>

// forward declarations
class HGTD_DetectorManager;
class HGTD_ID;
class PRD_MultiTruthCollection;
class TFile;
class TTree;

namespace CLHEP {
class HepRandomEngine;
}

class HGTD_SmearedDigitizationTool : virtual public PileUpToolBase {
public:
  using Cluster_t = HGTD_Cluster;
  using ClusterCollection_t = HGTD_ClusterCollection;
  using ClusterContainer_t = HGTD_ClusterContainer;

  using HGTD_DetElement_RIO_Map_t =
      std::multimap<IdentifierHash, const Cluster_t*>;

  HGTD_SmearedDigitizationTool(const std::string& type, const std::string& name,
                               const IInterface* parent);

  ~HGTD_SmearedDigitizationTool();

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  // main method to be called when using this tool
  virtual StatusCode processAllSubEvents(const EventContext& ctx) override;

private:
  // methods called within processAllSubEvents
  StatusCode retrieveTruth();

  TimedHitCollection<SiHit> setupTimedHitCollection();

  // creat HGTD_Cluster from SiHit
  StatusCode digitize(const EventContext& ctx);

  StatusCode fillMultiTruthCollection(PRD_MultiTruthCollection*, Cluster_t*,
                                      TimedHitPtr<SiHit>);

  StatusCode fillClusterContainer();

  float smearPosition(float pos, float sig, float boundary, CLHEP::HepRandomEngine * rndmEngine);

  float smearMeanTime(float time, float time_res, CLHEP::HepRandomEngine * rndmEngine);

  const HGTD_DetectorManager* m_hgtd_det_manager{nullptr};
  const HGTD_ID* m_hgtd_idhelper{nullptr};

  PRD_MultiTruthCollection* m_prd_truth_coll{nullptr};
  std::string m_prd_truth_coll_name{"PRD_MultiTruthPixel"};

  ServiceHandle<PileUpMergeSvc> m_merge_svc{this, "MergeSvc", "PileUpMergeSvc", "Merge service used in HGTD fast digitization"}; //!
  ServiceHandle<IAthRNGSvc> m_rndm_svc{this, "RndmSvc", "AthRNGSvc", "Random number service used in HGTD fast digitization"}; //!< Random number service

  ClusterContainer_t* m_cluster_container{nullptr};
  StringProperty m_cluster_name{this, "HGTD_ClustersContainerName", "HGTD_Cluster", "Name of the HGTD cluster container"};

  StringProperty m_si_hit_collection_name{this, "SiHitCollectionName", "HGTD_Hits", "Name of the Si Hit collection"};
  TimedHitCollection<SiHit>* m_timed_hit_collection{nullptr};

  std::unique_ptr<HGTD_DetElement_RIO_Map_t> m_det_element_rio_map{nullptr};

  BooleanProperty m_smear_intersection_position{this, "SmearIntersectionPosition", true, ""};
  BooleanProperty m_smear_mean_time{this, "SmearMeanTime", true, ""};
  BooleanProperty m_write_tree{this, "WriteTree", false, ""};

  // will default to 1.3x1.3 pitch size for HGTD, but can be set via property
  FloatProperty m_pitch_x{this, "pitch_X", 1.3, ""};
  FloatProperty m_pitch_y{this, "pitch_Y", 1.3, ""};
  float m_time_res{0.035}; // time resolution in ns

  // variables used only for writing out validation tree (not written by default)

  ITHistSvc* m_hist_svc;
  std::unique_ptr<TFile> m_output_file;
  std::unique_ptr<TTree> m_tree;

  float m_x_hit;
  float m_y_hit;
  float m_x_hit_smeared;
  float m_y_hit_smeared;

  float m_x_entry_hit;
  float m_y_entry_hit;
  float m_z_entry_hit;
  float m_x_exit_hit;
  float m_y_exit_hit;
  float m_z_exit_hit;

  float m_hit_time;
  float m_hit_time_smeared;

  float m_x_cluster_global;
  float m_y_cluster_global;
  float m_z_cluster_global;

  float m_err_x_hit;
  float m_err_y_hit;

  static constexpr unsigned int m_crazy_bc =
      std::numeric_limits<int32_t>::max();
};

#endif // HGTD_FASTSIDIGITIZATION_HGTD_SMEAREDDIGITIZATIONTOOL_H
