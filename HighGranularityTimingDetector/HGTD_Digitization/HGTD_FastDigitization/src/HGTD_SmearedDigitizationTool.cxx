/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_FastDigitization/src/HGTD_SmearedDigitizationTool.cxx
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 * @author
 * @date May, 2021
 * @brief Implementation file for class HGTD_SmearedDigitizationTool
 */

#include "HGTD_FastDigitization/HGTD_SmearedDigitizationTool.h"

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "HGTD_Identifier/HGTD_ID.h"
#include "HGTD_ReadoutGeometry/HGTD_DetectorManager.h"
#include "InDetSimEvent/SiHit.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"

#include "TFile.h"
#include "TTree.h"

HGTD_SmearedDigitizationTool::HGTD_SmearedDigitizationTool(
    const std::string& type, const std::string& name, const IInterface* parent)
    : PileUpToolBase(type, name, parent),
      m_hgtd_det_manager(nullptr),
      m_hgtd_idhelper(nullptr),
      m_prd_truth_coll(nullptr),
      m_prd_truth_coll_name("PRD_MultiTruthPixel"),
      m_merge_svc("PileUpMergeSvc", name),
      m_rndm_svc("AtRndmGenSvc", name),
      m_random_engine(nullptr),
      m_random_engine_name("SmearedDigitization"),
      m_timed_hit_collection(nullptr),
      m_det_element_rio_map(nullptr),
      m_smear_intersection_position(true),
      m_smear_mean_time(true),
      m_write_tree(false),
      m_pitch_x(1.3),
      m_pitch_y(1.3),
      m_time_res(0.035) {
  declareProperty("RndmSvc", m_rndm_svc,
                  "Random Number Service used in fast digitization");
  declareProperty("SiHitCollectionName", m_si_hit_collection_name = "HGTD_Hits",
                  "Name of the Si Hit collection");
  declareProperty("HGTD_ClustersContainerName",
                  m_cluster_name = "HGTD_Cluster");
  declareProperty("SmearIntersectionPosition", m_smear_intersection_position);
  declareProperty("SmearMeanTime", m_smear_mean_time);
  declareProperty("WriteTree", m_write_tree);
  declareProperty("pitch_X", m_pitch_x);
  declareProperty("pitch_Y", m_pitch_y);
}

