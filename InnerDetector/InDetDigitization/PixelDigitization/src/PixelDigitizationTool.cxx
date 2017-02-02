/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////
// PixelDigitizationTool.cxx
//   Implementation file for class PixelDigitizationTool
////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
////////////////////////////////////////////////////////////////////////////

// Pixel digitization includes
#include "PixelDigitizationTool.h"
#include "PixelDiodeCrossTalkGenerator.h"
#include "PixelCellDiscriminator.h"
#include "PixelNoisyCellGenerator.h"
#include "PixelChargeSmearer.h"
#include "PixelGangedMerger.h"
#include "TimeSvc.h"
#include "PixelRandomDisabledCellGenerator.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "InDetSimEvent/SiHitCollection.h"

// Mother Package includes
#include "SiDigitization/SiHelper.h"
#include "SiDigitization/SiChargedDiodeCollection.h"
#include "SiDigitization/ISiChargedDiodesProcessorTool.h"

#include "SubChargesTool.h"

// Det Descr
#include "Identifier/Identifier.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SiReadoutCellId.h"
#include "InDetReadoutGeometry/SiDetectorDesign.h"
#include "InDetReadoutGeometry/SiCellId.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetSimData/InDetSimDataCollection.h"

// Random numbers
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "CLHEP/Random/RandFlat.h"

// Gaudi includes
#include "AthenaKernel/errorcheck.h"

// DataHandle
#include "StoreGate/DataHandle.h"
#include "CxxUtils/make_unique.h"

//
#include <limits>
#include <cstdint>
static constexpr unsigned int crazyParticleBarcode(std::numeric_limits<int32_t>::max());
//Barcodes at the HepMC level are int

using namespace InDetDD;

// Constructor with parameters:
PixelDigitizationTool::PixelDigitizationTool(const std::string &type,
                                             const std::string &name,
                                             const IInterface * pIID) :
  PileUpToolBase(type,name,pIID),
  m_offlineCalibSvc("PixelOfflineCalibSvc", name),
  m_rdoContainer("PixelRDOs"),
  m_simDataColl("PixelSDO_Map"),
  m_HardScatterSplittingMode(0),
  m_HardScatterSplittingSkipper(false),
  m_rndmEngineName("PixelDigitization"),
  m_onlyHitElements(false),
  m_diodesProcsTool(),
  m_chargeTool(),
  m_pixelCalibSvc("PixelCalibSvc",name),
  m_detID(nullptr),
  m_vetoThisBarcode(crazyParticleBarcode),
  m_timedHits(nullptr),
  m_rndmSvc("AtRndmGenSvc",name),
  m_mergeSvc("PileUpMergeSvc",name),
  m_TimeSvc("TimeSvc",name),
  m_pixelConditionsSvc("PixelConditionsSummarySvc",name),
  m_rndmEngine(nullptr),
  m_detManager(nullptr),
  m_inputObjectName(""),
  m_createNoiseSDO(false)
{
  declareProperty("PixelTools",               m_diodesProcsTool,   "List of processor tools");
  declareProperty("ChargeTools",              m_chargeTool,        "List of charge tools");
  declareProperty("RndmSvc",                  m_rndmSvc,                   "Random number service used in Pixel Digitization");
  declareProperty("MergeSvc",                 m_mergeSvc,                  "Merge service used in Pixel digitization");
  declareProperty("TimeSvc",                  m_TimeSvc,                   "Time Svc");
  declareProperty("PixelCalibSvc",            m_pixelCalibSvc);
  declareProperty("PixelConditionsSummarySvc",m_pixelConditionsSvc);
  declareProperty("OfflineCalibSvc",         m_offlineCalibSvc);
  declareProperty("InputObjectName",         m_inputObjectName,        "Input Object name" );
  declareProperty("CreateNoiseSDO",          m_createNoiseSDO,      "Set create noise SDO flag");

  declareProperty("RDOCollName",        m_rdoContainer,          "RDO collection name");
  declareProperty("SDOCollName",        m_simDataColl,           "SDO collection name");

  declareProperty("LVL1Latency",        m_maxToT,               "LVL1 latency (max possible ToT)");
  declareProperty("ToTMinCut",          m_minToT,               "Minimum ToT cut (online cut)");
  declareProperty("ApplyDupli",         m_applyDupli,           "Duplicate low ToT hits");
  declareProperty("LowTOTduplication",  m_maxToTForDupli,       "ToT value below which the hit is duplicated");

  // random number stream name
  declareProperty("RndmEngine",         m_rndmEngineName,       "Random engine name");

  declareProperty("OnlyHitElements",    m_onlyHitElements,      "Process only elements with hits");
  declareProperty("HardScatterSplittingMode", m_HardScatterSplittingMode, "Control pileup & signal splitting" );
  declareProperty("ParticleBarcodeVeto",m_vetoThisBarcode=crazyParticleBarcode, "Barcode of particle to ignore");
}


