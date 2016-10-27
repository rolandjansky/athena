/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////
// PixelLightDigitizationTool.cxx
//   Implementation file for class PixelLightDigitizationTool
////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
////////////////////////////////////////////////////////////////////////////

// Pixel digitization includes
#include "PixelLightDigitizationTool.h"
#include "TimeSvc.h"
#include "InDetSimEvent/SiHitCollection.h"

// Mother Package includes 
#include "SiDigitization/SiHelper.h"
#include "SiDigitization/SiChargedDiodeCollection.h"
#include "SiDigitization/SiSurfaceCharge.h"

// Det Descr
#include "Identifier/Identifier.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SiReadoutCellId.h"
#include "InDetReadoutGeometry/SiDetectorDesign.h"
#include "InDetReadoutGeometry/SiCellId.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetSimEvent/SiHit.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "InDetIdentifier/PixelID.h"
#include "PileUpTools/PileUpMergeSvc.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

// Gaudi includes
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PropertyMgr.h"
#include "AthenaKernel/errorcheck.h"

// DataHandle
#include "StoreGate/DataHandle.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

// Trk includes
#include "TrkDigEvent/RectangularSegmentation.h"
#include "TrkDigEvent/TrapezoidSegmentation.h"
#include "TrkDigEvent/DigitizationCell.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"

// Amg includes
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"

#include "CLHEP/Random/RandGauss.h"

//
#include <limits>
#include <cstdint>

using namespace InDetDD;

// Constructor with parameters:
PixelLightDigitizationTool::PixelLightDigitizationTool(const std::string &type, 
				     const std::string &name,
				     const IInterface * pIID) :
  PileUpToolBase(type,name,pIID),
  m_managerName("Pixel"),
  m_rdoCollName("PixelRDOs"),
  m_sdoCollName("PixelSDO_Map"),
  m_rdoContainer(NULL),
  m_simDataColl(NULL),
  m_detID(NULL),
  m_thpcsi(NULL),
  m_useLorentzAngle(true),
  m_smearing(true),
  m_smearingSigma(0.1),
  m_rndmSvc("AtDSFMTGenSvc",name),
  m_rndmEngine(0),    
  m_rndmEngineName("PixelDigitization"),
  m_pathCutOff(0.00),
  m_atlasID(NULL),
  m_detManager(NULL),
  m_mergeSvc("PileUpMergeSvc",name),
  m_TimeSvc("TimeSvc",name),
  m_eventCounter(0),
  m_inputObjectName("PixelHits"),
  m_digitizationStepper("Trk::PlanarModuleStepper" )
{
  
  declareInterface<PixelLightDigitizationTool>(this);
  
  declareProperty("TimeSvc",                 m_TimeSvc,              "Time Svc");
  declareProperty("InputObjectName",         m_inputObjectName="",   "Input Object name" );
  declareProperty("TimeSvc",                 m_TimeSvc,              "Time Svc");
  declareProperty("UseLorentzAngle",         m_useLorentzAngle,      "Enable Lorentz angle in digi");
  
  // smearing and cutting
  declareProperty("SmearingMode",            m_smearing);
  declareProperty("SmearingSigma",           m_smearingSigma);
  declareProperty("RndmSvc",                 m_rndmSvc,              "Random Number Service used in SCT & Pixel digitization");
  declareProperty("RndmEngine",              m_rndmEngineName,       "Random engine name");
  declareProperty("PathCutOff",              m_pathCutOff);

  // Get parameter values from jobOptions file
  declareProperty("ManagerName",             m_managerName,          "Pixel manager name");
  declareProperty("RDOCollName",             m_rdoCollName,          "RDO collection name");
  declareProperty("SDOCollName",             m_sdoCollName,          "SDO collection name");

  declareProperty("DigitizationStepper",     m_digitizationStepper);

}



// Initialize method:
StatusCode PixelLightDigitizationTool::initialize()
{
  ATH_MSG_DEBUG("PixelLightDigitizationTool::Initialize()");
  //  
  // Print parameter values

  // check the input object name
  if (m_inputObjectName=="") {
    ATH_MSG_FATAL ( "Property InputObjectName not set !" );
    return StatusCode::FAILURE;
  }
  else ATH_MSG_DEBUG ( "Input objects: '" << m_inputObjectName << "'" );

   //==============================
  // initialize services etc
  //==============================
  CHECK( initServices() );

  CHECK( initRandom() );

  CHECK( initTools() );

  CHECK( initExtras() );

  return StatusCode::SUCCESS;
}

