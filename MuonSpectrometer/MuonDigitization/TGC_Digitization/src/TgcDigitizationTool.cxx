/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TGC_Digitization/TgcDigitizationTool.h"

#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/RNGWrapper.h"

// Other includes
#include "Identifier/Identifier.h"
#include "GeneratorObjects/HepMcParticleLink.h"

//TGC includes
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"

#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonSimEvent/TgcHitIdHelper.h"
#include "TGC_Digitization/TgcDigitMaker.h"

// for the tool
//#include "GaudiKernel/IToolSvc.h"

#include "PileUpTools/PileUpMergeSvc.h"
#include "PileUpTools/IPileUpTool.h" // for SubEventIterator
#include "xAODEventInfo/EventInfo.h"

//run number from geometry DB
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

static constexpr unsigned int crazyParticleBarcode(
    std::numeric_limits<int32_t>::max());
// Barcodes at the HepMC level are int

TgcDigitizationTool::TgcDigitizationTool(const std::string& type, 
					 const std::string& name,
					 const IInterface* parent) : 
  PileUpToolBase(type, name, parent),
  m_mergeSvc(0), 
  m_hitIdHelper(0), 
  m_idHelper(0),
  m_mdManager(0),
  m_digitizer(0),
  m_thpcTGC(0),
  m_inputHitCollectionName("TGC_Hits"),
  m_vetoThisBarcode(crazyParticleBarcode) 
{
  declareInterface<IMuonDigitizationTool>(this);

  declareProperty("InputObjectName",  m_inputHitCollectionName    = "TGC_Hits",   "name of the input object");
  declareProperty("IncludePileUpTruth",  m_includePileUpTruth     =  true,        "Include pile-up truth info");
  declareProperty("ParticleBarcodeVeto", m_vetoThisBarcode        =  crazyParticleBarcode, "Barcode of particle to ignore");
}

//--------------------------------------------
StatusCode TgcDigitizationTool::initialize()
{

  // retrieve MuonDetctorManager from DetectorStore
  if(detStore()->retrieve(m_mdManager).isFailure()) {
    return StatusCode::FAILURE; 
  } 
  ATH_MSG_DEBUG("Retrieved MuonDetectorManager from DetectorStore.");

  if(!m_mergeSvc) {
    //locate the PileUpMergeSvc
    const bool CREATEIF = true;
    if(!(service("PileUpMergeSvc", m_mergeSvc, CREATEIF)).isSuccess() ||
       !m_mergeSvc) {
      ATH_MSG_FATAL("Could not find PileUpMergeSvc");
      return StatusCode::FAILURE;
    }
  }
  
  //initialize the TgcIdHelper
  m_idHelper = m_mdManager->tgcIdHelper();
  if(!m_idHelper) {
    ATH_MSG_WARNING("tgcIdHelper could not be retrieved.");
    return StatusCode::FAILURE;
  }
  
  // TgcHitIdHelper
  m_hitIdHelper = TgcHitIdHelper::GetHelper();

  // check the input object name
  if(m_inputHitCollectionName=="") {
    ATH_MSG_FATAL("Property InputObjectName not set !");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Input objects: '" << m_inputHitCollectionName << "'");
  }

  //initialize the output WriteHandleKeys
  ATH_CHECK(m_outputDigitCollectionKey.initialize());
  ATH_CHECK(m_outputSDO_CollectionKey.initialize());
  ATH_MSG_DEBUG("Output Digits: '"<<m_outputDigitCollectionKey.key()<<"'");

  ATH_MSG_DEBUG("IncludePileUpTruth: " << m_includePileUpTruth);
  ATH_MSG_DEBUG("ParticleBarcodeVeto: " << m_vetoThisBarcode);

  const IGeoModelSvc* geoModel = nullptr;
  CHECK( service("GeoModelSvc", geoModel) );
  std::string atlasVersion = geoModel->atlasVersion();

  IRDBAccessSvc* rdbAccess = nullptr;
  CHECK( service("RDBAccessSvc",rdbAccess) );

  IRDBRecordset_ptr atlasCommonRec = rdbAccess->getRecordsetPtr("AtlasCommon",atlasVersion,"ATLAS");
  unsigned int runperiod = 1;
  if(atlasCommonRec->size()==0) runperiod = 1;
  else {
    std::string configVal = (*atlasCommonRec)[0]->getString("CONFIG");
    if(configVal=="RUN1") runperiod = 1;
    else if(configVal=="RUN2") runperiod = 2;
    else if(configVal=="RUN3") runperiod = 3; // currently runperiod 3 means no masking => ok for upgrade
    else if(configVal=="RUN4") runperiod = 3; // currently runperiod 3 means no masking => ok for upgrade
    else {
      ATH_MSG_FATAL("Unexpected value for geometry config read from the database: " << configVal);  
      return StatusCode::FAILURE;
    }
  }

  // initialize class to execute digitization 
  m_digitizer = new TgcDigitMaker(m_hitIdHelper, 
				  m_mdManager,
				  runperiod);
  m_digitizer->setMessageLevel(static_cast<MSG::Level>(msgLevel()));
  ATH_CHECK(m_rndmSvc.retrieve());

  ATH_CHECK(m_digitizer->initialize());

  return StatusCode::SUCCESS;
}

