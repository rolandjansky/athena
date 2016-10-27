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
#include "SurfaceChargesTool.h"
#include "PixelDiodeCrossTalkGenerator.h"
#include "PixelCellDiscriminator.h"
#include "PixelNoisyCellGenerator.h"
#include "SpecialPixelGenerator.h"
#include "PixelChargeSmearer.h"
#include "PixelGangedMerger.h"
#include "TimeSvc.h"
#include "CalibSvc.h"
#include "PixelRandomDisabledCellGenerator.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "InDetSimEvent/SiHitCollection.h"

// Mother Package includes 
#include "SiDigitization/SiHelper.h"
#include "SiDigitization/SiChargedDiodeCollection.h"
#include "SiDigitization/ISiChargedDiodesProcessorTool.h"

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
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PropertyMgr.h"
#include "AthenaKernel/errorcheck.h"

// DataHandle
#include "StoreGate/DataHandle.h"

#include "PixelConditionsServices/IPixelOfflineCalibSvc.h"

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
  m_overflowIBLToT(0),
  m_offlineCalibSvc("PixelOfflineCalibSvc", name),
  m_managerName("Pixel"),
  m_rdoCollName("PixelRDOs"),
  m_rdoCollNameSPM(),
  m_sdoCollName("PixelSDO_Map"),
  m_rdoContainer(NULL),
  m_rdoContainerSPM(NULL),
  m_simDataColl(NULL),
  //m_maxToT(255),
  //m_minToT(0),
  //m_applyDupli,
  //m_maxToTForDupli(255),
  m_IBLabsent(true),
  m_time_y_eq_zero(0.0),
  m_ComTime(NULL),
  m_HardScatterSplittingMode(0),
  m_HardScatterSplittingSkipper(false),
  m_rndmEngineName("PixelDigitization"),
  m_cosmics(false),
  m_useComTime(false),
  m_eventIOV(1),
  m_IOVFlag(PixelDigitizationTool::IOVonce),
  m_onlyHitElements(false),
  m_enableHits(true),
  m_enableNoise(true),
  m_enableSpecialPixels(true),
  m_doRDOforSPM(false),
  m_pixelNoisyCellGenerator("PixelNoisyCellGenerator"),
  m_pixelGangedMerger("PixelGangedMerger"),
  m_specialPixelGenerator("SpecialPixelGenerator"),
  m_pixelCellDiscriminator("PixelCellDiscriminator"),
  m_pixelChargeSmearer("PixelChargeSmearer"),
  m_pixelDiodeCrossTalkGenerator("PixelDiodeCrossTalkGenerator"),
  m_pixelIdMapping("PixelCablingSvc",name),
  m_pixelRandomDisabledCellGenerator("PixelRandomDisabledCellGenerator"),
  m_detID(NULL),
  m_thpcsi(NULL),
  m_SurfaceChargesTool("SurfaceChargesTool"),
  m_vetoThisBarcode(crazyParticleBarcode),
  m_rndmSvc("AtRndmGenSvc",name),
  m_mergeSvc("PileUpMergeSvc",name),
  m_CalibSvc("CalibSvc",name),
  m_TimeSvc("TimeSvc",name),
  m_IBLParameterSvc("IBLParameterSvc",name),
  m_rndmEngine(NULL),
  m_atlasID(NULL),
  m_detManager(NULL),
  m_specialPixelMapSvc("SpecialPixelMapSvc",name),
  m_specialPixelMapKey("SpecialPixelMap"),
  m_specialPixelMap(NULL),
  m_eventCounter(0),
  m_eventNextUpdate(0)
{
  
  declareInterface<PixelDigitizationTool>(this);
  declareProperty("PixelNoisyCellGenerator", m_pixelNoisyCellGenerator,   "PixelNoisyCellGenerator");
  declareProperty("PixelCellDiscriminator",  m_pixelCellDiscriminator,    "PixelCellDiscriminator");
  declareProperty("PixelChargeSmearer",      m_pixelChargeSmearer,        "PixelChargeSmearer");
  declareProperty("PixelDiodeCrossTalkGenerator", m_pixelDiodeCrossTalkGenerator,   "PixelDiodeCrossTalkGenerator");
  declareProperty("PixelGangedMerger",       m_pixelGangedMerger,         "PixelGangedMerger");
  declareProperty("PixelRandomDisabledCellGenerator", m_pixelRandomDisabledCellGenerator,   "PixelRandomDisabledCellGenerator");
  declareProperty("SpecialPixelGenerator",   m_specialPixelGenerator,     "Special pixel generator");
  declareProperty("SurfaceChargesTool",      m_SurfaceChargesTool,        "Surface charges tool");
  declareProperty("PixelCablingSvc",         m_pixelIdMapping,            "Pixel Cabling Service");
  declareProperty("RndmSvc",                 m_rndmSvc,                   "Random number service used in Pixel Digitization");
  declareProperty("MergeSvc",                m_mergeSvc,                  "Merge service used in Pixel digitization");
  declareProperty("TimeSvc",                 m_TimeSvc,                   "Time Svc");
  declareProperty("CalibSvc",                m_CalibSvc,                  "Calib Svc");
  declareProperty("OfflineCalibSvc",         m_offlineCalibSvc);
  declareProperty("InputObjectName",         m_inputObjectName="",        "Input Object name" );
  declareProperty("CreateNoiseSDO",          m_createNoiseSDO=false,      "Set create noise SDO flag");
  m_inputObjectName = "PixelHits";
  declareProperty("SpecialPixelMapKey",      m_specialPixelMapKey,        "Special Pixel Map Key"); 
  declareProperty("SpecialPixelMapSvc",      m_specialPixelMapSvc);

  //
  // Get parameter values from jobOptions file
  declareProperty("ManagerName",        m_managerName,          "Pixel manager name");
  declareProperty("RDOCollName",        m_rdoCollName,          "RDO collection name");
  declareProperty("SDOCollName",        m_sdoCollName,          "SDO collection name");
  declareProperty("EventIOV",           m_eventIOV,             "Number of events per IOV");
  declareProperty("IOVFlag",            m_IOVFlag,              "IOV flag - how to simulate the validity period");
  declareProperty("LVL1Latency",        m_maxToT,               "LVL1 latency (max possible ToT)");
  declareProperty("ToTMinCut",          m_minToT,               "Minimum ToT cut (online cut)");
  declareProperty("ApplyDupli",         m_applyDupli,           "Duplicate low ToT hits");
  declareProperty("LowTOTduplication",  m_maxToTForDupli,       "ToT value below which the hit is duplicated");

  //
  // random number stream name
  //
  declareProperty("RndmEngine",         m_rndmEngineName,       "Random engine name");
  //
  //For cosmics need to mask some modules
  declareProperty("CosmicsRun",         m_cosmics,              "Cosmics run");
  declareProperty("UseComTime",         m_useComTime,           "Use ComTime for timing");
  declareProperty("EnableHits",         m_enableHits,           "Enable hits");
  declareProperty("EnableNoise",        m_enableNoise,          "Enable noise generation");
  declareProperty("EnableSpecialPixels",m_enableSpecialPixels,  "Enable special pixels");
  declareProperty("OnlyHitElements",    m_onlyHitElements,      "Process only elements with hits");
  declareProperty("RDOforSPM",          m_doRDOforSPM,          "Create RDOs for special pixels");
  declareProperty("HardScatterSplittingMode", m_HardScatterSplittingMode, "Control pileup & signal splitting" );
  declareProperty("ParticleBarcodeVeto",m_vetoThisBarcode=crazyParticleBarcode, "Barcode of particle to ignore");
}



