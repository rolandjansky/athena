/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////
// PixelDigitizationTool.cxx
//   Implementation file for class PixelDigitizationTool
////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
////////////////////////////////////////////////////////////////////////////
#include "PixelDigitizationTool.h"

#include "PileUpTools/PileUpMergeSvc.h"

#include "SiDigitization/SiChargedDiodeCollection.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"

// Random Number Generation
#include "AthenaKernel/RNGWrapper.h"
#include "CLHEP/Random/RandomEngine.h"

#include <limits>
#include <cstdint>
static constexpr unsigned int crazyParticleBarcode(std::numeric_limits<int32_t>::max());
//Barcodes at the HepMC level are int

PixelDigitizationTool::PixelDigitizationTool(const std::string &type,
                                             const std::string &name,
                                             const IInterface * pIID) :
  PileUpToolBase(type,name,pIID),
  m_rdoContainerKey("PixelRDOs"),
  m_simDataCollKey("PixelSDO_Map"),
  m_HardScatterSplittingMode(0),
  m_HardScatterSplittingSkipper(false),
  m_onlyHitElements(false),
  m_chargeTool(nullptr),
  m_fesimTool(nullptr),
  m_energyDepositionTool(nullptr),
  m_detID(nullptr),
  m_vetoThisBarcode(crazyParticleBarcode),
  m_timedHits(nullptr),
  m_mergeSvc("PileUpMergeSvc",name),
  m_detManager(nullptr),
  m_inputObjectName(""),
  m_createNoiseSDO(false)
{
  declareProperty("ChargeTools",      m_chargeTool,      "List of charge tools");
  declareProperty("FrontEndSimTools", m_fesimTool,       "List of Front-End simulation tools");
  declareProperty("EnergyDepositionTool",   m_energyDepositionTool,       "Energy deposition tool");
  declareProperty("MergeSvc",         m_mergeSvc,        "Merge service used in Pixel digitization");
  declareProperty("InputObjectName",  m_inputObjectName, "Input Object name" );
  declareProperty("CreateNoiseSDO",   m_createNoiseSDO,  "Set create noise SDO flag");
  declareProperty("RDOCollName",      m_rdoContainerKey, "RDO collection name");
  declareProperty("SDOCollName",      m_simDataCollKey,  "SDO collection name");
  declareProperty("OnlyHitElements",  m_onlyHitElements, "Process only elements with hits");
  declareProperty("HardScatterSplittingMode", m_HardScatterSplittingMode, "Control pileup & signal splitting" );
  declareProperty("ParticleBarcodeVeto",m_vetoThisBarcode=crazyParticleBarcode, "Barcode of particle to ignore");
}

//=======================================
// I N I T I A L I Z E
//=======================================
StatusCode PixelDigitizationTool::initialize() {
  ATH_MSG_DEBUG("PixelDigitizationTool::Initialize()");

  // check the input object name
  if (m_inputObjectName=="") {
    ATH_MSG_FATAL("Property InputObjectName not set !");
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG("Input objects: '" << m_inputObjectName << "'");
  }

  // Initialize services
  CHECK(m_mergeSvc.retrieve());

  // Initialize random number generator
  ATH_CHECK(m_rndmSvc.retrieve());

  // Initialize detector manager
  CHECK(detStore()->retrieve(m_detManager,"Pixel"));

  CHECK(detStore()->retrieve(m_detID,"PixelID"));
  ATH_MSG_DEBUG("Pixel ID helper retrieved");

  // Initialize tools
  CHECK(m_chargeTool.retrieve());

  CHECK(m_fesimTool.retrieve());
  
  CHECK(m_energyDepositionTool.retrieve());

  // Initialize WriteHandleKey
  ATH_CHECK(m_rdoContainerKey.initialize());
  ATH_CHECK(m_simDataCollKey.initialize());

  return StatusCode::SUCCESS;
}

//=======================================
// F I N A L I Z E
//=======================================
StatusCode PixelDigitizationTool::finalize() {
  return StatusCode::SUCCESS;
}