//--------------------------------------------
StatusCode TgcDigitizationTool::prepareEvent(unsigned int)
{
  m_TGCHitCollList.clear();
  m_thpcTGC = new TimedHitCollection<TGCSimHit>();
 
  return StatusCode::SUCCESS;
}

//--------------------------------------------
StatusCode TgcDigitizationTool::processBunchXing(int bunchXing,
                                                 SubEventIterator bSubEvents,
                                                 SubEventIterator eSubEvents) {
  ATH_MSG_DEBUG ("TgcDigitizationTool::processBunchXing() " << bunchXing);

  typedef PileUpMergeSvc::TimedList<TGCSimHitCollection>::type TimedHitCollList;
  TimedHitCollList hitCollList;

  if (!(m_mergeSvc->retrieveSubSetEvtData(m_inputHitCollectionName, hitCollList, bunchXing,
					  bSubEvents, eSubEvents).isSuccess()) &&
        hitCollList.size() == 0) {
    ATH_MSG_ERROR("Could not fill TimedHitCollList");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_VERBOSE(hitCollList.size() << " TGCSimHitCollection with key " <<
		    m_inputHitCollectionName << " found");
  }

  TimedHitCollList::iterator iColl(hitCollList.begin());
  TimedHitCollList::iterator endColl(hitCollList.end());

  // Iterating over the list of collections
  for( ; iColl != endColl; iColl++){

    TGCSimHitCollection *hitCollPtr = new TGCSimHitCollection(*iColl->second);
    PileUpTimeEventIndex timeIndex(iColl->first);

    ATH_MSG_DEBUG("TGCSimHitCollection found with " << hitCollPtr->size() <<
		  " hits");
    ATH_MSG_VERBOSE("time index info. time: " << timeIndex.time()
		    << " index: " << timeIndex.index()
		    << " type: " << timeIndex.type());

    m_thpcTGC->insert(timeIndex, hitCollPtr);
    m_TGCHitCollList.push_back(hitCollPtr);

  }

  return StatusCode::SUCCESS;
}