// Initialize method:
StatusCode PixelDigitizationTool::initialize()
{
  ATH_MSG_DEBUG("PixelDigitizationTool::Initialize()");
  m_rdoCollNameSPM = m_rdoCollName + "_SPM";
  //  
  // Print parameter values

  // check the input object name
  if (m_inputObjectName=="") {
    ATH_MSG_FATAL ( "Property InputObjectName not set !" );
    return StatusCode::FAILURE;
  }
  else ATH_MSG_DEBUG ( "Input objects: '" << m_inputObjectName << "'" );

  if (m_IBLParameterSvc.retrieve().isFailure()) {
       ATH_MSG_WARNING( "Could not retrieve IBLParameterSvc");
  }
  else {
	m_IBLParameterSvc->setBoolParameters(m_enableSpecialPixels,"EnableSpecialPixels");
	m_IBLParameterSvc->setBoolParameters(m_IBLabsent,"IBLAbsent");
  }
  //==============================
  // initialize services etc
  //==============================
  CHECK( initServices() );

  CHECK( initRandom() );

  CHECK( initTools() );

  CHECK( initExtras() );

  unsigned int m_nPixTot = m_detID->pixel_hash_max();
  unsigned int m_nModTot = m_detID->wafer_hash_max();
  m_specialPixelGenerator->setnPixTot(m_nPixTot);
  m_specialPixelGenerator->setnModTot(m_nModTot);


  //==============================
  // Create the charge model
  //==============================
  //
  // The structure is described below;
  // ----------------------------------------------------------------
  //  Charge source             Decription
  // ----------------------------------------------------------------
  // 1.  Surface charge         Generates charges based on the hits
  // 2.  Cross talk             Adds cross talk to neighboring pixels
  // 3.  Smearing               Adds (thermal) noise to pixels with hits, indep of cond db
  // 4.  General noise          PixelNoisyCellGenerator() - mergeCharge = true
  // 5.  Ganged pixels          Merge ganged pixels
  // 6.  Special pixels         Identify special pixels
  // 7.  Timewalk (timing)      PixelTimeWalkGenerator()
  // 8.  Discriminator          SiCellDiscriminator()
  // ----------------------------------------------------------------
  //

  // now cross talk (neighbouring pixels) 
  addCrossTalk();

  // add charge smearing to pixels with hits - thermal noise
  //addChargeSmearing(m_thermalNoise);
  addChargeSmearing();

  // add noise to random pixels - prob controlled by RndNoiseProb
  addNoise();

  // add/merge ganged pixels
  addGangedPixels();

  // mask special pixels
  // all pixels with status>0 will be masked out
  addSpecialPixels();

  // add randomly disabled pixels
  addRandomDisabled();

  // add discriminator
  // non-masked pixels, with timing ok and above threshold
  // will be accepted.
  addDiscriminator();

  return StatusCode::SUCCESS;
}

// Digitize method:
StatusCode PixelDigitizationTool::processAllSubEvents() 
{
  if (prepareEvent(0).isFailure()) return StatusCode::FAILURE;   
  ATH_MSG_VERBOSE("Begin digitizeAllHits");
  if (m_enableHits && !getNextEvent().isFailure()) digitizeAllHits();	
  else ATH_MSG_DEBUG("no hits found in event!");
  delete chargedDiodes;
  // loop over elements without hits
  ATH_MSG_DEBUG("Digitized Elements with Hits");
  digitizeNonHits();

  ATH_MSG_DEBUG("Digitized Elements without Hits");
  // create RDOs for Special Pixels TODO: Implement!
  createRDOforSPM();

  // increment event counter (for simulated IOV)
  m_eventCounter++;
 
  ATH_MSG_DEBUG("Digitize success!");
  return StatusCode::SUCCESS;
}

StatusCode PixelDigitizationTool::finalize() {
  return StatusCode::SUCCESS;
}



//----------------------------------------------------------------------
// createOutpuContainers method:
//----------------------------------------------------------------------
StatusCode PixelDigitizationTool::createOutputContainers() {
  /////////////////////////////////////////////////
  // create a new RDO conatiner and store in SG
  /////////////////////////////////////////////////

  try {
    m_rdoContainer=new PixelRDO_Container(m_detID->wafer_hash_max());
  } catch (std::bad_alloc) {
    ATH_MSG_FATAL ( "Could not create a new Pixel RawDataContainer !"  );
    return StatusCode::FAILURE;
  }
  ATH_MSG_VERBOSE("About to register rdoCollName in StoreGate");
  StatusCode sc=evtStore()->record(m_rdoContainer,m_rdoCollName);
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "Container '" << m_rdoCollName
          << "' could not be registered in StoreGate !" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_VERBOSE ( "Container '" << m_rdoCollName
          << "' registered in StoreGate" );
  }

  // do the same for the monitoring RDO
  if (m_doRDOforSPM) {
    ATH_MSG_VERBOSE("doRDOforSPM");
    try {
      m_rdoContainerSPM=new PixelRDO_Container(m_detID->wafer_hash_max());
    } catch (std::bad_alloc) {
      ATH_MSG_FATAL ( "Could not create a new Pixel RawDataContainer for Special Pixels Map !"  );
      return StatusCode::FAILURE;
    }
    //
    sc=evtStore()->record(m_rdoContainerSPM,m_rdoCollNameSPM);
    if (sc.isFailure()) {
      ATH_MSG_FATAL ( "Container '" << m_rdoCollNameSPM
            << "' could not be registered in StoreGate !" );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG ( "Container '" << m_rdoCollNameSPM
            << "' registered in StoreGate" );
    }
  }
  /////////////////////////////////////////////////
  // create a map for the SDO and register into SG
  /////////////////////////////////////////////////
  m_simDataColl = new InDetSimDataCollection();
  sc=evtStore()->record(m_simDataColl, m_sdoCollName);
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "InDetSimData map '" << m_sdoCollName
          << "' could not be registered in StoreGate !" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( "InDetSimData map '" << m_sdoCollName
          << "' registered in StoreGate" );
  }
return StatusCode::SUCCESS;
}