StatusCode PixelLightDigitizationTool::finalize() {
  return StatusCode::SUCCESS;
}

// initialize services
StatusCode PixelLightDigitizationTool::initServices()
{
  if (!m_TimeSvc.retrieve().isSuccess()) {
    ATH_MSG_ERROR("Could not find given TimeSvc" );
    return StatusCode::FAILURE;
  } 
  if (!m_mergeSvc.retrieve().isSuccess()) {
    ATH_MSG_ERROR("Could not find PileUpMergeSvc" );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

// initialize random generator
StatusCode PixelLightDigitizationTool::initRandom()
{
  // intialize the random number generators
  if ( m_rndmSvc.retrieve().isFailure() ) {
    ATH_MSG_ERROR ( " Can't get RndmSvc " );
    return StatusCode::FAILURE;
  } else ATH_MSG_DEBUG ( "Retrieved RndmSvc" );

  m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
  if (m_rndmEngine==0) {
    ATH_MSG_ERROR ( "Could not find RndmEngine : " << m_rndmEngineName );
    return StatusCode::FAILURE;
  } else ATH_MSG_DEBUG ( " Found RndmEngine : " << m_rndmEngineName );

  return StatusCode::SUCCESS;
}


StatusCode PixelLightDigitizationTool::initTools()
{
  return StatusCode::SUCCESS;
}

// initialize managers etc
StatusCode PixelLightDigitizationTool::initExtras()
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

// Digitize method:
StatusCode PixelLightDigitizationTool::processAllSubEvents() 
{
  if (prepareEvent(0).isFailure()) return StatusCode::FAILURE;   
  ATH_MSG_VERBOSE("Begin digitizeAllHits");
  if (!getNextEvent().isFailure()) 
    digitizeAllHits();	
  else ATH_MSG_DEBUG("no hits found in event!");
  delete chargedDiodes;

  ATH_MSG_DEBUG("Digitized Elements with Hits");
  
  // increment event counter (for simulated IOV)
  m_eventCounter++;
 
  ATH_MSG_DEBUG("Digitize success!");
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------
// Get next event and extract collection of hit collections:
//------------------------------------------------------------
StatusCode PixelLightDigitizationTool::getNextEvent() {
  ATH_MSG_DEBUG ( "PixelDigitizationTool::getNextEvent()" );
  //  get the container(s)
  typedef PileUpMergeSvc::TimedList<SiHitCollection>::type TimedHitCollList;
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
    const SiHitCollection* p_collection(iColl->second);
    m_thpcsi->insert(iColl->first, p_collection);
    ATH_MSG_DEBUG ( "SiTrackerHitCollection found with"<<p_collection->size()<<" hits");    // loop on the hit collections
    ++iColl;
  }
  return StatusCode::SUCCESS;
}



//----------------------------------------------------------------------
// PrepareEvent method:
//----------------------------------------------------------------------
StatusCode PixelLightDigitizationTool::prepareEvent(unsigned int) {
  ATH_MSG_VERBOSE("PixelLightDigitizationTool::prepareEvent()");
  
  //Create Output Containers
  ATH_MSG_DEBUG("Going to create output containers");
  if (createOutputContainers().isFailure()) return StatusCode::FAILURE;
  else ATH_MSG_DEBUG("Created output containers");
 
  m_processedElements.clear();
  m_processedElements.resize(m_detID->wafer_hash_max(),false);
  m_thpcsi = new TimedHitCollection<SiHit>(); 
  chargedDiodes = new SiChargedDiodeCollection;
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
// createOutpuContainers method:
//----------------------------------------------------------------------
StatusCode PixelLightDigitizationTool::createOutputContainers() {
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

//----------------------------------------------------------------------
// ProcessBunchXing method:
//----------------------------------------------------------------------
StatusCode PixelLightDigitizationTool::processBunchXing(int bunchXing, PileUpEventInfo::SubEvent::const_iterator bSubEvents, PileUpEventInfo::SubEvent::const_iterator eSubEvents) {
  ATH_MSG_VERBOSE("PixelLightDigitizationTool::processBunchXing() " << bunchXing); 
    
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

//----------------------------------------------------------------------
// MergeEvent method:
//----------------------------------------------------------------------
StatusCode PixelLightDigitizationTool::mergeEvent() {
  ATH_MSG_VERBOSE("PixelLightDigitizationTool::mergeEvent()");
  digitizeAllHits(); 
  delete chargedDiodes;
  
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

void PixelLightDigitizationTool::digitizeAllHits() {
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

    
    IdentifierHash idHash = chargedDiodes->identifyHash();
    
    assert (idHash < m_processedElements.size());
    m_processedElements[idHash] = true;
   
    // Create and store RDO and SDO
    // Don't create empty ones.
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

//---------------------------------------------------------------------------
// Digitize the list of hits:
//---------------------------------------------------------------------------
bool PixelLightDigitizationTool::digitizeElement(SiChargedDiodeCollection* chargedDiodes) {

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
      ATH_MSG_DEBUG("hash  = " << m_detID->wafer_hash(m_detID->wafer_id(phit->getBarrelEndcap(),phit->getLayerDisk(),phit->getPhiModule(),phit->getEtaModule())));
      
      // getting entry and exit of the SiHit
      HepGeom::Point3D<double> localStartPosition = sielement->hitLocalToLocal3D(phit->localStartPosition());
      HepGeom::Point3D<double> localEndPosition   = sielement->hitLocalToLocal3D(phit->localEndPosition());
      
      Amg::Vector3D entryPoint(localStartPosition.x(),localStartPosition.y(),localStartPosition.z());
      Amg::Vector3D exitPoint(localEndPosition.x(),localEndPosition.y(),localEndPosition.z());

      Trk::DigitizationModule * m_digitizationModule = buildDetectorModule(sielement);
      if(!m_digitizationModule){
	ATH_MSG_FATAL( " could not get build detector module "); 
	return StatusCode::FAILURE;
      } 
      // Getting the steps in the sensor
      std::vector<Trk::DigitizationStep> digitizationSteps = m_digitizationStepper->cellSteps(*m_digitizationModule,entryPoint,exitPoint);
      //delete m_digitizationModule;
      
      // measuring the total path --> used to reweight
      double totalPath = 0.;
      std::vector < double > paths;
      
      for (auto& dStep : digitizationSteps) {
	// cut off applied
	if (dStep.stepLength < m_pathCutOff) continue;
	
	double smearedPath =  dStep.stepLength;
	switch (m_smearing) {
	  // no smearing applied 
	case 0 : break;
	  // smear gaussian with 
	case 1 : {
	  smearedPath *= (1+m_smearingSigma*CLHEP::RandGauss::shoot(m_rndmEngine));
	} break;
	}
	totalPath += smearedPath;
	paths.push_back(smearedPath);
      }
      
      unsigned int step = 0;
      for (auto& dStep : digitizationSteps){
	// cut off applied
	if (dStep.stepLength < m_pathCutOff) continue;
	
	// position on the diode map
	Trk::DigitizationCell cell(dStep.stepCell.first,dStep.stepCell.second);
	Amg::Vector2D PositionOnModule = m_digitizationModule->segmentation().cellPosition(cell);
	SiCellId diode = sielement->cellIdOfPosition(PositionOnModule);
	
	// weighted energy (using the path in the cell and the total path)
	double energy = phit->energyLoss()*paths.at(step++)/totalPath;
	SiCharge charge(energy,hitTime(phit),SiCharge::track,HepMcParticleLink(phit->trackNumber(),phit.eventId()));
	
	if (diode.isValid()) chargedDiodes->add(diode,charge);

	ATH_MSG_DEBUG ( "charges filled!" );
      }
      delete m_digitizationModule;
    }
  }	
  return true;
}

Trk::DigitizationModule* PixelLightDigitizationTool::buildDetectorModule(const InDetDD::SiDetectorElement* hitSiDetElement ) const {
  
  const InDetDD::PixelModuleDesign* design(dynamic_cast<const InDetDD::PixelModuleDesign*>(&hitSiDetElement->design()));
  
  if (!design) {
    ATH_MSG_DEBUG ( "Could not get design"<< design) ;
    return nullptr;
  }
  
    //Read from the SiDetectorElement information to build the digitization module 
    const double halfThickness = hitSiDetElement->thickness() * 0.5;
    const double halfWidth     = design->width() * 0.5;
    const double halfLenght    = design->length() * 0.5;
    int binsX = design->rows();
    int binsY = design->columns();
    int numberOfChip = design->numberOfCircuits();

    ATH_MSG_VERBOSE("Retrieving infos: halfThickness = " << halfThickness << " --- halfWidth = " << halfWidth << " --- halfLenght = " << halfLenght );
    ATH_MSG_VERBOSE("Retrieving infos: binsX = " << binsX << " --- binsY = " << binsY << " --- numberOfChip = " << numberOfChip);
    
    int readoutDirection = design->readoutSide();
    float lorentzAngle   = m_useLorentzAngle ? hitSiDetElement->hitDepthDirection()*hitSiDetElement->hitPhiDirection()*std::atan(hitSiDetElement->getTanLorentzAngle()) : 0.; 

    // added for degugging
    //double shift =  m_useLorentzAngle ? hitSiDetElement->getLorentzCorrection() : 0.;
    // std::cout << "barrel_ec  = " << m_detID->barrel_ec(hitSiDetElement->identify())
    //           << " --  layer_disk = " << m_detID->layer_disk(hitSiDetElement->identify())
    //           << " --  eta_module = " << m_detID->eta_module(hitSiDetElement->identify()) 
    //           << " --  lorentzAngle = " << lorentzAngle
    // 	      << " --  lorentzCorrection = " << shift
    // 	      << " --  readoutDirection = " << readoutDirection << std::endl;
    // std::cout << "element->hitDepthDirection() = " << hitSiDetElement->hitDepthDirection()
    // 	      << " --  element->hitPhiDirection() = " << hitSiDetElement->hitPhiDirection() << std::endl;
    
    // rectangle bounds 
    std::shared_ptr<const Trk::RectangleBounds> rectangleBounds(new Trk::RectangleBounds(halfWidth,halfLenght));    
    ATH_MSG_VERBOSE("Initialized rectangle Bounds");
    // create the segmentation
    std::shared_ptr<const Trk::Segmentation> rectangleSegmentation(new Trk::RectangularSegmentation(rectangleBounds,(size_t)binsX,(size_t)binsY));
    // build the module
    ATH_MSG_VERBOSE("Initialized rectangleSegmentation");
    Trk::DigitizationModule * m_digitizationModule = new Trk::DigitizationModule(rectangleSegmentation, 
										 halfThickness,
										 readoutDirection,
										 lorentzAngle);
    ATH_MSG_VERBOSE("Building Rectangle Segmentation with dimensions (halfX, halfY) = (" << halfWidth << ", " << halfLenght << ")");
    
    // success return
    return m_digitizationModule;   
}

//
StatusCode PixelLightDigitizationTool::createAndStoreRDO(SiChargedDiodeCollection *chDiodeCollection)
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
  return StatusCode::SUCCESS;
}



//-----------------------------------------------------------------------------------------------
// Convert a SiTotalCharge to a InDetSimData, and store it. (this needs working...)
//-----------------------------------------------------------------------------------------------
void PixelLightDigitizationTool::addSDO(SiChargedDiodeCollection* collection) {

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
      if (barcode == 0) continue;
      
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
    if(real_particle_hit) m_simDataColl->insert(std::make_pair( collection->getId((*i_chargedDiode).first), InDetSimData(deposits,(*i_chargedDiode).second.flag())));
  }
}


PixelRDO_Collection*
PixelLightDigitizationTool::createRDO(SiChargedDiodeCollection *collection)
{
  //
  // Create the collection
  //
  
  Identifier collID = collection->identify();
  ATH_MSG_DEBUG ( "PixelLightDigitizationTool::createRDO() collection : "
        << m_detID->show_to_string( collID ) );
  IdentifierHash idhash = collection->identifyHash();
  PixelRDO_Collection* PixelRDOColl = new PixelRDO_Collection(idhash);
  PixelRDOColl->setIdentifier(collID );

  //
  // Determine the Pixel Region (IBL, BL, L1, L2, EC or DBM), to be used for latency, ToT cut and duplication:
  //
  
  const PixelID* pixelId = dynamic_cast<const PixelID *>(collection->element()->getIdHelper());
  int barrel_ec  = pixelId->barrel_ec(collection->element()->identify());
  int layer_disk = pixelId->layer_disk(collection->element()->identify());
  int PixRegion = -1;
  if ( barrel_ec == 0 ) {
      // PixRegion = 0, 1, 2, 3 for the first four pixel barrel layers,
      //             6 for the fifth pixel barrel layer and higher (4 and 5 already taken)
      PixRegion = layer_disk;
      if ( layer_disk >= 4 ) PixRegion = 6;
  }
  if ( barrel_ec == 2 || barrel_ec == -2 ) PixRegion = 4; // 4 for disks
  if ( barrel_ec == 4 || barrel_ec == -4 ) PixRegion = 5; // 5 for DBM
  ATH_MSG_DEBUG ( "doITk , bec " << barrel_ec << ", layer " << layer_disk << ", PixRegion " << PixRegion );

  if ( PixRegion < 0 || PixRegion > 6 ) {
    ATH_MSG_ERROR ( "PixelLightDigitizationTool::createRDO() collection : " << " bad Barrel/EC or Layer/Disk " );
  }

  const PixelModuleDesign *p_design = dynamic_cast<const PixelModuleDesign*>(&(collection->element())->design());
  const int maxRow = p_design->rowsPerCircuit();
  const int maxCol = p_design->columnsPerCircuit();
  
  ATH_MSG_DEBUG ( "PixRegion = " << PixRegion << " MaxRow = " << maxRow << " MaxCol = " << maxCol);
  
  //
  // Loop over the charged diodes.
  //
  
  InDetDD::SiLocalPosition meanpos(0,0,0);
  float totalChargeNorm = 0.;
  for(SiChargedDiodeIterator i_chargedDiode=collection->begin();
      i_chargedDiode!=collection->end(); ++i_chargedDiode) {
    //
    // Check if diode is masked out or has been disable by random mapping - if not, create the RDO.
    //
    if (!( SiHelper::isMaskOut((*i_chargedDiode).second) || SiHelper::isDisabled((*i_chargedDiode).second) )) {

      // This is defining the ~600e threshold
      int nToT   = round((*i_chargedDiode).second.charge()*1000.*0.5); //charge instead of ToT
      if (nToT==0) continue; 
	
      nToT       = (nToT > 255) ? 255 : nToT; //charge instead of ToT

      int flag   = (*i_chargedDiode).second.flag();
      int bunch  = (flag >>  8) & 0xff;
      
      ATH_MSG_DEBUG ( "RDO: "
            << (*i_chargedDiode).second.charge() << "  "
            << nToT << "  "
            << bunch << "  "
            << flag << "  "
            << PixRegion << " "
            );
      
      SiReadoutCellId cellId=(*i_chargedDiode).second.getReadoutCell();
      const Identifier id_readout = collection->element()->identifierFromCellId(cellId);
      
      if ( bunch >= 0 && bunch < m_TimeSvc->getTimeBCN()) {
	Pixel1RawData *p_rdo= new Pixel1RawData(id_readout, nToT, bunch, 0, bunch );
	PixelRDOColl->push_back(p_rdo);
	int row = pixelId->phi_index(id_readout);
	int col = pixelId->eta_index(id_readout);
	ATH_MSG_DEBUG ( "Cell = " << row << " -  " << col );
	meanpos+= p_design->positionFromColumnRow(col,row)*float(nToT);	
	ATH_MSG_DEBUG ( "p_design->positionFromColumnRow(col,row) = "<< p_design->positionFromColumnRow(col,row).xPhi() << " ,  " << p_design->positionFromColumnRow(col,row).xEta());
	ATH_MSG_DEBUG ( "nTot = "<< float(nToT));
	totalChargeNorm += nToT;
      }
    }
  }

  // this is used for debugging --> To be checked in the PixelClusterOnTrackTool
  ATH_MSG_DEBUG ( "totalCharge = "<< totalChargeNorm);
  ATH_MSG_DEBUG ( "Collection size = " << PixelRDOColl->size());
  ATH_MSG_DEBUG ( "Collection --> " << *PixelRDOColl);
  meanpos = meanpos/totalChargeNorm;
  ATH_MSG_DEBUG ( "MeanPosition before shift = " << meanpos.xPhi() << " , " << meanpos.xEta());
  double shift =  m_useLorentzAngle ? m_detManager->getDetectorElement(collID)->getLorentzCorrection() : 0.;
  ATH_MSG_DEBUG ( "shift  = " << shift);
  Amg::Vector2D locpos = Amg::Vector2D(meanpos.xPhi()+shift, meanpos.xEta());
  ATH_MSG_DEBUG ( "MeanPosition after shift  = " << locpos);
  
  return PixelRDOColl;
}