//=======================================
// P R O C E S S   S U B E V E N T S
//=======================================
StatusCode PixelDigitizationTool::processAllSubEvents() {

  // Prepare event
  ATH_MSG_DEBUG("Prepare event");
  CHECK(prepareEvent(0));

  // Get the container(s)
  typedef PileUpMergeSvc::TimedList<SiHitCollection>::type TimedHitCollList;
  TimedHitCollList hitCollList;
  unsigned int numberOfSiHits(0);
  CHECK(m_mergeSvc->retrieveSubEvtsData(m_inputObjectName,hitCollList,numberOfSiHits));
  m_timedHits->reserve(numberOfSiHits);
  // Now merge all collections into one
  for (TimedHitCollList::iterator iColl=hitCollList.begin(); iColl!=hitCollList.end(); iColl++) {
    // Decide if this event will be processed depending on HardScatterSplittingMode
    if (m_HardScatterSplittingMode==2 && !m_HardScatterSplittingSkipper) { m_HardScatterSplittingSkipper=true; continue; }
    if (m_HardScatterSplittingMode==1 &&  m_HardScatterSplittingSkipper) { continue; }
    if (m_HardScatterSplittingMode==1 && !m_HardScatterSplittingSkipper) { m_HardScatterSplittingSkipper=true; }
    const SiHitCollection* p_collection(iColl->second);
    m_timedHits->insert(iColl->first, p_collection);
    ATH_MSG_DEBUG("SiTrackerHitCollection found with"<<p_collection->size()<<" hits");    // loop on the hit collections
  }

  // Digitize hits
  CHECK(digitizeEvent());

  ATH_MSG_DEBUG("Digitize success!");
  return StatusCode::SUCCESS;
}