void PixelDigitizationTool::digitizeAllHits() {
  /////////////////////////////////////////////////
  //
  // In order to process all element rather than just those with hits we
  // create a vector to keep track of which elements have been processed.
  // NB. an element is a pixel module - 1744 in the full detector
  //
  /////////////////////////////////////////////////
  //
  // Need to do a clear before resize() since resize() sets only NEW elements to
  // the given value.
  //

  /////////////////////////////////////////////////
  // Loop over the Detectors with hits
  /////////////////////////////////////////////////
  ATH_MSG_DEBUG ( "Digitizing hits" );
  int hitcount = 0; // First, elements with hits.
 while (digitizeElement(chargedDiodes) ) {
    ATH_MSG_DEBUG ( "Hit collection ID=" << m_detID->show_to_string( chargedDiodes->identify() ) );

    hitcount++;  // Hitcount will be a number in the hit collection minus number of hits in missing mods
                 // NB: not really a hit but a collection of hits
                 // When in this loop, we are always in same module (side)
    
    ATH_MSG_DEBUG ( "in digitize elements with hits: ec - layer - eta - phi  "
	  << m_detID->barrel_ec(chargedDiodes->identify()  ) << " - " 
          << m_detID->layer_disk(chargedDiodes->identify() ) << " - "  
          << m_detID->eta_module(chargedDiodes->identify() ) << " - "
          << m_detID->phi_module(chargedDiodes->identify() ) << " - "
          << " processing hit number " << hitcount        
          );

    //if ( m_detID->barrel_ec(chargedDiodes->identify()) == 0 && m_detID->layer_disk(chargedDiodes->identify()) == 0 ) {
    //  ATH_MSG_DEBUG ( "in digitize elements with hits in innermost barrel: eta - phi  "
    //      << m_detID->eta_module(chargedDiodes->identify() ) << " - "
    //      << m_detID->phi_module(chargedDiodes->identify() ) << " - "
    //      << " processing hit number " << hitcount
    //      );
    //}

    // Have a flag to check if the module is present or not
    // Generally assume it is:
    //    m_module_status = 0;   //module is present we assume

    // Pixel EndCap A cosmic test:
    // ---------------------------
    // This is the place where we check on the status of the pixel
    // endcap module. Whether it was in the readout or not... 
    // For the cosmic geometry only the endcap elements exist and 
    // it will only loop over existing modules anyway.  At the moment this 
    // is just a place holder for access to COOL to query the module status.
    //   
    IdentifierHash idHash = chargedDiodes->identifyHash();
    
    assert (idHash < m_processedElements.size());
    m_processedElements[idHash] = true;
   
    // Create and store RDO and SDO
    // Don't create empty ones.
    //    if (!chargedDiodes->empty() && m_module_status==0 ) {
    if (!chargedDiodes->empty() ) {
      StatusCode sc = createAndStoreRDO(chargedDiodes);
      if (sc.isSuccess()) // error msg is given inside createAndStoreRDO()
	{
	  addSDO(chargedDiodes);
	}
    }
    
    chargedDiodes->clear();
  }
  ATH_MSG_DEBUG ( "hits processed" );
  return;
}

// digitize elements without hits
void PixelDigitizationTool::digitizeNonHits() {
  if (m_onlyHitElements) return;
  //
  ATH_MSG_DEBUG ( "processing elements without hits" );
  chargedDiodes = new SiChargedDiodeCollection;
  for (unsigned int i=0; i < m_processedElements.size(); i++) {
    if (!m_processedElements[i]) {
      IdentifierHash idHash = i;
      if (!idHash.is_valid()) {
        ATH_MSG_ERROR ( "PixelDetector element id hash is invalid = " << i );
      }
      const SiDetectorElement * element = m_detManager->getDetectorElement(idHash);
      if (element) {
        ATH_MSG_DEBUG ( 
                 "In digitize of untouched elements: layer - phi - eta  " 
              << m_detID->layer_disk(element->identify() ) << " - "  
              << m_detID->phi_module(element->identify() ) << " - "
              << m_detID->eta_module(element->identify() ) << " - "
              << "size: " << m_processedElements.size() 
              );  
        
        //if ( m_detID->barrel_ec(element->identify()) == 0 && m_detID->layer_disk(element->identify()) == 0 ) {
        //  ATH_MSG_DEBUG ( "in digitize of untouched elements in innermost barrel: eta - phi  "
        //      << m_detID->eta_module(element->identify() ) << " - "
        //      << m_detID->phi_module(element->identify() ) << " - "
        //      << "size: " << m_processedElements.size()
        //      );
        //}

        // Have a flag to check if the module is present or not
        // Generally assume it is: 
        //          m_module_status = 0;     // module is present we assume
        
        // Pixel EndCap A cosmic test:
        // ---------------------------
        // This is the place where we check on the status of the pixel
        // endcap module. Whether it was in the readout or not...
        //
	
	
        chargedDiodes->setDetectorElement(element);
        ATH_MSG_DEBUG ( "calling digitizeElements() for NON hits" );
        applyProcessorTools(chargedDiodes);
	
        // Create and store RDO and SDO
        // Don't create empty ones.
        if (!chargedDiodes->empty()) {
          StatusCode sc = createAndStoreRDO( chargedDiodes);
          if (sc.isSuccess()) // error msg is given inside createAndStoreRDO()
            addSDO(chargedDiodes);
        }
        chargedDiodes->clear();
      }
    }
  }
  delete chargedDiodes;
  return;
}


//---------------------------------------------------------------------------
// Digitize the list of hits:
//---------------------------------------------------------------------------
bool PixelDigitizationTool::digitizeElement(SiChargedDiodeCollection* chargedDiodes) {

  if (0 == m_thpcsi ) {
    ATH_MSG_ERROR ( "BUG TRAP: tphcsi==0 although it should not be the case" );
    return false;
  }

  // get the iterator pairs for this DetEl
 
 TimedHitCollection<SiHit>::const_iterator i, e;
  if(m_thpcsi->nextDetectorElement(i, e) == false) { // no more hits
    delete m_thpcsi;
    m_thpcsi = 0;
    return false;	
  }
	
  // create the identifier for the collection:
  ATH_MSG_DEBUG ( "create ID for the hit collection" );
  Identifier id;
  const TimedHitPtr<SiHit>& firstHit = *i;

  int Barrel = firstHit->getBarrelEndcap();

  // For testbeam
  if(m_atlasID == NULL) id = 0;
  else {
      const PixelID* PID = dynamic_cast<const PixelID*>(m_atlasID);
      if (PID==0) {
        ATH_MSG_ERROR ( "expected a PixelID but failed..." );
        return false;
      }
      id = PID->wafer_id(Barrel, firstHit->getLayerDisk(), firstHit->getPhiModule(), firstHit->getEtaModule());
  }
	
  // get the det element from the manager
  SiDetectorElement* sielement = m_detManager->getDetectorElement(id);
 
  if (sielement==0) {
    ATH_MSG_DEBUG("Barrel=" << Barrel << " layer=" << firstHit->getLayerDisk() << " Eta=" << firstHit->getEtaModule() << " Phi=" << firstHit->getPhiModule());
    ATH_MSG_ERROR ( "detector manager could not find element with id = " << id );
    return false;
  }
  // create the charged diodes collection
  chargedDiodes->setDetectorElement(sielement);

  // Loop over the hits and created charged diodes:
  while (i != e) {
  
    TimedHitPtr<SiHit> phit(*i++);

    //skip hits which are more than 10us away
    if(fabs(phit->meanTime() ) < 10000. *CLHEP::ns ) { 
      ATH_MSG_DEBUG("HASH = " << m_detID->wafer_hash(m_detID->wafer_id(phit->getBarrelEndcap(),phit->getLayerDisk(),phit->getPhiModule(),phit->getEtaModule())));
      ATH_MSG_DEBUG ( "calling process() for all methods" );
      if  (m_SurfaceChargesTool->process(phit,*chargedDiodes,*sielement)==StatusCode::FAILURE) return false;
      ATH_MSG_DEBUG ( "charges filled!" );
    }
  }	
  applyProcessorTools(chargedDiodes) ;       //!< Use of the new AlgTool surface charges generator class	
  return true;
}

