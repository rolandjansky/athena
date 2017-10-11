/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// MdtDigitizationTool:
//            Athena algorithm which produces MDTDigits out of MHits.
// ------------
// Authors:
//            Daniela Rebuzzi (daniela.rebuzzi@pv.infn.it)
//            Ketevi A. Assamagan (ketevi@lbn.gov) 
//             Modified by:
//             2004-03-02 Niels Van Eldik (nveldik@nikhef.nl)
//             Modified by:
//             2004-04-05 Daniel Levin (dslevin@umich.edu)
//             generate drift time deltas based on wire sag.
//             first pass: assume wires are geomtrically centered-
//             but have top/bottom RT functions that would obtain 
//             if the wire was off  axis by an amount determined 
//             by the tube length, orientation, hit location and track angle
//             Modified by:
//             2004-02-08 Daniel Levin (dslevin@umich.edu)
//             generate new impact parameters based on physical wire sag.
//             2012-7-5 Shannon Walch (srwalch@umich.edu)
//             implement bug fixes in RT wiresag calculations
//             Modified by:
//             Dinos Bachas (konstantinos.bachas@cern.ch)
//
////////////////////////////////////////////////////////////////////////////////

//Inputs
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/MuonSimData.h"

//Outputs
#include "MuonDigitContainer/MdtDigitContainer.h"

//MDT digitization includes
#include "MDT_Digitization/MdtDigitizationTool.h"
#include "MDT_Digitization/IMDT_DigitizationTool.h"
#include "MDT_Digitization/MdtDigiToolInput.h"
#include "MDT_Digitization/chargeCalculator.h" 
#include "MDT_Digitization/particleGamma.h" 

//Gaudi - Core
#include "PathResolver/PathResolver.h"

//Geometry
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonSimEvent/MdtHitIdHelper.h"
#include "TrkDetDescrUtils/GeometryStatics.h"
#include "EventPrimitives/EventPrimitives.h"

//Pile-up
#include "PileUpTools/PileUpMergeSvc.h"

//Truth
#include "CLHEP/Units/PhysicalConstants.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "HepMC/GenParticle.h" 

//Random Numbers
#include "AthenaKernel/IAtRndmGenSvc.h"

//Calibration Service
#include "MdtCalibData/MdtFullCalibData.h"
#include "MdtCalibData/MdtTubeCalibContainer.h"
#include "MdtCalibSvc/MdtCalibrationDbSvc.h"


MdtDigitizationTool::MdtDigitizationTool(const std::string& type,const std::string& name,const IInterface* pIID)
  : PileUpToolBase(type, name, pIID)
  , m_digitContainer(0)
  , m_sdoContainer(0)
  , m_idHelper(0)
  , m_muonHelper(0)
  , m_MuonGeoMgr(0)
  , m_digiTool("MDT_Response_DigiTool", this)
  , m_inv_c_light(1./(CLHEP::c_light))
  , m_thpcMDT(0)
  , m_mergeSvc(0)
  , m_inputObjectName("")
  , m_outputObjectName("")
  , m_rndmSvc("AtRndmGenSvc", name )
  , m_rndmEngine(0)
  , m_rndmEngineName("MuonDigitization")
  , m_twinRndmSvc("AtRndmGenSvc", name )                         // TWIN TUBE
  , m_twinRndmEngine(0)                                          // TWIN TUBE
  , m_twinRndmEngineName("MuonDigitizationTwin")                 // TWIN TUBE
  , m_calibDbSvc("MdtCalibrationDbSvc", name) 
  , m_pSummarySvc("MDTCondSummarySvc", name)
{
  declareInterface<IMuonDigitizationTool>(this);

  //Random numbers service	          
  declareProperty("RndmSvc", 	         m_rndmSvc,                           "Random Number Service used in Muon digitization");
  declareProperty("RndmEngine",          m_rndmEngineName,                    "Random engine name");
  declareProperty("DigitizationTool",    m_digiTool,                          "Tool which handle the digitization process");
  //Conditions Database
  declareProperty("MdtCalibrationDbSvc", m_calibDbSvc);
  declareProperty("MDTCondSummarySvc",   m_pSummarySvc);
  declareProperty("UseDeadChamberSvc",   m_UseDeadChamberSvc   =  false );
  declareProperty("GetT0FromBD",         m_t0_from_DB          =  false );
  //TDC electronics
  declareProperty("OffsetTDC",           m_offsetTDC           =  800.,       "TDC offset");
  declareProperty("ns2TDC",              m_ns2TDC              =  0.78125,  "Conversion factor TDC/ns");
  declareProperty("ResolutionTDC",       m_resTDC              =  0.5,      "TDC resolution");
  declareProperty("SignalSpeed",         m_signalSpeed         =  299.792458, "Light speed" );
  //Object names		          
  declareProperty("InputObjectName",     m_inputObjectName     =  "MDT_Hits");
  declareProperty("OutputObjectName",    m_outputObjectName    =  "MDT_DIGITS");
  declareProperty("OutputSDOName",       m_outputSDOName       =  "MDT_SDO");
  //Corrections		          
  declareProperty("UseAttenuation",      m_useAttenuation      =  false);
  declareProperty("UseTof",              m_useTof              =  true,       "Option for the tof calculation");
  declareProperty("UseProp",             m_useProp             =  true);
  declareProperty("UseWireSagGeom",      m_useWireSagGeom      =  false,      "Option for the wire sagitta correction");
  declareProperty("UseWireSagRT",        m_useWireSagRT        =  false,      "Option for the wire sagitta correction");
  declareProperty("UseDeformations",     m_useDeformations     =  false);
  //Timing scheme		          
  declareProperty("UseTimeWindow",       m_useTimeWindow       =  true);
  declareProperty("BunchCountOffset",    m_bunchCountOffset    =  -200.,      "Bunch crossing offset");

  declareProperty("MatchingWindow",      m_matchingWindow      =  1000.,      "Matching window");
  declareProperty("MaskWindow",          m_maskWindow          =   250.,      "Masked window");  // was 700 for large time window
  declareProperty("DeadTime",            m_deadTime            =   700.,      "MDT drift tube dead time");
  declareProperty("DiscardEarlyHits",    m_DiscardEarlyHits    =  true);
  //Configurations
  declareProperty("CheckSimHits",        m_checkMDTSimHits     =  true,       "Control on the hit validity");
  declareProperty("MaskedStations",      m_maskedStations,                    "Stations to be masked at digi level");
  //Twin Tube  
  declareProperty("UseTwin",             m_useTwin             =  false);
  declareProperty("UseAllBOLTwin",       m_useAllBOLTwin       =  false);
  declareProperty("TwinRndmSvc", 	 m_twinRndmSvc,                       "Random Number Service used in Muon digitization for Twin Tubes");
  declareProperty("TwinRndmEngine",      m_twinRndmEngineName,                "Random engine name for Twin Tubes");
  declareProperty("ResolutionTwinTube",  m_resTwin             =  1.05,    "Twin Tube resolution");
  //Multi-charge particle digitization
  declareProperty("DoQballCharge",       m_DoQballCharge       =  false,      "dEdx for Qballs with account of electric charge"); 
  //Cosmics
  declareProperty("UseOffSet1",          m_useOffSet1          =  true);
  declareProperty("UseOffSet2",          m_useOffSet2          =  true);
}