//=======================================
// D I G I T I Z E   E V E N T (main)
//=======================================
StatusCode PixelDigitizationTool::digitizeEvent() {
  ATH_MSG_VERBOSE("PixelDigitizationTool::digitizeEvent()");

  std::unique_ptr<SiChargedDiodeCollection> chargedDiodes =  std::make_unique<SiChargedDiodeCollection>();
  std::vector<std::pair<double,double> > trfHitRecord; trfHitRecord.clear(); 
  std::vector<double> initialConditions; initialConditions.clear();

  std::vector<bool> processedElements;
  processedElements.resize(m_detID->wafer_hash_max(),false);

  // Set the RNG to use for this event.
  ATHRNG::RNGWrapper* rngWrapper = m_rndmSvc->getEngine(this);
  rngWrapper->setSeed( name(), Gaudi::Hive::currentContext() );
  CLHEP::HepRandomEngine *rndmEngine = *rngWrapper;

  TimedHitCollection<SiHit>::const_iterator firstHit, lastHit;
  
  ////////////////////////////////////////////////
  // **** Loop over the Detectors with hits ****
  ////////////////////////////////////////////////
  while (m_timedHits->nextDetectorElement(firstHit,lastHit)) {

    // Create the identifier for the collection
    ATH_MSG_DEBUG("create ID for the hit collection");
    const PixelID* PID = static_cast<const PixelID*>(m_detID);
    Identifier id = PID->wafer_id((*firstHit)->getBarrelEndcap(),(*firstHit)->getLayerDisk(),(*firstHit)->getPhiModule(),(*firstHit)->getEtaModule());

    // Get the det element from the manager
    InDetDD::SiDetectorElement* sielement = m_detManager->getDetectorElement(id);

    if (sielement==0) {
      ATH_MSG_DEBUG(" Barrel=" << (*firstHit)->getBarrelEndcap() << " Layer="  << (*firstHit)->getLayerDisk() << " Eta="    << (*firstHit)->getEtaModule() << " Phi="    << (*firstHit)->getPhiModule());
      ATH_MSG_ERROR("detector manager could not find element with id = " << id);
      break;
    }

    // Create the charged diodes collection
    chargedDiodes->setDetectorElement(sielement);
    const InDetDD::PixelModuleDesign *p_design= static_cast<const InDetDD::PixelModuleDesign*>(&(sielement->design()));

    ///////////////////////////////////////////////////////////
    // **** Loop over the hits and created charged diodes ****
    ///////////////////////////////////////////////////////////
    for (TimedHitCollection<SiHit>::const_iterator phit=firstHit; phit!=lastHit; phit++) {
      //skip hits which are more than 10us away
      if (fabs((*phit)->meanTime())<10000.0*CLHEP::ns) {
        ATH_MSG_DEBUG("HASH = " << m_detID->wafer_hash(m_detID->wafer_id((*phit)->getBarrelEndcap(),(*phit)->getLayerDisk(),(*phit)->getPhiModule(),(*phit)->getEtaModule())));

        // Apply charge collection tools
        ATH_MSG_DEBUG("Running sensor simulation.");

        //Deposit energy in sensor
        CHECK(m_energyDepositionTool->depositEnergy( *phit,  *sielement, trfHitRecord, initialConditions, rndmEngine));

        //Create signal in sensor, loop over collection of loaded sensorTools
        for (unsigned int itool=0; itool<m_chargeTool.size(); itool++) {
          ATH_MSG_DEBUG("Executing tool " << m_chargeTool[itool]->name());
          if (m_chargeTool[itool]->induceCharge( *phit, *chargedDiodes, *sielement, *p_design, trfHitRecord, initialConditions, rndmEngine)==StatusCode::FAILURE) { break; }
        }
        initialConditions.clear();
        trfHitRecord.clear();
        ATH_MSG_DEBUG("charges filled!");
      }
    }

    ATH_MSG_DEBUG("Hit collection ID=" << m_detID->show_to_string(chargedDiodes->identify()));
    ATH_MSG_DEBUG("in digitize elements with hits: ec - layer - eta - phi  " << m_detID->barrel_ec(chargedDiodes->identify()) << " - " << m_detID->layer_disk(chargedDiodes->identify()) << " - " << m_detID->eta_module(chargedDiodes->identify()) << " - " << m_detID->phi_module(chargedDiodes->identify()));

    IdentifierHash idHash = chargedDiodes->identifyHash();

    assert(idHash<processedElements.size());
    processedElements[idHash] = true;

    ///////////////////////////////////////////////////////////
    // ***       Create and store RDO and SDO   ****
    ///////////////////////////////////////////////////////////
    PixelRDO_Collection *RDOColl = new PixelRDO_Collection(chargedDiodes->identifyHash());
    RDOColl->setIdentifier(chargedDiodes->identify());
    for (unsigned int itool=0; itool<m_fesimTool.size(); itool++) {
      ATH_MSG_DEBUG("Executing tool " << m_fesimTool[itool]->name());
      m_fesimTool[itool]->process(*chargedDiodes,*RDOColl, rndmEngine);
    }
    CHECK(m_rdoContainer->addCollection(RDOColl,RDOColl->identifyHash()));

    ATH_MSG_DEBUG("Pixel RDOs '" << RDOColl->identifyHash() << "' added to container");
    addSDO(chargedDiodes.get());
    chargedDiodes->clear();
  }
  delete m_timedHits;
  m_timedHits = nullptr;
  ATH_MSG_DEBUG("hits processed");

  ///////////////////////////////////////////////////////////
  //  ***  Loop over the Detectors without hits ****
  ///////////////////////////////////////////////////////////
  if (!m_onlyHitElements) { 
    ATH_MSG_DEBUG("processing elements without hits");
    for (unsigned int i=0; i<processedElements.size(); i++) {
      if (!processedElements[i]) {
        IdentifierHash idHash = i;
        if (!idHash.is_valid()) { ATH_MSG_ERROR("PixelDetector element id hash is invalid = " << i); }

        const InDetDD::SiDetectorElement *element = m_detManager->getDetectorElement(idHash);
        if (element) {
          ATH_MSG_DEBUG ("In digitize of untouched elements: layer - phi - eta  " << m_detID->layer_disk(element->identify()) << " - " << m_detID->phi_module(element->identify()) << " - " << m_detID->eta_module(element->identify()) << " - " << "size: " << processedElements.size());

          chargedDiodes->setDetectorElement(element);
          ATH_MSG_DEBUG("Digitize non hit element");

          // Create and store RDO and SDO
          PixelRDO_Collection *RDOColl = new PixelRDO_Collection(chargedDiodes->identifyHash());
          RDOColl->setIdentifier(chargedDiodes->identify());
          for (unsigned int itool=0; itool<m_fesimTool.size(); itool++) {
            ATH_MSG_DEBUG("Executing tool " << m_fesimTool[itool]->name());
            m_fesimTool[itool]->process(*chargedDiodes,*RDOColl, rndmEngine);
          }
          CHECK(m_rdoContainer->addCollection(RDOColl,RDOColl->identifyHash()));

          ATH_MSG_DEBUG("Pixel RDOs '" << RDOColl->identifyHash() << "' added to container");
          addSDO(chargedDiodes.get());
          chargedDiodes->clear();
        }
      }
    }
  }
  ATH_MSG_DEBUG("non-hits processed");

  return StatusCode::SUCCESS;
}