// Initialize method:
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

  CHECK(m_TimeSvc.retrieve());

  CHECK(m_pixelCalibSvc.retrieve());

  CHECK(m_pixelConditionsSvc.retrieve());

  if (!m_offlineCalibSvc.empty()) {
    CHECK(m_offlineCalibSvc.retrieve());
    ATH_MSG_INFO("Retrieved tool " <<  m_offlineCalibSvc.type());
  }

  // Initialize random number generator
  CHECK(m_rndmSvc.retrieve());
  m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
  if (!m_rndmEngine) {
    ATH_MSG_ERROR("Could not find RndmEngine : " << m_rndmEngineName);
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG("Found RndmEngine : " << m_rndmEngineName);
  }

  // Initialize detector manager
  CHECK(detStore()->retrieve(m_detManager,"Pixel"));

  CHECK(detStore()->retrieve(m_detID,"PixelID"));
  ATH_MSG_DEBUG("Pixel ID helper retrieved");

  // Initialize tools
  CHECK(m_diodesProcsTool.retrieve());

  CHECK(m_chargeTool.retrieve());

  return StatusCode::SUCCESS;
}

// Digitize method:
StatusCode PixelDigitizationTool::processAllSubEvents() {

  //===============
  // Prepare event
  //===============
  // Create Output Containers
  ATH_MSG_DEBUG("Prepare event");
  CHECK(prepareEvent(0));

  if (!m_rdoContainer.isValid()) {
    if (!(m_rdoContainer=CxxUtils::make_unique<PixelRDO_Container>(m_detID->wafer_hash_max())).isValid()) {
      ATH_MSG_FATAL("Could not create PixelRDO_Container");
      return StatusCode::FAILURE;
    }
  }
  ATH_MSG_DEBUG("PixelRDO_Container " << m_rdoContainer.name() << " registered in StoreGate");

  if (!m_simDataColl.isValid()) {
    if (!(m_simDataColl = CxxUtils::make_unique<InDetSimDataCollection>()).isValid()) {
      ATH_MSG_FATAL("Could not create InDetSimDataCollection");
      return StatusCode::FAILURE;
    }
  }
  ATH_MSG_DEBUG("InDetSimDataCollection " << m_simDataColl.name() << " registered in StoreGate");

  // Get the container(s)
  typedef PileUpMergeSvc::TimedList<SiHitCollection>::type TimedHitCollList;
  TimedHitCollList hitCollList;
  unsigned int numberOfSiHits(0);
  CHECK(m_mergeSvc->retrieveSubEvtsData(m_inputObjectName,hitCollList,numberOfSiHits));

  // Create hit collection
  m_timedHits     = new TimedHitCollection<SiHit>(numberOfSiHits);
  auto chargedDiodes = std::make_unique<SiChargedDiodeCollection>();

  std::vector<bool> processedElements; /**< vector of processed elements - set by digitizeHits() */
  processedElements.resize(m_detID->wafer_hash_max(),false);

  //================
  // Generate event
  //================
  ATH_MSG_VERBOSE("Digitize hits");

  // Now merge all collections into one
  for (TimedHitCollList::iterator iColl=hitCollList.begin(); iColl!=hitCollList.end(); iColl++) {
    // Decide if this event will be processed depending on HardScatterSplittingMode
    if (m_HardScatterSplittingMode==2 && !m_HardScatterSplittingSkipper) { m_HardScatterSplittingSkipper=true; continue; }
    if (m_HardScatterSplittingMode==1 &&  m_HardScatterSplittingSkipper) { continue; }
    if (m_HardScatterSplittingMode==1 && !m_HardScatterSplittingSkipper) { m_HardScatterSplittingSkipper=true; }
    const SiHitCollection* p_collection(iColl->second);
    m_timedHits->insert(iColl->first, p_collection);
    ATH_MSG_DEBUG ( "SiTrackerHitCollection found with"<<p_collection->size()<<" hits");    // loop on the hit collections
  }

  // Loop over the Detectors with hits
  ATH_MSG_DEBUG("Digitizing hits");
  TimedHitCollection<SiHit>::const_iterator firstHit, lastHit;
  while (m_timedHits->nextDetectorElement(firstHit,lastHit)) {

    // Create the identifier for the collection
    ATH_MSG_DEBUG("create ID for the hit collection");
    const PixelID* PID = static_cast<const PixelID*>(m_detID);
    Identifier id = PID->wafer_id((*firstHit)->getBarrelEndcap(),(*firstHit)->getLayerDisk(),(*firstHit)->getPhiModule(),(*firstHit)->getEtaModule());

    // Get the det element from the manager
    SiDetectorElement* sielement = m_detManager->getDetectorElement(id);

    if (sielement==0) {
      ATH_MSG_DEBUG(" Barrel=" << (*firstHit)->getBarrelEndcap() << 
                    " Layer="  << (*firstHit)->getLayerDisk() << 
                    " Eta="    << (*firstHit)->getEtaModule() << 
                    " Phi="    << (*firstHit)->getPhiModule());
      ATH_MSG_ERROR("detector manager could not find element with id = " << id);
      break;
    }

    // Create the charged diodes collection
    chargedDiodes->setDetectorElement(sielement);

    // Loop over the hits and created charged diodes:
    for (TimedHitCollection<SiHit>::const_iterator phit=firstHit; phit!=lastHit; phit++) {
      //skip hits which are more than 10us away
      if (fabs((*phit)->meanTime())<10000.0*CLHEP::ns) {
        ATH_MSG_DEBUG("HASH = " << m_detID->wafer_hash(m_detID->wafer_id((*phit)->getBarrelEndcap(),(*phit)->getLayerDisk(),(*phit)->getPhiModule(),(*phit)->getEtaModule())));
        ATH_MSG_DEBUG("calling process() for all methods");

        // Apply charge collection tools
        for (unsigned int itool=0; itool<m_chargeTool.size(); itool++) {
          ATH_MSG_DEBUG("Executing tool " << m_chargeTool[itool]->name());
          if (m_chargeTool[itool]->charge(*phit,*chargedDiodes,*sielement)==StatusCode::FAILURE) { break; }
        }
        ATH_MSG_DEBUG("charges filled!");
      }
    }

    // Apply processor tools
    ATH_MSG_DEBUG("Apply processor tools");
    for (unsigned int itool=0; itool<m_diodesProcsTool.size(); itool++) {
      ATH_MSG_DEBUG("Executing tool " << m_diodesProcsTool[itool]->name());
      m_diodesProcsTool[itool]->process(*chargedDiodes);
    }

    ATH_MSG_DEBUG("Hit collection ID=" << m_detID->show_to_string(chargedDiodes->identify()));
    ATH_MSG_DEBUG("in digitize elements with hits: ec - layer - eta - phi  "
                  << m_detID->barrel_ec(chargedDiodes->identify()) << " - "
                  << m_detID->layer_disk(chargedDiodes->identify()) << " - "
                  << m_detID->eta_module(chargedDiodes->identify()) << " - "
                  << m_detID->phi_module(chargedDiodes->identify()));

    IdentifierHash idHash = chargedDiodes->identifyHash();

    assert(idHash<processedElements.size());
    processedElements[idHash] = true;

    // Create and store RDO and SDO
    if (!chargedDiodes->empty()) {
      PixelRDO_Collection *RDOColl=this->createRDO(chargedDiodes.get());
  
      CHECK(m_rdoContainer->addCollection(RDOColl,RDOColl->identifyHash()));
      ATH_MSG_DEBUG("Pixel RDOs '" << RDOColl->identifyHash() << "' added to container");
      addSDO(chargedDiodes.get());
    }
    chargedDiodes->clear();
  }
  delete m_timedHits;
  m_timedHits = 0;
  ATH_MSG_DEBUG("hits processed");

  // Loop over the Detectors without hits
  if (!m_onlyHitElements) { 
    ATH_MSG_DEBUG("processing elements without hits");
    for (unsigned int i=0; i<processedElements.size(); i++) {
      if (!processedElements[i]) {
        IdentifierHash idHash = i;
        if (!idHash.is_valid()) { ATH_MSG_ERROR("PixelDetector element id hash is invalid = " << i); }

        const SiDetectorElement *element = m_detManager->getDetectorElement(idHash);
        if (element) {
          ATH_MSG_DEBUG ("In digitize of untouched elements: layer - phi - eta  "
              << m_detID->layer_disk(element->identify()) << " - "
              << m_detID->phi_module(element->identify()) << " - "
              << m_detID->eta_module(element->identify()) << " - "
              << "size: " << processedElements.size());

          chargedDiodes->setDetectorElement(element);
          ATH_MSG_DEBUG("Digitize non hit element");

          // Apply processor tools
          ATH_MSG_DEBUG("Apply processor tools");
          for (unsigned int itool=0; itool<m_diodesProcsTool.size(); itool++) {
            ATH_MSG_DEBUG("Executing tool " << m_diodesProcsTool[itool]->name());
            m_diodesProcsTool[itool]->process(*chargedDiodes);
          }

          // Create and store RDO and SDO
          if (!chargedDiodes->empty()) {
            PixelRDO_Collection *RDOColl=this->createRDO(chargedDiodes.get());

            CHECK(m_rdoContainer->addCollection(RDOColl,RDOColl->identifyHash()));
            ATH_MSG_DEBUG("Pixel RDOs '" << RDOColl->identifyHash() << "' added to container");
            addSDO(chargedDiodes.get());
          }
          chargedDiodes->clear();
        }
      }
    }
  }
  ATH_MSG_DEBUG("non-hits processed");

  ATH_MSG_DEBUG("Digitize success!");
  return StatusCode::SUCCESS;
}