MdtDigitizationTool::~MdtDigitizationTool() {

}

StatusCode MdtDigitizationTool::initialize() {

  ATH_MSG_INFO ( "Configuration  MdtDigitizationTool" );
  ATH_MSG_INFO ( "RndmSvc                " << m_rndmSvc             );
  ATH_MSG_INFO ( "RndmEngine             " << m_rndmEngineName      );
  ATH_MSG_INFO ( "DigitizationTool       " << m_digiTool            );
  ATH_MSG_INFO ( "MdtCalibrationDbSvc    " << m_calibDbSvc          );
  ATH_MSG_INFO ( "MDTCondSummarySvc      " << m_pSummarySvc         );
  ATH_MSG_INFO ( "UseDeadChamberSvc      " << m_UseDeadChamberSvc   );
  if (!m_UseDeadChamberSvc) ATH_MSG_INFO ( "MaskedStations         " << m_maskedStations      );
  ATH_MSG_INFO ( "GetT0FromDB            " << m_t0_from_DB          );
  ATH_MSG_INFO ( "OffsetTDC              " << m_offsetTDC           );
  ATH_MSG_INFO ( "ns2TDC                 " << m_ns2TDC              );
  ATH_MSG_INFO ( "ResolutionTDC          " << m_resTDC              );
  ATH_MSG_INFO ( "SignalSpeed            " << m_signalSpeed         );
  ATH_MSG_INFO ( "InputObjectName        " << m_inputObjectName     );
  ATH_MSG_INFO ( "OutputObjectName       " << m_outputObjectName    );
  ATH_MSG_INFO ( "OutputSDOName          " << m_outputSDOName       );
  ATH_MSG_INFO ( "UseAttenuation         " << m_useAttenuation      );
  ATH_MSG_INFO ( "UseTof                 " << m_useTof              );
  ATH_MSG_INFO ( "UseProp                " << m_useProp             );
  ATH_MSG_INFO ( "UseWireSagGeom         " << m_useWireSagGeom      );
  ATH_MSG_INFO ( "UseWireSagRT           " << m_useWireSagRT        );
  ATH_MSG_INFO ( "UseDeformations        " << m_useDeformations     );
  ATH_MSG_INFO ( "UseTimeWindow          " << m_useTimeWindow       );
  ATH_MSG_INFO ( "BunchCountOffset       " << m_bunchCountOffset    );
  ATH_MSG_INFO ( "MatchingWindow         " << m_matchingWindow      );
  ATH_MSG_INFO ( "MaskWindow             " << m_maskWindow          );
  ATH_MSG_INFO ( "DeadTime               " << m_deadTime            );
  ATH_MSG_INFO ( "DiscardEarlyHits       " << m_DiscardEarlyHits    );
  ATH_MSG_INFO ( "CheckSimHits           " << m_checkMDTSimHits     );
  ATH_MSG_INFO ( "UseTwin                " << m_useTwin             );
  ATH_MSG_INFO ( "UseAllBOLTwin          " << m_useAllBOLTwin       );
  ATH_MSG_INFO ( "TwinRndmSvc            " << m_twinRndmSvc         );
  ATH_MSG_INFO ( "TwinRndmEngine         " << m_twinRndmEngineName  );
  ATH_MSG_INFO ( "ResolutionTwinTube     " << m_resTwin             );
  ATH_MSG_INFO ( "DoQballCharge          " << m_DoQballCharge       );
  if(!m_useTof) {
    ATH_MSG_INFO ( "UseCosmicsOffSet1      " << m_useOffSet1          );
    ATH_MSG_INFO ( "UseCosmicsOffSet2      " << m_useOffSet2          );
  }


  // initialize transient detector store and MuonGeoModel OR MuonDetDescrManager  
  if(detStore()->contains<MuonGM::MuonDetectorManager>( "Muon" )){
    if (detStore()->retrieve(m_MuonGeoMgr).isFailure()) {
      ATH_MSG_FATAL ( "Could not retrieve MuonGeoModelDetectorManager!" );
      return StatusCode::FAILURE;
    } 
    else {
      ATH_MSG_DEBUG ( "Retrieved MuonGeoModelDetectorManager from StoreGate" );
      //initialize the MdtIdHelper
      m_idHelper  = m_MuonGeoMgr->mdtIdHelper();
      if(!m_idHelper) return StatusCode::FAILURE;
      ATH_MSG_DEBUG ( "Retrieved MdtIdHelper " << m_idHelper );
    }
  }  


  // check the input object name
  if (m_inputObjectName=="") {
    ATH_MSG_FATAL ( "Property InputObjectName not set !" );
    return StatusCode::FAILURE;
  } 
  else {
    ATH_MSG_DEBUG ( "Input objects: '" << m_inputObjectName << "'" );
  }
  
  // check the output object name
  if (m_outputObjectName=="") {
    ATH_MSG_FATAL ( "Property OutputObjectName not set !" );
    return StatusCode::FAILURE;
  } 
  else {
    ATH_MSG_DEBUG ( "Output digits: '" << m_outputObjectName << "'" );
  }
  
  //initialize the digit container  
  try{
    m_digitContainer = new MdtDigitContainer(m_idHelper->module_hash_max());
  } 
  catch(std::bad_alloc){
    ATH_MSG_FATAL ( "Could not create a new MdtDigitContainer!" );
    return StatusCode::FAILURE;
  }
  m_digitContainer->addRef();
  
  //simulation identifier helper	
  m_muonHelper = MdtHitIdHelper::GetHelper();
  
  //get the r->t conversion tool
  if( m_digiTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL("Could not retrieve digitization tool! " << m_digiTool);
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG("Retrieved digitization tool!" << m_digiTool);
  }

  // initialize inverse lightSpeed (c_light in m/s)
  m_inv_c_light = 1./(CLHEP::c_light);
  
  if (!m_rndmSvc.retrieve().isSuccess()) {
    ATH_MSG_ERROR("Could not initialize Random Number Service");
  }      
  
  // getting our random numbers stream
  ATH_MSG_DEBUG ( "Getting random number engine : <" << m_rndmEngineName << ">" );
  if (!m_rndmSvc.retrieve().isSuccess()){
    ATH_MSG_ERROR("Could not initialize Random Number Service");
  }
  m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
  if (m_rndmEngine==0) {
    ATH_MSG_ERROR("Could not find RndmEngine : " << m_rndmEngineName );
    return StatusCode::FAILURE;
  }
  
  // TWIN TUBE
  if (m_useTwin){
    if (!m_twinRndmSvc.retrieve().isSuccess()) {
      ATH_MSG_ERROR("Could not initialize Random Number Service for Twin Tubes");
    }      
    // getting our random numbers stream
    ATH_MSG_DEBUG("Getting random number engine : <" << m_twinRndmEngineName << ">" );
    m_twinRndmEngine = m_twinRndmSvc->GetEngine(m_twinRndmEngineName);
    if (m_twinRndmEngine==0) {
      ATH_MSG_ERROR("Could not find RndmEngine : " << m_twinRndmEngineName );
      return StatusCode::FAILURE;
    }
  }
  
  
  // Get pointer to MdtCalibrationDbSvc and cache it :
  if ( m_t0_from_DB ){
    if ( !m_calibDbSvc.retrieve().isSuccess() ) {
      ATH_MSG_FATAL("Unable to retrieve pointer to MdtCalibrationDbSvc");
      return StatusCode::FAILURE;
    }
  }
  
  //Gather masked stations
  for (unsigned int i=0;i<m_maskedStations.size();i++) {
    std::string mask=m_maskedStations[i];
    std::string maskedName=mask.substr(0,mask.find(':'));
    std::string temps=mask.substr(maskedName.size()+1,std::string::npos);
    std::string maskedEta=temps.substr(0,temps.find(':'));
    std::string maskedPhi=temps.substr(maskedEta.size()+1,std::string::npos);
    maskedStation ms(maskedName,maskedEta,maskedPhi);
    m_vMaskedStations.push_back(ms);
    if (!m_UseDeadChamberSvc ) ATH_MSG_DEBUG ( "mask = " << mask << "  maskedName = " << maskedName <<  "  temps = " << temps << "  maskedEta = " <<maskedEta << "  maskedPhi = " << maskedPhi ); 
  }
  
  //Retrieve the Conditions service
  if (m_UseDeadChamberSvc==true) {
    ATH_MSG_DEBUG("Using Database DCS MDT Conditions for masking dead/missing chambers");
    if (StatusCode::SUCCESS != m_pSummarySvc.retrieve()) {
      ATH_MSG_ERROR("Could not retrieve the summary service");
    }
  }
  else {
    ATH_MSG_DEBUG("Using JobOptions for masking dead/missing chambers");
  }
  
  return StatusCode::SUCCESS;
}