//=======================================
// A D D   S D O
//=======================================
// Convert a SiTotalCharge to a InDetSimData, and store it. (this needs working...)
//-----------------------------------------------------------------------------------------------
void PixelDigitizationTool::addSDO(SiChargedDiodeCollection* collection) {

  typedef SiTotalCharge::list_t list_t;

  std::vector<InDetSimData::Deposit> deposits;
  deposits.reserve(5); // no idea what a reasonable number for this would be with pileup

  // loop over the charged diodes
  SiChargedDiodeIterator EndOfDiodeCollection = collection->end();
  for(SiChargedDiodeIterator i_chargedDiode=collection->begin(); i_chargedDiode!=EndOfDiodeCollection; ++i_chargedDiode) {
    deposits.clear();
    const list_t& charges = (*i_chargedDiode).second.totalCharge().chargeComposition();

    bool real_particle_hit = false;
    // loop over the list
    list_t::const_iterator EndOfChargeList =  charges.end();
    for ( list_t::const_iterator i_ListOfCharges = charges.begin(); i_ListOfCharges!=EndOfChargeList; ++i_ListOfCharges) {

      const HepMcParticleLink& trkLink = i_ListOfCharges->particleLink();
      int barcode = trkLink.barcode();
      if ((barcode == 0) || (barcode == m_vetoThisBarcode)){
        continue;
      }
      if(!real_particle_hit) { real_particle_hit = trkLink.isValid(); }
      // check if this track number has been already used.
      std::vector<InDetSimData::Deposit>::reverse_iterator theDeposit = deposits.rend();  //dummy value
      std::vector<InDetSimData::Deposit>::reverse_iterator depositsR_end = deposits.rend();
      std::vector<InDetSimData::Deposit>::reverse_iterator i_Deposit = deposits.rbegin();
      for ( ; i_Deposit != depositsR_end; ++i_Deposit) {
        if( (*i_Deposit).first == trkLink ) {theDeposit = i_Deposit; break;}
      }

      // if the charge has already hit the Diode add it to the deposit
      if(theDeposit != depositsR_end ) (*theDeposit).second += i_ListOfCharges->charge();
      else { // create a new deposit
        InDetSimData::Deposit deposit(trkLink, i_ListOfCharges->charge());
        deposits.push_back(deposit);
      }
    }
    // add the simdata object to the map:
    if(real_particle_hit || m_createNoiseSDO) {
      m_simDataColl->insert(std::make_pair(collection->getId((*i_chargedDiode).first),InDetSimData(deposits,(*i_chargedDiode).second.flag())));
    }
  }
}

