/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_DigitizationTool.h"

// Mother Package includes
#include "SiDigitization/SiHelper.h"
#include "SiDigitization/SiChargedDiodeCollection.h"

// EDM includes
#include "InDetRawData/SCT1_RawData.h"
#include "InDetRawData/SCT3_RawData.h"

// Hit class includes
#include "InDetSimEvent/SiHit.h"
#include "Identifier/Identifier.h"

// Det Descr includes
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SCT_ModuleSideDesign.h"

// Data Handle
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/ReadHandle.h"

// Random Number Generation
#include "AthenaKernel/RNGWrapper.h"
#include "CLHEP/Random/RandomEngine.h"

// C++ Standard Library
#include <limits>
#include <memory>
#include <sstream>

static constexpr unsigned int crazyParticleBarcode(std::numeric_limits<int32_t>::max());
// Barcodes at the HepMC level are int

using InDetDD::SiCellId;

SCT_DigitizationTool::SCT_DigitizationTool(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent) :
  base_class(type, name, parent),
  m_HardScatterSplittingSkipper{false},
  m_detID{nullptr},
  m_mergeSvc{"PileUpMergeSvc", name},
  m_thpcsi{nullptr},
  m_vetoThisBarcode{crazyParticleBarcode} {
    declareProperty("FixedTime", m_tfix = -999., "Fixed time for Cosmics run selection");
    declareProperty("CosmicsRun", m_cosmicsRun = false, "Cosmics run selection");
    declareProperty("EnableHits", m_enableHits = true, "Enable hits");
    declareProperty("OnlyHitElements", m_onlyHitElements = false, "Process only elements with hits");
    declareProperty("BarrelOnly", m_barrelonly = false, "Only Barrel layers");
    declareProperty("RandomDisabledCells", m_randomDisabledCells = false, "Use Random disabled cells, default no");
    declareProperty("CreateNoiseSDO", m_createNoiseSDO = false, "Set create noise SDO flag");
    declareProperty("WriteSCT1_RawData", m_WriteSCT1_RawData = false, "Write out SCT1_RawData rather than SCT3_RawData");
    declareProperty("InputObjectName", m_inputObjectName = "", "Input Object name");
    declareProperty("MergeSvc", m_mergeSvc, "Merge service used in Pixel & SCT digitization");
    declareProperty("HardScatterSplittingMode", m_HardScatterSplittingMode = 0, "Control pileup & signal splitting");
    declareProperty("ParticleBarcodeVeto", m_vetoThisBarcode = crazyParticleBarcode, "Barcode of particle to ignore");

    m_WriteSCT1_RawData.declareUpdateHandler(&SCT_DigitizationTool::SetupRdoOutputType, this);
  }

SCT_DigitizationTool::~SCT_DigitizationTool() {
  delete m_thpcsi;
  for (SiHitCollection* hit: m_hitCollPtrs) {
    hit->Clear();
    delete hit;
  }
  m_hitCollPtrs.clear();
}

// ----------------------------------------------------------------------
// Initialize method:
// ----------------------------------------------------------------------
StatusCode SCT_DigitizationTool::initialize() {
  ATH_MSG_DEBUG("SCT_DigitizationTool::initialize()");

  if (m_inputObjectName == "") {
    ATH_MSG_FATAL("Property InputObjectName not set !");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Input objects: '" << m_inputObjectName << "'");
  }

  // +++ Init the services
  ATH_CHECK(initServices());

  // +++ Get the Surface Charges Generator tool
  ATH_CHECK(initSurfaceChargesGeneratorTool());

  // +++ Get the Front End tool
  ATH_CHECK(initFrontEndTool());

  // +++ Initialise for disabled cells from the random disabled cells tool
  // +++ Default off, since disabled cells taken form configuration in
  // reconstruction stage
  if (m_randomDisabledCells) {
    ATH_CHECK(initDisabledCells());
    ATH_MSG_INFO("Use of Random disabled cells");
  } else {
    m_sct_RandomDisabledCellGenerator.disable();
  }
  
  // Initialize ReadHandleKey
  if (!m_hitsContainerKey.key().empty()) {
    ATH_MSG_INFO("Loading single input HITS");
  }
  ATH_CHECK(m_hitsContainerKey.initialize(!m_hitsContainerKey.key().empty()));

  // +++ Initialize WriteHandleKey
  ATH_CHECK(m_rdoContainerKey.initialize());
  ATH_CHECK(m_simDataCollMapKey.initialize());

  // Initialize ReadCondHandleKey
  ATH_CHECK(m_SCTDetEleCollKey.initialize());

  ATH_MSG_DEBUG("SiDigitizationTool::initialize() complete");

  return StatusCode::SUCCESS;
}