StatusCode MdtDigitizationTool::prepareEvent(unsigned int nInputEvents) {
  
  ATH_MSG_DEBUG("MdtDigitizationTool::prepareEvent() called for " << nInputEvents << " input events" );

  m_MDTHitCollList.clear();
  m_thpcMDT = new TimedHitCollection<MDTSimHit>() ;
  
  return StatusCode::SUCCESS;
}

StatusCode MdtDigitizationTool::processBunchXing(int bunchXing,
                                                 SubEventIterator bSubEvents,
                                                 SubEventIterator eSubEvents)
{
  ATH_MSG_DEBUG( "MdtDigitizationTool::processBunchXing() " << bunchXing);
  SubEventIterator iEvt = bSubEvents;
  for (; iEvt!=eSubEvents; ++iEvt)
    {
        StoreGateSvc& seStore = *iEvt->ptr()->evtStore();
        PileUpTimeEventIndex thisEventIndex = PileUpTimeEventIndex(static_cast<int>(iEvt->time()),iEvt->index());
        ATH_MSG_VERBOSE( "SubEvt StoreGate " << seStore.name() << " :"
                         << " bunch crossing : " << bunchXing
                         << " time offset : " << iEvt->time()
                         << " event number : " << iEvt->ptr()->eventNumber()
                         << " run number : " << iEvt->ptr()->runNumber());
        const MDTSimHitCollection* seHitColl(nullptr);
        if (!seStore.retrieve(seHitColl,m_inputObjectName).isSuccess())
          {
            ATH_MSG_ERROR ( "SubEvent MDTSimHitCollection not found in StoreGate " << seStore.name() );
            return StatusCode::FAILURE;
          }
        ATH_MSG_VERBOSE ( "MDTSimHitCollection found with " << seHitColl->size() << " hits" );
        //Copy hit Collection
        MDTSimHitCollection* MDTHitColl = new MDTSimHitCollection("MDT_Hits");
        MDTSimHitCollection::const_iterator i = seHitColl->begin();
        MDTSimHitCollection::const_iterator e = seHitColl->end();

        // Read hits from this collection
        for (; i!=e; ++i)
          {
            MDTHitColl->Emplace(*i);
          }
        m_thpcMDT->insert(thisEventIndex, MDTHitColl);
        //store these for deletion at the end of mergeEvent
        m_MDTHitCollList.push_back(MDTHitColl);
    }

  return StatusCode::SUCCESS;
}


StatusCode MdtDigitizationTool::getNextEvent()
{
  
  ATH_MSG_DEBUG ( "MdtDigitizationTool::getNextEvent()" );
  
  if (!m_mergeSvc) {
    const bool CREATEIF(true);
    if (!(service("PileUpMergeSvc", m_mergeSvc, CREATEIF)).isSuccess() || 
	0 == m_mergeSvc) {
      ATH_MSG_ERROR ("Could not find PileUpMergeSvc" );
      return StatusCode::FAILURE;
    }
  }
  
  // initialize pointer
  m_thpcMDT = 0;
  
  //  get the container(s)
  typedef PileUpMergeSvc::TimedList<MDTSimHitCollection>::type TimedHitCollList;
  
  //this is a list<info<time_t, DataLink<MDTSimHitCollection> > >
  TimedHitCollList hitCollList;
  
  if (!(m_mergeSvc->retrieveSubEvtsData(m_inputObjectName, hitCollList).isSuccess()) ) {
    ATH_MSG_ERROR ( "Could not fill TimedHitCollList" );
    return StatusCode::FAILURE;
  }
  if (hitCollList.size()==0) {
    ATH_MSG_ERROR ( "TimedHitCollList has size 0" );
    return StatusCode::FAILURE;
  } 
  else {
    ATH_MSG_DEBUG ( hitCollList.size() << " MDTSimHitCollections with key " << m_inputObjectName << " found" );
  }
  
  // create a new hits collection
  m_thpcMDT = new TimedHitCollection<MDTSimHit>() ;
  
  //now merge all collections into one
  TimedHitCollList::iterator iColl(hitCollList.begin());
  TimedHitCollList::iterator endColl(hitCollList.end());
  while (iColl != endColl) {
    const MDTSimHitCollection* p_collection(iColl->second);
    m_thpcMDT->insert(iColl->first, p_collection);
    ATH_MSG_DEBUG ( "MDTSimHitCollection found with " << p_collection->size() << " hits"  );
    ++iColl;
  }
  return StatusCode::SUCCESS;
}


StatusCode MdtDigitizationTool::mergeEvent() {

  StatusCode status = StatusCode::SUCCESS;

  ATH_MSG_DEBUG ( "MdtDigitizationTool::in mergeEvent()" );

  // Cleanup and record the Digit container in StoreGate
  m_digitContainer->cleanup();
  status = evtStore()->record(m_digitContainer,m_outputObjectName);
  if (status.isFailure())  {
    ATH_MSG_ERROR ( "Unable to record MDT digit container in StoreGate" );
    return status;
  }
  ATH_MSG_DEBUG ( "MdtDigitContainer recorded in StoreGate." );
  
  // Create and record the SDO container in StoreGate
  m_sdoContainer = new MuonSimDataCollection();
  status = evtStore()->record(m_sdoContainer,m_outputSDOName);
  if (status.isFailure())  {
    ATH_MSG_ERROR ( "Unable to record MDT SDO collection in StoreGate" );
    return status;
  }
  ATH_MSG_DEBUG ( "MdtSDOCollection recorded in StoreGate." );
  
  status = doDigitization();
  if (status.isFailure())  {
    ATH_MSG_ERROR ( "doDigitization Failed" );
  }

  std::list<MDTSimHitCollection*>::iterator MDTHitColl = m_MDTHitCollList.begin();
  std::list<MDTSimHitCollection*>::iterator MDTHitCollEnd = m_MDTHitCollList.end();
  while(MDTHitColl!=MDTHitCollEnd) {
    delete (*MDTHitColl);
    ++MDTHitColl;
  }
  m_MDTHitCollList.clear();
  
  return status;
}


