/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 *
 * @file HGTD_Digitization/src/HGTD_DigitizationTool.cxx
 *
 * @author Tao Wang <tao.wang@cern.ch>
 * @author Alexander Leopold <alexander.leopold@cern.ch>
 *
 * @date August, 2021
 *
 * @brief
 */

#include "HGTD_Digitization/HGTD_DigitizationTool.h"

#include "HGTD_Identifier/HGTD_ID.h"
#include "HGTD_ReadoutGeometry/HGTD_DetectorManager.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "InDetSimEvent/SiHit.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "PileUpTools/PileUpTypeHelper.h"
#include "SiDigitization/IFrontEnd.h"
#include "SiDigitization/ISiChargedDiodesProcessorTool.h"
#include "SiDigitization/SiChargedDiodeCollection.h"

static constexpr unsigned int
    crazyParticleBarcode(std::numeric_limits<int32_t>::max());

HGTD_DigitizationTool::HGTD_DigitizationTool(const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent)
    : PileUpToolBase(type, name, parent),
      m_merge_svc("PileUpMergeSvc", name),
      m_id_helper(nullptr),
      m_det_mgr(nullptr),
      m_hgtd_rdo_container(nullptr),
      m_output_rdo_cont_name("HGTD_RDOs"),
      m_sdo_collection(nullptr),
      m_output_sdo_coll_name("HGTD_SDO_Map"),
      m_integrated_luminosity(0),
      m_smear_meantime(true),
      m_input_collection_name("HGTD_Hits"),
      m_timed_hit_coll(nullptr),
      m_active_time_window(10000),
      m_charge_threshold(625.),
      m_rndm_svc("AtRndmGenSvc", name),
      m_rndm_engine(nullptr),
      m_hgtd_surf_charge_gen("HGTD_SurfaceChargesGenerator", this),
      m_hgtd_front_end_tool("HGTD_FrontEndTool", this) {

  declareProperty("MergeSvc", m_merge_svc,
                  "Merge service used in Pixel & HGTD digitization");
  declareProperty("InputCollecionName", m_input_collection_name = "HGTD_Hits",
                  "Name of the collection that is digitized");
  declareProperty("OutoutRDOContName", m_output_rdo_cont_name = "HGTD_RDOs",
                  "Name of the retrieved RDO collection");
  declareProperty("ActiveTimeWindow", m_active_time_window = 10000,
                  "Hits within this time window are used for digitization");
  declareProperty("ChargeThreshold", m_charge_threshold = 625.,
                  "Minimum charge threshold in ASIC");
  declareProperty("RndmSvc", m_rndm_svc,
                  "Random Number Service used in HGTD & Pixel digitization");
  declareProperty("HGTD_SurfaceChargesGenerator", m_hgtd_surf_charge_gen,
                  "Choice of using a more detailed charge drift model");
  declareProperty("HGTD_FrontEndTool", m_hgtd_front_end_tool,
                  "Tool for pulse shape simulation");
  declareProperty("OutputSDOName", m_output_sdo_coll_name = "HGTD_SDO_Map",
                  "Output SDO container name");
  declareProperty("SmearMeanTime", m_smear_meantime);
  declareProperty("IntegratedLuminosity", m_integrated_luminosity = 0,
                  "Integraed Luminosity");
}

StatusCode HGTD_DigitizationTool::initialize() {

  ATH_CHECK(detStore()->retrieve(m_id_helper, "HGTD_ID"));

  ATH_CHECK(detStore()->retrieve(m_det_mgr, "HGTD"));

  ATH_CHECK(m_merge_svc.retrieve());

  ATH_CHECK(m_rndm_svc.retrieve());

  m_rndm_engine = m_rndm_svc->GetEngine("HGTD_Digitization");

  ATH_CHECK(m_hgtd_surf_charge_gen.retrieve());
  m_hgtd_surf_charge_gen->setIntegratedLuminosity(m_integrated_luminosity);
  m_hgtd_surf_charge_gen->createTimingResolutionTool();
  m_hgtd_surf_charge_gen->setSmearingTime(m_smear_meantime);

  ATH_CHECK(m_hgtd_front_end_tool.retrieve());
  m_hgtd_front_end_tool->setRandomEngine(m_rndm_engine);

  // add the front end to the set of tools used for diode processing
  m_diode_processor_tools.push_back(&(*m_hgtd_front_end_tool));

  return StatusCode::SUCCESS;
}