//------------------------------------------------------------------------------
// Applies processors to the current detector element for the current element:
//------------------------------------------------------------------------------
void PixelDigitizationTool::applyProcessorTools(SiChargedDiodeCollection* chargedDiodes) {
	ATH_MSG_DEBUG ("applyProcessorTools()" );
	int processorNumber = 0;
	
	for(std::list<ISiChargedDiodesProcessorTool *>::iterator p_proc= m_diodesProcsTool.begin();p_proc!=m_diodesProcsTool.end() ; ++p_proc) {
		(*p_proc)->process(*chargedDiodes);
		
		processorNumber++;
		ATH_MSG_DEBUG("Applied processor # " << processorNumber);
	}
}

//------------------------------------------------------------
// Get next event and extract collection of hit collections:
//------------------------------------------------------------
StatusCode PixelDigitizationTool::getNextEvent() {
  ATH_MSG_DEBUG ( "PixelDigitizationTool::getNextEvent()" );
  //  get the container(s)
  typedef PileUpMergeSvc::TimedList<SiHitCollection>::type TimedHitCollList;
  //this is a list<pair<time_t, DataLink<SiHitCollection> >
  TimedHitCollList hitCollList;
  unsigned int numberOfSiHits(0);
  if (!(m_mergeSvc->retrieveSubEvtsData(m_inputObjectName, hitCollList, numberOfSiHits).isSuccess()) && hitCollList.size()==0) {
    ATH_MSG_ERROR ( "Could not fill TimedHitCollList" );
    return StatusCode::FAILURE;
  }
  else
    {
      ATH_MSG_DEBUG ( hitCollList.size()<< " SiHitCollections with key " << m_inputObjectName << " found" );
    }
  // create a new hits collection
  m_thpcsi = new TimedHitCollection<SiHit>(numberOfSiHits) ;
  //now merge all collections into one
  TimedHitCollList::iterator iColl(hitCollList.begin());
  TimedHitCollList::iterator endColl(hitCollList.end());
  while (iColl != endColl) {
    //decide if this event will be processed depending on HardScatterSplittingMode & bunchXing
    if (m_HardScatterSplittingMode == 2 && !m_HardScatterSplittingSkipper ) { m_HardScatterSplittingSkipper = true; ++iColl; continue; }
    if (m_HardScatterSplittingMode == 1 && m_HardScatterSplittingSkipper )  { ++iColl; continue; }
    if (m_HardScatterSplittingMode == 1 && !m_HardScatterSplittingSkipper ) { m_HardScatterSplittingSkipper = true; }
    const SiHitCollection* p_collection(iColl->second);
    m_thpcsi->insert(iColl->first, p_collection);
    ATH_MSG_DEBUG ( "SiTrackerHitCollection found with"<<p_collection->size()<<" hits");    // loop on the hit collections
    ++iColl;
  }
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
    if(real_particle_hit || m_createNoiseSDO) m_simDataColl->insert(std::make_pair( collection->getId((*i_chargedDiode).first), InDetSimData(deposits,(*i_chargedDiode).second.flag())));
  }
}


// create RDO collection for Special Pixels
// will create just one collection per event
void PixelDigitizationTool::createRDOforSPM() {
  if (!m_doRDOforSPM) return;
  if (m_rdoContainerSPM==0) return;
  //
  IdentifierHash idhash = 0;
  PixelRDO_Collection* coll = new PixelRDO_Collection(idhash);
  const DetectorSpecialPixelMap *spmap = getSpecialPixelMap();
  DetectorSpecialPixelMap::const_iterator spmBegin = spmap->begin();
  DetectorSpecialPixelMap::const_iterator spmEnd   = spmap->end();
  //
  unsigned int pixfe;
  unsigned int pixcol;
  unsigned int pixrow;
  //
  // loop over special pixel map and make RDOs
  for (; spmBegin != spmEnd; ++spmBegin) {
    IdentifierHash moduleHash(spmBegin - spmap->begin());
    Identifier modid = m_detID->wafer_id(moduleHash);
    const ModuleSpecialPixelMap* moduleMap = spmap->module(moduleHash);    
    if (moduleMap == 0) {
      ATH_MSG_DEBUG ( "BUG TRAP - special modulemap ptr == 0!!!" <<
		     " for module " << moduleHash );
      delete coll;
      return;
    }
    //int ecb = m_detID->barrel_ec(mmod);
    //int phi = m_detID->phi_module(mmod);
    int ecb = m_detID->barrel_ec(modid);
    int phi = m_detID->phi_module(modid);
    for(ModuleSpecialPixelMap::const_iterator IterSpe = moduleMap->begin(); IterSpe!=moduleMap->end(); IterSpe++){
      unsigned int pixelID = IterSpe->first;
      unsigned int pixelID0 = pixelID;
      unsigned int chip = pixelID & 0xF;
      pixelID >>=4;
      pixcol = pixelID & 0x1F;
      pixelID >>=5;
      pixrow = pixelID;
      if (chip >= moduleMap->chipsPerModule()){
        pixfe = chip-moduleMap->chipsPerModule();
      } else {
        pixfe  = moduleMap->chipsPerModule()-chip-1;
        pixcol = moduleMap->columnsPerChip()-pixcol-1;
        pixrow = 164-pixrow-1;
      }  	 
     if ( (ecb!=0) && (phi%2==0) ) pixrow=164-pixrow-1;
     int nToT=100;
      Identifier idpix = m_pixelIdMapping->getPixelId(modid,pixfe,pixrow,pixcol);
      Pixel1RawData *p_rdo= new Pixel1RawData(idpix, nToT, 0, 0, 0 );
      coll->push_back(p_rdo);
      ATH_MSG_DEBUG ("RDO for SPM "<< pixelID0 <<","<<pixfe<<","<<pixcol<<","<<pixrow<<","<<idpix<<" status= " <<
      moduleMap->pixelStatus(pixelID0));
    }   

    // 
    // Count noisy pixels in special regions (special chips or special column pairs)
    // 
    if(moduleMap->hasSpecialChips()||moduleMap->hasSpecialColumnPairs()){
      for(unsigned int ichip = 0; ichip < moduleMap->chipsPerModule(); ichip++){
        if(moduleMap->chipStatus(ichip)==0){
          if(moduleMap->hasSpecialColumnPairs(ichip)){
	    for(unsigned int icolp = 0; icolp < moduleMap->columnsPerChip()/2; icolp++){
	      if(moduleMap->columnPairStatus(ichip,icolp)!=0 && moduleMap->columnPairStatus(ichip,icolp) & (1 << 15)){
                for (unsigned int irow = 0; irow < 164; irow++){
	          for (unsigned int icol = 2*icolp; icol < 2*(icolp+1); icol++) {
		    int nToT = 100;
		    Identifier idpix = m_pixelIdMapping->getPixelId(modid,ichip,irow,icol);
                    Pixel1RawData *p_rdo= new Pixel1RawData(idpix, nToT, 0, 0, 0 );
                    coll->push_back(p_rdo);
                    ATH_MSG_DEBUG ("RDO for SPM = ? " <<","<<ichip<<","<<icol<<","<<irow<<","<<idpix);
		  }   
                }
	      }  
            }
          }
        }
        else{
          if(moduleMap->chipStatus(ichip) & (1 << 15)){
            for (unsigned int irow = 0; irow < 164; irow++){
	      for (unsigned int icol = 0; icol < moduleMap->columnsPerChip(); icol++){
		int nToT = 100;
		Identifier idpix = m_pixelIdMapping->getPixelId(modid,ichip,irow,icol);
                Pixel1RawData *p_rdo= new Pixel1RawData(idpix, nToT, 0, 0, 0 );
                coll->push_back(p_rdo);
                ATH_MSG_DEBUG ("RDO for SPM = ? " <<","<<ichip<<","<<icol<<","<<irow<<","<<idpix);
              }
            }
          }
        } 
      } 
    } 
  }
  m_rdoContainerSPM->addCollection( coll, coll->identifyHash() );
}