StatusCode MdtDigitizationTool::digitize() {
  return this->processAllSubEvents();
}


StatusCode MdtDigitizationTool::processAllSubEvents() {
    
  StatusCode status = StatusCode::SUCCESS;
  
  ATH_MSG_DEBUG ( "MdtDigitizationTool::processAllSubEvents()" );

  // Cleanup and record the Digit container in StoreGate
  m_digitContainer->cleanup();
  status = evtStore()->record(m_digitContainer, m_outputObjectName);
  if (status.isFailure())  {
    ATH_MSG_ERROR ( "Unable to record MDT digit container in StoreGate" );
    return status;
  }
  ATH_MSG_DEBUG ( "MdtDigitContainer recorded in StoreGate." );

  // Create and record the SDO container in StoreGate
  m_sdoContainer = new MuonSimDataCollection();
  status = evtStore()->record(m_sdoContainer,m_outputSDOName);
  if (status.isFailure())  {
    ATH_MSG_ERROR ( "Unable to record MDT SDO collection in StoreGate" );
    return status;
  }
  ATH_MSG_DEBUG ( "MdtSDOCollection recorded in StoreGate." );

  if (0 == m_thpcMDT ) {
    status = getNextEvent();
    if (StatusCode::FAILURE == status) {
      ATH_MSG_INFO ( "There are no MDT hits in this event" );
      return status;
    }
  }

  status = doDigitization();
  if (status.isFailure())  {
    ATH_MSG_ERROR ( "doDigitization Failed" );
  }

  return status;
}


StatusCode MdtDigitizationTool::doDigitization() {
  
  //Get the list of dead/missing chambers and cache it
  if ( m_UseDeadChamberSvc ) { 
    m_IdentifiersToMask.clear();
    int size_id = m_pSummarySvc->deadStationsId().size();
    ATH_MSG_DEBUG ( "Number of dead/missing stations retrieved from CondService= "<< size_id );	
    
    for(int k=0;k<size_id;k++) {
      Identifier Id = m_pSummarySvc->deadStationsId()[k];
      m_IdentifiersToMask.push_back( m_pSummarySvc->deadStationsId()[k] );
      ATH_MSG_VERBOSE ( "Dead/missing chambers id from CondDB: " << m_idHelper->show_to_string(Id) );
    }  
  }
       
  // get the iterator infos for this DetEl
  //iterate over hits and fill id-keyed drift time map
  TimedHitCollection< MDTSimHit >::const_iterator i, e; 
  
  // Perform null check on m_thpcMDT
  if(!m_thpcMDT) {
    ATH_MSG_ERROR ( "m_thpcMDT is null" );
    return StatusCode::FAILURE;
  }
  
  while( m_thpcMDT->nextDetectorElement(i, e) ) {
    // Loop over the hits:
    while (i != e) {
      handleMDTSimhit(*i);
      ++i;
    }
  }

  //loop over drift time map entries, convert to tdc value and construct/store the digit
  createDigits();
  
  // reset hits
  m_hits.clear();
  
  // reset the pointer if it is not null
  if (m_thpcMDT) {
    delete m_thpcMDT;
    m_thpcMDT=0;
  }
  
  return StatusCode::SUCCESS;
}