StatusCode HGTD_DigitizationTool::processAllSubEvents() {

  ATH_MSG_DEBUG("processAllSubEvents");

  if (prepareEvent(0).isFailure()) {
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Begin digitizeAllHits");

  if (retrieveHitCollection().isSuccess()) {
    ATH_CHECK(digitizeHitsPerDetectorElement());
  } else {
    ATH_MSG_DEBUG("retrieveHitCollection found no hits");
  }

  // FIXME: needs to be added once noise/selftriggering has to go in
  // ATH_CHECK(simulateNoisePerDetectorElement());

  ATH_MSG_DEBUG("processAllSubEvents successful!");
  return StatusCode::SUCCESS;
}

StatusCode
HGTD_DigitizationTool::processBunchXing(int bunch_xing,
                                        SubEventIterator sub_event_itr,
                                        SubEventIterator sub_event_itr_end) {

  ATH_MSG_DEBUG("HGTD_DigitizationTool::processBunchXing " << bunch_xing);

  SubEventIterator loc_sub_event_itr = sub_event_itr;

  for (; loc_sub_event_itr != sub_event_itr_end; loc_sub_event_itr++) {
    StoreGateSvc &store_gate_svc = *loc_sub_event_itr->ptr()->evtStore();
    ATH_MSG_DEBUG("SubEvt StoreGate "
                  << store_gate_svc.name() << " :"
                  << " bunch crossing : " << bunch_xing << " time offset : "
                  << loc_sub_event_itr->time() << " event number : "
                  << loc_sub_event_itr->ptr()->eventNumber()
                  << " run number : " << loc_sub_event_itr->ptr()->runNumber());

    const SiHitCollection *hit_coll(nullptr);

    if (store_gate_svc.retrieve(hit_coll, m_input_collection_name)
            .isFailure()) {
      ATH_MSG_ERROR("SubEvent HGTD SiHitCollection not found in StoreGate "
                    << store_gate_svc.name());
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("SiHitCollection found with " << hit_coll->size() << " hits");
    PileUpTimeEventIndex time_idx(loc_sub_event_itr->time(),
                                  loc_sub_event_itr->index(),
                                  pileupTypeMapper(loc_sub_event_itr->type()));

    // FIXME do I really need this?
    SiHitCollection *hit_coll_cpy = new SiHitCollection(*hit_coll);

    m_timed_hit_coll->insert(time_idx, hit_coll_cpy);

    m_hit_coll_ptrs.push_back(hit_coll_cpy);
  }

  return StatusCode::SUCCESS;
}

StatusCode HGTD_DigitizationTool::mergeEvent() {
  ATH_MSG_VERBOSE("HGTD_DigitizationTool::mergeEvent()");

  ATH_CHECK(digitizeHitsPerDetectorElement());

  // FIXME: needs to be added once noise/selftriggering has to go in
  // ATH_CHECK(simulateNoisePerDetectorElement());

  for (std::vector<SiHitCollection *>::iterator it = m_hit_coll_ptrs.begin();
       it != m_hit_coll_ptrs.end(); it++) {
    (*it)->Clear();
    delete (*it);
  }
  m_hit_coll_ptrs.clear();

  ATH_MSG_DEBUG("Digitize success!");
  return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

StatusCode HGTD_DigitizationTool::prepareEvent(unsigned int /*index*/) {
  ATH_MSG_DEBUG("HGTD_DigitizationTool::prepareEvent()");
  // Create the IdentifiableContainer to contain the digit collections Create
  // a new RDO container
  m_hgtd_rdo_container = new HGTD_RDOContainer(m_id_helper->wafer_hash_max());

  // register RDO container with storegate
  ATH_CHECK(evtStore()->record(m_hgtd_rdo_container, m_output_rdo_cont_name));

  // Create a map for the SDO and register it into StoreGate
  m_sdo_collection = new InDetSimDataCollection();
  ATH_CHECK(evtStore()->record(m_sdo_collection, m_output_sdo_coll_name));

  m_processed_elements.clear(); // FIXME not sure I need this vector??
  m_processed_elements.resize(m_id_helper->wafer_hash_max(), false);

  m_timed_hit_coll = std::make_unique<TimedHitCollection<SiHit>>();

  return StatusCode::SUCCESS;
}

StatusCode HGTD_DigitizationTool::retrieveHitCollection() {
  ATH_MSG_DEBUG("HGTD_DigitizationTool::retrieveHitCollection()");

  using TimedHitCollList_t = PileUpMergeSvc::TimedList<SiHitCollection>::type;

  TimedHitCollList_t timed_hit_coll_list;
  unsigned int n_si_hits(0);

  if (m_merge_svc
          ->retrieveSubEvtsData(m_input_collection_name, timed_hit_coll_list,
                                n_si_hits)
          .isFailure() and
      timed_hit_coll_list.size() == 0) { // FIXME is the size check relevant??
    ATH_MSG_ERROR("Could not fill TimedHitCollList_t");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG(" SiHitCollections with key "
                  << m_input_collection_name << " of size "
                  << timed_hit_coll_list.size() << " found");
  }

  // create a new hits collection
  m_timed_hit_coll = std::make_unique<TimedHitCollection<SiHit>>(n_si_hits);

  // now merge all collections into one
  TimedHitCollList_t::iterator coll_itr(timed_hit_coll_list.begin());
  TimedHitCollList_t::iterator coll_itr_end(timed_hit_coll_list.end());

  while (coll_itr != coll_itr_end) {

    // FIXME removed m_HardScatterSplittingMode and
    // m_HardScatterSplittingSkipper until we have confirmed we need this

    const SiHitCollection *collection(coll_itr->second);

    m_timed_hit_coll->insert(coll_itr->first, collection);

    ATH_MSG_DEBUG("SiTrackerHitCollection found with" << collection->size()
                                                      << " hits");
    ++coll_itr;
  }
  return StatusCode::SUCCESS;
}

StatusCode HGTD_DigitizationTool::digitizeHitsPerDetectorElement() {
  ATH_MSG_DEBUG("HGTD_DigitizationTool::digitizeHitsPerDetectorElement");

  // the iterators mark the start and end of si hits that belong to one
  // collection which means that they were found on the same detector element
  // (module)
  TimedHitCollection<SiHit>::const_iterator coll_itr;
  TimedHitCollection<SiHit>::const_iterator coll_itr_end;

  // the nextDetectorElement method sets the iterators to a range belonging to
  // one module, so we iterate over modules (detector elements) in the while
  // loop
  while (m_timed_hit_coll->nextDetectorElement(coll_itr, coll_itr_end)) {

    const TimedHitPtr<SiHit> &fist_hit = *coll_itr;

    Identifier id = m_id_helper->wafer_id(
        fist_hit->getBarrelEndcap(), fist_hit->getLayerDisk(),
        fist_hit->getPhiModule(), fist_hit->getEtaModule());

    // get the det element from the manager
    HGTD_DetectorElement *det_elem = m_det_mgr->getDetectorElement(id);
    // FIXME check for null??
    // create a diode collection holding the digitized hits
    // FIXME (init once outside the while loop and use clear and set det elem??)
    std::unique_ptr<SiChargedDiodeCollection> charged_diode_coll =
        std::make_unique<SiChargedDiodeCollection>(det_elem);

    ///////////////// LOOP TO FILL A COLLECTION /////////////////
    // Loop over the hits on the selected detector element and created charged
    // diodes
    for (; coll_itr != coll_itr_end; coll_itr++) {

      const TimedHitPtr<SiHit> &current_hit = *coll_itr;

      // skip hits that are far away in time
      if (std::abs(current_hit->meanTime()) >
          m_active_time_window * CLHEP::ns) {
        continue;
      }

      // use the surface charge generator to produce the charged diode
      // and add it to the charged diode collection
      m_hgtd_surf_charge_gen->createSurfaceChargesFromHit(
          current_hit, charged_diode_coll.get(), det_elem, m_rndm_engine);

    } // END LOOP over hits
    ////////////////////////////////////////////////////////////////
    // now that the charges have been built, apply all digitization tools
    applyProcessorTools(charged_diode_coll.get());
    // at this point, the RDOs and SDOs need to be created!!!
    std::unique_ptr<HGTD_RDOCollection> rdo_collection =
        createRDOCollection(charged_diode_coll.get());

    ATH_CHECK(storeRDOCollection(std::move(rdo_collection)));

    createAndStoreSDO(charged_diode_coll.get());

    ATH_MSG_DEBUG("charges filled for module " << id);
  } // END LOOP over detector elements

  return StatusCode::SUCCESS;
}

void HGTD_DigitizationTool::applyProcessorTools(
    SiChargedDiodeCollection *charged_diodes) {
  ATH_MSG_DEBUG("applyProcessorTools()");
  int processorNumber = 0;

  std::list<ISiChargedDiodesProcessorTool *>::iterator provessor_itr =
      m_diode_processor_tools.begin();
  for (; provessor_itr != m_diode_processor_tools.end(); ++provessor_itr) {
    (*provessor_itr)->process(*charged_diodes);

    processorNumber++;
    ATH_MSG_DEBUG("Applied processor # " << processorNumber);
  }
  return;
}

StatusCode HGTD_DigitizationTool::storeRDOCollection(
    std::unique_ptr<HGTD_RDOCollection> coll) {
  // Create the RDO collection

  if (m_hgtd_rdo_container
          ->addCollection(coll.release(), coll->identifierHash())
          .isFailure()) {
    ATH_MSG_FATAL("HGTD RDO collection could not be added to container!");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

std::unique_ptr<HGTD_RDOCollection> HGTD_DigitizationTool::createRDOCollection(
    SiChargedDiodeCollection* charged_diodes) {

  IdentifierHash idHash_de = charged_diodes->identifyHash();

  std::unique_ptr<HGTD_RDOCollection> rdo_collection =
      std::make_unique<HGTD_RDOCollection>(idHash_de);

  // need the DE identifier
  const Identifier id_de = charged_diodes->identify();
  rdo_collection->setIdentifier(id_de);

  SiChargedDiodeIterator i_chargedDiode = charged_diodes->begin();
  SiChargedDiodeIterator i_chargedDiode_end = charged_diodes->end();

  for (; i_chargedDiode != i_chargedDiode_end; ++i_chargedDiode) {

    // skip deposits below the charge threshold of ~2fC after amplif.
    // the charge here is the purely deposited charge, so factor ~20 less
    // FIXME: gain radiation dependent, compensated with higher bias voltage
    // more precise modelling in the future
    SiChargedDiode& diode = (*i_chargedDiode).second;
    if (diode.totalCharge().charge() < m_charge_threshold) {
      ATH_MSG_DEBUG("charge of " << diode.totalCharge().charge()
                                 << " does not pass threshold of "
                                 << m_charge_threshold);
      continue;
    }

    InDetDD::SiReadoutCellId readout_cell =
        (*i_chargedDiode).second.getReadoutCell();
    int eta_index = readout_cell.etaIndex();
    int phi_index = readout_cell.phiIndex();

    ATH_MSG_DEBUG("readout_cell ID: " << readout_cell << " eta:" << eta_index
                                      << " phi:" << phi_index);

    // FIXME the method takes ID, phi, eta
    // FIXME switching order here to fix upstream issue with orientations
    const Identifier id_readout =
        m_id_helper->pixel_id(charged_diodes->identify(), phi_index, eta_index);

    const SiTotalCharge& charge = diode.totalCharge();

    // this is the time of the main charge. For now this might be OK as long as
    // the toal deposit just gets transformed into "one charge", but will need a
    // change in the future!!
    float toa = charge.time();

    int dummy_tot = 256;
    int dummy_bcid = 0;
    int dummy_l1a = 0;
    int dummy_l1id = 0;

    std::unique_ptr<HGTD_RDO> p_rdo = std::make_unique<HGTD_RDO>(
        id_readout, toa, dummy_tot, dummy_bcid, dummy_l1a, dummy_l1id);

    rdo_collection->push_back(p_rdo.release());
  }
  return rdo_collection;
}

void HGTD_DigitizationTool::createAndStoreSDO(
    SiChargedDiodeCollection *charged_diodes) {
  using list_t = SiTotalCharge::list_t;
  std::vector<InDetSimData::Deposit> deposits;
  deposits.reserve(5); // no idea what a reasonable number for this would be
                       // with pileup

  // loop over the charged diodes
  SiChargedDiodeIterator i_chargedDiode = charged_diodes->begin();
  SiChargedDiodeIterator i_chargedDiode_end = charged_diodes->end();

  for (; i_chargedDiode != i_chargedDiode_end; ++i_chargedDiode) {
    deposits.clear();
    const list_t &charges =
        (*i_chargedDiode).second.totalCharge().chargeComposition();

    bool real_particle_hit = false;

    // loop over the list of elements inside the charged diode
    list_t::const_iterator charge_list_itr_end = charges.end();
    list_t::const_iterator charge_list_itr = charges.begin();

    for (; charge_list_itr != charge_list_itr_end; ++charge_list_itr) {

      const HepMcParticleLink &trkLink = charge_list_itr->particleLink();
      const int barcode = trkLink.barcode();

      if ((barcode == 0) or (barcode == crazyParticleBarcode)) {
        continue;
      }
      if (!real_particle_hit) {
        // Types of SiCharges expected from HGTD
        // Noise:                        barcode==0 &&
        // processType()==SiCharge::noise
        // Delta Rays:                   barcode==0 &&
        // processType()==SiCharge::track
        // Pile Up Tracks With No Truth: barcode!=0 &&
        // processType()==SiCharge::cut_track
        // Tracks With Truth:            barcode!=0 &&
        // processType()==SiCharge::track
        if (barcode != 0 && charge_list_itr->processType() == SiCharge::track) {
          real_particle_hit = true;
        }
        // real_particle_hit = trkLink.isValid();
      }

      // check if this track number has been already used.
      std::vector<InDetSimData::Deposit>::reverse_iterator theDeposit =
          deposits.rend(); // dummy value
      std::vector<InDetSimData::Deposit>::reverse_iterator depositsR_end =
          deposits.rend();
      std::vector<InDetSimData::Deposit>::reverse_iterator i_Deposit =
          deposits.rbegin();
      for (; i_Deposit != depositsR_end; ++i_Deposit) {
        if ((*i_Deposit).first == trkLink) {
          theDeposit = i_Deposit;
          break;
        }
      }

      // if the charge has already hit the Diode add it to the deposit
      if (theDeposit != depositsR_end) {
        (*theDeposit).second += charge_list_itr->time();
      } else { // create a new deposit
        InDetSimData::Deposit deposit(trkLink, charge_list_itr->charge());
        deposits.push_back(deposit);
      }
    } // END LOOP charges within diode

    // add the simdata object to the map if the deposit originated from a
    // particle to which the truth information was kept. Can be HS and PU.
    if (real_particle_hit) {

      InDetDD::SiReadoutCellId readout_cell =
          (*i_chargedDiode).second.getReadoutCell();
      int eta_index = readout_cell.etaIndex();
      int phi_index = readout_cell.phiIndex();
      const Identifier id_readout = m_id_helper->pixel_id(
          charged_diodes->identify(), phi_index, eta_index);

      m_sdo_collection->insert(std::make_pair(
          id_readout, InDetSimData(deposits, (*i_chargedDiode).second.flag())));
    }
  } // END LOOP charged diodes
}
