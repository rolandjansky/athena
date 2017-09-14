/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRTDigitizationTool.cxx
//
//   Implementation file for class TRTDigitizationTool
//
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TRTDigitizationTool.h"
#include "HitManagement/TimedHitCollection.h"
#include "InDetSimEvent/TRTUncompressedHitCollection.h"//hide?
#include "TRTProcessingOfStraw.h"
#include "TRTElectronicsProcessing.h"

#include "TRTDigCondBase.h"
#include "TRTDigCondFakeMap.h"

#include "TRTNoise.h"

#include "TRTElectronicsNoise.h"

#include "Identifier/Identifier.h"
#include "InDetSimData/InDetSimData.h"
#include "InDetSimData/InDetSimDataCollection.h"
#include "TRT_PAI_Process/ITRT_PAITool.h"
#include "TRT_Digitization/ITRT_SimDriftTimeTool.h"
#include "InDetSimEvent/TRTHitIdHelper.h"

#include "GeneratorObjects/HepMcParticleLink.h"

// Det descr includes:
#include "InDetIdentifier/TRT_ID.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "TRTDigSettings.h"

#include "InDetRawData/TRT_LoLumRawData.h"
#include "InDetRawData/TRT_RDO_Collection.h"

// Other includes
#include "PileUpTools/PileUpMergeSvc.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

// particle table
#include "HepPDT/ParticleDataTable.hh"
#include "GaudiKernel/IPartPropSvc.h"

// for the tool
#include "GaudiKernel/IToolSvc.h"

// Gaudi includes
#include "GaudiKernel/SmartDataPtr.h"

#include <set>
#include <sstream>
#include "TRT_ConditionsServices/ITRT_StrawNeighbourSvc.h"

//CondDB
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "TRT_ConditionsData/StrawStatusMultChanContainer.h"
#include <limits>
#include <cstdint>
static constexpr unsigned int crazyParticleBarcode(std::numeric_limits<int32_t>::max());
//Barcodes at the HepMC level are int

#include "CLHEP/Random/RandGaussZiggurat.h"

//AJB Temporary for debugging and development
//#include "driftCircle.h"

//_____________________________________________________________________________
TRTDigitizationTool::TRTDigitizationTool(const std::string& type,
					 const std::string& name,
					 const IInterface* parent)
  : PileUpToolBase(type, name, parent),
    m_TRTpaiToolXe("TRT_PAI_Process_Xe"),
    m_TRTpaiToolAr("TRT_PAI_Process_Ar"),
    m_TRTpaiToolKr("TRT_PAI_Process_Kr"),
    m_TRTsimdrifttimetool("TRT_SimDriftTimeTool"),
    m_mergeSvc("PileUpMergeSvc",name),
    m_pElectronicsProcessing(NULL),
    m_pProcessingOfStraw(NULL),
    m_pDigConditions(NULL),
    m_pNoise(NULL),
    m_container(NULL),
    m_atRndmGenSvc ("AtDSFMTGenSvc", name),
    m_TRTStrawNeighbourSvc("TRT_StrawNeighbourSvc",name),
    m_manager(NULL),
    m_trt_id(NULL),
    m_thpctrt(NULL),
    m_alreadyPrintedPDGcodeWarning(false),
    m_minCrossingTimeSDO(0.0),
    m_maxCrossingTimeSDO(0.0),
    m_minpileuptruthEkin(0.0),
    //m_ComTime(NULL),
    m_particleTable(NULL),
    m_dig_vers_from_condDB(-1),
    m_digverscontainerkey("/TRT/Cond/DigVers"),
    m_first_event(true),
    m_condDBdigverfoldersexists(false),
    m_HardScatterSplittingMode(0),
    m_HardScatterSplittingSkipper(false),
    m_UseGasMix(0),                  // postInclude UseGasMix for the whole detector: 0="default:use statusHT", 1="use Xe", 2="use Kr", 3="use Ar"
    m_cosmicEventPhase(0.0),
    m_particleFlag(0),
    m_sumSvc("TRT_StrawStatusSummarySvc","TRT_StrawStatusSummarySvc") // added by Sasha for Argon