bool MdtDigitizationTool::handleMDTSimhit(const TimedHitPtr<MDTSimHit>& phit){

  const MDTSimHit& hit(*phit);
  MDTSimHit newSimhit(*phit); //hit can be modified later
  
  double globalHitTime(hitTime(phit));
      
  // Important checks for hits (global time, position along tube, masked chambers etc..) DO NOT SET THIS CHECK TO FALSE IF YOU DON'T KNOW WHAT YOU'RE DOING !
  if(m_checkMDTSimHits) {
    if(checkMDTSimHit(hit) == false) return false;
  }
  	 
  const int id = hit.MDTid();
  double driftRadius = hit.driftRadius();
  ATH_MSG_DEBUG ( "Hit bunch time  " << globalHitTime - hit.globalTime() << " tot " << globalHitTime << " tof " << hit.globalTime() << " driftRadius " << driftRadius );

  std::string stationName = m_muonHelper->GetStationName(id);
  int stationEta = m_muonHelper->GetZSector(id);
  int stationPhi  = m_muonHelper->GetPhiSector(id);
  int multilayer = m_muonHelper->GetMultiLayer(id);
  int layer = m_muonHelper->GetLayer(id);
  int tube = m_muonHelper->GetTube(id);

  //construct Atlas identifier from components
  Identifier DigitId = m_idHelper-> channelID(stationName, stationEta,stationPhi, multilayer, layer, tube);

  // get distance to readout
  double distRO(0.);
      
  //find the detector element associated to the hit
  const MuonGM::MdtReadoutElement* element = m_MuonGeoMgr->getMdtReadoutElement(DigitId);
  
  if (0 == element) { 
    ATH_MSG_ERROR( "MuonGeoManager does not return valid element for given id!" );
    return StatusCode::FAILURE;
  } 
  else {
    distRO = element->tubeFrame_localROPos(multilayer,layer,tube).z();
  }
  
  if( m_useDeformations ) {
    newSimhit =applyDeformations(hit,element,DigitId);
    driftRadius = newSimhit.driftRadius();
  }

  //store local hit position + sign
  GeoCorOut result = correctGeometricalWireSag( hit, DigitId, element );
  if (m_useDeformations){
    result = correctGeometricalWireSag( newSimhit, DigitId, element );
  }
  double saggingSign = result.sagSign;
  double trackingSign = result.trackingSign;
  Amg::Vector3D lpos = result.localPosition;   
  double localSag = result.localSag;
  
  // set segment (radius + distance to readout)
  if(m_useWireSagGeom) { 
    driftRadius= lpos.perp();
    
    double maxSag = 0.;
    double actualMaxSag = 0.;
    //double localSag = 0.;
    double driftTime = 0.;
    double projectiveSag = hit.driftRadius() - fabs(driftRadius);
    if (m_useDeformations ) projectiveSag = newSimhit.driftRadius() - fabs(driftRadius);
    
    ATH_MSG_DEBUG( " Geometrical WIRESAGINFO "<< stationName << " " << stationEta << " " << stationPhi << " " 
		   << multilayer << " " << layer << " " << tube <<" " 
		   << hit.localPosition().x() <<" " << hit.localPosition().y() <<" " << hit.localPosition().z() << " " <<hit.driftRadius() <<" " 
		   << element->tubeLength(DigitId)    << " "<< maxSag        <<" " << actualMaxSag    <<" " 
		   << localSag  << " " << projectiveSag  << " "<< driftRadius<<" " << driftTime  <<" " << saggingSign );
  }
  
  // correctly set sign of drift radius
  driftRadius *= trackingSign;

  //+Implementation for RT_Relation_DB_Tool
  MdtDigiToolInput digiInput(fabs(driftRadius),distRO,0.,0.,0.,0.);
  double qcharge=1.;
  double qgamma=-9999.;
  
  if(m_DoQballCharge == true ) {
    // chargeCalculator returns the value of electric charge for Qball particle.
    // particleGamma returns the value of gamma for Qball particle.
    qgamma=particleGamma(hit);
    qcharge=chargeCalculator(hit); 
    
    MdtDigiToolInput digiInput1(fabs(driftRadius),distRO,0.,0.,qcharge,qgamma); 
    digiInput = digiInput1;
    
    if (m_digiTool.name() == "RT_Relation_DB_DigiTool") {
      MdtDigiToolInput digiInput2(fabs(driftRadius),distRO,0.,0.,qcharge,qgamma,DigitId); 
      digiInput = digiInput2;
    }
  }
  else {
    MdtDigiToolInput digiInput1(fabs(driftRadius),distRO,0.,0.,0.,0.); 
    digiInput = digiInput1;
    
    if (m_digiTool.name() == "RT_Relation_DB_DigiTool") {
      MdtDigiToolInput digiInput2(fabs(driftRadius),distRO,0.,0.,0.,0.,DigitId); 
      digiInput = digiInput2;
    }
  }

  // digitize input
  MdtDigiToolOutput digiOutput( m_digiTool->digitize(digiInput) );
  //-Implementation for RT_Relation_DB_Tool
  
  // simulate tube response, check if tube fired
  if (digiOutput.wasEfficient()) {
    double driftTime = digiOutput.driftTime();
    double adc       = digiOutput.adc();
    
    ATH_MSG_VERBOSE( "Tube efficient: driftTime  " << driftTime << " adc value " << adc ); 
    
    // compute RT effect
    if( m_useWireSagRT && !element->isInBarrel() && stationName!="EOS" && stationName!="EOL" ){
      Amg::Vector3D gpos = element->localToGlobalCoords(lpos,DigitId);
      
      // fit parameters for drift time difference vs impact radius for a wire 500 microns off axis
      // garfield calculation. details on http://dslevin.home.cern.ch/atlas/wiresag.ppt
      // Line below: old code
      //double param[4] = {-0.3025,0.58303,0.012177,0.0065818};
      //New code  
      double param[6] = {-4.47741E-3, 1.75541E-2, -1.32913E-2, 2.57938E-3, -4.55015E-5, -1.70821E-7};
      
      // get delta T, change in drift time for reference sag (default=100 microns) and scale by projective sag 
      double deltaT = 0;
      double dR = fabs(driftRadius);
      for (int i = 0; i < 6; ++i) {
	deltaT += param[i]*pow(dR,i);
      }
      
      // reference sag now set to 0.1 mm
      double referenceSag = 0.1;
      
      //Calculate angle at which track cross plane of sag.
      //Note that this assumes the track is coming from the center of the detector.
      double gX = fabs(gpos.x());
      double gY = fabs(gpos.y());
      double gZ = fabs(gpos.z());
      double cosTheta = gZ/sqrt(gZ*gZ + gY*gY + gX*gX);

      //This calculates the sag seen by a track; if a particle passes parallel to the sag, 
      //the shift in drift circle location will have no affect.
      double projectiveSag = localSag*cosTheta;

      deltaT *= (projectiveSag / referenceSag);

      // saggingSign is calculated by the correctGeometricalWireSag function of this class
      // It is +/- 1 depending on whether or not the track passed above or below the wire.
      deltaT = -1 * saggingSign * deltaT;
      
      double driftTimeOriginal=driftTime;
      driftTime +=deltaT; // update drift time

      ATH_MSG_DEBUG( " RT WIRESAGINFO "<< stationName << " " << stationEta << " " << stationPhi << " " 
		     << multilayer << " " << layer << " " << tube <<" " 
		     << hit.localPosition().x() <<" " << hit.localPosition().y() <<" " << hit.localPosition().z() << " " <<driftRadius <<" " 
		     << element->tubeLength(DigitId)/1000.   << " " << cosTheta << " " << localSag 
		     << " " <<  projectiveSag << " " << deltaT << "   " << driftTimeOriginal <<"    "<< driftTime );
    } //m_useWireSagRT   
    
    if(m_useProp) {
      double position_along_wire = hit.localPosition().z();
      if (m_useDeformations ){
	position_along_wire = newSimhit.localPosition().z();
      }
      
      // prop delay calculated with respect to the center of the tube
      double sign(-1.);  
      if(distRO < 0.) sign = 1.;
      double propagation_delay = sign*(1./m_signalSpeed)*position_along_wire;
      //------------------------------------------------------------
      // calculate propagation delay, as readout side the side with 
      // negative local 
      // position along the wire is taken

      driftTime += propagation_delay; // add prop time
      ATH_MSG_VERBOSE( "Position along wire:  " << position_along_wire << " propagation delay:  " << propagation_delay << " new driftTime " << driftTime );          
    }
	
    // add tof + bunch time
    if(m_useTof){
      driftTime += globalHitTime;
      ATH_MSG_VERBOSE("Time off Flight + bunch offset:  " << globalHitTime << " new driftTime " << driftTime );    
    }
    ATH_MSG_DEBUG( m_idHelper->show_to_string(DigitId) << "  Drift time computation " << driftTime  << " radius " << driftRadius << " adc " << adc );
    
    // add hit to hit collection
    m_hits.insert( mdt_hit_info(DigitId,driftTime,adc,driftRadius,&phit));
    ATH_MSG_VERBOSE( " handleMDTSimHit() phit-" << &phit << "  hit.localPosition().z() = " <<  hit.localPosition().z() << " driftRadius = " << driftRadius );
       
    // + TWIN TUBES  (A. Koutsman)
    if(m_useTwin) {
      // two chambers in ATLAS are installed with Twin Tubes; in detector coordinates BOL4A13 & BOL4C13; only INNER multilayer(=1) is with twin tubes
      bool BOL4X13 = false;
      // find these two chambers in identifier scheme coordinates as in MdtIdHelper
      if(stationName == "BOL" && std::abs(stationEta) == 4 && stationPhi == 7 && multilayer == 1){ 
	BOL4X13 = true;
      }
    
      // implement twin tubes digitizing either for all BOL (m_useAllBOLTwin = true) _OR_ only for two chambers really installed
      if( ( m_useAllBOLTwin && stationName == "BOL" ) || BOL4X13 ) {
 
	int twin_tube = 0;
	Identifier twin_DigitId;
	double twin_sign_driftTime = 0.;
	//twinpair is connected via a HV-jumper with a delay of ~6ns
	double HV_delay = 6.;
	double twin_tubeLength = 0.;
	double twin_geo_pos_along_wire = 0.;
	double twin_sign_pos_along_wire = 0.;
	double twin_sign(-1.);
      
	//twinpair is interconnected with one tube in between, so modulo 4 they are identical
	if     (tube % 4 == 1 || tube % 4 == 2) twin_tube = tube + 2;
	else if(tube % 4 == 0 || tube % 4 == 3) twin_tube = tube - 2;
	//construct Atlas identifier from components for the twin
	twin_DigitId = m_idHelper-> channelID(stationName, stationEta,
					      stationPhi, multilayer, layer, twin_tube);
	// get twin tube length for propagation delay
	twin_tubeLength = element->getTubeLength(layer,twin_tube); 

	// prop delay calculated with respect to the center of the tube
	if(distRO < 0.) twin_sign = 1.;
	twin_geo_pos_along_wire = hit.localPosition().z();
	if (m_useDeformations ) {
	  twin_geo_pos_along_wire = newSimhit.localPosition().z();
	}
	twin_sign_pos_along_wire = twin_sign*twin_geo_pos_along_wire;
	double twin_propagation_delay = twin_sign*(1./m_signalSpeed)*twin_geo_pos_along_wire;
     
	//calculate drift-time for twin from prompt driftTime + propagation delay + length of tube + hv-delay
	// ( -2* for propagation_delay, cause prop_delay already in driftTime)
	twin_sign_driftTime = driftTime + twin_tubeLength/m_signalSpeed  - 2*twin_propagation_delay + HV_delay;

	//smear the twin time by a gaussian with a stDev given by m_resTwin
	double rand = CLHEP::RandGaussZiggurat::shoot(m_twinRndmEngine, twin_sign_driftTime, m_resTwin);
	twin_sign_driftTime = rand;
	
	ATH_MSG_DEBUG( " TWIN TUBE stname " << stationName << " steta " << stationEta
		       << " stphi " << stationPhi << " mLayer " << multilayer << " layer " << layer << " tube " <<  tube
		       << " signed position along wire = "  << twin_sign_pos_along_wire 
		       << " propagation delay = " << twin_propagation_delay
		       << " drifttime = " << driftTime << "    twin_driftTime = " << twin_sign_driftTime 
		       << "  TWIN time-difference = " << (twin_sign_driftTime - driftTime) );
	
	
     	// add twin-hit to hit collection
	m_hits.insert(mdt_hit_info(twin_DigitId,twin_sign_driftTime,adc,driftRadius,&phit));
	
      } // end select all BOLs or installed chambers
    } // end if(m_useTwin){
    // - TWIN TUBES   (A. Koutsman)
  }
  else {
    ATH_MSG_DEBUG( m_idHelper->show_to_string(DigitId) << "  Tube not efficient " << " radius " << driftRadius ); 
  }
  
  return true;
}