// initialize random generator
StatusCode PixelDigitizationTool::initRandom()
{

  //
  // get the Rndmsvc
  //
  if ( m_rndmSvc.retrieve().isFailure() ) {
    ATH_MSG_ERROR ( " Can't get RndmSvc " );
    return StatusCode::FAILURE;
  } else { ATH_MSG_DEBUG ( "Retrieved RndmSvc" );
  
  }
  //
  // get the PixelDigitizationTool random stream
  //
  ATH_MSG_DEBUG ( "Getting random number engine : <" << m_rndmEngineName << ">" );
  m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
  if (m_rndmEngine==0) {
    ATH_MSG_ERROR ( "Could not find RndmEngine : " << m_rndmEngineName );
    return StatusCode::FAILURE;
  } else { ATH_MSG_DEBUG ( " Found RndmEngine : " << m_rndmEngineName );  
  }
  //
  return StatusCode::SUCCESS;
}

// initialize services
StatusCode PixelDigitizationTool::initServices()
{
  StatusCode result;
  if (!m_mergeSvc.retrieve().isSuccess()) {
	ATH_MSG_ERROR("Could not find PileUpMergeSvc" );
	return StatusCode::FAILURE;
  }
  if (!m_rndmSvc.retrieve().isSuccess()) {
	ATH_MSG_ERROR("Could not find given RndmSvc" );
	return StatusCode::FAILURE;
  }
  if (!m_TimeSvc.retrieve().isSuccess()) {
	ATH_MSG_ERROR("Could not find given TimeSvc" );
	return StatusCode::FAILURE;
  } 
  if (!m_CalibSvc.retrieve().isSuccess()) {
	ATH_MSG_ERROR("Could not find given CalibSvc" );
	return StatusCode::FAILURE;
  }

  if ( !m_offlineCalibSvc.empty() ) {
    StatusCode sc = m_offlineCalibSvc.retrieve();
    if (sc.isFailure() || !m_offlineCalibSvc ) {
      ATH_MSG_ERROR( m_offlineCalibSvc.type() << " not found! ");
      return StatusCode::RECOVERABLE;
    }
    else{
      ATH_MSG_INFO ( "Retrieved tool " <<  m_offlineCalibSvc.type() );
    }
  }
  
  //
  // get SpecialPixelMapSvc
  //
  if (m_CalibSvc->usePixMapCDB()) {
    if ( ! m_specialPixelMapSvc.retrieve().isSuccess() ) {
      ATH_MSG_FATAL ( "Unable to retrieve SpecialPixelMapSvc" );
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG ( "Retrieved SpecialPixelMapSvc" );
    //
    result = detStore()->regFcn(&ISpecialPixelMapSvc::IOVCallBack,
                                dynamic_cast<ISpecialPixelMapSvc*>(m_specialPixelMapSvc.operator->()),
                                &PixelDigitizationTool::specialPixelMapCallBack,
                                this);
    if ( ! result.isSuccess() ) {
      ATH_MSG_FATAL ( "Unable to register call back for special pixel map" );
      return StatusCode::FAILURE;
    }
  } 

  return result;
}

// initialize tools
StatusCode PixelDigitizationTool::initTools()
{
  StatusCode result;

  result = m_SurfaceChargesTool.retrieve();
  if ( result != StatusCode::SUCCESS ) {
    ATH_MSG_ERROR ( "Can't get tool SurfaceChargesTool" );
    return result;
  } else {
    ATH_MSG_DEBUG ( "Tool SurfaceChargesTool retrieved" );
  }

 if (m_enableNoise) {
    result = m_pixelNoisyCellGenerator.retrieve();
    if ( result != StatusCode::SUCCESS ) {
      ATH_MSG_ERROR ( "Can't get tool PixelNoisyCellGenerator" );
      return result;
    } else {
      ATH_MSG_DEBUG ( "Tool PixelNoisyCellGenerator retrieved" );
    }
  }

  result = m_pixelGangedMerger.retrieve();
  if ( result != StatusCode::SUCCESS ) {
    ATH_MSG_ERROR ( "Can't get tool PixelGangedMerger" );
    return result;
  } else {
    ATH_MSG_DEBUG ( "Tool PixelGangedMerger retrieved" );
  }

  if (m_enableSpecialPixels) {
    result = m_specialPixelGenerator.retrieve();
    if ( result != StatusCode::SUCCESS ) {
      ATH_MSG_ERROR ( "Can't get tool SpecialPixelGenerator" );
      return result;
    } else {
      ATH_MSG_DEBUG ( "Tool SpecialPixelGenerator retrieved" );
    }
  }

  result = m_pixelChargeSmearer.retrieve();
  if ( result != StatusCode::SUCCESS ) {
    ATH_MSG_ERROR ( "Can't get tool PixelChargeSmearer" );
    return result;
  } else {
    ATH_MSG_DEBUG ( "Tool PixelChargeSmearer retrieved" );
  }

  result = m_pixelDiodeCrossTalkGenerator.retrieve();
  if ( result != StatusCode::SUCCESS ) {
    ATH_MSG_ERROR ( "Can't get tool PixelDiodeCrossTalkGenerator" );
    return result;
  } else {
    ATH_MSG_DEBUG ( "Tool PixelDiodeCrossTalkGenerator retrieved" );
  }


  result = m_pixelCellDiscriminator.retrieve();
  if ( result != StatusCode::SUCCESS ) {
    ATH_MSG_ERROR ( "Can't get tool PixelCellDiscriminator" );
    return result;
  } else {
    ATH_MSG_DEBUG ( "Tool PixelCellDiscriminator retrieved" );
  }

  // Only used if m_doRDOforSPM true. 
  if (m_doRDOforSPM) {
    result = m_pixelIdMapping.retrieve();
    if ( result != StatusCode::SUCCESS ) {
      ATH_MSG_ERROR ( "Can't get tool PixelIdMapping" );
      return result;
    } else {
      ATH_MSG_DEBUG ( "Tool PixelIdMapping retrieved" );
    }
  } 

  result = m_pixelRandomDisabledCellGenerator.retrieve();
  if ( result != StatusCode::SUCCESS ) {
    ATH_MSG_ERROR ( "Can't get tool PixelRandomDisabledCellGenerator" );
    return result;
  } else {
    ATH_MSG_DEBUG ( "Tool PixelRandomDisabledCellGenerator retrieved" );
  }
  return result;
}

// initialize managers etc
StatusCode PixelDigitizationTool::initExtras()
{
  //
  // get the Pixel detector manager
  //
  const PixelDetectorManager *pixmgr;
  if ( ! detStore()->retrieve(pixmgr, m_managerName).isSuccess() ) {
    ATH_MSG_ERROR ( "Could not locate PixelDetectorManager: <" << m_managerName << ">" );
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG ( "PixelDetectorManager: <" << m_managerName << "> retrieved" );
  setManager(pixmgr);
  //  
  // get PixelID
  //
  if ( detStore()->retrieve(m_detID,"PixelID").isFailure() ) {
    // if this fails, it's probably a bug -> FATAL!
    ATH_MSG_FATAL ( "Could not get Pixel ID helper" );
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG ( "Pixel ID helper retrieved" );
  store(m_detID);

  return StatusCode::SUCCESS;
}

//
StatusCode PixelDigitizationTool::createAndStoreRDO(SiChargedDiodeCollection *chDiodeCollection)
{
  // Create the RDO collection
  PixelRDO_Collection *RDOColl=this->createRDO(chDiodeCollection);
  
  
  //StatusCode sc=m_storeGate->record(RDOColl,key_rdo);
  StatusCode sc=m_rdoContainer->addCollection(RDOColl,RDOColl->identifyHash());
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "Pixel RDOs could not be added to container !"
          );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( "Pixel RDOs '" << RDOColl->identifyHash() << "' added to container"
          );
  }
  /*
  //symlink the collection
  InDetRawDataCollection<PixelRDORawData> *p_symcoll=0;
  sc=evtStore()->symLink(RDOColl,p_symcoll);
  if (sc.isFailure()) {
  ATH_MSG_FATAL ( "Pixel RDOs could not be symlinked in StoreGate !"
  );
  return StatusCode::FAILURE;
  } else {
  ATH_MSG_DEBUG ( "Pixel RDOs '" << key_rdo << "' symlinked in StoreGate"
  );
  } */
  return StatusCode::SUCCESS;
}

PixelRDO_Collection*
PixelDigitizationTool::createRDO(SiChargedDiodeCollection *collection)
{
  //
  // Create the collection
  //
  
  Identifier collID = collection->identify();
  ATH_MSG_DEBUG ( "PixelDigitizationTool::createRDO() collection : "
        << m_detID->show_to_string( collID ) );
  IdentifierHash idhash = collection->identifyHash();
  PixelRDO_Collection* PixelRDOColl = new PixelRDO_Collection(idhash);
  PixelRDOColl->setIdentifier(collID );

  //
  // Determine the Pixel Region (IBL, BL, L1, L2, EC, DBM, ITk 5th layer), to be used for latency, ToT cut and duplication:
  //
  
  const PixelID* pixelId = dynamic_cast<const PixelID *>(collection->element()->getIdHelper());
  int barrel_ec  = pixelId->barrel_ec(collection->element()->identify());
  int layerIndex = pixelId->layer_disk(collection->element()->identify());
  if (layerIndex>=4)     { layerIndex=6; }  // ITk 5th layer
  if (abs(barrel_ec)==2) { layerIndex=4; }  // disks
  if (abs(barrel_ec)==4) { layerIndex=5; }  // DBM

  const PixelModuleDesign *p_design = dynamic_cast<const PixelModuleDesign*>(&(collection->element())->design());
  std::vector<Pixel1RawData*> p_rdo_small_fei4;
  int maxFEI4SmallHit = 2;
  int nSmallHitsFEI4 = 0;
  std::vector<int> row;
  std::vector<int> col;
  const int maxRow = p_design->rowsPerCircuit();
  const int maxCol = p_design->columnsPerCircuit();
  std::vector < std::vector < int > > FEI4Map ( maxRow, std::vector < int > ( maxCol) );
  ATH_MSG_DEBUG ( "layerIndex = " << layerIndex << " MaxRow = " << maxRow << " MaxCol = " << maxCol);
  
  //
  
  m_overflowIBLToT = (!m_offlineCalibSvc.empty() ?  m_offlineCalibSvc->getIBLToToverflow() : 16);
  ATH_MSG_DEBUG ( " ***** Overflow for IBL = " << m_overflowIBLToT );
  
  if ( m_overflowIBLToT == 14 ) maxFEI4SmallHit = 0;
  if ( m_overflowIBLToT == 15 ) maxFEI4SmallHit = 1;
  if ( m_overflowIBLToT == 16 ) maxFEI4SmallHit = 2;

  //
  // ToT scale, to be used for FEI4, which has at most 4 bits for ToT,
  // so physical ToT values are from 1 to 15
  //
  // Loop over the charged diodes.
  //
  for(SiChargedDiodeIterator i_chargedDiode=collection->begin();
      i_chargedDiode!=collection->end(); ++i_chargedDiode) {
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
    Identifier diodeID = collection->getId((*i_chargedDiode).first);

    //  unsigned int flagmask = (*i_chargedDiode).second.flag() & 0xFE ;
    //  if(!flagmask ) {
    //
    // Check if diode is masked out or has been disable by random mapping - if not, create the RDO.
    //
    if (!( SiHelper::isMaskOut((*i_chargedDiode).second) || SiHelper::isDisabled((*i_chargedDiode).second) )) {
      //
      // calculate ToT from the total charge, add gaussian smearing
      // take the integer value to get the actual observed ToT
      // minimum ToT = 1
      //
      int nToT   = m_CalibSvc->getTot( diodeID, (*i_chargedDiode).second.charge() );
      // Fix for IBL, it sets the most probable value for a mip to 8.
      // Need to keep it hardcoded for a cache (no header change)
      // but when moving to full release, one has to accomodate additional 
      // properties and a proper function.
      // - HitDiscriminator configuration
      // - Referenca value (a mip)
      // - ToT for that reference value
      
      if (p_design->getReadoutTechnology()==PixelModuleDesign::FEI4) {
        const PixelID* pixelId = dynamic_cast<const PixelID *>(collection->element()->getIdHelper());
	if (pixelId->is_dbm(collection->element()->identify())) {
	  nToT = 8*((*i_chargedDiode).second.charge() - 1200. )/(8000. - 1200.);
	}
        if ( nToT<=0 ) nToT=1;
        else if ( nToT == 2 && m_overflowIBLToT == 16) nToT = 1; 
	else if ( nToT >= m_overflowIBLToT ) nToT = m_overflowIBLToT;
      }
      int flag   = (*i_chargedDiode).second.flag();
      int bunch  = (flag >>  8) & 0xff;
      //
      ATH_MSG_DEBUG("RDO: " << (*i_chargedDiode).second.charge() << "  " << nToT << "  " << bunch << "  " << flag << "  " << layerIndex);
      
      if (p_design->getReadoutTechnology()==PixelModuleDesign::FEI3) {
        if ( nToT > m_maxToT.at(layerIndex) ) continue; // skip hits with ToT exceeding LVL1 Latency
        if ( nToT < m_minToT.at(layerIndex) ) continue; // skip hits with ToT less than ToT cut
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
      //
      
      SiReadoutCellId cellId=(*i_chargedDiode).second.getReadoutCell();
      const Identifier id_readout = collection->element()->identifierFromCellId(cellId);

      int iirow = cellId.phiIndex();
      int iicol = cellId.etaIndex();
      if (p_design->getReadoutTechnology()==PixelModuleDesign::FEI4 && iicol>=maxCol) { iicol = iicol - maxCol; } // FEI4 copy mechanism works per FE.

      //
      //if (correct_id_readout!=diodeID) {
      //  ATH_MSG_DEBUG ( "correct_readout_id != diodeID" );
      // }
      // Create hit only if bunch within the acceptance (not for IBL and DBM):
      
      if (p_design->getReadoutTechnology()!=PixelModuleDesign::FEI4) {
        if ( bunch >= 0 && bunch < m_TimeSvc->getTimeBCN()) {
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
        if ( m_applyDupli.at(layerIndex)) {
          if ( nToT <= m_maxToTForDupli.at(layerIndex) && bunch > 0 && bunch <= m_TimeSvc->getTimeBCN()) {
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
//////////////////////////////////////////////////////////////////////////////


void PixelDigitizationTool::addCrossTalk() {
  //
  // Creating diode-level cross-talk generator
  // Use from calib cond db: discriminator threshold
  //
  ATH_MSG_DEBUG ( "creating diode cross-talk generator..." );
  storeTool(&(*m_pixelDiodeCrossTalkGenerator)) ;
}

void PixelDigitizationTool::addChargeSmearing() {
  // Creating a charge smearer - gaussian with amplitude <amp>
  //ATH_MSG_DEBUG ( "creating charge smearer with amplitude = " << amp );
  ATH_MSG_DEBUG ( "creating charge smearer");
    storeTool(&(*m_pixelChargeSmearer)) ;
}

void PixelDigitizationTool::addSpecialPixels() {
  // Creating special pixels generator
  // Uses special pixel map
  if (m_enableSpecialPixels) {
    ATH_MSG_DEBUG ( "creating special pixels generator..." );
      storeTool(&(*m_specialPixelGenerator)) ;
  }
}

void PixelDigitizationTool::addRandomDisabled() {
  // add random disabling of pixels
  // independant of SpecialPixelMap
  ATH_MSG_DEBUG ( "creating random disabling generator..." );
  storeTool(&(*m_pixelRandomDisabledCellGenerator)) ;
}

void PixelDigitizationTool::addNoise() {
  // Creating diode-level noise generator
  // Use from calib cond db: discriminator threshold + sigma
  // The probability for noise is given by RndNoiseProb.
  if (m_enableNoise) {
    ATH_MSG_DEBUG ( "creating diode noise generator..." );
    storeTool(&(*m_pixelNoisyCellGenerator)) ;
    
    m_pixelNoisyCellGenerator->setNoisyPixel(&m_noisyPixel);
  }
}

void PixelDigitizationTool::addGangedPixels() {
  //
  // Create ganged pixels handler
  //
  ATH_MSG_DEBUG ( "creating ganged pixels generator..." );
    storeTool(&(*m_pixelGangedMerger)) ;
}


void PixelDigitizationTool::addDiscriminator() {
  // Creating discriminator
  // Uses from cond db: threshold and its sigma
  ATH_MSG_DEBUG ( "creating discriminator with timewalk..." );
    storeTool(&(*m_pixelCellDiscriminator)) ;
}


const DetectorSpecialPixelMap* PixelDigitizationTool::getSpecialPixelMap() const { 
  return m_specialPixelGenerator->getDetectorMap(); 
}


void PixelDigitizationTool::setNextUpdate() {
  // set next event to update special pixels map
  if (!(m_eventCounter==m_eventNextUpdate)) return; // next update is pending
  //
  switch (m_IOVFlag) {
  case IOVonce: // only once
    m_eventNextUpdate=0;
    break;
  case IOVall:  // every event
    m_eventNextUpdate = m_eventCounter+1;
    break;
  case IOVstep: // every Nth event
    m_eventNextUpdate = m_eventCounter+m_eventIOV;
    break;
  case IOVrnd:  // random N
    m_eventNextUpdate = m_eventCounter+CLHEP::RandFlat::shootInt(m_rndmEngine,m_eventIOV)+1;
    break;
  default:      // once
    ATH_MSG_WARNING ( "got an unknown IOVFlag = " << m_IOVFlag );
    m_eventNextUpdate=0;
    break;
  }
  if (m_eventNextUpdate>0) {
    ATH_MSG_DEBUG ( "next special pixel map update scheduled for event counter = " << m_eventNextUpdate
          << "  -  current counter = " << m_eventCounter );
  } else {
    ATH_MSG_DEBUG ( "no more special pixel map updates!" );
  }
}
// Call back function:

StatusCode PixelDigitizationTool::specialPixelMapCallBack(IOVSVC_CALLBACK_ARGS_P(I,keys)){

  ATH_MSG_DEBUG ( "Special pixel map call back for key " << *keys.begin() << " number " << I );
  StatusCode sc = detStore()->retrieve(m_specialPixelMap,m_specialPixelMapKey);
  if ( !sc.isSuccess() ) {
    ATH_MSG_DEBUG ( " Unable to retrieve special pixel map " );
  }
  //   
  // Clear noisy pixel map:
  //
  m_noisyPixel.clear(); 
  //
  // Count number of noisy pixels in special pixel map and keep their ID in m_noisyPixel map:
  //
  unsigned int nmod = m_detID->wafer_hash_max();
  for (unsigned int i = 0; i < nmod; i++){
    int numNoisy = 0;
    ATH_MSG_DEBUG ( "********************************************************" );
    ATH_MSG_DEBUG ( " Noisy Pixels of Module " << i );
    ATH_MSG_DEBUG ( " numNoisy =  " << numNoisy );
    std::vector<unsigned int> noisypixel;
    IdentifierHash moduleHash = i;
    const ModuleSpecialPixelMap* moduleMap = m_specialPixelMap->module(moduleHash);   
    if (moduleMap == 0) {
      ATH_MSG_DEBUG ( "BUG TRAP - special modulemap ptr == 0!!!"
            << " for module "
            << moduleHash
            );
      return StatusCode::FAILURE;
    }
    for(ModuleSpecialPixelMap::const_iterator IterSpe = moduleMap->begin(); IterSpe!=moduleMap->end(); IterSpe++){
      unsigned int pixelID;
      pixelID = IterSpe->first;
      if(moduleMap->validPixelID(pixelID)){
        if(moduleMap->pixelNoisy(pixelID)){
          noisypixel.push_back(pixelID);
          numNoisy++;
          ATH_MSG_DEBUG ( " Noisy pixel number  " << numNoisy << " Pixel ID = " << pixelID );
        }
      }   
    }
    //
    // Count noisy pixels in special regions (special chips or special column pairs)
    //
    if(moduleMap->hasSpecialChips()||moduleMap->hasSpecialColumnPairs()){
      for(unsigned int ichip = 0; ichip < moduleMap->chipsPerModule(); ichip++){
        if(moduleMap->chipStatus(ichip)==0){
          if(moduleMap->hasSpecialColumnPairs(ichip)){
            for(unsigned int icolp = 0; icolp < moduleMap->columnsPerChip()/2; icolp++){
              if(moduleMap->columnPairStatus(ichip,icolp)!=0 && moduleMap->columnPairStatus(ichip,icolp) & (1 << 15)){
                for (unsigned int irow = 0; irow < moduleMap->rowsPerChip(); irow++){
                  for (unsigned int icol = 2*icolp; icol < 2*(icolp+1); icol++) {
                    noisypixel.push_back(moduleMap->encodePixelID(ichip,icol,irow));
                    numNoisy++;
                    ATH_MSG_DEBUG ( " Noisy pixel number  " << numNoisy << " Pixel ID = " << moduleMap->encodePixelID(ichip,icol,irow) );
                  }   
                }
              } 
            }
          }
        }
        else{
          if(moduleMap->chipStatus(ichip) & (1 << 15)){
            for (unsigned int irow = 0; irow < moduleMap->rowsPerChip(); irow++){
              for (unsigned int icol = 0; icol < moduleMap->columnsPerChip(); icol++){
                noisypixel.push_back(moduleMap->encodePixelID(ichip,icol,irow));
                numNoisy++;
                ATH_MSG_DEBUG ( " Noisy pixel number  " << numNoisy << " Pixel ID = " << moduleMap->encodePixelID(ichip,icol,irow) );
              }
            }
          }
        }
      }
    }
    ATH_MSG_DEBUG ( " Number of noisy Pixels = " << numNoisy );
    if (numNoisy > 0){
      m_noisyPixel.insert(std::pair<unsigned int,std::vector<unsigned int> >(i,noisypixel));   
      ATH_MSG_DEBUG ( "Module = " << i << " Noisy pixels = " << m_noisyPixel.find(i)->second );
      noisypixel.clear();   
    } else {
      ATH_MSG_DEBUG ( "No noisy pixels in module " << i << " Size = " << noisypixel.size() );
    } 
  }
  return sc;
} 


//----------------------------------------------------------------------
// PrepareEvent method:
//----------------------------------------------------------------------
StatusCode PixelDigitizationTool::prepareEvent(unsigned int) {
  ATH_MSG_VERBOSE("PixelDigitizationTool::prepareEvent()");

  /////////////////////////////////////////////////
  // update special pixel map
  /////////////////////////////////////////////////
  if (m_enableSpecialPixels) {
    if (!m_CalibSvc->usePixMapCDB()) {
      if (doUpdate()) {
        ATH_MSG_DEBUG ( "will update generated special pixel map this event" );
        m_specialPixelGenerator->setUpdate(); // set update flag
      }
      setNextUpdate(); // set next event for update
    }
    m_specialPixelGenerator->updatePixelMap();
  }

  //Create Output Containers
  ATH_MSG_DEBUG("Going to create output containers");
  if (createOutputContainers().isFailure()) return StatusCode::FAILURE;
  else ATH_MSG_DEBUG("Created output containers");
  ////////////// ComTime for Cosmic Simulation in Pit
  m_time_y_eq_zero = 0;
  if (m_useComTime) {
    if ( StatusCode::SUCCESS == evtStore()->retrieve(m_ComTime,"ComTime")) {
      m_time_y_eq_zero = m_ComTime->getTime() ;
      ATH_MSG_DEBUG ( "Found tool for cosmic/commissioning timing: ComTime" );
   
   } else { 
      ATH_MSG_WARNING ( "Did not find tool needed for cosmic/commissioning timing: ComTime" );
    }
  }

  //////////////////////////////
  m_TimeSvc->setComTime( m_time_y_eq_zero );

  m_processedElements.clear();
  m_processedElements.resize(m_detID->wafer_hash_max(),false);
  m_thpcsi = new TimedHitCollection<SiHit>(); 
  chargedDiodes = new SiChargedDiodeCollection;
  m_HardScatterSplittingSkipper = false;
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
// MergeEvent method:
//----------------------------------------------------------------------
StatusCode PixelDigitizationTool::mergeEvent() {
  ATH_MSG_VERBOSE("PixelDigitizationTool::mergeEvent()");
  if (m_enableHits) digitizeAllHits(); 
  delete chargedDiodes;
  
  digitizeNonHits();

  createRDOforSPM();
  // increment event counter (for simulated IOV)
  m_eventCounter++;
  for (std::vector<SiHitCollection*>::iterator it = hitCollPtrs.begin();it!=hitCollPtrs.end();it++) {
	(*it)->Clear();
	delete(*it);
  }
  hitCollPtrs.clear();

  ATH_MSG_DEBUG("Digitize success!");
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
// ProcessBunchXing method:
//----------------------------------------------------------------------
StatusCode PixelDigitizationTool::processBunchXing(int bunchXing, PileUpEventInfo::SubEvent::const_iterator bSubEvents, PileUpEventInfo::SubEvent::const_iterator eSubEvents) {
  ATH_MSG_VERBOSE("PixelDigitizationTool::processBunchXing() " << bunchXing); 
  //decide if this event will be processed depending on HardScatterSplittingMode & bunchXing
  if (m_HardScatterSplittingMode == 2 && !m_HardScatterSplittingSkipper ) { m_HardScatterSplittingSkipper = true; return StatusCode::SUCCESS; }
  if (m_HardScatterSplittingMode == 1 && m_HardScatterSplittingSkipper )  { return StatusCode::SUCCESS; }
  if (m_HardScatterSplittingMode == 1 && !m_HardScatterSplittingSkipper ) { m_HardScatterSplittingSkipper = true; }

  PileUpEventInfo::SubEvent::const_iterator iEvt = bSubEvents;
  for (; iEvt!=eSubEvents; iEvt++) {
    StoreGateSvc& seStore = *iEvt->pSubEvtSG;
    const EventInfo* pEI = 0;
    if (seStore.retrieve(pEI).isSuccess()) {
      ATH_MSG_DEBUG("SubEvt EventInfo from StoreGate " << seStore.name() << " :"
                   " bunch crossing : " << bunchXing << " time offset : " << iEvt->time()
                   << " event number : " << pEI->event_ID()->event_number()
                   << " run number : " << pEI->event_ID()->run_number());
    }
    const SiHitCollection* seHitColl(0);   
    if (!seStore.retrieve(seHitColl,m_inputObjectName).isSuccess()) {
      msg(MSG::ERROR) << "SubEvent Pixel SiHitCollection not found in StoreGate " << seStore.name() << endreq;
      return StatusCode::FAILURE;
    } 
    ATH_MSG_DEBUG("SiHitCollection found with " << seHitColl->size() << " hits");  
    PileUpTimeEventIndex timeIndex(iEvt->time(),iEvt->index());
    SiHitCollection *hitCollPtr = new SiHitCollection(*seHitColl);
    m_thpcsi->insert(timeIndex,hitCollPtr);
    hitCollPtrs.push_back(hitCollPtr);
  }

  return StatusCode::SUCCESS;
}
