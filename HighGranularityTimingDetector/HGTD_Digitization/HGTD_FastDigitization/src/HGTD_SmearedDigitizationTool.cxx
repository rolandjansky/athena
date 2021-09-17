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

#include "AthenaKernel/RNGWrapper.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandomEngine.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "HGTD_Identifier/HGTD_ID.h"
#include "HGTD_ReadoutGeometry/HGTD_DetectorManager.h"
#include "InDetSimEvent/SiHit.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"

#include "TFile.h"
#include "TTree.h"

HGTD_SmearedDigitizationTool::HGTD_SmearedDigitizationTool(
  const std::string& type, const std::string& name, const IInterface* parent) :
  PileUpToolBase(type, name, parent) {
}

HGTD_SmearedDigitizationTool::~HGTD_SmearedDigitizationTool(){}

StatusCode HGTD_SmearedDigitizationTool::initialize() {

  ATH_MSG_DEBUG("HGTD_SmearedDigitizationTool::initialize()");

  // Get the HGTD Detector Manager
  ATH_CHECK(detStore()->retrieve(m_hgtd_det_manager, "HGTD"));

  // Get the HGTD ID helper
  ATH_CHECK(detStore()->retrieve(m_hgtd_idhelper, "HGTD_ID"));

  // locate the PileUpMergeSvc
  ATH_CHECK(m_merge_svc.retrieve());

  if (m_write_tree) {
    ATH_CHECK(service("THistSvc", m_hist_svc));
    m_output_file = std::make_unique<TFile>("HGTD_SmearedDigiOutput.root", "RECREATE");
    m_tree = std::make_unique<TTree>("SmearedDigiTree", "SmearedDigiTree");
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

    ATH_CHECK(m_hist_svc->regTree("HGTD_ClusterSmearedDigi", m_tree.get()));
  }

  return StatusCode::SUCCESS;
}

// Finalize method:
StatusCode HGTD_SmearedDigitizationTool::finalize() {

  ATH_MSG_DEBUG("HGTD_SmearedDigitizationTool::finalize()");

  if (m_write_tree) {
    m_output_file->cd();
    m_tree->Write();
    m_output_file->Close();
  }

  return StatusCode::SUCCESS;
}

StatusCode HGTD_SmearedDigitizationTool::processAllSubEvents(const EventContext& ctx) {

  ATH_MSG_DEBUG("[HGTD_SmearedDigitizationTool::processAllSubEvents]");

  m_cluster_container =
      new ClusterContainer_t(m_hgtd_idhelper->wafer_hash_max());

  m_cluster_container->cleanup();

  ATH_CHECK(evtStore()->record(m_cluster_container, m_cluster_name));

  ATH_CHECK(retrieveTruth());

  auto timed_hit_collection = setupTimedHitCollection();
  m_timed_hit_collection =
      &timed_hit_collection; // avoids allocating dynamically

  ATH_CHECK(digitize(ctx));

  // TODO: also needs an EventContext (is it the equivalent of createAndStoreRIOs in SiSmearedDigitizationTool)?
  ATH_CHECK(fillClusterContainer());

  return StatusCode::SUCCESS;
}