bool MdtDigitizationTool::checkMDTSimHit(const MDTSimHit& hit) const {
	
  //get the hit Identifier and info
  const int id = hit.MDTid();
  std::string stationName = m_muonHelper->GetStationName(id);
  int stationEta = m_muonHelper->GetZSector(id);
  int stationPhi  = m_muonHelper->GetPhiSector(id);
  int multilayer = m_muonHelper->GetMultiLayer(id);
  int layer = m_muonHelper->GetLayer(id);
  int tube = m_muonHelper->GetTube(id);
  
  Identifier DigitId = m_idHelper->channelID(stationName, stationEta,stationPhi, multilayer, layer, tube);
  ATH_MSG_DEBUG("Working on hit: " << m_idHelper->show_to_string(DigitId) << "  "<< m_idHelper->stationNameString(m_idHelper->stationName(DigitId))<< " "<< stationEta << " "<< stationPhi);
  
  //+MASKING OF DEAD/MISSING CHAMBERS
  if ( m_UseDeadChamberSvc ) {
    for (unsigned int i=0;i<m_IdentifiersToMask.size();i++) {
      Identifier Id = m_IdentifiersToMask[i];
      
      if ((stationName == m_idHelper->stationNameString(m_idHelper->stationName(Id))) && (stationEta ==  m_idHelper->stationEta(Id)) && (stationPhi ==  m_idHelper->stationPhi(Id)) ) {
	ATH_MSG_DEBUG("Hit is located in chamber that is dead/missing: Masking the hit!");
	return false;
      }
    }  
  }
  else {
    bool nameMasked=false;
    bool etaMasked=false;
    bool phiMasked=false;  
    bool masked=false;
    
    for (unsigned int i=0;i<m_maskedStations.size();i++) { 
      bool temp=true;
      
      for (unsigned int k=0;k<3;k++) {
	char c=m_vMaskedStations[i].maskedName[k];
	char cc=stationName[k];
	if (c=='*') continue;
	if (c==cc) continue;
	temp=false;
      }
      if (!temp) continue;
      nameMasked=temp;
      
      etaMasked = m_vMaskedStations[i].maskedEta=="*" || m_vMaskedStations[i].imaskedEta==stationEta ;
      phiMasked = m_vMaskedStations[i].maskedPhi=="*" || m_vMaskedStations[i].imaskedPhi==stationPhi ;
      masked = nameMasked && etaMasked && phiMasked;
      if (masked) {
	return false;
      }
    }
  }
  //-MASKING OF DEAD/MISSING CHAMBERS
  
  double tubeL(0.);
  double tubeR(0.);
  
  const MuonGM::MdtReadoutElement* element = m_MuonGeoMgr->getMdtReadoutElement(DigitId);
  
  if (0 == element) { 
    ATH_MSG_ERROR( "MuonGeoManager does not return valid element for given id!" );
  } 
  else {
    tubeL = element->tubeLength(DigitId);
    tubeR = element->innerTubeRadius();
  }
  
  bool ok(true);
  
  if( fabs(hit.driftRadius()) > tubeR ) {
    ok = false;
    ATH_MSG_DEBUG( "MDTSimHit has invalid radius: " << hit.driftRadius() << "   tubeRadius " << tubeR );
  }
  
  if( fabs(hit.localPosition().z()) > 0.5*tubeL ) {
    ok = false;
    ATH_MSG_DEBUG( "MDTSimHit has invalid position along tube: " << hit.localPosition().z() << "   tubeLength " << tubeL );
  }
  
  if (m_useTof) {
    double minTof = minimumTof(DigitId);
    if(( hit.globalTime() < 0 || hit.globalTime() > 10*minTof) && m_DiscardEarlyHits) {
      ok = false;
      ATH_MSG_DEBUG( "MDTSimHit has invalid global time: " << hit.globalTime() << "   minimum Tof " << minTof );
    }
  }
  else {
    ATH_MSG_DEBUG( "MDTSimHit global time: " << hit.globalTime() << " accepted anyway as UseTof is false" );
  }
  
  return ok;
}