{

  declareInterface<TRTDigitizationTool>(this);
  declareProperty("PAI_Tool_Xe", m_TRTpaiToolXe, "The PAI model for ionisation in the TRT Xe gas" );
  declareProperty("PAI_Tool_Ar", m_TRTpaiToolAr, "The PAI model for ionisation in the TRT Ar gas" );
  declareProperty("PAI_Tool_Kr", m_TRTpaiToolKr, "The PAI model for ionisation in the TRT Kr gas" );
  declareProperty("SimDriftTimeTool", m_TRTsimdrifttimetool, "Drift time versus distance (r-t-relation) for TRT straws" );
  declareProperty("MergeSvc", m_mergeSvc, "Merge service" );
  declareProperty("DataObjectName", m_dataObjectName="TRTUncompressedHits", "Data Object Name" );
  declareProperty("OutputObjectName", m_outputRDOCollName="TRT_RDOs",            "Output Object name" );
  declareProperty("OutputSDOName", m_outputSDOCollName="TRT_SDO_Map",            "Output SDO container name");

  declareProperty("PrintOverrideableSettings", m_printOverrideableSettings = false, "Print overrideable settings" );
  declareProperty("PrintDigSettings", m_printUsedDigSettings = true, "Print ditigization settings" );

  m_settings = new TRTDigSettings();
  m_settings->addPropertiesForOverrideableParameters(static_cast<AlgTool*>(this));
  declareProperty("RndmSvc",                       m_atRndmGenSvc, "Random Number Service used in TRT digitization" );
  declareProperty("TRT_StrawNeighbourSvc",         m_TRTStrawNeighbourSvc);
  declareProperty("InDetTRTStrawStatusSummarySvc", m_sumSvc);  // need for Argon
  declareProperty("UseGasMix",                     m_UseGasMix);
  declareProperty("HardScatterSplittingMode",      m_HardScatterSplittingMode);
  declareProperty("ParticleBarcodeVeto",           m_vetoThisBarcode=crazyParticleBarcode, "Barcode of particle to ignore");

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

  ATH_MSG_INFO ( "TRTDigitization::initialize() begin" );
  //ATH_MSG_DEBUG ( "TRTDigitization::initialize() begin" );

  // Get the TRT Detector Manager
  if (StatusCode::SUCCESS != detStore()->retrieve(m_manager,"TRT") ) {
    ATH_MSG_ERROR ( "Can't get TRT_DetectorManager " );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( "Retrieved TRT_DetectorManager with version "  << m_manager->getVersion().majorNum() );
  }

  if (detStore()->retrieve(m_trt_id, "TRT_ID").isFailure()) {
    ATH_MSG_ERROR ( "Could not get TRT ID helper" );
    return StatusCode::FAILURE;
  }

  // Fill setting defaults and process joboption overrides:
  m_settings->initialize(m_manager);

  if (m_printOverrideableSettings) {
    m_settings->printFlagsForOverrideableParameters("TRTDigSettings Overrideables : ");
  }

  /// Get the PAI Tool for Xe, Ar, Kr gas mixtures:

  if ( StatusCode::SUCCESS != m_TRTpaiToolXe.retrieve() ) {
    ATH_MSG_ERROR ( "Can't get the Xe PAI Tool" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( "Retrieved the PAI Tool for Xe straws" );
  }

  if ( StatusCode::SUCCESS != m_TRTpaiToolAr.retrieve() ) {
    ATH_MSG_WARNING ( "Can't get the Ar PAI Tool --> default PAI tool will be used for all straws (okay, if you don't use Argon straws)" );
  } else {
    ATH_MSG_DEBUG ( "Retrieved the PAI Tool for Ar straws" );
  }

  if ( StatusCode::SUCCESS != m_TRTpaiToolKr.retrieve() ) {
    ATH_MSG_WARNING ( "Can't get the Kr PAI Tool --> default PAI tool will be used for all straws (okay, if you don't use Krypton straws)" );
  } else {
    ATH_MSG_DEBUG ( "Retrieved the PAI Tool for Kr straws" );
  }

  /// Get the Sim-DriftTime Tool:
  if ( StatusCode::SUCCESS != m_TRTsimdrifttimetool.retrieve() ) {
    ATH_MSG_ERROR ( "Can't get the Sim. Drifttime Tool" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( "Retrieved the Sim. Drifttime Tool" );
  }

  // Get Random Service
  if (!m_atRndmGenSvc.retrieve().isSuccess()) {
    ATH_MSG_FATAL ( "Could not initialize Random Number Service." );
    return StatusCode::FAILURE;
  }

  m_pHRengine = m_atRndmGenSvc->GetEngine("TRT_DigitizationTool"); // For cosmic event phase

  // Get the Particle Properties Service
  IPartPropSvc* p_PartPropSvc = 0;
  static const bool CREATEIFNOTTHERE(true);
  StatusCode PartPropStatus = service("PartPropSvc", p_PartPropSvc, CREATEIFNOTTHERE);
  if ( !PartPropStatus.isSuccess() || 0 == p_PartPropSvc ) {
    ATH_MSG_ERROR ( "Could not initialize Particle Properties Service" );
    return PartPropStatus;
  }
  m_particleTable = p_PartPropSvc->PDT();

  //locate the PileUpMergeSvc and initialize our local ptr
  if (!m_mergeSvc.retrieve().isSuccess()) {
    ATH_MSG_ERROR ( "Could not find PileUpMergeSvc" );
    return StatusCode::FAILURE;
  }

  //Retrieve TRT_StrawNeighbourService.
  if (!m_TRTStrawNeighbourSvc.retrieve().isSuccess()) {
    ATH_MSG_FATAL ( "Could not get StrawNeighbourSvc!" );
    return StatusCode::FAILURE;
  }

  //   // Create new  TRT_RDO_Container
  //   m_container = new TRT_RDO_Container(m_trt_id->straw_layer_hash_max());
  //   ATH_MSG_DEBUG ( " TRT_RDO_Container created " );
  //   // Prevent SG from deleting object
  //   m_container->addRef();
  //   ATH_MSG_DEBUG ( " TRT_RDO_Container created " );

  // Check data object name
  if (m_dataObjectName == "")  {
    ATH_MSG_FATAL ( "Property DataObjectName not set!" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( "Input hits: " << m_dataObjectName );
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

  //CondDB access
  m_condDBdigverfoldersexists = detStore()->StoreGateSvc::contains<AthenaAttributeList>(m_digverscontainerkey) ;
  // Register callback function for cache updates:
  if (m_condDBdigverfoldersexists) {
    const DataHandle<AthenaAttributeList> aptr; //CondAttrListCollection
    if (StatusCode::SUCCESS == detStore()->regFcn(&TRTDigitizationTool::update,this, aptr, m_digverscontainerkey )) {
      ATH_MSG_DEBUG ("Registered callback for TRT_Digitization.");
    } else {
      ATH_MSG_ERROR ("Callback registration failed for TRT_Digitization! ");
    }
  }

  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode TRTDigitizationTool::prepareEvent(unsigned int)
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

  SubEventIterator iEvt(bSubEvents);

  while (iEvt != eSubEvents) {
    StoreGateSvc& seStore(*iEvt->ptr()->evtStore());
    PileUpTimeEventIndex thisEventIndex(PileUpTimeEventIndex(static_cast<int>(iEvt->time()),iEvt->index()));
    const TRTUncompressedHitCollection* seHitColl(NULL);
    if (!seStore.retrieve(seHitColl,m_dataObjectName).isSuccess()) {
      ATH_MSG_ERROR ( "SubEvent TRTUncompressedHitCollection not found in StoreGate " << seStore.name() );
      return StatusCode::FAILURE;
    }
    ATH_MSG_VERBOSE ( "TRTUncompressedHitCollection found with " << seHitColl->size() << " hits" );
    //Copy Hit Collection
    //TRTUncompressedHitCollection* trtHitColl(new TRTUncompressedHitCollection("TRTUncompressedHits"));
    TRTUncompressedHitCollection* trtHitColl(new TRTUncompressedHitCollection("TRTUncompressedHits", seHitColl->size())); // saves memory
    TRTUncompressedHitCollection::const_iterator i(seHitColl->begin());
    TRTUncompressedHitCollection::const_iterator e(seHitColl->end());
    // Read hits from this collection
    for (; i!=e; ++i) {
      const TRTUncompressedHit trthit(*i);
      trtHitColl->Insert(trthit);
    }
    m_thpctrt->insert(thisEventIndex, trtHitColl);
    //store these for deletion at the end of mergeEvent
    m_trtHitCollList.push_back(trtHitColl);

    ++iEvt;
  }

  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode TRTDigitizationTool::lateInitialize() {

  m_first_event=false;

  if (m_condDBdigverfoldersexists) {

    if ( ConditionsDependingInitialization().isFailure() ) {
      ATH_MSG_ERROR ( "Folder holder TRT digitization version exists in condDB, but tag is faulty" );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG ( "Using Digitization version as defined in conditions tag" );
    }

  } else {
    ATH_MSG_DEBUG ( "No folder containing TRT digitization version found in CondDB. Using default from Det Desc tag: " << m_settings->digVers() );
  }

  //Resuming initialiazation. Section below had to be moved into event loop due to dependence on conditions data

  TRTElectronicsNoise *electronicsNoise(NULL);
  if ( m_settings->noiseInUnhitStraws() || m_settings->noiseInSimhits() ) {
    electronicsNoise = new TRTElectronicsNoise(m_settings, m_atRndmGenSvc);
  }
  // ElectronicsProcessing is needed for the regular straw processing,
  // but also for the noise (it assumes ownership of electronicsnoise )
  m_pElectronicsProcessing = new TRTElectronicsProcessing( m_settings, m_atRndmGenSvc, electronicsNoise );

  m_pDigConditions = new TRTDigCondFakeMap(m_settings,
					   m_manager,
					   m_atRndmGenSvc,
					   m_trt_id,
					   m_UseGasMix,
					   m_sumSvc);

  m_pDigConditions->initialize();

  if ( m_settings->noiseInUnhitStraws() || m_settings->noiseInSimhits() ) {

    // In short this next constructor does 3 things;
    //     i)  tunes the amplitude of the electronics noise,
    //    ii) creates a pool of noise digits,
    //   iii) figures out exact low thresholds needed to reproduce actual
    //        straw noise-frequencies:
    m_pNoise = new TRTNoise( m_settings,
			     m_manager,
			     m_atRndmGenSvc,
			     m_pDigConditions,
			     m_pElectronicsProcessing,
			     electronicsNoise,
			     m_trt_id,
			     m_UseGasMix,
			     m_sumSvc);

    ATH_MSG_DEBUG ( "Average straw noise level is " << m_pDigConditions->strawAverageNoiseLevel() );

  } else {
    m_pNoise = NULL;
  }

  ITRT_PAITool *TRTpaiToolXe = &(* m_TRTpaiToolXe);
  ITRT_PAITool *TRTpaiToolAr = &(* m_TRTpaiToolAr);
  ITRT_PAITool *TRTpaiToolKr = &(* m_TRTpaiToolKr);

  ITRT_SimDriftTimeTool *pTRTsimdrifttimetool = &(*m_TRTsimdrifttimetool);

  m_pProcessingOfStraw =
    new TRTProcessingOfStraw( m_settings,
			      m_manager,
			      TRTpaiToolXe,
			      pTRTsimdrifttimetool,
			      m_atRndmGenSvc,
			      m_pElectronicsProcessing,
			      m_pNoise,
			      m_pDigConditions,
			      m_particleTable,
			      m_trt_id,
			      TRTpaiToolAr,
			      TRTpaiToolKr);

  ATH_MSG_INFO ( "Gas Property:             UseGasMix is " << m_UseGasMix );

  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode TRTDigitizationTool::processStraws(std::set<int>& sim_hitids, std::set<Identifier>& simhitsIdentifiers) {

  // Create a map for the SDO
  InDetSimDataCollection *simDataMap(new InDetSimDataCollection);

  // Register the map into StoreGate
  if ( evtStore()->record(simDataMap, m_outputSDOCollName).isFailure() ) {
    ATH_MSG_FATAL ( "InDetSimData map " << m_outputSDOCollName << " could not be registered in StoreGate !" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( "InDetSimData map " << m_outputSDOCollName << " registered in StoreGate" );
  }

  m_cosmicEventPhase = 0.0;
  if (m_settings->doCosmicTimingPit()) {
    m_cosmicEventPhase = getCosmicEventPhase();
    //std::cout << "AJB " << m_cosmicEventPhase << std::endl;
  };

  // Create  a vector of deposits
  std::vector<InDetSimData::Deposit> depositVector(100);

  // loop over all straws
  TimedHitCollection<TRTUncompressedHit>::const_iterator i, e;
  while (m_thpctrt->nextDetectorElement(i, e)) {

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
    for (TimedHitCollection<TRTUncompressedHit>::const_iterator hit_iter(i); hit_iter != e; ++hit_iter ) {

      // create a new deposit
      InDetSimData::Deposit deposit( HepMcParticleLink((*hit_iter)->GetTrackID(), hit_iter->eventId()), (*hit_iter)->GetEnergyDeposit() );
      if(deposit.first.barcode()==0 || deposit.first.barcode() == m_vetoThisBarcode){
          continue;
      }
      ATH_MSG_VERBOSE ( "Deposit: trackID " << deposit.first << " energyDeposit " << deposit.second );
      depositVector.push_back(deposit);
      // evtIndex != 0 ? hit_iter->eventId() : 0 ;   commented for now. Once mc09 campaign is over, consider to implement + implement cuts on hitTime in addition/in stead bunchcrossing cuts.
    }

    const TimedHitPtr<TRTUncompressedHit>& theHit(*i);
    //double globalHitTime(hitTime(theHit));
    //double globalTime(static_cast<double>(theHit->GetGlobalTime()));
    //double bunchCrossingTime(globalHitTime - globalTime);
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

    // This will be set in TRTProcessingOfStraw::ProcessStraw
    // according to what types of particles hit the straw.
    m_particleFlag=0;

    // if StatusHT == 6 thats emulate argon, ==7 that's emulate krypton
    bool emulateArFlag = m_sumSvc->getStatusHT(idStraw) == 6;
    bool emulateKrFlag = m_sumSvc->getStatusHT(idStraw) == 7;

    m_pProcessingOfStraw->ProcessStraw(i, e, digit_straw,
                                       m_alreadyPrintedPDGcodeWarning,
                                       m_cosmicEventPhase, //m_ComTime,
                                       StrawGasType(idStraw),
				       emulateArFlag,
				       emulateKrFlag,
                                       m_particleFlag);

    // Sorry, a lot of test code here before the output digit is saved.

    // query m_particleFlag bits 0 to 15 (left-to-right)
    //std::cout << "AJB "; for (unsigned i=0;i<16;i++) std::cout << particleFlagQueryBit(i,m_particleFlag); std::cout << std::endl;

    //AJB, Print out the digits etc (for debugging)
    //int          mstrw = digit_straw.GetStrawID();
    //unsigned int mword = digit_straw.GetDigit();
    //std::cout << "AJB " << mstrw << ":" << mword << std::endl;
    //print_mword_properties(mword); // AJB
    //std::cout << "AJB "; bits24(mword);
    //std::cout << "AJB "; bits27(mword);

/*
    if (particleFlagQueryBit(10,m_particleFlag)) { // Do this only for pions > 10 GeV
      if ( digit_straw.GetDigit() ) {
        // bool HT=highLevel(mword);
        // if (HT) {
          unsigned int region = getRegion(hitID); // 1=barrelShort, 2=barrelLong, 3=ECA, 4=ECB
          // bool HT1=highLevel1(mword); bool HT2=highLevel2(mword); bool HT3=highLevel3(mword);
          std::cout << "AJB " << region
                    <<    " " << timeOverThreshold(mword)
                    <<    " " << rawDriftTime(mword)
                    <<    " " << highLevel2(mword)
                    << std::endl;
          //int ichip = 0; m_TRTStrawNeighbourSvc->getChip(idStraw, ichip);
          //int detid = m_trt_id->barrel_ec(idLayer); // +1=BA, -1=BC, +2=EA, -2=EC
          //if (detid==+1) std::cout << "AJBBA " << HT1 << " " << HT2 << " " << HT3 << std::endl;
          //if (detid==-1) std::cout << "AJBBC " << HT1 << " " << HT2 << " " << HT3 << std::endl;
          //if (detid==-2) std::cout << "AJBEC " << HT1 << " " << HT2 << " " << HT3 << std::endl;
          //if (detid==+2) std::cout << "AJBEA " << HT1 << " " << HT2 << " " << HT3 << std::endl;
       // }
      }
    }
*/

    // finally push back the output digit.
    if ( digit_straw.GetDigit() ) {
      m_vDigits.push_back(digit_straw);
      //int          mstrw = digit_straw.GetStrawID();
      //unsigned int mword = digit_straw.GetDigit();
      //std::cout << "AJB "; bits32(mword);
    }

  } // end of straw loop

  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode TRTDigitizationTool::processAllSubEvents() {

  if (m_first_event) {
    if(this->lateInitialize().isFailure()) {
      ATH_MSG_FATAL ( "lateInitialize method failed!" );
      return StatusCode::FAILURE;
    }
  }

  m_alreadyPrintedPDGcodeWarning = false;

  ATH_MSG_DEBUG ( "TRTDigitization::execute()" );

  //make new RDO container at each event
  try {
    m_container = new TRT_RDO_Container(m_trt_id->straw_layer_hash_max());
  } catch (std::bad_alloc) {
    ATH_MSG_FATAL ( "Could not create a new TRT_RDO_Container!" );
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG ( " TRT_RDO_Container created " );

  //  Register it to StoreGate
  if ( evtStore()->record(m_container, m_outputRDOCollName).isFailure() ) {
    ATH_MSG_FATAL ( "Container " << m_outputRDOCollName << " could not be registered in StoreGate!" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( "Container " << m_outputRDOCollName << " registered in StoreGate" );
  }

  m_vDigits.clear();

  //Set of all hitid's with simhits (used for noise simulation).
  std::set<int> sim_hitids;
  std::set<Identifier> simhitsIdentifiers;

  //  get the container(s)
  typedef PileUpMergeSvc::TimedList<TRTUncompressedHitCollection>::type TimedHitCollList;

  //this is a list<pair<time_t, DataLink<TRTUncompressedHitCollection> > >
  TimedHitCollList hitCollList;
  unsigned int numberOfSimHits(0);
  if ( !(m_mergeSvc->retrieveSubEvtsData(m_dataObjectName, hitCollList, numberOfSimHits).isSuccess()) && hitCollList.size()==0 ) {
    ATH_MSG_ERROR ( "Could not fill TimedHitCollList" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( hitCollList.size() << " TRTUncompressedHitCollections with key " << m_dataObjectName << " found" );
  }

  // Define Hit Collection
  TimedHitCollection<TRTUncompressedHit> thpctrt(numberOfSimHits);

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
  m_thpctrt = &thpctrt;

  // Process the Hits straw by straw: get the iterator pairs for given straw
  if(this->processStraws(sim_hitids, simhitsIdentifiers).isFailure()) {
    ATH_MSG_FATAL ( "processStraws method failed!" );
    return StatusCode::FAILURE;
  }

  // no more hits

  //Noise in straws without simhits:
  if (m_settings->noiseInUnhitStraws()) {
    const int numberOfDigitsBeforeNoise(m_vDigits.size());

    m_pNoise->appendPureNoiseToProperDigits(m_vDigits, sim_hitids);
    if (m_settings->doCrosstalk()) {
      m_pNoise->appendCrossTalkNoiseToProperDigits(m_vDigits, simhitsIdentifiers,m_TRTStrawNeighbourSvc);
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

//_____________________________________________________________________________
StatusCode TRTDigitizationTool::mergeEvent() {
  std::vector<std::pair<unsigned int, int> >::iterator ii(m_seen.begin());
  std::vector<std::pair<unsigned int, int> >::iterator ee(m_seen.end());
  while (ii != ee) {
    ATH_MSG_DEBUG( "mergeEvent: there are " << ii->first << " events in bunch xing " << ii->second );
    ++ii;
  }

  if (m_first_event) {
    if(this->lateInitialize().isFailure()) {
      ATH_MSG_FATAL ( "lateInitialize method failed!" );
      return StatusCode::FAILURE;
    }
  }

  m_alreadyPrintedPDGcodeWarning = false;

  ATH_MSG_DEBUG ( "TRTDigitization::execute()"  );

  //make new RDO container at each event
  try {
    m_container = new TRT_RDO_Container(m_trt_id->straw_layer_hash_max());
  } catch (std::bad_alloc) {
    ATH_MSG_FATAL ( "Could not create a new TRT_RDO_Container !" );
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG ( " TRT_RDO_Container created " );

  //  Register it to StoreGate
  if ( evtStore()->record(m_container, m_outputRDOCollName ).isFailure() ) {
    ATH_MSG_FATAL ( "Container " << m_outputRDOCollName << " could not be registered in StoreGate !" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( "Container " << m_outputRDOCollName << " registered in StoreGate" );
  }

  //Set of all hitid's with simhits (used for noise simulation).
  std::set<int> sim_hitids;
  std::set<Identifier> simhitsIdentifiers;

  // Process the Hits straw by straw:
  //   get the iterator pairs for given straw
  if(this->processStraws(sim_hitids, simhitsIdentifiers).isFailure()) {
    ATH_MSG_FATAL ( "processStraws method failed!" );
    return StatusCode::FAILURE;
  }

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

    m_pNoise->appendPureNoiseToProperDigits(m_vDigits, sim_hitids);
    if (m_settings->doCrosstalk()) {
      m_pNoise->appendCrossTalkNoiseToProperDigits(m_vDigits, simhitsIdentifiers,m_TRTStrawNeighbourSvc);
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
  TRT_RDO_Collection *RDOColl(NULL);

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
      if (m_container->addCollection(RDOColl, RDOColl->identifyHash()).isFailure()) {
	ATH_MSG_FATAL ( "Container " << m_outputRDOCollName << " could not be registered in StoreGate !" );
	return StatusCode::FAILURE;
      } else {
	ATH_MSG_DEBUG ( "Container " << m_outputRDOCollName << " registered in StoreGate" );
      }
    }

    // Put RDO into Collection
    TRT_LoLumRawData *p_rdo(new TRT_LoLumRawData(idStraw, TRTDigitIter->GetDigit()));
    if (RDOColl!=NULL) { RDOColl->push_back(p_rdo); }
    else {
      ATH_MSG_FATAL ( "Failed to create the TRT_RDO_Collection before trying to add an RDO to it! IdHash = " << static_cast<int>(IdHash) );
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

/* ----------------------------------------------------------------------------------- */
// Callback function to update constants from database:
/* ----------------------------------------------------------------------------------- */

StatusCode TRTDigitizationTool::update( IOVSVC_CALLBACK_ARGS_P(I,keys) ) {

  ATH_MSG_INFO ("Updating condition settings TRT_Digitization! ");

  const AthenaAttributeList* atrlist(NULL);

  if (StatusCode::SUCCESS == detStore()->retrieve(atrlist, m_digverscontainerkey ) && atrlist != 0) {
    std::list<std::string>::const_iterator itr;
    if (msgLvl(MSG::INFO)) {
      for( itr=keys.begin(); itr !=keys.end(); ++itr) {
	msg(MSG::INFO)<< "IOVCALLBACK for key "<< *itr << " number " << I << endmsg;
      }
    }
    m_dig_vers_from_condDB =(*atrlist)["TRT_Dig_Vers"].data<int>();
  } else {
    ATH_MSG_ERROR ( "Problem reading condDB object." );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
int TRTDigitizationTool::StrawGasType(Identifier& TRT_Identifier) const {

  // TRT/Cond/StatusHT provides: enum { Undefined, Dead(Ar), Good(Xe), Xenon(Xe), Argon(Ar), Krypton(Kr) }
  // The m_UseGasMix default behaviour (0) is to use TRT/Cond/StatusHT, other values can be set to force
  // the whole detector to (1)Xenon, (2)Krypton, (3)Argon:

  int strawGasType=99;

  if (m_UseGasMix==0) { // use StatusHT
    int stat =  m_sumSvc->getStatusHT(TRT_Identifier);
    if       ( stat==2 || stat==3 ) { strawGasType = 0; } // Xe
    else if  ( stat==5 )            { strawGasType = 1; } // Kr
    else if  ( stat==1 || stat==4 ) { strawGasType = 2; } // Ar
    else if  ( stat==6 )            { strawGasType = 0; } // Xe
    else if  ( stat==7 )            { strawGasType = 0; } // Xe
    // stat==6 is emulate argon, make it xenon here,
    // and emulate argon later with reduced TR eff.
    // stat==7 is emulate krypton, make it xenon here too.
  }
  else if (m_UseGasMix==1) { strawGasType = 0; } // force whole detector to Xe
  else if (m_UseGasMix==2) { strawGasType = 1; } // force whole detector to Kr
  else if (m_UseGasMix==3) { strawGasType = 2; } // force whole detector to Ar

  return strawGasType;

}

//_____________________________________________________________________________

StatusCode TRTDigitizationTool::ConditionsDependingInitialization() {

  if (m_dig_vers_from_condDB!=0) {

    if (StatusCode::SUCCESS == m_settings->DigSettingsFromCondDB(m_dig_vers_from_condDB)) {
      ATH_MSG_INFO ( "Retrieved TRT_Settings from CondDB with TRT digitization version: digversion = " <<
                      m_dig_vers_from_condDB );
    } else {
      ATH_MSG_WARNING ( "Unknown TRT digitization version: digversion = " << m_dig_vers_from_condDB <<
                        " read from CondDB. Overriding to use default from Det Desc tag: " <<
                        m_settings->digVers() );
    }

  } else {
    ATH_MSG_WARNING ( "TRT digitization version: digversion = " << m_dig_vers_from_condDB <<
                      " read from CondDB. Overriding to use default from Det Desc tag: " <<
                      m_settings->digVers() );
  }

  return StatusCode::SUCCESS;

}

//_____________________________________________________________________________
//The meaning of these 16 bits is given in TRTProcessingOfStraw::particleFlagSetBit()
bool TRTDigitizationTool::particleFlagQueryBit(int bitposition, unsigned short particleFlag) const {
  if ( bitposition >= 0 && bitposition < 16) {
    return (1 << bitposition) & particleFlag;
  } else {
    ATH_MSG_WARNING ( "Trying to read bit position " << bitposition << " in unsigned short m_particleFlag !");
    return 0; // returning a false neagtive is preferred over a false positive.
  }
}

//_____________________________________________________________________________
unsigned int TRTDigitizationTool::getRegion(int hitID) {
// 1=barrelShort, 2=barrelLong, 3=ECA, 4=ECB
  const int mask(0x0000001F);
  const int word_shift(5);
  int layerID, ringID, wheelID;
  unsigned int region(0);

  if ( !(hitID & 0x00200000) ) { // barrel

    hitID >>= word_shift;
    layerID = hitID & mask;
    hitID >>= word_shift;
    hitID >>= word_shift;
    ringID = hitID & mask;
    region = ( (layerID < 9) && (ringID == 0) ) ? 1 : 2;

  } else { // endcap

    hitID >>= word_shift;
    hitID >>= word_shift;
    hitID >>= word_shift;
    wheelID = hitID & mask;
    region = wheelID < 8 ?  3 : 4;

  }

  return region;

}

double TRTDigitizationTool::getCosmicEventPhase() {
  // 13th February 2015: replace ComTime with a hack (fixme) based on an
  // event phase distribution from Alex (alejandro.alonso@cern.ch) that
  // is modelled as a Guassian of mean 5.48 ns and sigma 8.91 ns.
  return CLHEP::RandGaussZiggurat::shoot(m_pHRengine, 5.48, 8.91);
}
