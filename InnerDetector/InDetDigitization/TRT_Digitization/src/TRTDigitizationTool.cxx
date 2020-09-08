/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRTDigitizationTool.cxx
//
//   Implementation file for class TRTDigitizationTool
//
///////////////////////////////////////////////////////////////////

#include "TRTDigitizationTool.h"
#include "HitManagement/TimedHitCollection.h"
#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include "TRTProcessingOfStraw.h"
#include "TRTElectronicsProcessing.h"

#include "TRTDigCondBase.h"
#include "TRTDigCondFakeMap.h"

#include "TRTNoise.h"
#include "TRTDigiHelper.h"
#include "TRTElectronicsNoise.h"

#include "Identifier/Identifier.h"
#include "InDetSimData/InDetSimData.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "InDetSimEvent/TRTHitIdHelper.h"

#include "GeneratorObjects/HepMcParticleLink.h"

// Det descr includes:
#include "InDetIdentifier/TRT_ID.h"
#include "TRT_ReadoutGeometry/TRT_DetectorManager.h"
#include "TRTDigSettings.h"

#include "InDetRawData/TRT_LoLumRawData.h"
#include "InDetRawData/TRT_RDO_Collection.h"

// particle table
#include "HepPDT/ParticleDataTable.hh"
#include "GaudiKernel/IPartPropSvc.h"

// for the tool
#include "GaudiKernel/IToolSvc.h"

// Gaudi includes
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"


//CondDB
#include "StoreGate/ReadCondHandle.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "TRT_ConditionsData/StrawStatusMultChanContainer.h"
#include <limits>
#include <cstdint>

// Random Number Generation
#include "AthenaKernel/RNGWrapper.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandGaussZiggurat.h"

#include "CxxUtils/checker_macros.h"

//#include "driftCircle.h" // local copy for debugging and development

//_____________________________________________________________________________
TRTDigitizationTool::TRTDigitizationTool(const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent)
: PileUpToolBase(type, name, parent)
{
  m_settings = new TRTDigSettings();
  m_settings->addPropertiesForOverrideableParameters(static_cast<AlgTool*>(this));
}

//_____________________________________________________________________________
TRTDigitizationTool::~TRTDigitizationTool() {

  ATH_MSG_VERBOSE ( "Begin TRTDigitizationTool::Destructor");
  delete m_pElectronicsProcessing;
  delete m_pProcessingOfStraw;
  delete m_pDigConditions;
  delete m_pNoise;
  delete m_settings;
  ATH_MSG_VERBOSE ( "End TRTDigitizationTool::Destructor");

}