bool MdtDigitizationTool::createDigits(){

  Identifier currentDigitId;
  Identifier currentElementId;
 
  double currentDeadTime =0.;
  MdtDigitCollection* digitCollection = 0;
  // loop over sorted hits
  m_hits.sort();
  HitIt it = m_hits.begin();
  
  // +For Cosmics add 
  double timeOffsetEvent = 0.0;
  double timeOffsetTotal = 0.0;
  
  //this offset emulates the timing spead of cosmics: +/- 1 BC 
  if (m_useTof == false && m_useOffSet2 == true) {
    int inum = CLHEP::RandFlat::shootInt(m_rndmEngine,0,10);
    if (inum == 8  ) {timeOffsetEvent =  -25.0;}
    else if (inum == 9  ) {timeOffsetEvent =  25.0;}
    ATH_MSG_DEBUG ( "Emulating timing spead of cosmics: +/- 1 BC. Adding  " << timeOffsetEvent << " ns to time" );
  }
  //-ForCosmics
  
  for (; it != m_hits.end(); ++it) {

    Identifier idDigit   = it->id;
    Identifier elementId = m_idHelper->elementID(idDigit);
    const MuonGM::MdtReadoutElement* geo = m_MuonGeoMgr->getMdtReadoutElement(idDigit);
 
    //Check if we are in a new chamber, if so get the DigitCollection
    if(elementId != currentElementId) {
      currentElementId = elementId;
      digitCollection = getDigitCollection(elementId);
      
      //+ForCosmics
      //this offset emulates the time jitter of cosmic ray muons w.r.t LVL1 accept 
      if (m_useTof == false && m_useOffSet1 == true) {
	timeOffsetTotal = timeOffsetEvent + CLHEP::RandFlat::shoot(m_rndmEngine,-12.5 ,12.5 );
	ATH_MSG_DEBUG ( "Emulating time jitter of cosmic ray muons w.r.t LVL1 accept. Adding  " << timeOffsetTotal << " ns to time" );
      }
      //-ForCosmics
    }
    if(digitCollection==NULL) {
      ATH_MSG_ERROR( "Trying to use NULL pointer digitCollection" );
      return StatusCode::FAILURE;
    }
      
    
    float driftRadius = it->radius;
    double driftTime = it->time;
    double charge    = it->adc;
    
    ATH_MSG_VERBOSE( "New hit : driftTime " << driftTime << " adc  " << charge );

    // check if we are in a new tube
    if(idDigit != currentDigitId) {
      currentDigitId = idDigit;
      // set the deadTime
      currentDeadTime = driftTime + charge + m_deadTime;
      ATH_MSG_VERBOSE( "New tube, setting dead time:  " << currentDeadTime << "  driftTime " << driftTime);
    }
    else {
      // check if tube is dead
      if(driftTime > currentDeadTime) {
	// tube produces a second hit, set the new deadtime
	currentDeadTime = driftTime + charge + m_deadTime;
	ATH_MSG_VERBOSE( "Additional hit, setting dead time:  " << currentDeadTime << "  driftTime " << driftTime);
      }
      else {
	// tube is dead go to next hit
	ATH_MSG_VERBOSE( "Hit within dead time:  " << currentDeadTime << "  driftTime " << driftTime);
	continue;
      }
    }
    
    const TimedHitPtr<MDTSimHit>& phit = *(it->simhit);
    const MDTSimHit& hit(*phit);
    
    // check if the hits lies within the TDC time window
    // subtrack the minimum Tof (= globalPosition().mag()/c) from the tof of the hit 
    double relativeTime = driftTime - minimumTof(idDigit);
    bool insideMatch = insideMatchingWindow( relativeTime );
    bool insideMask = insideMaskWindow( relativeTime );
    if( insideMask && insideMatch ) {
      ATH_MSG_WARNING(" Digit in matching AND masking window, please check window definition: relative time " << relativeTime );
      insideMask = false;
    }
    if( insideMatch || insideMask ) {
      // get calibration constants from DbSvc
      double t0 = m_offsetTDC;
      if ( m_t0_from_DB ) {
	MuonCalib::MdtFullCalibData data = m_calibDbSvc->getCalibration( geo->collectionHash(), geo->detectorElementHash() ); 
	if ( data.tubeCalib ) {
	  int ml    = m_idHelper->multilayer(idDigit)-1;
	  int layer = m_idHelper->tubeLayer(idDigit)-1;
	  int tube  = m_idHelper->tube(idDigit)-1;
	  if ( ml>=0 && layer>=0 && tube>=0 ) {
	    // extract calibration constants for single tube
	    const MuonCalib::MdtTubeCalibContainer::SingleTubeCalib* singleTubeData = data.tubeCalib->getCalib( ml, layer, tube );
	    if ( singleTubeData ){
	      t0 = singleTubeData->t0;
	    }
	  }
	}
      }
      
      int tdc = digitizeTime(driftTime + t0 + timeOffsetTotal);
      int adc = digitizeTime(it->adc);
      ATH_MSG_DEBUG( " >> Digit Id = " << m_idHelper->show_to_string(idDigit) << " driftTime " << driftTime
		     << " driftRadius " << driftRadius << " TDC " << tdc << " ADC " << adc << " mask bit " << insideMask );

      MdtDigit*  newDigit = new MdtDigit(idDigit, tdc, adc, insideMask);
      digitCollection->push_back(newDigit);
      
      float localZPos = (float) hit.localPosition().z();
      ATH_MSG_VERBOSE( " createDigits() phit-" << &phit << " hit-" << hit.print() << "    localZPos = " << localZPos ); 
    
      //Create the Deposit for MuonSimData
      MuonSimData::Deposit deposit(HepMcParticleLink(phit->trackNumber(),phit.eventId()), MuonMCData((float)driftRadius,localZPos));

      //Record the SDO collection in StoreGate
      std::vector<MuonSimData::Deposit> deposits;
      deposits.push_back(deposit);
      MuonSimData tempSDO(deposits,0);
      const Amg::Vector3D& tempLocPos = (*(it->simhit))->localPosition();
      Amg::Vector3D p = geo->localToGlobalCoords(tempLocPos,idDigit);
      tempSDO.setPosition(p); 
      tempSDO.setTime( hitTime(phit) );
      m_sdoContainer->insert ( std::make_pair ( idDigit, tempSDO ) );
	
    } else {
      ATH_MSG_DEBUG( "  >> OUTSIDE TIME WINDOWS << "<< " Digit Id = " << m_idHelper->show_to_string(idDigit) << " driftTime " << driftTime << " --> hit ignored");
    }
  }//for (; it != hits.end(); ++it)
  
  return true;
}


MdtDigitCollection* MdtDigitizationTool::getDigitCollection(Identifier elementId){
  MdtDigitCollection*      digitCollection;
  
  IdContext mdtContext = m_idHelper->module_context();
  IdentifierHash coll_hash;
  if (m_idHelper->get_hash(elementId, coll_hash, &mdtContext)) {
    ATH_MSG_ERROR ( "Unable to get MDT hash id from MDT Digit collection " 
		    << "context begin_index = " << mdtContext.begin_index()
		    << " context end_index  = " << mdtContext.end_index()
		    << " the identifier is ");
    elementId.show();
  } 
  
  StatusCode status;
  // Get the messaging service, print where you are
  MdtDigitContainer::const_iterator it_coll = m_digitContainer->indexFind(coll_hash);
  if (m_digitContainer->end() ==  it_coll) {
    digitCollection = new MdtDigitCollection(elementId, coll_hash);
    status = m_digitContainer->addCollection(digitCollection, coll_hash);
    if (status.isFailure())
      ATH_MSG_ERROR ( "Couldn't record MdtDigitCollection with key=" << coll_hash  << " in StoreGate!" );
    else
      ATH_MSG_DEBUG ( "New MdtDigitCollection with key=" << coll_hash << " recorded in StoreGate." );
  } 
  else { 
    digitCollection = const_cast<MdtDigitCollection*>( *it_coll );
  }
  return digitCollection;
}