namespace {
  class SiDigitizationSurfaceChargeInserter
    : public ISiSurfaceChargesInserter
  {
  public:
    SiDigitizationSurfaceChargeInserter(const InDetDD::SiDetectorElement* sielement,
                                        SiChargedDiodeCollection* chargedDiodes)
      : m_sielement(sielement),
        m_chargedDiodes(chargedDiodes) {
    }

    void operator () (const SiSurfaceCharge& scharge) const;
  private:
    const InDetDD::SiDetectorElement* m_sielement;
    SiChargedDiodeCollection* m_chargedDiodes;
  };


  void SiDigitizationSurfaceChargeInserter::operator ()
    (const SiSurfaceCharge& scharge) const {
    // get the diode in which this charge is
    SiCellId diode{m_sielement->cellIdOfPosition(scharge.position())};

    if (diode.isValid()) {
      // add this charge to the collection (or merge in existing charged diode)
      m_chargedDiodes->add(diode, scharge.charge());
    }
  }
} // anonymous namespace

// ----------------------------------------------------------------------
// Initialise the surface charge generator Tool
// ----------------------------------------------------------------------
StatusCode SCT_DigitizationTool::initSurfaceChargesGeneratorTool() {
  ATH_CHECK(m_sct_SurfaceChargesGenerator.retrieve());

  if (m_cosmicsRun and m_tfix > -998) {
    m_sct_SurfaceChargesGenerator->setFixedTime(m_tfix);
    ATH_MSG_INFO("Use of FixedTime = " << m_tfix << " in cosmics");
  }

  ATH_MSG_DEBUG("Retrieved and initialised tool " << m_sct_SurfaceChargesGenerator);

  return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------
// Initialise the Front End electronics Tool
// ----------------------------------------------------------------------
StatusCode SCT_DigitizationTool::initFrontEndTool() {
  ATH_CHECK(m_sct_FrontEnd.retrieve());

  storeTool(&(*m_sct_FrontEnd));

  ATH_MSG_DEBUG("Retrieved and initialised tool " << m_sct_FrontEnd);
  return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------
// Initialize the different services
// ----------------------------------------------------------------------
StatusCode SCT_DigitizationTool::initServices() {
  // Get SCT ID helper for hash function and Store them using methods from the
  // SiDigitization.
  ATH_CHECK(detStore()->retrieve(m_detID, "SCT_ID"));

  ATH_CHECK(m_mergeSvc.retrieve());
  ATH_CHECK(m_rndmSvc.retrieve());

  return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------
// Initialize the disabled cells for cosmics or CTB cases
// ----------------------------------------------------------------------
StatusCode SCT_DigitizationTool::initDisabledCells() {
  // +++ Retrieve the SCT_RandomDisabledCellGenerator
  ATH_CHECK(m_sct_RandomDisabledCellGenerator.retrieve());

  storeTool(&(*m_sct_RandomDisabledCellGenerator));

  ATH_MSG_INFO("Retrieved the SCT_RandomDisabledCellGenerator tool:" << m_sct_RandomDisabledCellGenerator);
  return StatusCode::SUCCESS;
}

StatusCode SCT_DigitizationTool::processAllSubEvents() {
  if (prepareEvent(0).isFailure()) {
    return StatusCode::FAILURE;
  }
  // Set the RNG to use for this event.
  ATHRNG::RNGWrapper* rngWrapper = m_rndmSvc->getEngine(this);
  rngWrapper->setSeed( name(), Gaudi::Hive::currentContext() );
  CLHEP::HepRandomEngine *rndmEngine = *rngWrapper;

  ATH_MSG_VERBOSE("Begin digitizeAllHits");
  if (m_enableHits and (not getNextEvent().isFailure())) {
    digitizeAllHits(&m_rdoContainer, &m_simDataCollMap, &m_processedElements, m_thpcsi, rndmEngine);
  } else {
    ATH_MSG_DEBUG("no hits found in event!");
  }
  ATH_MSG_DEBUG("Digitized Elements with Hits");

  // loop over elements without hits
  if (not m_onlyHitElements) {
    digitizeNonHits(&m_rdoContainer, &m_simDataCollMap, &m_processedElements, rndmEngine);
    ATH_MSG_DEBUG("Digitized Elements without Hits");
  }

  delete m_thpcsi;
  m_thpcsi = nullptr;

  ATH_MSG_VERBOSE("Digitize success!");
  return StatusCode::SUCCESS;
}

// ======================================================================
// prepareEvent
// ======================================================================
StatusCode SCT_DigitizationTool::prepareEvent(unsigned int /*index*/) {
  ATH_MSG_VERBOSE("SCT_DigitizationTool::prepareEvent()");
  // Create the IdentifiableContainer to contain the digit collections Create
  // a new RDO container
  m_rdoContainer = SG::makeHandle(m_rdoContainerKey);
  ATH_CHECK(m_rdoContainer.record(std::make_unique<SCT_RDO_Container>(m_detID->wafer_hash_max())));

  // Create a map for the SDO and register it into StoreGate
  m_simDataCollMap = SG::makeHandle(m_simDataCollMapKey);
  ATH_CHECK(m_simDataCollMap.record(std::make_unique<InDetSimDataCollection>()));

  m_processedElements.clear();
  m_processedElements.resize(m_detID->wafer_hash_max(), false);

  m_thpcsi = new TimedHitCollection<SiHit>();
  m_HardScatterSplittingSkipper = false;
  return StatusCode::SUCCESS;
}

// =========================================================================
// mergeEvent
// =========================================================================
StatusCode SCT_DigitizationTool::mergeEvent() {
  ATH_MSG_VERBOSE("SCT_DigitizationTool::mergeEvent()");

  // Set the RNG to use for this event.
  ATHRNG::RNGWrapper* rngWrapper = m_rndmSvc->getEngine(this);
  rngWrapper->setSeed( name(), Gaudi::Hive::currentContext() );
  CLHEP::HepRandomEngine *rndmEngine = *rngWrapper;

  if (m_enableHits) {
    digitizeAllHits(&m_rdoContainer, &m_simDataCollMap, &m_processedElements, m_thpcsi, rndmEngine);
  }

  if (not m_onlyHitElements) {
    digitizeNonHits(&m_rdoContainer, &m_simDataCollMap, &m_processedElements, rndmEngine);
  }

  for (SiHitCollection* hit: m_hitCollPtrs) {
    hit->Clear();
    delete hit;
  }
  m_hitCollPtrs.clear();

  delete m_thpcsi;
  m_thpcsi = nullptr;

  ATH_MSG_DEBUG("Digitize success!");
  return StatusCode::SUCCESS;
}

void SCT_DigitizationTool::digitizeAllHits(SG::WriteHandle<SCT_RDO_Container>* rdoContainer, SG::WriteHandle<InDetSimDataCollection>* simDataCollMap, std::vector<bool>* processedElements, TimedHitCollection<SiHit>* thpcsi, CLHEP::HepRandomEngine * rndmEngine) const {
  /////////////////////////////////////////////////
  //
  // In order to process all element rather than just those with hits we
  // create a vector to keep track of which elements have been processed.
  // NB. an element is an sct module
  //
  /////////////////////////////////////////////////
  ATH_MSG_DEBUG("Digitizing hits");
  int hitcount{0}; // First, elements with hits.

  SiChargedDiodeCollection chargedDiodes;

  while (digitizeElement(&chargedDiodes, thpcsi, rndmEngine)) {
    ATH_MSG_DEBUG("Hit collection ID=" << m_detID->show_to_string(chargedDiodes.identify()));

    hitcount++;  // Hitcount will be a number in the hit collection minus
    // number of hits in missing mods

    ATH_MSG_DEBUG("in digitize elements with hits: ec - layer - eta - phi  "
                  << m_detID->barrel_ec(chargedDiodes.identify()) << " - "
                  << m_detID->layer_disk(chargedDiodes.identify()) << " - "
                  << m_detID->eta_module(chargedDiodes.identify()) << " - "
                  << m_detID->phi_module(chargedDiodes.identify()) << " - "
                  << " processing hit number " << hitcount);

    // Have a flag to check if the module is present or not
    // Generally assume it is:

    IdentifierHash idHash{chargedDiodes.identifyHash()};

    assert(idHash < processedElements->size());
    (*processedElements)[idHash] = true;

    // create and store RDO and SDO

    if (not chargedDiodes.empty()) {
      StatusCode sc{createAndStoreRDO(&chargedDiodes, rdoContainer)};
      if (sc.isSuccess()) { // error msg is given inside
        // createAndStoreRDO()
        addSDO(&chargedDiodes, simDataCollMap);
      }
    }

    chargedDiodes.clear();
  }
  ATH_MSG_DEBUG("hits processed");
  return;
}

// digitize elements without hits
void SCT_DigitizationTool::digitizeNonHits(SG::WriteHandle<SCT_RDO_Container>* rdoContainer, SG::WriteHandle<InDetSimDataCollection>* simDataCollMap, const std::vector<bool>* processedElements, CLHEP::HepRandomEngine * rndmEngine) const {
  // Get SCT_DetectorElementCollection
  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> sctDetEle(m_SCTDetEleCollKey);
  const InDetDD::SiDetectorElementCollection* elements{sctDetEle.retrieve()};
  if (elements==nullptr) {
    ATH_MSG_FATAL(m_SCTDetEleCollKey.fullKey() << " could not be retrieved");
    return;
  }

  ATH_MSG_DEBUG("processing elements without hits");
  SiChargedDiodeCollection chargedDiodes;

  for (unsigned int i{0}; i < processedElements->size(); i++) {
    if (not (*processedElements)[i]) {
      IdentifierHash idHash{i};
      if (not idHash.is_valid()) {
        ATH_MSG_ERROR("SCT Detector element id hash is invalid = " << i);
      }

      const InDetDD::SiDetectorElement* element{elements->getDetectorElement(idHash)};
      if (element) {
        ATH_MSG_DEBUG("In digitize of untouched elements: layer - phi - eta  "
                      << m_detID->layer_disk(element->identify()) << " - "
                      << m_detID->phi_module(element->identify()) << " - "
                      << m_detID->eta_module(element->identify()) << " - "
                      << "size: " << processedElements->size());

        chargedDiodes.setDetectorElement(element);
        ATH_MSG_DEBUG("calling applyProcessorTools() for NON hits");
        applyProcessorTools(&chargedDiodes, rndmEngine);

        // Create and store RDO and SDO
        // Don't create empty ones.
        if (not chargedDiodes.empty()) {
          StatusCode sc{createAndStoreRDO(&chargedDiodes, rdoContainer)};
          if (sc.isSuccess()) {// error msg is given inside
            // createAndStoreRDO()
            addSDO(&chargedDiodes, simDataCollMap);
          }
        }

        chargedDiodes.clear();
      }
    }
  }

  return;
}

bool SCT_DigitizationTool::digitizeElement(SiChargedDiodeCollection* chargedDiodes, TimedHitCollection<SiHit>*& thpcsi, CLHEP::HepRandomEngine * rndmEngine) const {
  if (nullptr == thpcsi) {
    ATH_MSG_ERROR("thpcsi should not be nullptr!");

    return false;
  }

  // get the iterator pairs for this DetEl

  TimedHitCollection<SiHit>::const_iterator i, e;
  if (thpcsi->nextDetectorElement(i, e) == false) { // no more hits
    return false;
  }

  // create the identifier for the collection:
  ATH_MSG_DEBUG("create ID for the hit collection");
  const TimedHitPtr<SiHit>& firstHit{*i};
  int barrel{firstHit->getBarrelEndcap()};
  Identifier id{m_detID->wafer_id(barrel,
                                  firstHit->getLayerDisk(),
                                  firstHit->getPhiModule(),
                                  firstHit->getEtaModule(),
                                  firstHit->getSide())};
  IdentifierHash waferHash{m_detID->wafer_hash(id)};

  // Get SCT_DetectorElementCollection
  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> sctDetEle(m_SCTDetEleCollKey);
  const InDetDD::SiDetectorElementCollection* elements(sctDetEle.retrieve());
  if (elements==nullptr) {
    ATH_MSG_FATAL(m_SCTDetEleCollKey.fullKey() << " could not be retrieved");
    return false;
  }

  // get the det element from the manager
  const InDetDD::SiDetectorElement* sielement{elements->getDetectorElement(waferHash)};

  if (sielement == nullptr) {
    ATH_MSG_DEBUG("Barrel=" << barrel << " layer=" << firstHit->getLayerDisk() << " Eta=" << firstHit->getEtaModule() << " Phi=" << firstHit->getPhiModule() << " Side=" << firstHit->getSide());
    ATH_MSG_ERROR("detector manager could not find element with id = " << id);
    return false;
  }
  // create the charged diodes collection
  chargedDiodes->setDetectorElement(sielement);

  // Loop over the hits and created charged diodes:
  while (i != e) {
    TimedHitPtr<SiHit> phit{*i++};

    // skip hits which are more than 10us away
    if (fabs(phit->meanTime()) < 10000. * CLHEP::ns) {
      ATH_MSG_DEBUG("HASH = " << m_detID->wafer_hash(m_detID->wafer_id(phit->getBarrelEndcap(),
                                                                       phit->getLayerDisk(),
                                                                       phit->getPhiModule(),
                                                                       phit->getEtaModule(),
                                                                       phit->getSide())));
      ATH_MSG_DEBUG("calling process() for all methods");
      m_sct_SurfaceChargesGenerator->process(sielement, phit, SiDigitizationSurfaceChargeInserter(sielement, chargedDiodes), rndmEngine);
      ATH_MSG_DEBUG("charges filled!");
    }
  }
  applyProcessorTools(chargedDiodes, rndmEngine); // !< Use of the new AlgTool surface
  // charges generator class
  return true;
}

// -----------------------------------------------------------------------------
// Applies processors to the current detector element for the current element:
// -----------------------------------------------------------------------------
void SCT_DigitizationTool::applyProcessorTools(SiChargedDiodeCollection* chargedDiodes, CLHEP::HepRandomEngine * rndmEngine) const {
  ATH_MSG_DEBUG("applyProcessorTools()");
  int processorNumber{0};

  for (ISiChargedDiodesProcessorTool* proc: m_diodeCollectionTools) {
    proc->process(*chargedDiodes, rndmEngine);

    processorNumber++;
    ATH_MSG_DEBUG("Applied processor # " << processorNumber);
  }
}

StatusCode SCT_DigitizationTool::processBunchXing(int bunchXing,
                                                  SubEventIterator bSubEvents,
                                                  SubEventIterator eSubEvents) {
    ATH_MSG_VERBOSE("SCT_DigitizationTool::processBunchXing() " << bunchXing);
    // decide if this event will be processed depending on
    // HardScatterSplittingMode & bunchXing
    if (m_HardScatterSplittingMode == 2 and (not m_HardScatterSplittingSkipper)) {
        m_HardScatterSplittingSkipper = true;
        return StatusCode::SUCCESS;
    }
    if (m_HardScatterSplittingMode == 1 and m_HardScatterSplittingSkipper) {
        return StatusCode::SUCCESS;
    }
    if (m_HardScatterSplittingMode == 1 and (not m_HardScatterSplittingSkipper)) {
        m_HardScatterSplittingSkipper = true;
    }

    typedef PileUpMergeSvc::TimedList<SiHitCollection>::type TimedHitCollList;
    TimedHitCollList hitCollList;

    if ((not (m_mergeSvc->retrieveSubSetEvtData(m_inputObjectName, hitCollList, bunchXing,
                                                bSubEvents, eSubEvents).isSuccess())) and
        hitCollList.size() == 0) {
      ATH_MSG_ERROR("Could not fill TimedHitCollList");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_VERBOSE(hitCollList.size() << " SiHitCollections with key " <<
		      m_inputObjectName << " found");
    }

    TimedHitCollList::iterator endColl{hitCollList.end()};
    for (TimedHitCollList::iterator iColl{hitCollList.begin()}; iColl != endColl; iColl++) {
      SiHitCollection *hitCollPtr{new SiHitCollection(*iColl->second)};
      PileUpTimeEventIndex timeIndex{iColl->first};
      ATH_MSG_DEBUG("SiHitCollection found with " << hitCollPtr->size() <<
                    " hits");
      ATH_MSG_VERBOSE("time index info. time: " << timeIndex.time()
		      << " index: " << timeIndex.index()
		      << " type: " << timeIndex.type());
      m_thpcsi->insert(timeIndex, hitCollPtr);
      m_hitCollPtrs.push_back(hitCollPtr);
    }

    return StatusCode::SUCCESS;

}

// =========================================================================
// property handlers
// =========================================================================
void SCT_DigitizationTool::SetupRdoOutputType(Property &) {
}

// Does nothing, but required by Gaudi

// ----------------------------------------------------------------------
// Digitisation of non hit elements
// ----------------------------------------------------------------------

class DigitizeNonHitElementsDebugPrinter
{
public:
  DigitizeNonHitElementsDebugPrinter(const SCT_ID* detID) :
    m_detID{detID}, m_msgNo{-1} {
    }

  std::string msg(const InDetDD::SiDetectorElement* element) {
    std::ostringstream ost;

    ost << "Digitized unprocessed elements: layer - phi - eta - side  "
        << m_detID->layer_disk(element->identify()) << " - "
        << m_detID->phi_module(element->identify()) << " - "
        << m_detID->eta_module(element->identify()) << " - "
        << m_detID->side(element->identify()) << " - "
        << " unprocessed hit number: " << ++m_msgNo << '\n';

    return ost.str();
  }

private:
  const SCT_ID* m_detID;
  int m_msgNo;
};

// ----------------------------------------------------------------------//
// createAndStoreRDO                                                     //
// ----------------------------------------------------------------------//
StatusCode SCT_DigitizationTool::createAndStoreRDO(SiChargedDiodeCollection* chDiodeCollection, SG::WriteHandle<SCT_RDO_Container>* rdoContainer) const {

  // Create the RDO collection
  SCT_RDO_Collection* RDOColl{createRDO(chDiodeCollection)};

  // Add it to storegate
  Identifier id_coll{RDOColl->identify()};
  int barrelec{m_detID->barrel_ec(id_coll)};

  if ((not m_barrelonly) or (std::abs(barrelec) <= 1)) {
    if ((*rdoContainer)->addCollection(RDOColl, RDOColl->identifyHash()).isFailure()) {
      ATH_MSG_FATAL("SCT RDO collection could not be added to container!");
      delete RDOColl;
      RDOColl = nullptr;
      return StatusCode::FAILURE;
    }
  } else {
    ATH_MSG_VERBOSE("Not saving SCT_RDO_Collection: " << m_detID->show_to_string(RDOColl->identify()) << " to container!");
    delete RDOColl;
    RDOColl = nullptr;
  }
  return StatusCode::SUCCESS;
} // SCT_Digitization::createAndStoreRDO()

// ----------------------------------------------------------------------
// createRDO
// ----------------------------------------------------------------------
SCT_RDO_Collection* SCT_DigitizationTool::createRDO(SiChargedDiodeCollection* collection) const {

  // create a new SCT RDO collection
  SCT_RDO_Collection* p_rdocoll{nullptr};

  // need the DE identifier
  const Identifier id_de{collection->identify()};
  IdentifierHash idHash_de{collection->identifyHash()};
  try {
    p_rdocoll = new SCT_RDO_Collection(idHash_de);
  } catch (const std::bad_alloc&) {
    ATH_MSG_FATAL("Could not create a new SCT_RDORawDataCollection !");
  }
  p_rdocoll->setIdentifier(id_de);

  SiChargedDiodeIterator i_chargedDiode{collection->begin()};
  SiChargedDiodeIterator i_chargedDiode_end{collection->end()};
  // Choice of producing SCT1_RawData or SCT3_RawData
  if (m_WriteSCT1_RawData.value()) {
    for (; i_chargedDiode != i_chargedDiode_end; ++i_chargedDiode) {
      unsigned int flagmask{static_cast<unsigned int>((*i_chargedDiode).second.flag() & 0xFE)};

      if (!flagmask) { // now check it wasn't masked:
        // create new SCT RDO, using method 1 for mask:
        // GroupSize=1: need readout id, make use of
        // SiTrackerDetDescr
        InDetDD::SiReadoutCellId roCell{(*i_chargedDiode).second.getReadoutCell()};
        int strip{roCell.strip()};
        if (strip > 0xffff) { // In upgrade layouts strip can be bigger
          // than 4000
          ATH_MSG_FATAL("Strip number too big for SCT1 raw data format.");
        }
        const Identifier id_readout{m_detID->strip_id(collection->identify(), strip)};

        // build word, masks taken from SiTrackerEvent/SCTRawData.cxx
        const unsigned int strip_rdo{static_cast<unsigned int>((strip & 0xFFFF) << 16)};

        // user can define what GroupSize is, here 1: TC. Incorrect,
        // GroupSize >= 1
        int size{SiHelper::GetStripNum((*i_chargedDiode).second)};
        unsigned int size_rdo{static_cast<unsigned int>(size & 0xFFFF)};

        // TC. Need to check if there are disabled strips in the cluster
        int cluscounter{0};
        if (size > 1) {
          SiChargedDiodeIterator it2{i_chargedDiode};
          ++it2;
          for (; it2 != i_chargedDiode_end; ++it2) {
            ++cluscounter;
            if (cluscounter >= size) {
              break;
            }
            if (it2->second.flag() & 0xDE) {
              int tmp{cluscounter};
              while ((it2 != i_chargedDiode_end) and (cluscounter < size - 1) and (it2->second.flag() & 0xDE)) {
                it2++;
                cluscounter++;
              }
              if ((it2 != collection->end()) and !(it2->second.flag() & 0xDE)) {
                SiHelper::ClusterUsed(it2->second, false);
                SiHelper::SetStripNum(it2->second, size - cluscounter, &msg());
              }
              // groupSize=tmp;
              size_rdo = tmp & 0xFFFF;
              break;
            }
          }
        }
        unsigned int SCT_Word{strip_rdo | size_rdo};
        SCT1_RawData* p_rdo{new SCT1_RawData(id_readout, SCT_Word)};
        if (p_rdo) {
          p_rdocoll->push_back(p_rdo);
        }
      }
    }
  } else {
    // Under the current scheme time bin and ERRORS are hard-coded to
    // default values.
    int ERRORS{0};
    static std::vector<int> dummyvector;
    for (; i_chargedDiode != i_chargedDiode_end; ++i_chargedDiode) {
      unsigned int flagmask{static_cast<unsigned int>((*i_chargedDiode).second.flag() & 0xFE)};

      if (!flagmask) { // Check it wasn't masked
        int tbin{SiHelper::GetTimeBin((*i_chargedDiode).second)};
        // create new SCT RDO
        InDetDD::SiReadoutCellId roCell{(*i_chargedDiode).second.getReadoutCell()};
        int strip{roCell.strip()};
        const InDetDD::SiDetectorDesign& detDesign{collection->design()};
        const InDetDD::SCT_ModuleSideDesign& sctDesign{dynamic_cast<const InDetDD::SCT_ModuleSideDesign&>(detDesign)};
        int row2D{sctDesign.row(strip)};
        Identifier id_readout;
        if (row2D < 0) { // SCT sensors
          id_readout = m_detID->strip_id(collection->identify(), strip);
        } else { // Upgrade sensors
          int strip2D{sctDesign.strip(strip)};
          id_readout = m_detID->strip_id(collection->identify(), row2D, strip2D);
        }
                
        // build word (compatible with
        // SCT_RawDataByteStreamCnv/src/SCT_RodDecoder.cxx)
        int size{SiHelper::GetStripNum((*i_chargedDiode).second)};
        int groupSize{size};

        // TC. Need to check if there are disabled strips in the cluster
        int cluscounter{0};
        if (size > 1) {
          SiChargedDiode* diode{i_chargedDiode->second.nextInCluster()};
          while (diode) {//check if there is a further strip in the cluster
            ++cluscounter;
            if (cluscounter >= size) {
              ATH_MSG_WARNING("Cluster size reached while neighbouring strips still defined.");
              break;
            }
            if (diode->flag() & 0xDE) {//see if it is disabled/below threshold/disconnected/etc (0xDE corresponds to BT_SET | DISABLED_SET | BADTOT_SET | DISCONNECTED_SET | MASKOFF_SET)
              int tmp{cluscounter};
              while ((cluscounter < size - 1) and (diode->flag() & 0xDE)) { //check its not the end and still disabled
                diode = diode->nextInCluster();
                cluscounter++;
              }
              if (diode and !(diode->flag() & 0xDE)) {
                SiHelper::ClusterUsed(*diode, false);
                SiHelper::SetStripNum(*diode, size - cluscounter, &msg());
              }
              groupSize = tmp;
              break;
            }
            diode = diode->nextInCluster();
          }
        }

        int stripIn11bits{strip & 0x7ff};
        if (stripIn11bits != strip) {
          ATH_MSG_DEBUG("Strip number " << strip << " doesn't fit into 11 bits - will be truncated");
        }
                
        unsigned int SCT_Word{static_cast<unsigned int>(groupSize | (stripIn11bits << 11) | (tbin << 22) | (ERRORS << 25))};
        SCT3_RawData *p_rdo{new SCT3_RawData(id_readout, SCT_Word, &dummyvector)};
        if (p_rdo) {
          p_rdocoll->push_back(p_rdo);
        }
      }
    }
  }
  return p_rdocoll;
} // SCT_Digitization::createRDO()

// ------------------------------------------------------------
// Get next event and extract collection of hit collections:
// ------------------------------------------------------------
StatusCode SCT_DigitizationTool::getNextEvent() {
  ATH_MSG_DEBUG("SCT_DigitizationTool::getNextEvent()");
  //  get the container(s)
  typedef PileUpMergeSvc::TimedList<SiHitCollection>::type TimedHitCollList;
  // this is a list<pair<time_t, DataLink<SiHitCollection> >

  // In case of single hits container just load the collection using read handles
  if (!m_hitsContainerKey.key().empty()) {
    SG::ReadHandle<SiHitCollection> hitCollection(m_hitsContainerKey);
    if (!hitCollection.isValid()) {
      ATH_MSG_ERROR("Could not get SCT SiHitCollection container " << hitCollection.name() << " from store " << hitCollection.store());
      return StatusCode::FAILURE;
    }

    // create a new hits collection
    m_thpcsi = new TimedHitCollection<SiHit>{1};
    m_thpcsi->insert(0, hitCollection.cptr());
    ATH_MSG_DEBUG("SiHitCollection found with " << hitCollection->size() << " hits");

    return StatusCode::SUCCESS;
  }

  TimedHitCollList hitCollList;
  unsigned int numberOfSiHits{0};
  if (not (m_mergeSvc->retrieveSubEvtsData(m_inputObjectName, hitCollList, numberOfSiHits).isSuccess()) and hitCollList.size() == 0) {
    ATH_MSG_ERROR("Could not fill TimedHitCollList");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG(hitCollList.size() << " SiHitCollections with key " << m_inputObjectName << " found");
  }
  // create a new hits collection
  m_thpcsi = new TimedHitCollection<SiHit>{numberOfSiHits};
  // now merge all collections into one
  TimedHitCollList::iterator endColl{hitCollList.end()};
  for (TimedHitCollList::iterator iColl{hitCollList.begin()}; iColl != endColl; ++iColl) {
    // decide if this event will be processed depending on
    // HardScatterSplittingMode & bunchXing
    if (m_HardScatterSplittingMode == 2 and (not m_HardScatterSplittingSkipper)) {
      m_HardScatterSplittingSkipper = true;
      continue;
    }
    if (m_HardScatterSplittingMode == 1 and m_HardScatterSplittingSkipper) {
      continue;
    }
    if (m_HardScatterSplittingMode == 1 and (not m_HardScatterSplittingSkipper)) {
      m_HardScatterSplittingSkipper = true;
    }
    const SiHitCollection* p_collection{iColl->second};
    m_thpcsi->insert(iColl->first, p_collection);
    ATH_MSG_DEBUG("SiTrackerHitCollection found with " << p_collection->size() << " hits"); // loop on the hit collections
  }
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------------------------
// Convert a SiTotalCharge to a InDetSimData, and store it.
// -----------------------------------------------------------------------------------------------
void SCT_DigitizationTool::addSDO(SiChargedDiodeCollection* collection, SG::WriteHandle<InDetSimDataCollection>* simDataCollMap) const {
  typedef SiTotalCharge::list_t list_t;
  std::vector<InDetSimData::Deposit> deposits;
  deposits.reserve(5); // no idea what a reasonable number for this would be
  // with pileup
  // loop over the charged diodes
  SiChargedDiodeIterator EndOfDiodeCollection{collection->end()};
  for (SiChargedDiodeIterator i_chargedDiode{collection->begin()}; i_chargedDiode != EndOfDiodeCollection; ++i_chargedDiode) {
    deposits.clear();
    const list_t& charges{(*i_chargedDiode).second.totalCharge().chargeComposition()};

    bool real_particle_hit{false};
    // loop over the list
    list_t::const_iterator EndOfChargeList{charges.end()};
    for (list_t::const_iterator i_ListOfCharges{charges.begin()}; i_ListOfCharges != EndOfChargeList; ++i_ListOfCharges) {
      const HepMcParticleLink& trkLink{i_ListOfCharges->particleLink()};
      const int barcode{trkLink.barcode()};
      if ((barcode == 0) or (barcode == m_vetoThisBarcode)) {
        continue;
      }
      if (not real_particle_hit) {
        // Types of SiCharges expected from SCT
        // Noise:                        barcode==0 and
        // processType()==SiCharge::noise
        // Delta Rays:                   barcode==0 and
        // processType()==SiCharge::track
        // Pile Up Tracks With No Truth: barcode!=0 and
        // processType()==SiCharge::cut_track
        // Tracks With Truth:            barcode!=0 and
        // processType()==SiCharge::track
        if (barcode != 0 and i_ListOfCharges->processType() == SiCharge::track) {
          real_particle_hit = true;
        }
      }
      // check if this track number has been already used.
      std::vector<InDetSimData::Deposit>::reverse_iterator theDeposit{deposits.rend()};  // dummy value
      std::vector<InDetSimData::Deposit>::reverse_iterator depositsR_end{deposits.rend()};
      std::vector<InDetSimData::Deposit>::reverse_iterator i_Deposit{deposits.rbegin()};
      for (; i_Deposit != depositsR_end; ++i_Deposit) {
        if ((*i_Deposit).first == trkLink) {
          theDeposit = i_Deposit;
          break;
        }
      }

      // if the charge has already hit the Diode add it to the deposit
      if (theDeposit != depositsR_end) {
        (*theDeposit).second += i_ListOfCharges->charge();
      } else { // create a new deposit
        InDetSimData::Deposit deposit(trkLink, i_ListOfCharges->charge());
        deposits.push_back(deposit);
      }
    }

    // add the simdata object to the map:
    if (real_particle_hit or m_createNoiseSDO) {
      InDetDD::SiReadoutCellId roCell{(*i_chargedDiode).second.getReadoutCell()};
      int strip{roCell.strip()};
      const InDetDD::SiDetectorDesign& detDesign{collection->design()};
      const InDetDD::SCT_ModuleSideDesign& sctDesign{dynamic_cast<const InDetDD::SCT_ModuleSideDesign&>(detDesign)};

      int row2D{sctDesign.row(strip)};
      Identifier id_readout;
      if (row2D < 0) { // SCT sensors
        id_readout = m_detID->strip_id(collection->identify(),strip);
      } else { // Upgrade sensors
        int strip2D{sctDesign.strip(strip)};
        id_readout = m_detID->strip_id(collection->identify(),row2D, strip2D);
      }

      (*simDataCollMap)->insert(std::make_pair(id_readout, InDetSimData(deposits, (*i_chargedDiode).second.flag())));
    }
  }
}