StatusCode HGTD_SmearedDigitizationTool::initialize() {

  ATH_MSG_DEBUG("HGTD_SmearedDigitizationTool::initialize()");

  // Get the HGTD Detector Manager
  if (detStore()->retrieve(m_hgtd_det_manager, "HGTD").isFailure()) {
    ATH_MSG_ERROR("Can't get HGTD_DetectorManager ");
    return StatusCode::FAILURE;
  }

  // Get the HGTD ID helper
  if (detStore()->retrieve(m_hgtd_idhelper, "HGTD_ID").isFailure()) {
    ATH_MSG_ERROR("Could not get HGTD ID helper");
    return StatusCode::FAILURE;
  }

  // locate the PileUpMergeSvc
  if (!m_merge_svc.retrieve().isSuccess()) {
    ATH_MSG_ERROR("Could not find PileUpMergeSvc");
    return StatusCode::FAILURE;
  }

  // Get rndm engine with own seeds:
  m_random_engine = m_rndm_svc->GetEngine(m_random_engine_name);
  if (!m_random_engine) {
    ATH_MSG_ERROR("Could not get random engine '" << m_random_engine_name
                                                  << "'");
    return StatusCode::FAILURE;
  }

  if (m_write_tree) {
    if (service("THistSvc", m_hist_svc).isFailure()) {
      ATH_MSG_ERROR("Cannot find THistSvc ");
      return StatusCode::FAILURE;
    }
    m_output_file = new TFile("HGTD_SmearedDigiOutput.root", "RECREATE");
    m_tree = new TTree("SmearedDigiTree", "SmearedDigiTree");
    m_tree->Branch("m_x_hit", &m_x_hit);
    m_tree->Branch("m_y_hit", &m_y_hit);
    m_tree->Branch("m_x_entry_hit", &m_x_entry_hit);
    m_tree->Branch("m_y_entry_hit", &m_y_entry_hit);
    m_tree->Branch("m_z_entry_hit", &m_z_entry_hit);
    m_tree->Branch("m_x_exit_hit", &m_x_exit_hit);
    m_tree->Branch("m_y_exit_hit", &m_y_exit_hit);
    m_tree->Branch("m_z_exit_hit", &m_z_exit_hit);
    m_tree->Branch("m_x_cluster_global", &m_x_cluster_global);
    m_tree->Branch("m_y_cluster_global", &m_y_cluster_global);
    m_tree->Branch("m_z_cluster_global", &m_z_cluster_global);
    m_tree->Branch("m_x_hit_smeared", &m_x_hit_smeared);
    m_tree->Branch("m_y_hit_smeared", &m_y_hit_smeared);
    m_tree->Branch("m_hit_time", &m_hit_time);
    m_tree->Branch("m_hit_time_smeared", &m_hit_time_smeared);
    m_tree->Branch("m_err_x_hit", &m_err_x_hit);
    m_tree->Branch("m_err_y_hit", &m_err_y_hit);

    if (m_hist_svc->regTree("HGTD_ClusterSmearedDigi", m_tree).isFailure()) {
      ATH_MSG_ERROR("Cannot register HGTD_ClusterSmearedDigi Tree");
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

// Finalize method:
StatusCode HGTD_SmearedDigitizationTool::finalize() {

  if (m_write_tree) {
    m_output_file->cd();
    m_tree->Write();
    m_output_file->Close();
  }

  ATH_MSG_DEBUG("HGTD_SmearedDigitizationTool : finalize()");

  return StatusCode::SUCCESS;
}

StatusCode HGTD_SmearedDigitizationTool::processAllSubEvents() {

  ATH_MSG_DEBUG("[HGTD_SmearedDigitizationTool::processAllSubEvents]");

  m_cluster_container =
      new ClusterContainer_t(m_hgtd_idhelper->wafer_hash_max());

  m_cluster_container->cleanup();

  if ((evtStore()->record(m_cluster_container, m_cluster_name)).isFailure()) {
    if ((evtStore()->retrieve(m_cluster_container, m_cluster_name))
            .isFailure()) {
      ATH_MSG_FATAL(
          "[ hitproc ] Error while registering HGTD_Cluster container");
      return StatusCode::FAILURE;
    }
  }

  if (retrieveTruth().isFailure()) {
    ATH_MSG_FATAL("retrieveTruth() failed!");
    return StatusCode::FAILURE;
  }

  auto timed_hit_collection = setupTimedHitCollection();
  m_timed_hit_collection =
      &timed_hit_collection; // avoids allocating dynamically

  if (digitize().isFailure()) {
    ATH_MSG_FATAL("digitize() failed!");
    return StatusCode::FAILURE;
  }

  if (fillClusterContainer().isFailure()) {
    ATH_MSG_FATAL("fillClusterContainer() failed!");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode HGTD_SmearedDigitizationTool::retrieveTruth() {

  m_prd_truth_coll = new PRD_MultiTruthCollection;

  if ((evtStore()->contains<PRD_MultiTruthCollection>(m_prd_truth_coll_name))) {
    if ((evtStore()->retrieve(m_prd_truth_coll, m_prd_truth_coll_name))
            .isFailure()) {
      ATH_MSG_FATAL("Could not retrieve collection " << m_prd_truth_coll_name);
      return StatusCode::FAILURE;
    }
  } else {
    if ((evtStore()->record(m_prd_truth_coll, m_prd_truth_coll_name))
            .isFailure()) {
      ATH_MSG_FATAL("Could not record collection " << m_prd_truth_coll_name);
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

TimedHitCollection<SiHit>
HGTD_SmearedDigitizationTool::setupTimedHitCollection() {
  // Define Hit Collection
  TimedHitCollection<SiHit> timed_hit_coll;

  //  get the container(s)
  typedef PileUpMergeSvc::TimedList<SiHitCollection>::type TimedHitCollList_t;

  // this is a list<pair<time_t, DataLink<SCTUncompressedHitCollection> > >
  TimedHitCollList_t hit_coll_list;
  unsigned int num_si_hits(0);
  if (!(m_merge_svc
            ->retrieveSubEvtsData(m_si_hit_collection_name, hit_coll_list,
                                  num_si_hits)
            .isSuccess()) and
      hit_coll_list.size() == 0) {
    ATH_MSG_ERROR("Could not fill TimedHitCollList_t");
    return timed_hit_coll;
  }

  ATH_MSG_DEBUG(hit_coll_list.size() << " SiHitCollections with key "
                                     << m_si_hit_collection_name << " found");

  timed_hit_coll.reserve(num_si_hits);

  // now merge all collections into one
  TimedHitCollList_t::iterator coll_iter(hit_coll_list.begin());
  TimedHitCollList_t::iterator coll_end(hit_coll_list.end());

  // loop on the hit collections
  while (coll_iter != coll_end) {

    const SiHitCollection* si_collection(coll_iter->second);
    timed_hit_coll.insert(coll_iter->first, si_collection);
    ATH_MSG_DEBUG("SiHitCollection found with " << si_collection->size()
                                                << " hits");
    ++coll_iter;
  }
  return timed_hit_coll;
}

StatusCode HGTD_SmearedDigitizationTool::digitize() {
  ATH_MSG_DEBUG("--- HGTD_SmearedDigitizationTool: in digitize() ---");

  m_det_element_rio_map = std::make_unique<HGTD_DetElement_RIO_Map_t>();

  // i, e are iterator and end of one detector element
  TimedHitCollection<SiHit>::const_iterator i, e;

  while (m_timed_hit_collection->nextDetectorElement(i, e) and i != e) {
    TimedHitPtr<SiHit> hit(*i++);

    // FIXME should I cut on the deposited energy??
    // n electrons = hit->energyLoss() / electron ionisation energy
    // n * e * gain  > 4fCs

    int endcap = hit->getBarrelEndcap();
    int layer = hit->getLayerDisk();
    int phi_module = hit->getPhiModule();
    int eta_module = hit->getEtaModule();

    ATH_MSG_DEBUG("HGTD DetectorElement --> endcap "
                  << endcap << ", layer_disk " << layer << ", phi_module "
                  << phi_module << ", eta_module " << eta_module);

    const InDetDD::SolidStateDetectorElementBase* curr_det_element =
        m_hgtd_det_manager->getDetectorElement(endcap, layer, phi_module,
                                               eta_module);

    if (!curr_det_element) {
      ATH_MSG_ERROR("could not get detector element for SolidStateDetector");
      continue;
    }

    IdentifierHash wafer_id =
        m_hgtd_idhelper->wafer_hash(curr_det_element->identify());

    HepGeom::Point3D<double> hit_loc_start_pos = hit->localStartPosition();
    HepGeom::Point3D<double> hit_loc_end_pos = hit->localEndPosition();

    ATH_MSG_DEBUG("hit meanTime=" << hit->meanTime());
    ATH_MSG_DEBUG("hit_loc_start_pos xEta="
                  << hit_loc_start_pos[SiHit::xEta]
                  << ", xPhi=" << hit_loc_start_pos[SiHit::xPhi]
                  << ", xDep=" << hit_loc_start_pos[SiHit::xDep]);

    double globa_entry_x = hit_loc_start_pos[SiHit::xPhi];
    double globa_entry_y = hit_loc_start_pos[SiHit::xEta];
    m_x_entry_hit = globa_entry_x;
    m_y_entry_hit = globa_entry_y;
    m_z_entry_hit = hit_loc_start_pos[SiHit::xDep];

    double globa_exit_x = hit_loc_end_pos[SiHit::xPhi];
    double globa_exit_y = hit_loc_end_pos[SiHit::xEta];
    m_x_exit_hit = globa_exit_x;
    m_y_exit_hit = globa_exit_y;
    m_z_exit_hit = hit_loc_end_pos[SiHit::xDep];

    double dist_x = std::abs(std::abs(globa_exit_x) - std::abs(globa_entry_x));
    double dist_y = std::abs(std::abs(globa_exit_y) - std::abs(globa_entry_y));

    const InDetDD::SiLocalPosition position_entry(
        curr_det_element->hitLocalToLocal(globa_entry_y, globa_entry_x));
    const InDetDD::SiLocalPosition position_exit(
        curr_det_element->hitLocalToLocal(globa_exit_y, globa_exit_x));

    InDetDD::SiCellId entry_cell_id =
        curr_det_element->cellIdOfPosition(position_entry);
    InDetDD::SiCellId exit_cell_id =
        curr_det_element->cellIdOfPosition(position_exit);

    ATH_MSG_DEBUG("--- HGTD_SmearedDigitizationTool: entryCellId "
                  << entry_cell_id << " --- exitCellId " << exit_cell_id);

    if (not entry_cell_id.isValid() or not exit_cell_id.isValid()) {
      continue;
    }

    // the intersecetion id and cellId of it. yes, this is correct
    double intersection_x = 0.5 * (globa_entry_x + globa_exit_x);
    double intersection_y = 0.5 * (globa_entry_y + globa_exit_y);
    m_x_hit = intersection_x;
    m_y_hit = intersection_y;

    // calculate how many pixels the particle crosses in x and y direction
    double times_x = floor(dist_x / m_pitch_x);
    double times_y = floor(dist_y / m_pitch_y);

    double sigma_x = m_pitch_x / m_sqrt12;
    double sigma_y = m_pitch_y / m_sqrt12;

    int element_x = times_x + 1;
    int element_y = times_y + 1;

    // Smear pixel in x and y direction
    // FIXME make sure the length width dimensions are correct!!!
    if (m_smear_intersection_position) {
      intersection_x = smearPosition(intersection_x, sigma_x,
                                     curr_det_element->width() / 2.);
      intersection_y = smearPosition(intersection_y, sigma_y,
                                     curr_det_element->length() / 2.);
    }
    m_x_hit_smeared = intersection_x;
    m_y_hit_smeared = intersection_y;

    Amg::Vector2D intersection(intersection_x, intersection_y);

    Identifier intersection_id =
        curr_det_element->identifierOfPosition(intersection);

    // the pixel positions and other elements for the geometrical clustering
    std::vector<Identifier> rdo_list = {intersection_id};

    InDetDD::SiCellId current_cell_id =
        curr_det_element->cellIdFromIdentifier(intersection_id);

    if (!current_cell_id.isValid()) {
      continue;
    }

    double length_x = element_x * m_pitch_x;
    double length_y = element_y * m_pitch_y;

    InDet::SiWidth si_width(Amg::Vector2D(element_x, element_y),
                            Amg::Vector2D(length_x, length_y));

    AmgSymMatrix(2) covariance;
    covariance.setIdentity();
    covariance(Trk::locX, Trk::locX) = sigma_x * sigma_x;
    covariance(Trk::locY, Trk::locY) = sigma_y * sigma_y;
    Amg::MatrixX* cluster_err = new Amg::MatrixX(covariance);

    float hit_time = hit->meanTime();
    m_hit_time = hit_time;
    if (m_smear_mean_time) {
      hit_time = smearMeanTime(hit_time, m_time_res);
    }
    m_hit_time_smeared = hit_time;

    std::vector<int> tot_vec = {0}; // dummy, not used in reco currently

    Cluster_t* cluster = new Cluster_t(intersection_id, intersection, rdo_list,
                                       si_width, curr_det_element, cluster_err,
                                       hit_time, m_time_res, tot_vec);

    m_x_cluster_global = (cluster->globalPosition()).x();
    m_y_cluster_global = (cluster->globalPosition()).y();
    m_z_cluster_global = (cluster->globalPosition()).z();
    m_err_x_hit = Amg::error(cluster->localCovariance(), Trk::locX);
    m_err_y_hit = Amg::error(cluster->localCovariance(), Trk::locY);

    if (m_write_tree) {
      m_tree->Fill();
    }

    m_det_element_rio_map->insert(
        std::pair<IdentifierHash, const Cluster_t*>(wafer_id, cluster));

    if (fillMultiTruthCollection(m_prd_truth_coll, cluster, hit).isFailure()) {
      ATH_MSG_FATAL("fillMultiTruthCollection() for pixel failed!");
      return StatusCode::FAILURE;
    }

  } // END while
  return StatusCode::SUCCESS;
}

float HGTD_SmearedDigitizationTool::smearPosition(float pos, float sig,
                                                  float boundary) {
  ATH_MSG_DEBUG("[HGTD_SmearedDigitizationTool::smearPosition] pos: "
                << pos << " sig: " << sig << " boundary: " << boundary);
  float smeared_pos = pos;
  float smear_para = 0.;
  if (sig != 0.) {
    do {
      smear_para = CLHEP::RandGauss::shoot(m_random_engine, 0., sig);

    } while (std::abs(smeared_pos + smear_para) > boundary);
    smeared_pos += smear_para;
  }
  return smeared_pos;
}

float HGTD_SmearedDigitizationTool::smearMeanTime(float time, float time_res) {
  return time + CLHEP::RandGauss::shoot(m_random_engine, 0., time_res);
}

StatusCode HGTD_SmearedDigitizationTool::fillMultiTruthCollection(
    PRD_MultiTruthCollection* map, Cluster_t* cluster, TimedHitPtr<SiHit> hit) {

  // FIXME is this a dummy or does this actually mean something>
  EBC_EVCOLL ev_coll = EBC_MAINEVCOLL; // enum from HepMcParticleLink.h, usually
                                       // contains the HS GenEvent

  const bool is_event_index_is_position = (hit.eventId() == 0);
  HepMcParticleLink trk_link(hit->trackNumber(), hit.eventId(), ev_coll,
                             is_event_index_is_position);

  ATH_MSG_DEBUG("Truth map filling with cluster "
                << *cluster << " and link = " << trk_link);

  if (trk_link.isValid()) {
    const int barcode(trk_link.barcode());
    if (barcode != 0 && barcode != m_crazy_bc) {
      map->insert(std::make_pair(cluster->identify(), trk_link));
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode HGTD_SmearedDigitizationTool::fillClusterContainer() {
  ATH_MSG_DEBUG(
      "--- HGTD_SmearedDigitizationTool: in fillClusterContainer() ---");
  using RIO_map_t = HGTD_DetElement_RIO_Map_t;

  RIO_map_t::iterator i = m_det_element_rio_map->begin();
  RIO_map_t::iterator e = m_det_element_rio_map->end();

  for (; i != e; i = m_det_element_rio_map->upper_bound(i->first)) {

    std::pair<RIO_map_t::iterator, RIO_map_t::iterator> range;
    range = m_det_element_rio_map->equal_range(i->first);

    RIO_map_t::iterator first_det_elem = range.first;

    IdentifierHash wafer_id = first_det_elem->first;

    const InDetDD::SolidStateDetectorElementBase* det_element =
        m_hgtd_det_manager->getDetectorElement(wafer_id);

    ClusterCollection_t* cluster_coll = new ClusterCollection_t(wafer_id);
    cluster_coll->setIdentifier(det_element->identify());

    for (RIO_map_t::iterator iter = range.first; iter != range.second; ++iter) {

      Cluster_t* cluster = const_cast<Cluster_t*>((*iter).second);
      cluster->setHashAndIndex(cluster_coll->identifyHash(),
                               cluster_coll->size());
      cluster_coll->push_back(cluster);
    }

    if (m_cluster_container->addCollection(cluster_coll, wafer_id)
            .isFailure()) {
      ATH_MSG_WARNING("Could not add collection to Identifyable container!");
    }
  } // end for

  m_det_element_rio_map->clear();
  return StatusCode::SUCCESS;
}