int MdtDigitizationTool::digitizeTime(double time) const {
  int    tdcCount;
  double tmpCount = time/m_ns2TDC;
  double rand = CLHEP::RandGaussZiggurat::shoot(m_rndmEngine, tmpCount, m_resTDC);
  tdcCount = static_cast<long>(rand);
  
  if (tdcCount < 0 || tdcCount > 4096){
    ATH_MSG_DEBUG( " Count outside TDC window: " << tdcCount );
  }
  return tdcCount;
}


double MdtDigitizationTool::minimumTof(Identifier DigitId) const { 
  if(!m_useTof) return 0.;
  
  // get distance to vertex for tof correction before applying the time window
  double distanceToVertex(0.);
  const MuonGM::MdtReadoutElement* element = m_MuonGeoMgr->getMdtReadoutElement(DigitId);
  
  if (0 == element) {
    ATH_MSG_ERROR( "MuonGeoManager does not return valid element for given id!" );
  } 
  else {
    distanceToVertex = element->tubePos(DigitId).mag(); 
  }
  
  ATH_MSG_DEBUG( "minimumTof calculated " << distanceToVertex*m_inv_c_light);
  
  return distanceToVertex*m_inv_c_light;
}


bool MdtDigitizationTool::insideMatchingWindow(double time) const {
  if( m_useTimeWindow )
    if(time < m_bunchCountOffset || time > m_bunchCountOffset+m_matchingWindow) {
      ATH_MSG_VERBOSE( "hit outside MatchingWindow " << time );
      return false;
    }
  return true;
}

bool MdtDigitizationTool::insideMaskWindow(double time) const {
  if( m_useTimeWindow )
    if(time < m_bunchCountOffset-m_maskWindow || time > m_bunchCountOffset){
      ATH_MSG_VERBOSE( "hit outside MaskWindow " << time );
      return false;
    }
  return true;
}


//+emulate deformations here
MDTSimHit MdtDigitizationTool::applyDeformations(const MDTSimHit &hit,const MuonGM::MdtReadoutElement* element,const Identifier &DigitId) {
  const int id = hit.MDTid();
  
  //make the deformation
  Amg::Vector3D hitAtGlobalFrame = element->nodeform_localToGlobalCoords(hit.localPosition(), DigitId );
  Amg::Vector3D hitDeformed = element->globalToLocalCoords(hitAtGlobalFrame,DigitId);
  
  double driftRadius = sqrt(hitDeformed.x()*hitDeformed.x()+hitDeformed.y()*hitDeformed.y());
  MDTSimHit simhit2 = MDTSimHit(id,hit.globalTime(),driftRadius,hitDeformed,hit.trackNumber());
  
  return simhit2;
}


MdtDigitizationTool::GeoCorOut MdtDigitizationTool::correctGeometricalWireSag( const MDTSimHit& hit, 
									       const Identifier& id, 
									       const MuonGM::MdtReadoutElement* element ) const {
  Amg::Vector3D lpos = hit.localPosition();
  Amg::Transform3D gToWireFrame = element->globalToLocalTransf( id );
  
  // local track direction in precision plane
  Amg::Vector3D ldir( -lpos.y(), lpos.x(), 0. );
  ldir.normalize();
  
  //  calculate the position of the hit sagged wire frame
  // transform to global coords
  Amg::Vector3D gpos = element->localToGlobalCoords( lpos, id );
  Amg::Vector3D gdir = gToWireFrame.inverse()*ldir;
  
  // get wire surface
  const Trk::SaggedLineSurface&  surface = element->surface(id);
  
  // check whether direction is pointing away from IP
  double pointingCheck = gpos.dot(gdir) < 0 ? -1. : 1.;
  if( pointingCheck < 0 ) {
    gdir *= pointingCheck;
  }

  double trackingSign = 1.;
  double localSag = 0.0;
  if( m_useWireSagGeom ) {
    // calculate local hit position in nominal wire frame
    Amg::Vector2D lp;
    surface.globalToLocal( gpos, gpos, lp );
    
    // calculate sagged wire position
    const Trk::StraightLineSurface* wireSurface = surface.correctedSurface(lp);
    //calculate displacement of wire from nominal
    //To do this, note that the sagged surface is modeled as a straight line
    //through the point in the space that the bit of wire closest to the hit
    //sagged to and is parallel to the nominal wire. Find the center of the 
    //sagged surface in global coordinates, transform this into the nominal
    //surface's local coordinates, and calculate that point's distance from 
    //the origin.
    const Amg::Vector3D gSaggedSpot = wireSurface->center();
    Amg::Vector3D lSaggedSpot = gToWireFrame*gSaggedSpot;
    double sagX = lSaggedSpot.x();
    double sagY = lSaggedSpot.y();

    localSag = sqrt(sagX*sagX + sagY*sagY);

    // global to local sagged wire frame transform
    gToWireFrame = wireSurface->transform().inverse();
    
    // local hit position in sagged wire frame
    lpos = gToWireFrame*gpos;
    ldir = gToWireFrame*gdir;
    ldir.normalize();

    // compute drift radius ( = impact parameter)
    double alpha = -1*lpos.dot(ldir);
    lpos = lpos + alpha*ldir;

    // calculate global point of closest approach
    Amg::Vector3D saggedGPos = wireSurface->transform()*lpos;
    
    // recalculate tracking sign
    Amg::Vector2D lpsag;
    wireSurface->globalToLocal( saggedGPos, gdir, lpsag );
    trackingSign = lpsag[Trk::locR] < 0 ? -1. : 1.;
    
    // don't forget to delete pointers....
    delete wireSurface;
  }
  else {
    // recalculate tracking sign
    Amg::Vector2D lpsag;
    surface.globalToLocal( gpos, gdir, lpsag );
    trackingSign = lpsag[Trk::locR] < 0 ? -1. : 1.;
  }
  
  // local gravity vector
  Amg::Vector3D gravityDir(-1.*Trk::s_yAxis);
  Amg::Vector3D lgravDir = gToWireFrame*gravityDir;
  
  //Project gravity vector onto X-Y plane, so the z-components (along the wire)
  //don't contribute to the dot-product.
  lgravDir.z() = 0.;
  
  // calculate whether hit above or below wire (using gravity direction
  //1 -> hit below wire (in same hemisphere as gravity vector)
  //-1 -> hit above wire (in opposite hemisphere as gravity vector)
  double sign = lpos.dot(lgravDir) < 0 ? -1. : 1.;
  
  return GeoCorOut( sign, trackingSign, lpos , localSag);
}


StatusCode MdtDigitizationTool::finalize() {
  
  ATH_MSG_DEBUG ( "Finalize.");
  m_digitContainer->release();
  
  return StatusCode::SUCCESS;
}