StatusCode PixelDigitizationTool::finalize() {
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------------------------
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

PixelRDO_Collection *PixelDigitizationTool::createRDO(SiChargedDiodeCollection *chargedDiodes) {

  // Create the collection
  Identifier collID = chargedDiodes->identify();
  ATH_MSG_DEBUG("PixelDigitizationTool::createRDO() collection : " << m_detID->show_to_string(collID));
  IdentifierHash idhash = chargedDiodes->identifyHash();
  PixelRDO_Collection* PixelRDOColl = new PixelRDO_Collection(idhash);
  PixelRDOColl->setIdentifier(collID );

  // Determine the Pixel Region (IBL, BL, L1, L2, EC, DBM, ITk 5th layer), to be used for latency, ToT cut and duplication:
  const PixelID* pixelId = static_cast<const PixelID *>(chargedDiodes->element()->getIdHelper());

  const IdentifierHash moduleHash = pixelId->wafer_hash(chargedDiodes->identify()); // wafer hash

//  int barrel_ec  = pixelId->barrel_ec(moduleHash);
//  int layerIndex = pixelId->layer_disk(moduleHash);
  int barrel_ec  = pixelId->barrel_ec(chargedDiodes->element()->identify());
  int layerIndex = pixelId->layer_disk(chargedDiodes->element()->identify());
  if (layerIndex>=4)     { layerIndex=6; }  // ITk 5th layer
  if (abs(barrel_ec)==2) { layerIndex=4; }  // disks
  if (abs(barrel_ec)==4) { layerIndex=5; }  // DBM

  const PixelModuleDesign *p_design = static_cast<const PixelModuleDesign*>(&(chargedDiodes->element())->design());
  std::vector<Pixel1RawData*> p_rdo_small_fei4;
  int maxFEI4SmallHit = 2;
  int nSmallHitsFEI4 = 0;
  std::vector<int> row;
  std::vector<int> col;
  const int maxRow = p_design->rowsPerCircuit();
  const int maxCol = p_design->columnsPerCircuit();
  std::vector < std::vector < int > > FEI4Map ( maxRow+1, std::vector < int > ( maxCol+1) );
  ATH_MSG_DEBUG ( "layerIndex = " << layerIndex << " MaxRow = " << maxRow << " MaxCol = " << maxCol);

  int overflowIBLToT = 14;
  if (!m_offlineCalibSvc.empty()) {
    overflowIBLToT = m_offlineCalibSvc->getIBLToToverflow();
    if (overflowIBLToT==14) { maxFEI4SmallHit=0; }
    if (overflowIBLToT==15) { maxFEI4SmallHit=1; }
    if (overflowIBLToT==16) { maxFEI4SmallHit=2; }
  }

  // ToT scale, to be used for FEI4, which has at most 4 bits for ToT,
  // so physical ToT values are from 1 to 15
  //
  // Loop over the charged diodes.

  for (SiChargedDiodeIterator i_chargedDiode=chargedDiodes->begin(); i_chargedDiode!=chargedDiodes->end(); ++i_chargedDiode) {
    //
    // Charged diode flag:
    //====================
    // I.  Mask = 0xff
    // 0x1  : Noise
    // 0x2  : Below threshold
    // 0x4  : Disabled
    // 0x8  : Corrupted TOT
    //
    // II. Mask = 0xff00
    // Shift right 8 bits => relative bunch number (0 or 1 for CTB 2004)
    // The bunch number is 1 if the time walk is larger than a cutoff (TimeCut in jobopts)
    //---------------------------------
    //
    // If diode is disconnected/disabled skip it
    //
    Identifier diodeID = chargedDiodes->getId((*i_chargedDiode).first);

    //  unsigned int flagmask = (*i_chargedDiode).second.flag() & 0xFE ;
    //  if(!flagmask ) {
    //
    // Check if diode is masked out or has been disable by random mapping - if not, create the RDO.
    //
    bool maskedOut = false;
    if (SiHelper::isMaskOut((*i_chargedDiode).second))  { maskedOut=true; } 
    if (SiHelper::isDisabled((*i_chargedDiode).second)) { maskedOut=true; } 

    if (!m_pixelConditionsSvc.empty()) {
      bool active = m_pixelConditionsSvc->isActive(moduleHash,diodeID);
      if (!active) {
        SiHelper::disabled((*i_chargedDiode).second,true,true);
        maskedOut=true;
      }
    }

    if (!maskedOut) {

      // calculate ToT from the total charge, add gaussian smearing
      // take the integer value to get the actual observed ToT
      // minimum ToT = 1

      double tot    = m_pixelCalibSvc->getTotMean(diodeID,(*i_chargedDiode).second.charge());
      double totsig = m_pixelCalibSvc->getTotRes(diodeID,tot);
      int nToT = static_cast<int>(CLHEP::RandGaussZiggurat::shoot(m_rndmEngine,tot,totsig));
      if (nToT<1) { nToT=1; }

      // Fix for IBL, it sets the most probable value for a mip to 8.
      // Need to keep it hardcoded for a cache (no header change)
      // but when moving to full release, one has to accomodate additional
      // properties and a proper function.
      // - HitDiscriminator configuration
      // - Referenca value (a mip)
      // - ToT for that reference value

      if (p_design->getReadoutTechnology()==PixelModuleDesign::FEI4) {
        const PixelID* pixelId = static_cast<const PixelID *>(chargedDiodes->element()->getIdHelper());
        if (pixelId->is_dbm(chargedDiodes->element()->identify())) {
          nToT = 8*((*i_chargedDiode).second.charge() - 1200. )/(8000. - 1200.);
        }
        if (nToT<=0) nToT=1;
        if (nToT==2 && maxFEI4SmallHit==2) { nToT=1; }
        if (nToT>=overflowIBLToT) { nToT=overflowIBLToT; }
      }
      int flag   = (*i_chargedDiode).second.flag();
      int bunch  = (flag >>  8) & 0xff;
      //
      ATH_MSG_DEBUG("RDO: " << (*i_chargedDiode).second.charge() << "  " << nToT << "  " << bunch << "  " << flag << "  " << layerIndex);

      if (p_design->getReadoutTechnology()==PixelModuleDesign::FEI3) {
        if (nToT>m_maxToT.at(layerIndex)) continue; // skip hits with ToT exceeding LVL1 Latency
        if (nToT<m_minToT.at(layerIndex)) continue; // skip hits with ToT less than ToT cut
      }

      //       float kToT= m_totparA + m_totparB / ( m_totparC + (*i_chargedDiode).second.charge() ) ;
      //       float kToTs  =  CLHEP::RandGaussZiggurat::shoot( m_rndmEngine, kToT ,  m_totparP1 + m_totparP2 * kToT ) ;
      //       int nToT  =  static_cast<int>(kToTs) ;
      //       if( nToT < 1 ) nToT=1;
      //
      // Fill the RDO word as in PixelRDO_Cnv.
      //  This incomprehensible code is to fill the RDO as they do in the
      //  zebra converter.
      //  (code from PixelRDO_Cnv.cxx, some comments removed)

      SiReadoutCellId cellId=(*i_chargedDiode).second.getReadoutCell();
      const Identifier id_readout = chargedDiodes->element()->identifierFromCellId(cellId);

      int iirow = cellId.phiIndex();
      int iicol = cellId.etaIndex();
      if (p_design->getReadoutTechnology()==PixelModuleDesign::FEI4 && iicol>=maxCol) { iicol=iicol-maxCol; } // FEI4 copy mechanism works per FE.

      //if (correct_id_readout!=diodeID) {
      //  ATH_MSG_DEBUG ( "correct_readout_id != diodeID" );
      // }
      // Create hit only if bunch within the acceptance (not for IBL and DBM):

      if (p_design->getReadoutTechnology()!=PixelModuleDesign::FEI4) {
        if (bunch >= 0 && bunch < m_TimeSvc->getTimeBCN()) {
          Pixel1RawData *p_rdo= new Pixel1RawData(id_readout, nToT, bunch, 0, bunch );
          PixelRDOColl->push_back(p_rdo);
        }
      } else {
        // IBL: if big hit, create RDO and record it. If small hit, create RDO and store it in a vector:

        if ( nToT > maxFEI4SmallHit ) {
          if ( bunch >= 0 && bunch < m_TimeSvc->getTimeBCN()) {
            Pixel1RawData *p_rdo= new Pixel1RawData(id_readout, nToT, bunch, 0, bunch );
            PixelRDOColl->push_back(p_rdo);
            FEI4Map[iirow][iicol] = 2; //Flag for "big hits"
          }
        } else {
          if ( bunch >= 0 && bunch < m_TimeSvc->getTimeBCN()) {
            Pixel1RawData *p_rdo= new Pixel1RawData(id_readout, nToT, bunch, 0, bunch );
            p_rdo_small_fei4.push_back(p_rdo);
            row.push_back(iirow);
            col.push_back(iicol);
            nSmallHitsFEI4++;
            FEI4Map[iirow][iicol] = 1; //Flag for low hits
            ATH_MSG_DEBUG ( "Row small = " << iirow << " col small = " << iicol << " ToT = " << nToT << " Bunch = " << bunch << " ismallhits = " << nSmallHitsFEI4);
          }
        }
      }
      // Duplication mechanism for FEI3 small hits :
      if (p_design->getReadoutTechnology()==PixelModuleDesign::FEI3) {
        if (m_applyDupli.at(layerIndex)) {
          if (nToT<=m_maxToTForDupli.at(layerIndex) && bunch>0 && bunch<=m_TimeSvc->getTimeBCN()) {
            Pixel1RawData *p_rdo= new Pixel1RawData(id_readout, nToT, bunch-1, 0, bunch-1 );
            PixelRDOColl->push_back(p_rdo);
          }
        }
      }
    }
  }
  // Copy mechanism for IBL small hits:
  if (p_design->getReadoutTechnology()==PixelModuleDesign::FEI4 && m_applyDupli.at(layerIndex) && nSmallHitsFEI4>0){
    bool recorded = false;
    //First case: Record small hits which are in the same Pixel Digital Region than a big hit:

    for ( int ismall = 0; ismall != nSmallHitsFEI4; ++ismall ) {
      int rowPDR = row[ismall]/2;
      int colPDR = col[ismall]/2;
      for ( int rowBigHit = 2*rowPDR; rowBigHit != 2*rowPDR+2 && rowBigHit < maxRow; ++rowBigHit ) {
        for ( int colBigHit = 2*colPDR; colBigHit != 2*colPDR+2 && colBigHit < maxCol; ++colBigHit ) {
          ATH_MSG_DEBUG ( "rowBig = " << rowBigHit << " colBig = " << colBigHit << " Map Content = " << FEI4Map[rowBigHit][colBigHit]);
          if ( FEI4Map[rowBigHit][colBigHit] == 2 && !recorded) {
            PixelRDOColl->push_back(p_rdo_small_fei4[ismall]);
            recorded = true;
          }
        }
      }
      // Second case: Record small hits which are phi-neighbours with a big hit:

      if ( !recorded && row[ismall] < maxRow - 1 ) {
        if ( FEI4Map[row[ismall]+1][col[ismall]] == 2 ) {
          PixelRDOColl->push_back(p_rdo_small_fei4[ismall]);
          recorded = true;
        }
      }
      if ( !recorded && row[ismall] != 0 ) {
        if ( FEI4Map[row[ismall]-1][col[ismall]] == 2 ) {
          PixelRDOColl->push_back(p_rdo_small_fei4[ismall]);
          recorded = true;
        }
      }
    }
  }
  return PixelRDOColl;
}

//----------------------------------------------------------------------
// PrepareEvent method:
//----------------------------------------------------------------------
StatusCode PixelDigitizationTool::prepareEvent(unsigned int) {
  ATH_MSG_VERBOSE("PixelDigitizationTool::prepareEvent()");

  m_HardScatterSplittingSkipper = false;
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
// MergeEvent method:
//----------------------------------------------------------------------
StatusCode PixelDigitizationTool::mergeEvent() {
  ATH_MSG_VERBOSE("PixelDigitizationTool::mergeEvent()");

  //===============
  // Prepare event
  //===============
  // Create Output Containers
  if (!m_rdoContainer.isValid()) {
    if (!(m_rdoContainer=CxxUtils::make_unique<PixelRDO_Container>(m_detID->wafer_hash_max())).isValid()) {
      ATH_MSG_FATAL("Could not create PixelRDO_Container");
      return StatusCode::FAILURE;
    }
  }

  if (!m_simDataColl.isValid()) {
    if (!(m_simDataColl = CxxUtils::make_unique<InDetSimDataCollection>()).isValid()) {
      ATH_MSG_FATAL("Could not create InDetSimDataCollection");
      return StatusCode::FAILURE;
    }
  }

  auto chargedDiodes = std::make_unique<SiChargedDiodeCollection>();

  std::vector<bool> processedElements;
  processedElements.resize(m_detID->wafer_hash_max(),false);

  //================
  // Generate event
  //================
  // Loop over the Detectors with hits
  TimedHitCollection<SiHit>::const_iterator firstHit, lastHit;
  while (m_timedHits->nextDetectorElement(firstHit,lastHit)) {

    // Create the identifier for the collection
    const PixelID* PID = static_cast<const PixelID*>(m_detID);
    Identifier id = PID->wafer_id((*firstHit)->getBarrelEndcap(),(*firstHit)->getLayerDisk(),(*firstHit)->getPhiModule(),(*firstHit)->getEtaModule());

    // Get the det element from the manager
    SiDetectorElement* sielement = m_detManager->getDetectorElement(id);
    if (sielement==0) { break; }

    // Create the charged diodes collection
    chargedDiodes->setDetectorElement(sielement);

    // Loop over the hits and created charged diodes:
    for (TimedHitCollection<SiHit>::const_iterator phit=firstHit; phit!=lastHit; phit++) {
      //skip hits which are more than 10us away
      if (fabs((*phit)->meanTime())<10000.0*CLHEP::ns) {
        // Apply charge collection tools
        for (unsigned int itool=0; itool<m_chargeTool.size(); itool++) {
          if (m_chargeTool[itool]->charge(*phit,*chargedDiodes,*sielement)==StatusCode::FAILURE) { break; }
        }
      }
    }

    // Apply processor tools
    for (unsigned int itool=0; itool<m_diodesProcsTool.size(); itool++) {
      m_diodesProcsTool[itool]->process(*chargedDiodes);
    }

    IdentifierHash idHash = chargedDiodes->identifyHash();

    assert(idHash<processedElements.size());
    processedElements[idHash] = true;

    // Create and store RDO and SDO
    if (!chargedDiodes->empty()) {
      PixelRDO_Collection *RDOColl=this->createRDO(chargedDiodes.get());

      CHECK(m_rdoContainer->addCollection(RDOColl,RDOColl->identifyHash()));
      addSDO(chargedDiodes.get());
    }
    chargedDiodes->clear();
  }
  delete m_timedHits;
  m_timedHits = 0;

  if (!m_onlyHitElements) {
    for (unsigned int i=0; i < processedElements.size(); i++) {
      if (!processedElements[i]) {
        IdentifierHash idHash = i;
        const SiDetectorElement * element = m_detManager->getDetectorElement(idHash);
        if (element) {
          chargedDiodes->setDetectorElement(element);
          // Apply processor tools
          for (unsigned int itool=0; itool<m_diodesProcsTool.size(); itool++) {
            m_diodesProcsTool[itool]->process(*chargedDiodes);
          }
          if (!chargedDiodes->empty()) {
            PixelRDO_Collection *RDOColl=this->createRDO(chargedDiodes.get());
            CHECK(m_rdoContainer->addCollection(RDOColl,RDOColl->identifyHash()));
            addSDO(chargedDiodes.get());
          }
          chargedDiodes->clear();
        }
      }
    }
  }
  ATH_MSG_DEBUG("Digitize success!");
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
// ProcessBunchXing method:
//----------------------------------------------------------------------
StatusCode PixelDigitizationTool::processBunchXing(int bunchXing, SubEventIterator bSubEvents, SubEventIterator eSubEvents) {

  if (m_timedHits==0) { m_timedHits = new TimedHitCollection<SiHit>(); }

  ATH_MSG_VERBOSE("PixelDigitizationTool::processBunchXing() " << bunchXing);
  //decide if this event will be processed depending on HardScatterSplittingMode & bunchXing
  if (m_HardScatterSplittingMode==2 && !m_HardScatterSplittingSkipper) { m_HardScatterSplittingSkipper=true; return StatusCode::SUCCESS; }
  if (m_HardScatterSplittingMode==1 &&  m_HardScatterSplittingSkipper) { return StatusCode::SUCCESS; }
  if (m_HardScatterSplittingMode==1 && !m_HardScatterSplittingSkipper) { m_HardScatterSplittingSkipper=true; }

  SubEventIterator iEvt = bSubEvents;
  for (; iEvt!=eSubEvents; iEvt++) {
    StoreGateSvc& seStore = *iEvt->ptr()->evtStore();
    ATH_MSG_VERBOSE("SubEvt StoreGate " << seStore.name() << " :"
                    << " bunch crossing : " << bunchXing
                    << " time offset : " << iEvt->time()
                    << " event number : " << iEvt->ptr()->eventNumber()
                    << " run number : " << iEvt->ptr()->runNumber());

    const SiHitCollection* seHitColl(0);
    if (!seStore.retrieve(seHitColl,m_inputObjectName).isSuccess()) {
      msg(MSG::ERROR) << "SubEvent Pixel SiHitCollection not found in StoreGate " << seStore.name() << endmsg;
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("SiHitCollection found with " << seHitColl->size() << " hits");

    PileUpTimeEventIndex timeIndex(iEvt->time(),iEvt->index());
    SiHitCollection *hitCollPtr = new SiHitCollection(*seHitColl);
    m_timedHits->insert(timeIndex,hitCollPtr);
  }

  return StatusCode::SUCCESS;
}

