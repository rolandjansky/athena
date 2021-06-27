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
 *
 *  TODO: - change IAtRndmGenSvc to IAthRNGSvc when moving to Release 22!!
 */

#ifndef HGTD_FASTSIDIGITIZATION_HGTD_SMEAREDDIGITIZATIONTOOL_H
#define HGTD_FASTSIDIGITIZATION_HGTD_SMEAREDDIGITIZATIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "HGTD_PrepRawData/HGTD_Cluster.h"
#include "HGTD_PrepRawData/HGTD_ClusterCollection.h"
#include "HGTD_PrepRawData/HGTD_ClusterContainer.h"
#include "HitManagement/TimedHitCollection.h"
#include "HitManagement/TimedHitPtr.h"
#include "InDetSimEvent/SiHit.h"
#include "InDetSimEvent/SiHitCollection.h"
#include "PileUpTools/PileUpToolBase.h"
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
  using Cluster_t = HGTD::HGTD_Cluster;
  using ClusterCollection_t = HGTD::HGTD_ClusterCollection;
  using ClusterContainer_t = HGTD::HGTD_ClusterContainer;

  using HGTD_DetElement_RIO_Map_t =
      std::multimap<IdentifierHash, const Cluster_t*>;

  HGTD_SmearedDigitizationTool(const std::string& type, const std::string& name,
                               const IInterface* parent);

  StatusCode initialize();
  StatusCode finalize();

  // main method to be called when using this tool
  StatusCode processAllSubEvents();

private:
  // methods called within processAllSubEvents
  StatusCode retrieveTruth();

  TimedHitCollection<SiHit> setupTimedHitCollection();

  // creat HGTD_Cluster from SiHit
  StatusCode digitize();

  StatusCode fillMultiTruthCollection(PRD_MultiTruthCollection*, Cluster_t*,
                                      TimedHitPtr<SiHit>);

  StatusCode fillClusterContainer();

  float smearPosition(float pos, float sig, float boundary);

  float smearMeanTime(float time, float time_res);

  const HGTD_DetectorManager* m_hgtd_det_manager;
  const HGTD_ID* m_hgtd_idhelper;

  PRD_MultiTruthCollection* m_prd_truth_coll;
  std::string m_prd_truth_coll_name;

  ServiceHandle<PileUpMergeSvc> m_merge_svc;
  //TODO change IAtRndmGenSvc to IAthRNGSvc when moving code to R22
  ServiceHandle<IAtRndmGenSvc> m_rndm_svc;
  CLHEP::HepRandomEngine* m_random_engine;
  std::string m_random_engine_name;

  ClusterContainer_t* m_cluster_container;
  std::string m_cluster_name;

  std::string m_si_hit_collection_name;
  TimedHitCollection<SiHit>* m_timed_hit_collection;

  std::unique_ptr<HGTD_DetElement_RIO_Map_t> m_det_element_rio_map;

  bool m_smear_intersection_position;
  bool m_smear_mean_time;
  bool m_write_tree;

  // will default to 1.3x1.3 pitch size for HGTD, but can be set via property
  float m_pitch_x;
  float m_pitch_y;
  float m_time_res; // time resolution in ns

  ITHistSvc* m_hist_svc;
  TFile* m_output_file;
  TTree* m_tree;

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

  static constexpr float m_sqrt12 = std::sqrt(12.);

  static constexpr unsigned int m_crazy_bc =
      std::numeric_limits<int32_t>::max();
};

#endif // HGTD_FASTSIDIGITIZATION_HGTD_SMEAREDDIGITIZATIONTOOL_H