//_____________________________________________________________________________
StatusCode TRTDigitizationTool::initialize()
{

  ATH_MSG_DEBUG ( name()<<"::initialize() begin" );

  // Get the TRT Detector Manager
  ATH_CHECK(detStore()->retrieve(m_manager,"TRT"));
  ATH_MSG_DEBUG ( "Retrieved TRT_DetectorManager with version "  << m_manager->getVersion().majorNum() );

  ATH_CHECK(detStore()->retrieve(m_trt_id, "TRT_ID"));

  ATH_CHECK( m_digverscontainerkey.initialize (SG::AllowEmpty) );

  // Fill setting defaults and process joboption overrides:
  m_settings->initialize(m_manager);

  if (m_printOverrideableSettings) {
    m_settings->printFlagsForOverrideableParameters("TRTDigSettings Overrideables : ");
  }

  /// Get the PAI Tool for Xe, Ar, Kr gas mixtures:

  ATH_CHECK(m_TRTpaiToolXe.retrieve());
  ATH_MSG_DEBUG ( "Retrieved the PAI Tool for Xe straws" );

  ATH_CHECK(m_TRTpaiToolAr.retrieve());
  ATH_MSG_DEBUG ( "Retrieved the PAI Tool for Ar straws" );

  ATH_CHECK(m_TRTpaiToolKr.retrieve());
  ATH_MSG_DEBUG ( "Retrieved the PAI Tool for Kr straws" );

  /// Get the Sim-DriftTime Tool:
  ATH_CHECK(m_TRTsimdrifttimetool.retrieve());
  ATH_MSG_DEBUG ( "Retrieved the Sim. Drifttime Tool" );

  // Check data object name
  if (m_hitsContainerKey.key().empty()) {
    ATH_MSG_FATAL ( "Property DataObjectName not set!" );
    return StatusCode::FAILURE;
  }
  m_dataObjectName = m_hitsContainerKey.key();
  ATH_MSG_DEBUG ( "Input hits: " << m_dataObjectName );

  // Initialize ReadHandleKey
  ATH_CHECK(m_hitsContainerKey.initialize(!m_onlyUseContainerName));

  // Initialize data handle keys
  ATH_CHECK(m_outputRDOCollName.initialize());
  ATH_CHECK(m_outputSDOCollName.initialize());

  // Get Random Service
  ATH_CHECK(m_rndmSvc.retrieve());

  // Get the Particle Properties Service
  IPartPropSvc* p_PartPropSvc(nullptr);
  static const bool CREATEIFNOTTHERE(true);
  ATH_CHECK(service("PartPropSvc", p_PartPropSvc, CREATEIFNOTTHERE));
  m_particleTable = p_PartPropSvc->PDT();

  //locate the PileUpMergeSvc and initialize our local ptr
  if (m_onlyUseContainerName) {
    ATH_CHECK(m_mergeSvc.retrieve());
  }

  //Retrieve TRT_StrawNeighbourService.
  ATH_CHECK(m_TRTStrawNeighbourSvc.retrieve());

  //Retrieve TRT_CalDbTool
  if (m_settings->getT0FromData()) {
    ATH_CHECK(m_calDbTool.retrieve());
  }
  else {
    m_calDbTool.disable();
  }

  m_minpileuptruthEkin = m_settings->pileUpSDOsMinEkin();

  // Set SDO readout range
  const double intervalBetweenCrossings(m_settings->timeInterval() / 3.);

  switch ( (m_settings->storeSDO() )) {
  case 0:  m_minCrossingTimeSDO =  -1.*CLHEP::ns;  m_maxCrossingTimeSDO = -1.*CLHEP::ns; break;
  case 1:  m_minCrossingTimeSDO =  -1.*CLHEP::ns;  m_maxCrossingTimeSDO = (intervalBetweenCrossings * 2. + 1.*CLHEP::ns); break;
  case 2:  m_minCrossingTimeSDO = -(intervalBetweenCrossings * 2. + 1.*CLHEP::ns);  m_maxCrossingTimeSDO = (intervalBetweenCrossings * 2. + 1.*CLHEP::ns); break;
  default:
    ATH_MSG_ERROR ( "storeSDO is out of range:"<<m_settings->storeSDO()<<"allowed values are: 0,1,2. Setting storeSDO = 2 " );
    m_minCrossingTimeSDO = -51.*CLHEP::ns;  m_maxCrossingTimeSDO = 51.*CLHEP::ns;
    break;
  }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ATH_CHECK( m_fieldCacheCondObjInputKey.initialize() );
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode TRTDigitizationTool::prepareEvent(const EventContext& /*ctx*/, unsigned int)
{
  m_vDigits.clear();
  m_trtHitCollList.clear();
  m_thpctrt = new TimedHitCollection<TRTUncompressedHit>();
  m_HardScatterSplittingSkipper = false;
  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode TRTDigitizationTool::processBunchXing(int bunchXing,
                                                 SubEventIterator bSubEvents,
                                                 SubEventIterator eSubEvents) {

  m_seen.push_back(std::make_pair(std::distance(bSubEvents,eSubEvents), bunchXing));
  //decide if this event will be processed depending on HardScatterSplittingMode & bunchXing
  if (m_HardScatterSplittingMode == 2 && !m_HardScatterSplittingSkipper ) { m_HardScatterSplittingSkipper = true; return StatusCode::SUCCESS; }
  if (m_HardScatterSplittingMode == 1 && m_HardScatterSplittingSkipper )  { return StatusCode::SUCCESS; }
  if (m_HardScatterSplittingMode == 1 && !m_HardScatterSplittingSkipper ) { m_HardScatterSplittingSkipper = true; }


  //TRTUncompressedHit

  typedef PileUpMergeSvc::TimedList<TRTUncompressedHitCollection>::type TimedHitCollList;
  TimedHitCollList hitCollList;

  if (!(m_mergeSvc->retrieveSubSetEvtData(m_dataObjectName, hitCollList, bunchXing,
                                          bSubEvents, eSubEvents).isSuccess()) &&
      hitCollList.size() == 0) {
    ATH_MSG_ERROR("Could not fill TimedHitCollList");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_VERBOSE(hitCollList.size() << " TRTUncompressedHitCollection with key " <<
                    m_dataObjectName << " found");
  }

  TimedHitCollList::iterator iColl(hitCollList.begin());
  TimedHitCollList::iterator endColl(hitCollList.end());

  for( ; iColl != endColl; iColl++){
    TRTUncompressedHitCollection  *hitCollPtr = new TRTUncompressedHitCollection(*iColl->second);
    PileUpTimeEventIndex timeIndex(iColl->first);
    ATH_MSG_DEBUG("TRTUncompressedHitCollection found with " << hitCollPtr->size() <<
                  " hits");
    ATH_MSG_VERBOSE("time index info. time: " << timeIndex.time()
                    << " index: " << timeIndex.index()
                    << " type: " << timeIndex.type());
    m_thpctrt->insert(timeIndex, hitCollPtr);
    m_trtHitCollList.push_back(hitCollPtr);
  }

  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode TRTDigitizationTool::lateInitialize(const EventContext& ctx) {

  // setup the RNGs which are only used in the first event
  CLHEP::HepRandomEngine *fakeCondRndmEngine = getRandomEngine("TRT_FakeConditions", m_randomSeedOffset, ctx);
  CLHEP::HepRandomEngine *noiseInitRndmEngine = getRandomEngine("TRT_Noise", m_randomSeedOffset, ctx);
  CLHEP::HepRandomEngine *noiseElecRndmEngine = getRandomEngine("TRT_Noise_Electronics", m_randomSeedOffset, ctx);
  CLHEP::HepRandomEngine *noiseThreshRndmEngine = getRandomEngine("TRT_Noise_ThresholdFluctuations", m_randomSeedOffset, ctx);
  CLHEP::HepRandomEngine *noiseElecResetRndmEngine = getRandomEngine("TRT_ElectronicsNoiseReset", m_randomSeedOffset, ctx);
  m_first_event=false;

  if (!m_digverscontainerkey.empty()) {

    if ( ConditionsDependingInitialization(ctx).isFailure() ) {
      ATH_MSG_ERROR ( "Folder holder TRT digitization version exists in condDB, but tag is faulty" );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG ( "Using Digitization version as defined in conditions tag" );
    }

  } else {
    ATH_MSG_DEBUG ( "No folder containing TRT digitization version found in CondDB. Using default from Det Desc tag: " << m_settings->digVers() );
  }

  //Resuming initialiazation. Section below had to be moved into event loop due to dependence on conditions data

  TRTElectronicsNoise *electronicsNoise(nullptr);
  if ( m_settings->noiseInUnhitStraws() || m_settings->noiseInSimhits() ) {
    electronicsNoise = new TRTElectronicsNoise(m_settings, noiseElecRndmEngine);
  }
  // ElectronicsProcessing is needed for the regular straw processing,
  // but also for the noise (it assumes ownership of electronicsnoise )
  m_pElectronicsProcessing = new TRTElectronicsProcessing( m_settings, electronicsNoise );

  m_pDigConditions = new TRTDigCondFakeMap(m_settings,
                                           m_manager,
                                           m_trt_id,
                                           m_UseGasMix,
                                           m_sumTool);

  m_pDigConditions->initialize(fakeCondRndmEngine);

  if ( m_settings->noiseInUnhitStraws() || m_settings->noiseInSimhits() ) {

    // In short this next constructor does 3 things;
    //     i)  tunes the amplitude of the electronics noise,
    //    ii) creates a pool of noise digits,
    //   iii) figures out exact low thresholds needed to reproduce actual
    //        straw noise-frequencies:
    m_pNoise = new TRTNoise( m_settings,
                             m_manager,
                             noiseInitRndmEngine,
                             noiseElecRndmEngine,
                             noiseThreshRndmEngine,
                             noiseElecResetRndmEngine,
                             m_pDigConditions,
                             m_pElectronicsProcessing,
                             electronicsNoise,
                             m_trt_id,
                             m_UseGasMix,
                             m_sumTool);

    ATH_MSG_DEBUG ( "Average straw noise level is " << m_pDigConditions->strawAverageNoiseLevel() );

  } else {
    m_pNoise = nullptr;
  }

  ITRT_PAITool *TRTpaiToolXe = &(* m_TRTpaiToolXe);
  ITRT_PAITool *TRTpaiToolAr = &(* m_TRTpaiToolAr);
  ITRT_PAITool *TRTpaiToolKr = &(* m_TRTpaiToolKr);

  ITRT_SimDriftTimeTool *pTRTsimdrifttimetool = &(*m_TRTsimdrifttimetool);

  const ITRT_CalDbTool* calDbTool = nullptr;
  if (m_settings->getT0FromData()) {
    calDbTool = m_calDbTool.get();
  }
  m_pProcessingOfStraw =
    new TRTProcessingOfStraw( m_settings,
                              m_manager,
                              TRTpaiToolXe,
                              pTRTsimdrifttimetool,
                              m_pElectronicsProcessing,
                              m_pNoise,
                              m_pDigConditions,
                              m_particleTable,
                              m_trt_id,
                              TRTpaiToolAr,
                              TRTpaiToolKr,
                              calDbTool);

  ATH_MSG_INFO ( "Gas Property:             UseGasMix is " << m_UseGasMix );

  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode TRTDigitizationTool::processStraws(const EventContext& ctx,
                                              TimedHitCollection<TRTUncompressedHit>& thpctrt,
                                              std::set<int>& sim_hitids, std::set<Identifier>& simhitsIdentifiers,
                                              CLHEP::HepRandomEngine *rndmEngine,
                                              CLHEP::HepRandomEngine *strawRndmEngine,
                                              CLHEP::HepRandomEngine *elecProcRndmEngine,
                                              CLHEP::HepRandomEngine *elecNoiseRndmEngine,
                                              CLHEP::HepRandomEngine *paiRndmEngine) {

  // Create a map for the SDO
  SG::WriteHandle<InDetSimDataCollection> simDataMap(m_outputSDOCollName, ctx);

////////////////////////////////////////////////////////////////////////////////////////////////////
  MagField::AtlasFieldCache    fieldCache;
  if (m_settings->useMagneticFieldMap()) {
      
    // Get field cache object
    SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, ctx};
    const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};

    if (fieldCondObj == nullptr) {
        ATH_MSG_ERROR("Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCacheCondObjInputKey.key());
        return StatusCode::FAILURE;
    }
    fieldCondObj->getInitializedCache (fieldCache);
  }
  
////////////////////////////////////////////////////////////////////////////////////////////////////

  
  ATH_CHECK(simDataMap.record(std::make_unique<InDetSimDataCollection>() ));

  // Register the map into StoreGate
  if (not simDataMap.isValid()) {
    ATH_MSG_FATAL ( "InDetSimData map " << m_outputSDOCollName.key() << " could not be registered in StoreGate !" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( "InDetSimData map " << m_outputSDOCollName.key() << " registered in StoreGate" );
  }

  m_cosmicEventPhase = 0.0;
  if (m_settings->doCosmicTimingPit()) {
    m_cosmicEventPhase = getCosmicEventPhase(rndmEngine);
  };

  // Create  a vector of deposits
  std::vector<InDetSimData::Deposit> depositVector(100);

  // loop over all straws
  TimedHitCollection<TRTUncompressedHit>::const_iterator i, e;
  while (thpctrt.nextDetectorElement(i, e)) {

    int hitID((*i)->GetHitID()); // Get hitID

    // evtIndex should be 0 for main event and 1,2,3,... for pileup events:
    // (event Id is a property of the TimedHitPtr)
    HepMcParticleLink::index_type evtIndex(i->eventId());

    if ( m_settings->noiseInUnhitStraws() ) {
      sim_hitids.insert(hitID);
    }
    //Safeguard against a rare case of hitID corruption found by Davide:
    if ( hitID & 0xc0000000 ) {
      ATH_MSG_ERROR ( "Hit ID not Valid (" << MSG::hex << hitID << ")" << MSG::dec );
      continue;
    }

    // Convert hitID to Identifier
    IdentifierHash IdHash;
    Identifier idLayer;
    bool identifierOK;
    Identifier idStraw(getIdentifier(hitID, IdHash, idLayer, identifierOK));
    if ( !identifierOK ) {
      ATH_MSG_ERROR ( "Ignoring simhits with suspicious identifier (1)" );
      continue;
    }

    //For crosstalk sim
    simhitsIdentifiers.insert(idStraw);

    ///// START OF SDO CREATION
    // Fill a vector of deposits
    depositVector.clear();
    depositVector.reserve(std::distance(i,e));
    const EBC_EVCOLL evColl = EBC_MAINEVCOLL;
    for (TimedHitCollection<TRTUncompressedHit>::const_iterator hit_iter(i); hit_iter != e; ++hit_iter ) {
      const HepMcParticleLink::PositionFlag idxFlag = (hit_iter->eventId()==0) ? HepMcParticleLink::IS_POSITION: HepMcParticleLink::IS_INDEX; // suspect that we could use evtIndex here rather than hit_iter->eventId()
      // create a new deposit
      InDetSimData::Deposit deposit( HepMcParticleLink((*hit_iter)->GetTrackID(), hit_iter->eventId(), evColl, idxFlag), (*hit_iter)->GetEnergyDeposit() );
      if(deposit.first.barcode()==0 || deposit.first.barcode() == m_vetoThisBarcode){
        continue;
      }
      ATH_MSG_VERBOSE ( "Deposit: trackID " << deposit.first << " energyDeposit " << deposit.second );
      depositVector.push_back(deposit);
    }

    const TimedHitPtr<TRTUncompressedHit>& theHit(*i);
    const double bunchCrossingTime(hitTime(theHit) - static_cast<double>(theHit->GetGlobalTime()));

    // Add the simdata object to the map.
    if ( depositVector.size() &&
         (evtIndex == 0 || ((*i)->GetKineticEnergy()>m_minpileuptruthEkin))  &&
         (bunchCrossingTime < m_maxCrossingTimeSDO) && (bunchCrossingTime > m_minCrossingTimeSDO) ) {
      simDataMap->insert(std::make_pair(idStraw, InDetSimData(depositVector)));
    }
    ///// END OF SDO CREATION

    // Digitization for the given straw
    TRTDigit digit_straw;

    //// Get the ComTime tool
    //if (m_settings->doCosmicTimingPit()) {
    //  if ( StatusCode::SUCCESS == evtStore()->retrieve(m_ComTime,"ComTime")) {
    //	ATH_MSG_VERBOSE ( "Found tool for cosmic timing: ComTime" );
    //  } else {
    //    ATH_MSG_ERROR ( "Did not find tool needed for cosmic timing: ComTime" );
    //  }
    //}

    // if StatusHT == 6 thats emulate argon, ==7 that's emulate krypton
    bool emulateArFlag = m_sumTool->getStatusHT(idStraw) == 6;
    bool emulateKrFlag = m_sumTool->getStatusHT(idStraw) == 7;
    const int statusHT = m_sumTool->getStatusHT(idStraw);
    m_pProcessingOfStraw->ProcessStraw(fieldCache, i, e, digit_straw,
                                       m_alreadyPrintedPDGcodeWarning,
                                       m_cosmicEventPhase, //m_ComTime,
                                       TRTDigiHelper::StrawGasType(statusHT,m_UseGasMix, &msg()),
                                       emulateArFlag,
                                       emulateKrFlag,
                                       strawRndmEngine,
                                       elecProcRndmEngine,
                                       elecNoiseRndmEngine,
                                       paiRndmEngine);

    // Print out the digits etc (for debugging)
    //int          mstrw = digit_straw.GetStrawID();
    //unsigned int mword = digit_straw.GetDigit();
    //std::cout << "AJB " << mstrw << ":" << mword << std::endl;
    //print_mword_properties(mword);
    //std::cout << "AJB "; bits24(mword);
    //std::cout << "AJB "; bits27(mword);

    // finally push back the output digit.
    if ( digit_straw.GetDigit() ) {
      m_vDigits.push_back(digit_straw);
    }

  } // end of straw loop

  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode TRTDigitizationTool::processAllSubEvents(const EventContext& ctx) {

  // Set the RNGs to use for this event.
  CLHEP::HepRandomEngine *rndmEngine = getRandomEngine("", ctx);
  CLHEP::HepRandomEngine *elecNoiseRndmEngine = getRandomEngine("TRT_ElectronicsNoise", ctx);
  CLHEP::HepRandomEngine *noiseRndmEngine = getRandomEngine("TRT_NoiseDigitPool", ctx);
  CLHEP::HepRandomEngine *strawRndmEngine = getRandomEngine("TRT_ProcessStraw", ctx);
  CLHEP::HepRandomEngine *elecProcRndmEngine = getRandomEngine("TRT_ThresholdFluctuations", ctx);
  CLHEP::HepRandomEngine *paiRndmEngine = getRandomEngine("TRT_PAI", ctx);

  if (m_first_event) {
    if(this->lateInitialize(ctx).isFailure()) {
      ATH_MSG_FATAL ( "lateInitialize method failed!" );
      return StatusCode::FAILURE;
    }
  }

  m_alreadyPrintedPDGcodeWarning = false;

  ATH_MSG_DEBUG ( "TRTDigitizationTool::processAllSubEvents()" );

  m_trtrdo_container = SG::makeHandle(m_outputRDOCollName, ctx);
  ATH_CHECK(m_trtrdo_container.record(std::make_unique<TRT_RDO_Container>(m_trt_id->straw_layer_hash_max())));
  ATH_MSG_DEBUG ( " TRT_RDO_Container created " );

  if (not m_trtrdo_container.isValid()) {
    ATH_MSG_FATAL ( "Container " << m_outputRDOCollName.key() << " could not be registered in StoreGate !" );
    return StatusCode::FAILURE;
  }else {
    ATH_MSG_DEBUG ( "Container " << m_outputRDOCollName.key() << " registered in StoreGate" );
  }

  m_vDigits.clear();

  //  get the container(s)
  typedef PileUpMergeSvc::TimedList<TRTUncompressedHitCollection>::type TimedHitCollList;
  TimedHitCollection<TRTUncompressedHit> thpctrt;
  // In case of single hits container just load the collection using read handles
  if (!m_onlyUseContainerName) {
    SG::ReadHandle<TRTUncompressedHitCollection> hitCollection(m_hitsContainerKey, ctx);
    if (!hitCollection.isValid()) {
      ATH_MSG_ERROR("Could not get TRTUncompressedHitCollection container " << hitCollection.name() << " from store " << hitCollection.store());
      return StatusCode::FAILURE;
    }

    // Define Hit Collection
    thpctrt.reserve(1);

    // create a new hits collection
    thpctrt.insert(0, hitCollection.cptr());
    ATH_MSG_DEBUG("TRTUncompressedHitCollection found with " << hitCollection->size() << " hits");
  }
  else {
    TimedHitCollList hitCollList; // this is a list<pair<time_t, DataLink<TRTUncompressedHitCollection> > >
    unsigned int numberOfSimHits(0);
    if ( !(m_mergeSvc->retrieveSubEvtsData(m_dataObjectName, hitCollList, numberOfSimHits).isSuccess()) && hitCollList.size()==0 ) {
      ATH_MSG_ERROR ( "Could not fill TimedHitCollList" );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG ( hitCollList.size() << " TRTUncompressedHitCollections with key " << m_dataObjectName << " found" );
    }

    // Define Hit Collection
    thpctrt.reserve(numberOfSimHits);

    //now merge all collections into one
    TimedHitCollList::iterator   iColl(hitCollList.begin());
    TimedHitCollList::iterator endColl(hitCollList.end()  );
    m_HardScatterSplittingSkipper = false;
    // loop on the hit collections
    while ( iColl != endColl ) {
      //decide if this event will be processed depending on HardScatterSplittingMode & bunchXing
      if (m_HardScatterSplittingMode == 2 && !m_HardScatterSplittingSkipper ) { m_HardScatterSplittingSkipper = true; ++iColl; continue; }
      if (m_HardScatterSplittingMode == 1 && m_HardScatterSplittingSkipper )  { ++iColl; continue; }
      if (m_HardScatterSplittingMode == 1 && !m_HardScatterSplittingSkipper ) { m_HardScatterSplittingSkipper = true; }
      const TRTUncompressedHitCollection* p_collection(iColl->second);
      thpctrt.insert(iColl->first, p_collection);
      ATH_MSG_DEBUG ( "TRTUncompressedHitCollection found with " << p_collection->size() << " hits" );
      ++iColl;
    }
  }

  //Set of all hitid's with simhits (used for noise simulation).
  std::set<int> sim_hitids;
  std::set<Identifier> simhitsIdentifiers;

  // Process the Hits straw by straw: get the iterator pairs for given straw
  ATH_CHECK(this->processStraws(ctx, thpctrt, sim_hitids, simhitsIdentifiers, rndmEngine, strawRndmEngine, elecProcRndmEngine, elecNoiseRndmEngine,paiRndmEngine));

  // no more hits

  //Noise in straws without simhits:
  if (m_settings->noiseInUnhitStraws()) {
    const int numberOfDigitsBeforeNoise(m_vDigits.size());

    m_pNoise->appendPureNoiseToProperDigits(m_vDigits, sim_hitids, noiseRndmEngine);
    if (m_settings->doCrosstalk()) {
      m_pNoise->appendCrossTalkNoiseToProperDigits(m_vDigits, simhitsIdentifiers,m_TRTStrawNeighbourSvc, noiseRndmEngine);
    }

    ATH_MSG_DEBUG ( " Number of digits " << m_vDigits.size() << " (" << m_vDigits.size()-numberOfDigitsBeforeNoise << " of those are pure noise)" );

    m_pNoise->sortDigits(m_vDigits);

  } else {
    ATH_MSG_DEBUG ( " Number of digits " << m_vDigits.size() );
  }

  // All digits are ready.
  // We just need to convert to relevant identifiers and output to storegate.

  if (createAndStoreRDOs().isFailure()) {
    ATH_MSG_FATAL ( "createAndStoreRDOs() failed!" );
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG ( "createAndStoreRDOs() succeeded" );
  }

  return StatusCode::SUCCESS;
}

CLHEP::HepRandomEngine* TRTDigitizationTool::getRandomEngine(const std::string& streamName,
                                                             const EventContext& ctx) const
{
  ATHRNG::RNGWrapper* rngWrapper = m_rndmSvc->getEngine(this, streamName);
  std::string rngName = name()+streamName;
  rngWrapper->setSeed( rngName, ctx );
  return rngWrapper->getEngine(ctx);
}

//_____________________________________________________________________________
CLHEP::HepRandomEngine* TRTDigitizationTool::getRandomEngine(const std::string& streamName, unsigned long int randomSeedOffset,
                                                             const EventContext& ctx) const
{
  ATHRNG::RNGWrapper* rngWrapper = m_rndmSvc->getEngine(this, streamName);
  rngWrapper->setSeed( streamName, ctx.slot(), randomSeedOffset, ctx.eventID().run_number() );
  return rngWrapper->getEngine(ctx);
}

//_____________________________________________________________________________
StatusCode TRTDigitizationTool::mergeEvent(const EventContext& ctx) {
  std::vector<std::pair<unsigned int, int> >::iterator ii(m_seen.begin());
  std::vector<std::pair<unsigned int, int> >::iterator ee(m_seen.end());
  while (ii != ee) {
    ATH_MSG_DEBUG( "mergeEvent: there are " << ii->first << " events in bunch xing " << ii->second );
    ++ii;
  }

  // Set the RNGs to use for this event.
  CLHEP::HepRandomEngine *rndmEngine = getRandomEngine("", ctx);
  CLHEP::HepRandomEngine *elecNoiseRndmEngine = getRandomEngine("TRT_ElectronicsNoise", ctx);
  CLHEP::HepRandomEngine *noiseRndmEngine = getRandomEngine("TRT_NoiseDigitPool", ctx);
  CLHEP::HepRandomEngine *strawRndmEngine = getRandomEngine("TRT_ProcessStraw", ctx);
  CLHEP::HepRandomEngine *elecProcRndmEngine = getRandomEngine("TRT_ThresholdFluctuations", ctx);
  CLHEP::HepRandomEngine *paiRndmEngine = getRandomEngine("TRT_PAI", ctx);

  if (m_first_event) {
    if(this->lateInitialize(ctx).isFailure()) {
      ATH_MSG_FATAL ( "lateInitialize method failed!" );
      return StatusCode::FAILURE;
    }
  }

  m_alreadyPrintedPDGcodeWarning = false;

  ATH_MSG_DEBUG ( "TRTDigitization::execute()"  );

  m_trtrdo_container = SG::makeHandle(m_outputRDOCollName, ctx);
  ATH_CHECK(m_trtrdo_container.record(std::make_unique<TRT_RDO_Container>(m_trt_id->straw_layer_hash_max())));
  ATH_MSG_DEBUG ( " TRT_RDO_Container created " );
  if (not m_trtrdo_container.isValid()) {
    ATH_MSG_FATAL ( "Container " << m_outputRDOCollName.key() << " could not be registered in StoreGate !" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( "Container " << m_outputRDOCollName.key() << " registered in StoreGate" );
  }

  //Set of all hitid's with simhits (used for noise simulation).
  std::set<int> sim_hitids;
  std::set<Identifier> simhitsIdentifiers;

  // Process the Hits straw by straw:
  //   get the iterator pairs for given straw
  ATH_CHECK(this->processStraws(ctx, *m_thpctrt, sim_hitids, simhitsIdentifiers, rndmEngine, strawRndmEngine, elecProcRndmEngine, elecNoiseRndmEngine,paiRndmEngine));

  delete m_thpctrt;
  std::list<TRTUncompressedHitCollection*>::iterator trtHitColl(m_trtHitCollList.begin());
  std::list<TRTUncompressedHitCollection*>::iterator trtHitCollEnd(m_trtHitCollList.end());
  while(trtHitColl!=trtHitCollEnd)
    {
      delete (*trtHitColl);
      ++trtHitColl;
    }
  m_trtHitCollList.clear();
  // no more hits

  //Noise in straws without simhits:
  if (m_settings->noiseInUnhitStraws()) {
    const unsigned int numberOfDigitsBeforeNoise(m_vDigits.size());

    m_pNoise->appendPureNoiseToProperDigits(m_vDigits, sim_hitids, noiseRndmEngine);
    if (m_settings->doCrosstalk()) {
      m_pNoise->appendCrossTalkNoiseToProperDigits(m_vDigits, simhitsIdentifiers,m_TRTStrawNeighbourSvc, noiseRndmEngine);
    }

    ATH_MSG_DEBUG ( " Number of digits " << m_vDigits.size() << " (" << m_vDigits.size()-numberOfDigitsBeforeNoise << " of those are pure noise)" );

    m_pNoise->sortDigits(m_vDigits);

  } else {
    ATH_MSG_DEBUG ( " Number of digits " << m_vDigits.size() );
  };

  // All digits are ready.
  // We just need to convert to relevant identifiers and output to storegate.

  if (createAndStoreRDOs().isFailure()) {
    ATH_MSG_FATAL ( "createAndStoreRDOs() failed!" );
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG ( "createAndStoreRDOs() succeeded" );
  }

  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode TRTDigitizationTool::createAndStoreRDOs()
{

  std::vector<TRTDigit>::const_iterator TRTDigitIter(m_vDigits.begin());
  std::vector<TRTDigit>::const_iterator endOfTRTDigits(m_vDigits.end());

  // for testing
  IdentifierHash IdHash; // default value is 0xFFFFFFFF
  IdentifierHash IdHashOld; // default value is 0xFFFFFFFF
  TRT_RDO_Collection *RDOColl(nullptr);

  Identifier idStraw;

  while (TRTDigitIter != endOfTRTDigits) {
    ATH_MSG_DEBUG ( "Digit ID " << TRTDigitIter->GetStrawID() << " Digit " << TRTDigitIter->GetDigit() );

    Identifier layer_id;
    bool identifierOK(false);
    idStraw = getIdentifier(TRTDigitIter->GetStrawID(), IdHash, layer_id, identifierOK);
    if (!identifierOK) {
      ATH_MSG_ERROR ( "Ignoring simhits with suspicious identifier (2)" );
      ++TRTDigitIter;
      continue;
    };

    // Create new TRT RDO Collection
    ATH_MSG_DEBUG ( " RDO ID " << m_trt_id->print_to_string(idStraw) );

    //TK: wauv - we are really betting the farm on the fact that the
    //ordering of digits will result in a similar ordering of the
    //idhash'es here... (this is not immediately the case when noise
    //hits are appended afterwards).

    if (IdHash != IdHashOld) {
      RDOColl = new TRT_RDO_Collection(IdHash);
      ATH_MSG_DEBUG ( "New TRT RDO Collection created with IdHash " << static_cast<int>(IdHash) );
      IdHashOld = IdHash;
      RDOColl->setIdentifier(layer_id);

      // Add to the container
      if (m_trtrdo_container->addCollection(RDOColl, RDOColl->identifyHash()).isFailure()) {
        ATH_MSG_FATAL ( "Container " << m_outputRDOCollName.key() << " could not be registered in StoreGate !" );
        return StatusCode::FAILURE;
      } else {
        ATH_MSG_DEBUG ( "Container " << m_outputRDOCollName.key() << " registered in StoreGate" );
      }
    }

    // Put RDO into Collection
    TRT_LoLumRawData *p_rdo(new TRT_LoLumRawData(idStraw, TRTDigitIter->GetDigit()));
    if (RDOColl) {
      RDOColl->push_back(p_rdo);
    } else {
      ATH_MSG_FATAL ( "Failed to create the TRT_RDO_Collection before trying to add an RDO to it! IdHash = " << static_cast<int>(IdHash) );
      delete p_rdo;
      return StatusCode::FAILURE;
    }
    ++TRTDigitIter;
  }

  m_vDigits.clear();
  return StatusCode::SUCCESS;
}


//_____________________________________________________________________________
Identifier TRTDigitizationTool::getIdentifier ( int hitID,
                                                IdentifierHash& hashId,
                                                Identifier& IdLayer,
                                                bool & statusok ) const
{
  statusok = true;

  Identifier IdStraw;

  const int mask(0x0000001F);
  const int word_shift(5);
  int trtID, ringID, moduleID, layerID, strawID;
  int wheelID, planeID, sectorID;

  const InDetDD::TRT_BarrelElement *barrelElement;
  const InDetDD::TRT_EndcapElement *endcapElement;

  if ( !(hitID & 0x00200000) ) {      // barrel
    strawID   = hitID & mask;
    hitID   >>= word_shift;
    layerID   = hitID & mask;
    hitID   >>= word_shift;
    moduleID  = hitID & mask;
    hitID   >>= word_shift;
    ringID    = hitID & mask;
    trtID     = hitID >> word_shift;

    barrelElement = m_manager->getBarrelElement(trtID, ringID, moduleID, layerID);
    if ( barrelElement ) {
      hashId  = barrelElement->identifyHash();
      IdLayer = barrelElement->identify();
      IdStraw = m_trt_id->straw_id(IdLayer, strawID);
    } else {
      ATH_MSG_ERROR ( "Could not find detector element for barrel identifier with "
                      << "(ipos,iring,imod,ilayer,istraw) = ("
                      << trtID << ", " << ringID << ", " << moduleID << ", "
                      << layerID << ", " << strawID << ")" );
      statusok = false;
    }
  } else {                           // endcap
    strawID   = hitID & mask;
    hitID   >>= word_shift;
    planeID   = hitID & mask;
    hitID   >>= word_shift;
    sectorID  = hitID & mask;
    hitID   >>= word_shift;
    wheelID   = hitID & mask;
    trtID     = hitID >> word_shift;

    // change trtID (which is 2/3 for endcaps) to use 0/1 in getEndcapElement
    if (trtID == 3) { trtID = 0; }
    else            { trtID = 1; }

    endcapElement = m_manager->getEndcapElement(trtID, wheelID, planeID, sectorID);

    if ( endcapElement ) {
      hashId  = endcapElement->identifyHash();
      IdLayer = endcapElement->identify();
      IdStraw = m_trt_id->straw_id(IdLayer, strawID);
    } else {
      ATH_MSG_ERROR ( "Could not find detector element for endcap identifier with "
                      << "(ipos,iwheel,isector,iplane,istraw) = ("
                      << trtID << ", " << wheelID << ", " << sectorID << ", "
                      << planeID << ", " << strawID << ")" );
      ATH_MSG_ERROR ( "If this happens very rarely, don't be alarmed (it is a Geant4 'feature')" );
      ATH_MSG_ERROR ( "If it happens a lot, you probably have misconfigured geometry in the sim. job." );
      statusok = false;
    }

  }

  return IdStraw;
}

//_____________________________________________________________________________
StatusCode TRTDigitizationTool::finalize() {

  if (m_printUsedDigSettings) {
    m_settings->print("TRTDigSettings Settings : ");
  }

  ATH_MSG_INFO ( "TRTDigitizationTool::finalize()" );

  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________

StatusCode TRTDigitizationTool::ConditionsDependingInitialization(const EventContext& ctx)
{
  SG::ReadCondHandle<AthenaAttributeList> digvers (m_digverscontainerkey, ctx);
  int dig_vers_from_condDB = (**digvers)["TRT_Dig_Vers"].data<int>();

  if (dig_vers_from_condDB!=0) {

    if (StatusCode::SUCCESS == m_settings->DigSettingsFromCondDB(dig_vers_from_condDB)) {
      ATH_MSG_INFO ( "Retrieved TRT_Settings from CondDB with TRT digitization version: digversion = " <<
                     dig_vers_from_condDB );
    } else {
      ATH_MSG_WARNING ( "Unknown TRT digitization version: digversion = " << dig_vers_from_condDB <<
                        " read from CondDB. Overriding to use default from Det Desc tag: " <<
                        m_settings->digVers() );
    }

  } else {
    ATH_MSG_WARNING ( "TRT digitization version: digversion = " << dig_vers_from_condDB <<
                      " read from CondDB. Overriding to use default from Det Desc tag: " <<
                      m_settings->digVers() );
  }

  return StatusCode::SUCCESS;

}

//_____________________________________________________________________________
double TRTDigitizationTool::getCosmicEventPhase(CLHEP::HepRandomEngine *rndmEngine) {
  // 13th February 2015: replace ComTime with a hack (fixme) based on an
  // event phase distribution from Alex (alejandro.alonso@cern.ch) that
  // is modelled as a Guassian of mean 5.48 ns and sigma 8.91 ns.
  return CLHEP::RandGaussZiggurat::shoot(rndmEngine, 5.48, 8.91);
}