//=======================================
// P R E P A R E   E V E N T
//=======================================
StatusCode PixelDigitizationTool::prepareEvent(unsigned int) {
  ATH_MSG_VERBOSE("PixelDigitizationTool::prepareEvent()");

  // Prepare event
  m_rdoContainer = SG::makeHandle(m_rdoContainerKey);
  ATH_CHECK(m_rdoContainer.record(std::make_unique<PixelRDO_Container>(m_detID->wafer_hash_max())));
  ATH_MSG_DEBUG("PixelRDO_Container " << m_rdoContainer.name() << " registered in StoreGate");

  m_simDataColl = SG::makeHandle(m_simDataCollKey);
  ATH_CHECK(m_simDataColl.record(std::make_unique<InDetSimDataCollection>()));
  ATH_MSG_DEBUG("InDetSimDataCollection " << m_simDataColl.name() << " registered in StoreGate");

  // Create hit collection
  if(m_timedHits) delete m_timedHits;
  m_timedHits     = new TimedHitCollection<SiHit>();

  m_HardScatterSplittingSkipper = false;
  return StatusCode::SUCCESS;
}

//=======================================
// M E R G E   E V E N T
//=======================================
StatusCode PixelDigitizationTool::mergeEvent() {
  ATH_MSG_VERBOSE("PixelDigitizationTool::mergeEvent()");

  // Digitize hits
  CHECK(digitizeEvent());

  for (std::vector<SiHitCollection*>::iterator it = m_hitCollPtrs.begin();it!=m_hitCollPtrs.end();it++) {
    (*it)->Clear();
    delete(*it);
  }
  m_hitCollPtrs.clear();

  return StatusCode::SUCCESS;
}

//=======================================
// P R O C E S S   B U N C H  X I N G
//=======================================
StatusCode PixelDigitizationTool::processBunchXing(int bunchXing, SubEventIterator bSubEvents, SubEventIterator eSubEvents) {

  ATH_MSG_VERBOSE("PixelDigitizationTool::processBunchXing() " << bunchXing);
  //decide if this event will be processed depending on HardScatterSplittingMode & bunchXing
  if (m_HardScatterSplittingMode==2 && !m_HardScatterSplittingSkipper) { m_HardScatterSplittingSkipper=true; return StatusCode::SUCCESS; }
  if (m_HardScatterSplittingMode==1 &&  m_HardScatterSplittingSkipper) { return StatusCode::SUCCESS; }
  if (m_HardScatterSplittingMode==1 && !m_HardScatterSplittingSkipper) { m_HardScatterSplittingSkipper=true; }


  typedef PileUpMergeSvc::TimedList<SiHitCollection>::type TimedHitCollList;
  TimedHitCollList hitCollList;

  if (!(m_mergeSvc->retrieveSubSetEvtData(m_inputObjectName, hitCollList, bunchXing,
					  bSubEvents, eSubEvents).isSuccess()) &&
      hitCollList.size() == 0) {
    ATH_MSG_ERROR("Could not fill TimedHitCollList");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_VERBOSE(hitCollList.size() << " SiHitCollections with key " <<
		    m_inputObjectName << " found");
  }

  TimedHitCollList::iterator iColl(hitCollList.begin());
  TimedHitCollList::iterator endColl(hitCollList.end());

  for( ; iColl != endColl; iColl++){
    SiHitCollection *hitCollPtr = new SiHitCollection(*iColl->second);
    PileUpTimeEventIndex timeIndex(iColl->first);
    ATH_MSG_DEBUG("SiHitCollection found with " << hitCollPtr->size() <<
		  " hits");
    ATH_MSG_VERBOSE("time index info. time: " << timeIndex.time()
		    << " index: " << timeIndex.index()
		    << " type: " << timeIndex.type());
    m_timedHits->insert(timeIndex, hitCollPtr);
    m_hitCollPtrs.push_back(hitCollPtr);
  }

  return StatusCode::SUCCESS;
}