StatusCode HGTD_SmearedDigitizationTool::retrieveTruth() {

  m_prd_truth_coll = new PRD_MultiTruthCollection;

  if ((evtStore()->contains<PRD_MultiTruthCollection>(m_prd_truth_coll_name))) {
    ATH_CHECK((evtStore()->retrieve(m_prd_truth_coll, m_prd_truth_coll_name)));
  } else {
    ATH_CHECK(evtStore()->record(m_prd_truth_coll, m_prd_truth_coll_name));
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

  // loop on the hit collections
  for(auto hit_coll : hit_coll_list) {
    const SiHitCollection* si_collection(hit_coll.second);
    timed_hit_coll.insert(hit_coll.first, si_collection);
    ATH_MSG_DEBUG("SiHitCollection found with " << si_collection->size()
                                                << " hits");
  }
  return timed_hit_coll;
}

StatusCode HGTD_SmearedDigitizationTool::digitize(const EventContext& ctx) {
  ATH_MSG_DEBUG("--- HGTD_SmearedDigitizationTool: in digitize() ---");

  m_det_element_rio_map = std::make_unique<HGTD_DetElement_RIO_Map_t>();

  // Set the RNG to use for this event.
  ATHRNG::RNGWrapper* rngWrapper = m_rndm_svc->getEngine(this);
  rngWrapper->setSeed(name(), ctx);
  CLHEP::HepRandomEngine* rndmEngine = rngWrapper->getEngine(ctx);

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

    HepGeom::Point3D<double> hit_glob_start_pos =
        curr_det_element->hitLocalToLocal3D(hit_loc_start_pos);
    HepGeom::Point3D<double> hit_glob_end_pos =
        curr_det_element->hitLocalToLocal3D(hit_loc_end_pos);

    double globa_entry_x = hit_glob_start_pos.x();
    double globa_entry_y = hit_glob_start_pos.y();
    m_x_entry_hit = globa_entry_x;
    m_y_entry_hit = globa_entry_y;
    m_z_entry_hit = hit_glob_start_pos.z();

    double globa_exit_x = hit_glob_end_pos.x();
    double globa_exit_y = hit_glob_end_pos.y();
    m_x_exit_hit = globa_exit_x;
    m_y_exit_hit = globa_exit_y;
    m_z_exit_hit = hit_glob_end_pos.z();

    double dist_x = std::abs(std::abs(globa_exit_x) - std::abs(globa_entry_x));
    double dist_y = std::abs(std::abs(globa_exit_y) - std::abs(globa_entry_y));

    Amg::Vector2D local_entry(globa_entry_x, globa_entry_y);
    Amg::Vector2D local_exit(globa_exit_x, globa_exit_y);

    // get the identifier of the entry and the exit
    Identifier entry_id = curr_det_element->identifierOfPosition(local_entry);
    Identifier exit_id = curr_det_element->identifierOfPosition(local_exit);

    // now get the cellIds and check whether they're valid
    InDetDD::SiCellId entry_cell_id =
        curr_det_element->cellIdFromIdentifier(entry_id);
    InDetDD::SiCellId exit_cell_id =
        curr_det_element->cellIdFromIdentifier(exit_id);

    ATH_MSG_DEBUG("--- HGTD_SmearedDigitizationTool: entryId "
                  << entry_id << " --- exitId " << exit_id);
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

    double sigma_x = m_pitch_x / std::sqrt(12);
    double sigma_y = m_pitch_y / std::sqrt(12);

    int element_x = times_x + 1;
    int element_y = times_y + 1;

    // Smear pixel in x and y direction
    // FIXME make sure the length width dimensions are correct!!!
    if (m_smear_intersection_position) {
      intersection_x = smearPosition(intersection_x, sigma_x,
                                     curr_det_element->width() / 2.,
                                     rndmEngine);
      intersection_y = smearPosition(intersection_y, sigma_y,
                                     curr_det_element->length() / 2.,
                                     rndmEngine);
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
    Amg::MatrixX cluster_err = Amg::MatrixX(covariance);

    float hit_time = hit->meanTime();
    m_hit_time = hit_time;
    if (m_smear_mean_time) {
      hit_time = smearMeanTime(hit_time, m_time_res, rndmEngine);
    }
    m_hit_time_smeared = hit_time;

    std::vector<int> tot_vec = {0}; // dummy, not used in reco currently

    Cluster_t* cluster = new Cluster_t(intersection_id, intersection, std::move(rdo_list),
                                       si_width, curr_det_element, std::move(cluster_err),
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

    ATH_CHECK(fillMultiTruthCollection(m_prd_truth_coll, cluster, hit));

  } // END while
  return StatusCode::SUCCESS;
}

float HGTD_SmearedDigitizationTool::smearPosition(float pos, float sig,
                                                  float boundary, CLHEP::HepRandomEngine * rndmEngine) {
  ATH_MSG_DEBUG("[HGTD_SmearedDigitizationTool::smearPosition] pos: "
                << pos << " sig: " << sig << " boundary: " << boundary);
  float smeared_pos = pos;
  float smear_para = 0.;
  if (sig != 0.) {
    do {
      smear_para = CLHEP::RandGauss::shoot(rndmEngine, 0., sig);

    } while (std::abs(smeared_pos + smear_para) > boundary);
    smeared_pos += smear_para;
  }
  return smeared_pos;
}

float HGTD_SmearedDigitizationTool::smearMeanTime(float time, float time_res, CLHEP::HepRandomEngine * rndmEngine) {
  return time + CLHEP::RandGauss::shoot(rndmEngine, 0., time_res);
}

StatusCode HGTD_SmearedDigitizationTool::fillMultiTruthCollection(
    PRD_MultiTruthCollection* map, Cluster_t* cluster, TimedHitPtr<SiHit> hit) {

  // FIXME is this a dummy or does this actually mean something>
  EBC_EVCOLL ev_coll = EBC_MAINEVCOLL; // enum from HepMcParticleLink.h, usually
                                       // contains the HS GenEvent

  HepMcParticleLink::PositionFlag is_event_index_is_position = (hit.eventId() == 0)? HepMcParticleLink::IS_POSITION : HepMcParticleLink::IS_INDEX;
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
      ATH_MSG_WARNING("Could not add collection to Identifiable container!");
    }
  } // end for

  m_det_element_rio_map->clear();
  return StatusCode::SUCCESS;
}