//--------------------------------------------
StatusCode TgcDigitizationTool::mergeEvent() {
  ATH_MSG_DEBUG("TgcDigitizationTool::mergeEvent()");

  ATH_CHECK(digitizeCore());
  // reset the pointer (delete null pointer should be safe)
  delete m_thpcTGC;
  m_thpcTGC = 0;

  std::list<TGCSimHitCollection*>::iterator TGCHitColl    = m_TGCHitCollList.begin();
  std::list<TGCSimHitCollection*>::iterator TGCHitCollEnd = m_TGCHitCollList.end();
  while(TGCHitColl!=TGCHitCollEnd) {
    delete (*TGCHitColl);
    ++TGCHitColl;
  }
  m_TGCHitCollList.clear();

  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode TgcDigitizationTool::digitize() {
  return this->processAllSubEvents();
}

//_____________________________________________________________________________
StatusCode TgcDigitizationTool::processAllSubEvents() {
  ATH_MSG_DEBUG("TgcDigitizationTool::processAllSubEvents()");
  //merging of the hit collection in getNextEvent method
  if(!m_thpcTGC) {
    ATH_CHECK(getNextEvent());
  }
  ATH_CHECK(digitizeCore());
  // reset the pointer (delete null pointer should be safe)
  delete m_thpcTGC;
  m_thpcTGC = 0;
  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode TgcDigitizationTool::finalize() {
  ATH_MSG_DEBUG("finalize.");

  delete m_digitizer;
  m_digitizer = 0;

  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
// Get next event and extract collection of hit collections:
StatusCode TgcDigitizationTool::getNextEvent()
{
  if(!m_mergeSvc) { 
    //locate the PileUpMergeSvc 
    const bool CREATEIF = true;
    if(!(service("PileUpMergeSvc", m_mergeSvc, CREATEIF)).isSuccess() ||
       !m_mergeSvc) { 
      ATH_MSG_FATAL("Could not find PileUpMergeSvc");
      return StatusCode::FAILURE;
    }
  }
  
  // initialize pointer
  m_thpcTGC = 0;
  
  //  get the container(s)
  typedef PileUpMergeSvc::TimedList<TGCSimHitCollection>::type TimedHitCollList;
  
  //this is a list<pair<time_t, DataLink<TGCSimHitCollection> > >
  TimedHitCollList hitCollList;
  
  if(!(m_mergeSvc->retrieveSubEvtsData(m_inputHitCollectionName, hitCollList).isSuccess())) {
    ATH_MSG_FATAL("Could not fill TimedHitCollList"); 
    return StatusCode::FAILURE;
  }
  if(hitCollList.size()==0) {
    ATH_MSG_FATAL("TimedHitCollList has size 0"); 
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG(hitCollList.size() 
		  << " TGCSimHitCollections with key " << m_inputHitCollectionName 
		  << " found");  
  }
  
  // create a new hits collection
  m_thpcTGC = new TimedHitCollection<TGCSimHit>();
  
  //now merge all collections into one
  TimedHitCollList::iterator iColl = hitCollList.begin();
  TimedHitCollList::iterator endColl = hitCollList.end();
  while(iColl != endColl) {
    const TGCSimHitCollection* p_collection = iColl->second;
    m_thpcTGC->insert(iColl->first, p_collection);
    ATH_MSG_DEBUG("TGCSimHitCollection found with "
		  << p_collection->size() << " hits"); // loop on the hit collections
    ++iColl;
  }
  return StatusCode::SUCCESS;
}

StatusCode TgcDigitizationTool::digitizeCore() {

  ATHRNG::RNGWrapper* rngWrapper = m_rndmSvc->getEngine(this);
  rngWrapper->setSeed( name(), Gaudi::Hive::currentContext() );
  CLHEP::HepRandomEngine *rndmEngine = *rngWrapper;

  // create and record the Digit container in StoreGate
  SG::WriteHandle<TgcDigitContainer> digitContainer(m_outputDigitCollectionKey);
  ATH_CHECK(digitContainer.record(std::make_unique<TgcDigitContainer>(m_idHelper->module_hash_max())));
  ATH_MSG_DEBUG ( "TgcDigitContainer recorded in StoreGate." );

  // Create and record the SDO container in StoreGate
  SG::WriteHandle<MuonSimDataCollection> sdoContainer(m_outputSDO_CollectionKey);
  ATH_CHECK(sdoContainer.record(std::make_unique<MuonSimDataCollection>()));
  ATH_MSG_DEBUG ( "TgcSDOCollection recorded in StoreGate." );

  // get the iterator pairs for this DetEl
  //iterate over hits and fill id-keyed drift time map
  IdContext tgcContext = m_idHelper->module_context();
  
  TimedHitCollection<TGCSimHit>::const_iterator i, e; 
  while(m_thpcTGC->nextDetectorElement(i, e)) {
    ATH_MSG_DEBUG("TgcDigitizationTool::digitizeCore next element");
      
    // Loop over the hits:
    while(i != e) {
      TimedHitPtr<TGCSimHit> phit = *i++;
      const TGCSimHit& hit = *phit;
      double globalHitTime = hitTime(phit);
      double tof = phit->globalTime();
      TgcDigitCollection* digiHits = m_digitizer->executeDigi(&hit, globalHitTime, rndmEngine);

      if(!digiHits) continue;

      TgcDigitCollection::const_iterator it_digiHits;
      for(it_digiHits=digiHits->begin(); it_digiHits!=digiHits->end();
	  ++it_digiHits) {

	/**
	   NOTE: 
	   ----- 
	   Since not every hit might end up resulting in a
	   digit, this construction might take place after the hit loop 
	   in a loop of its own! 
	*/
	
	Identifier newDigiId = (*it_digiHits)->identify();
	uint16_t newBcTag    = (*it_digiHits)->bcTag();
	Identifier elemId    = m_idHelper->elementID(newDigiId);
	
	TgcDigitCollection* digitCollection = 0;
	
	IdentifierHash coll_hash;
	if(m_idHelper->get_hash(elemId, coll_hash, &tgcContext)) {
	  ATH_MSG_WARNING("Unable to get TGC hash id from TGC Digit collection " 
			  << "context begin_index = " << tgcContext.begin_index()
			  << " context end_index  = " << tgcContext.end_index()
			  << " the identifier is "); 
	  elemId.show();
	} 
	
	// make new TgcDigit and record it in StoreGate
	TgcDigit* newDigit = new TgcDigit(newDigiId, newBcTag);
  
	// record the digit container in StoreGate
	bool duplicate = false;
	TgcDigitContainer::const_iterator it_coll = digitContainer->indexFind(coll_hash);
	if(digitContainer->end() ==  it_coll) {
	  digitCollection = new TgcDigitCollection(elemId, coll_hash);
	  ATH_MSG_DEBUG("Digit Id(1st) = " << m_idHelper->show_to_string(newDigiId)
			<< " BC tag = " << newBcTag << " Coll. key = " << coll_hash); 
	  digitCollection->push_back(newDigit);
	  StatusCode status = digitContainer->addCollection(digitCollection, coll_hash);
	  if(status.isFailure()) {
	    ATH_MSG_WARNING("Couldn't record TgcDigitCollection with key=" << coll_hash << " in StoreGate!"); 
	  } else {
	    ATH_MSG_DEBUG("New TgcHitCollection with key=" << coll_hash << " recorded in StoreGate."); 
	  }
	} else {
	  digitCollection = const_cast<TgcDigitCollection*>(*it_coll);

	  // to avoid to store digits with identical id
	  TgcDigitCollection::const_iterator it_tgcDigit;
	  for(it_tgcDigit=digitCollection->begin(); it_tgcDigit != digitCollection->end(); ++it_tgcDigit) {
	    if(newDigiId == (*it_tgcDigit)->identify() &&
	       newBcTag  == (*it_tgcDigit)->bcTag()) {
	      duplicate = true;
	      IdContext context = m_idHelper->channel_context();
	      ATH_MSG_DEBUG("Duplicate digit(removed) = "
			    << m_idHelper->show_to_string(newDigiId, &context) 
			    << " BC tag = " << newBcTag);  
	      delete newDigit; 
	      newDigit = 0;
	      break;
	    }
	  }
	  if(!duplicate) {
	    digitCollection->push_back(newDigit);
	    ATH_MSG_DEBUG("Digit Id= " << m_idHelper->show_to_string(newDigiId) 
			  << " BC tag = " << newBcTag); 
	  }
	}
	
	if(!duplicate) {
          static double invalid_pos = -99999.;
          Amg::Vector3D gpos(invalid_pos,invalid_pos,invalid_pos);
          const MuonGM::TgcReadoutElement *tgcChamber = m_mdManager->getTgcReadoutElement(newDigiId);
          if(tgcChamber) {
            gpos = tgcChamber->localToGlobalCoords(hit.localPosition(),newDigiId);
          }

	  // fill the SDO collection in StoreGate if not pile-up
      if (!m_includePileUpTruth &&
          ((phit->trackNumber() == 0) || (phit->trackNumber() == m_vetoThisBarcode))) {
        continue;
      }

	  // link to MC info
	  //const HepMcParticleLink & particleLink = hit.particleLink();
	  // create here deposit for MuonSimData, link and tof
	  MuonSimData::Deposit deposit(HepMcParticleLink(phit->trackNumber(), phit.eventId()), MuonMCData(tof, 0));

	  std::vector<MuonSimData::Deposit> deposits;
	  deposits.push_back(deposit);
          MuonSimData simData(deposits,0);
          simData.setPosition(gpos);
          simData.setTime(hitTime(phit));
	  sdoContainer->insert(std::make_pair(newDigiId, simData));
	}
	
      }
      delete digiHits;
      digiHits = 0;
    }//while(i != e)
  }//while(m_thpcTGC->nextDetectorElement(i, e))

  return StatusCode::SUCCESS;
}
