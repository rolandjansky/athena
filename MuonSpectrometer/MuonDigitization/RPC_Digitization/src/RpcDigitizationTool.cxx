/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// RpcDigitizationTool
// ------------
// Authors:
//             Andrea Di Simone  <Andrea.Di.Simone@cern.ch>
//             Gabriele Chiodini <gabriele.chiodini@le.infn.it>
//             Stefania Spagnolo <stefania.spagnolo@le.infn.it>
////////////////////////////////////////////////////////////////////////////////

#include "RPC_Digitization/RpcDigitizationTool.h"

//Inputs
#include "MuonSimEvent/RPCSimHit.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
//Geometry
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonSimEvent/RpcHitIdHelper.h"


//run n. from geometry DB
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

//Truth
#include "GeneratorObjects/HepMcParticleLink.h"
#include "AtlasHepMC/GenParticle.h"

//Random Numbers
#include "AthenaKernel/RNGWrapper.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussZiggurat.h"
#include "CLHEP/Random/RandExponential.h"

//Core includes
#include "PathResolver/PathResolver.h"
#include "AIDA/IHistogram1D.h"
#include "EventInfo/TagInfo.h"
#include "EventInfoMgt/ITagInfoMgr.h"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

//12 charge points, 15 BetaGamma points, 180 efficiency points for fcp search
namespace {
  constexpr int N_Charge = 12;
  constexpr int N_Velocity = 15;
  constexpr double Charge[N_Charge] = {0.1, 0.2, 0.3, 0.33, 0.4, 0.5, 0.6, 0.66, 0.7, 0.8, 0.9, 1.0};
  constexpr double Velocity[N_Velocity] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 2.0, 3.0, 10.0, 100.0,1000.0};
  constexpr double Eff_garfield[N_Charge][N_Velocity] = {
    {0.8648,0.3476,0.1407,0.0618,0.0368,0.0234,0.0150,0.0120,0.0096,0.0079,0.0038,0.0041,0.0035,0.0049,0.0054},
    {0.9999,0.9238,0.6716,0.4579,0.3115,0.2238,0.1727,0.1365,0.1098,0.0968,0.0493,0.0451,0.0528,0.0694,0.0708},
    {1.0000,0.9978,0.9517,0.8226,0.6750,0.5611,0.4674,0.3913,0.3458,0.3086,0.1818,0.1677,0.1805,0.2307,0.2421},
    {1.0000,0.9994,0.9758,0.8918,0.7670,0.6537,0.5533,0.4856,0.4192,0.3852,0.2333,0.2186,0.2479,0.2957,0.2996},
    {1.0000,1.0000,0.9972,0.9699,0.9022,0.8200,0.7417,0.6660,0.6094,0.5622,0.3846,0.3617,0.3847,0.4578,0.4583},
    {1.0000,1.0000,0.9998,0.9956,0.9754,0.9479,0.9031,0.8604,0.8126,0.7716,0.5827,0.5545,0.5865,0.6834,0.6706},
    {1.0000,1.0000,1.0000,0.9997,0.9968,0.9876,0.9689,0.9464,0.9221,0.8967,0.7634,0.7385,0.7615,0.8250,0.8309},
    {1.0000,1.0000,1.0000,1.0000,0.9995,0.9952,0.9866,0.9765,0.9552,0.9427,0.8373,0.8127,0.8412,0.8899,0.8891},
    {1.0000,1.0000,1.0000,1.0000,0.9995,0.9981,0.9918,0.9803,0.9754,0.9602,0.8730,0.8564,0.8746,0.9178,0.9261},
    {1.0000,1.0000,1.0000,1.0000,1.0000,0.9993,0.9990,0.9951,0.9935,0.9886,0.9419,0.9277,0.9422,0.9686,0.9700},
    {1.0000,1.0000,1.0000,1.0000,1.0000,1.0000,0.9998,0.9996,0.9980,0.9966,0.9786,0.9718,0.9748,0.9875,0.9882},
    {1.0000,1.0000,1.0000,1.0000,1.0000,1.0000,0.9998,1.0000,0.9991,0.9988,0.9913,0.9872,0.9917,0.9970,0.9964}};
} // fixed arrays for FCP search


using namespace MuonGM;
#define SIG_VEL 4.8  // ns/m
static double time_correction(double, double, double);

RpcDigitizationTool::RpcDigitizationTool(const std::string& type,
                                         const std::string& name,
                                         const IInterface* pIID)
  : PileUpToolBase(type, name, pIID)
{
}

// member function implementation
//--------------------------------------------
StatusCode RpcDigitizationTool::initialize() {

  ATH_MSG_DEBUG ( "RpcDigitizationTool:: in initialize()" );
  ATH_MSG_DEBUG ( "Configuration  RpcDigitizationTool " );

  ATH_MSG_DEBUG ( "Parameters             " << m_paraFile                  );
  ATH_MSG_DEBUG ( "InputObjectName        " << m_inputHitCollectionName    );
  ATH_MSG_DEBUG ( "OutputObjectName       " << m_outputDigitCollectionKey.key() );
  ATH_MSG_DEBUG ( "OutputSDOName          " << m_outputSDO_CollectionKey.key());
  ATH_MSG_DEBUG ( "WindowLowerOffset      " << m_timeWindowLowerOffset     );
  ATH_MSG_DEBUG ( "WindowUpperOffset      " << m_timeWindowUpperOffset     );
  ATH_MSG_DEBUG ( "DeadTime               " << m_deadTime                  );
  ATH_MSG_DEBUG ( "RndmSvc                " << m_rndmSvc                   );
  ATH_MSG_DEBUG ( "PatchForRpcTime        " << m_patch_for_rpc_time        );
  ATH_MSG_DEBUG ( "RpcTimeShift           " << m_rpc_time_shift            );
  ATH_MSG_DEBUG ( "RPC_TimeSchema         " << m_RPC_TimeSchema            );
  ATH_MSG_DEBUG ( "RPCSDOareRPCDigits     " << m_sdoAreOnlyDigits          );


  ATH_MSG_DEBUG ( "IgnoreRunDependentConfig "<< m_ignoreRunDepConfig       );
  ATH_MSG_DEBUG ( "turnON_efficiency      " <<  m_turnON_efficiency        );
  ATH_MSG_DEBUG ( "Efficiency_fromCOOL    " <<  m_Efficiency_fromCOOL      );
  ATH_MSG_DEBUG ( "turnON_clustersize     " <<  m_turnON_clustersize       );
  ATH_MSG_DEBUG ( "ClusterSize_fromCOOL   " <<  m_ClusterSize_fromCOOL     );
  ATH_MSG_DEBUG ( "testbeam_clustersize   " <<  m_testbeam_clustersize     );
  ATH_MSG_DEBUG ( "FirstClusterSizeInTail " <<  m_FirstClusterSizeInTail   );
  ATH_MSG_DEBUG ( "ClusterSize1_2uncorr   " <<  m_ClusterSize1_2uncorr     );
  ATH_MSG_DEBUG ( "PrintCalibrationVector " <<  m_PrintCalibrationVector   );
  ATH_MSG_DEBUG ( "BOG_BOF_DoubletR2_OFF  " <<  m_BOG_BOF_DoubletR2_OFF    );
  ATH_MSG_DEBUG ( "DumpFromDbFirst        " <<  m_DumpFromDbFirst          );
  ATH_MSG_DEBUG ( "CutMaxClusterSize      " <<  m_CutMaxClusterSize        );
  ATH_MSG_DEBUG ( "CutProjectedTracks     " <<  m_CutProjectedTracks       );
  ATH_MSG_DEBUG ( "PanelId_OFF_fromlist   " <<  m_PanelId_OFF_fromlist     );
  ATH_MSG_DEBUG ( "FileName_DeadPanels    " <<  m_FileName_DeadPanels      );
  ATH_MSG_DEBUG ( "PanelId_OK_fromlist    " <<  m_PanelId_OK_fromlist      );
  ATH_MSG_DEBUG ( "FileName_GoodPanels    " <<  m_FileName_GoodPanels      );
  ATH_MSG_DEBUG ( "ValidationSetup        " <<  m_validationSetup          );
  ATH_MSG_DEBUG ( "IncludePileUpTruth     " <<  m_includePileUpTruth       );
  ATH_MSG_DEBUG ( "ParticleBarcodeVeto    " <<  m_vetoThisBarcode          );


  ATH_CHECK(detStore()->retrieve( m_GMmgr,"Muon" ));
  ATH_MSG_DEBUG ( "Retrieved GeoModel from DetectorStore." );

  if (m_onlyUseContainerName) {
    ATH_CHECK(m_mergeSvc.retrieve());
  }

  m_idHelper = m_GMmgr->rpcIdHelper();
  if(!m_idHelper) {
    return StatusCode::FAILURE;
  }

  // check the input object name
  if (m_hitsContainerKey.key().empty()) {
    ATH_MSG_FATAL("Property InputObjectName not set !");
    return StatusCode::FAILURE;
  }
  if(m_onlyUseContainerName) m_inputHitCollectionName = m_hitsContainerKey.key();
  ATH_MSG_DEBUG("Input objects in container : '" << m_inputHitCollectionName << "'");

  // Initialize ReadHandleKey
  ATH_CHECK(m_hitsContainerKey.initialize(!m_onlyUseContainerName));

  //initialize the output WriteHandleKeys
  ATH_CHECK(m_outputDigitCollectionKey.initialize());
  ATH_CHECK(m_outputSDO_CollectionKey.initialize());
  ATH_MSG_DEBUG ( "Output digits: '" << m_outputDigitCollectionKey.key() << "'" );

  //set the configuration based on run1/run2
  // Retrieve geometry config information from the database (RUN1, RUN2, etc...)
  IRDBAccessSvc* rdbAccess(nullptr);
  ATH_CHECK(service("RDBAccessSvc",rdbAccess));

  bool run1 = true;
  std::string configVal = "";
  const IGeoModelSvc* geoModel(nullptr);
  ATH_CHECK(service("GeoModelSvc", geoModel));
  // check the DetDescr version
  std::string atlasVersion = geoModel->atlasVersion();

  IRDBRecordset_ptr atlasCommonRec = rdbAccess->getRecordsetPtr("AtlasCommon",atlasVersion,"ATLAS");
  if (atlasCommonRec->size()==0) {
    run1 = true;
  }
  else {
    configVal = (*atlasCommonRec)[0]->getString("CONFIG");
    ATH_MSG_INFO( "From DD Database, Configuration is "<< configVal );
    std::string MSgeoVersion = m_GMmgr->geometryVersion().substr(0,4);
    ATH_MSG_INFO( "From DD Database, MuonSpectrometer geometry version is "<< MSgeoVersion );
    if(configVal=="RUN1" || MSgeoVersion=="R.06"){
      run1 = true;
    }
    else if (configVal=="RUN2" || configVal=="RUN3" || MSgeoVersion=="R.07") {
      run1 = false;
    }
    else {
      ATH_MSG_FATAL("Unexpected value for geometry config read from the database: " << configVal);
      return StatusCode::FAILURE;
    }
  }
  //
  if (run1) ATH_MSG_INFO("From Geometry DB: MuonSpectrometer configuration is: RUN1 or MuonGeometry = R.06");
  else      ATH_MSG_INFO("From Geometry DB: MuonSpectrometer configuration is: RUN2 or MuonGeometry = R.07");

  if (m_ignoreRunDepConfig==false) {
    m_BOG_BOF_DoubletR2_OFF = false;
    m_Efficiency_fromCOOL   = false;
    m_ClusterSize_fromCOOL  = false;
    m_RPCInfoFromDb         = false;
    m_kill_deadstrips       = false;
    m_applyEffThreshold     = false;
    if (run1) {
      //m_BOG_BOF_DoubletR2_OFF = true
      //m_Efficiency_fromCOOL   = true
      //m_ClusterSize_fromCOOL  = true
      m_BOG_BOF_DoubletR2_OFF = true;
      if (configVal=="RUN1") {// MC12 setup
        m_Efficiency_fromCOOL   = true;
        m_ClusterSize_fromCOOL  = true;
        m_RPCInfoFromDb         = true;
        m_kill_deadstrips       = true;
        m_applyEffThreshold     = false;
        m_CutProjectedTracks    = 50;
      }
    }
    else {
      //m_BOG_BOF_DoubletR2_OFF = false # do not turn off at digitization the hits in the dbR=2 chambers in the feet
      //m_Efficiency_fromCOOL   = false # use common average values in python conf.
      //m_ClusterSize_fromCOOL  = false # use common average values in python conf.
      m_BOG_BOF_DoubletR2_OFF = false;
      if (configVal=="RUN2" || configVal=="RUN3") {// MC15c setup
        m_Efficiency_fromCOOL   = true;
        m_ClusterSize_fromCOOL  = true;
        m_RPCInfoFromDb         = true;
        m_kill_deadstrips       = false;
        m_applyEffThreshold     = false; //for MC16 [2015-2016]IoV will use measurements, [2017]IoV will use measurements with threshold at 50% already applied in the condition data ////// it was true (with threshold 50%) for MC15c;
        m_CutProjectedTracks    = 100;
      }
    }
    ATH_MSG_DEBUG ( "Run1/Run2-dependent configuration is enforced; option setting reset for: " );
    ATH_MSG_DEBUG ( "......Efficiency_fromCOOL    " <<  m_Efficiency_fromCOOL      );
    ATH_MSG_DEBUG ( "......ClusterSize_fromCOOL   " <<  m_ClusterSize_fromCOOL     );
    ATH_MSG_DEBUG ( "......BOG_BOF_DoubletR2_OFF  " <<  m_BOG_BOF_DoubletR2_OFF    );
    ATH_MSG_DEBUG ( "......RPCInfoFromDb          " <<  m_RPCInfoFromDb            );
    ATH_MSG_DEBUG ( "......KillDeadStrips         " <<  m_kill_deadstrips          );
    ATH_MSG_DEBUG ( "......ApplyEffThreshold      " <<  m_applyEffThreshold        );
    ATH_MSG_DEBUG ( "......CutProjectedTracks     " <<  m_CutProjectedTracks       );
  }
  else {
    ATH_MSG_WARNING ( "Run1/Run2-dependent configuration is bypassed; be careful with option settings" );
    ATH_MSG_DEBUG ( "......Efficiency_fromCOOL    " <<  m_Efficiency_fromCOOL      );
    ATH_MSG_DEBUG ( "......ClusterSize_fromCOOL   " <<  m_ClusterSize_fromCOOL     );
    ATH_MSG_DEBUG ( "......BOG_BOF_DoubletR2_OFF  " <<  m_BOG_BOF_DoubletR2_OFF    );
    ATH_MSG_DEBUG ( "......RPCInfoFromDb          " <<  m_RPCInfoFromDb            );
    ATH_MSG_DEBUG ( "......KillDeadStrips         " <<  m_kill_deadstrips          );
    ATH_MSG_DEBUG ( "......ApplyEffThreshold      " <<  m_applyEffThreshold        );
    ATH_MSG_DEBUG ( "......CutProjectedTracks     " <<  m_CutProjectedTracks       );
  }


  ATH_MSG_DEBUG ( "Ready to read parameters for cluster simulation from file" );

  ATH_CHECK(readParameters());

  ATH_CHECK(m_rndmSvc.retrieve());

  // get TagInfoMgr
  ATH_CHECK(service("TagInfoMgr", m_tagInfoMgr));

  // fill the taginfo information
  ATH_CHECK(fillTagInfo());

  if (m_RPCInfoFromDb) {
    ATH_CHECK(m_readKey.initialize());
  }

  if(m_PrintCalibrationVector) {
    ATH_CHECK(PrintCalibrationVector());
  }

  m_DeadStripPanel.clear();

  //////////////////// special test
  //  m_turnON_clustersize=false;

  return StatusCode::SUCCESS;

}

//--------------------------------------------
StatusCode RpcDigitizationTool::prepareEvent(const EventContext& /*ctx*/, unsigned int) {

  ATH_MSG_DEBUG ( "RpcDigitizationTool::in prepareEvent()" );

  //John's Hacks START
  m_RPCHitCollList.clear();
  m_thpcRPC = new TimedHitCollection<RPCSimHit>();
  //John's Hacks END

  return StatusCode::SUCCESS;

}

//--------------------------------------------
StatusCode RpcDigitizationTool::processBunchXing(int bunchXing,
                                                 SubEventIterator bSubEvents,
                                                 SubEventIterator eSubEvents)
{
  ATH_MSG_DEBUG ( "RpcDigitizationTool::in processBunchXing()" );

  typedef PileUpMergeSvc::TimedList<RPCSimHitCollection>::type TimedHitCollList;
  TimedHitCollList hitCollList;

  if (!(m_mergeSvc->retrieveSubSetEvtData(m_inputHitCollectionName, hitCollList, bunchXing,
					  bSubEvents, eSubEvents).isSuccess()) &&
        hitCollList.size() == 0) {
    ATH_MSG_ERROR("Could not fill TimedHitCollList");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_VERBOSE(hitCollList.size() << " RPCSimHitCollection with key " <<
		    m_inputHitCollectionName << " found");
  }

  TimedHitCollList::iterator iColl(hitCollList.begin());
  TimedHitCollList::iterator endColl(hitCollList.end());

  // Iterating over the list of collections
  for( ; iColl != endColl; iColl++){

    RPCSimHitCollection *hitCollPtr = new RPCSimHitCollection(*iColl->second);
    PileUpTimeEventIndex timeIndex(iColl->first);

    ATH_MSG_DEBUG("RPCSimHitCollection found with " << hitCollPtr->size() <<
		  " hits");
    ATH_MSG_VERBOSE("time index info. time: " << timeIndex.time()
		    << " index: " << timeIndex.index()
		    << " type: " << timeIndex.type());

    m_thpcRPC->insert(timeIndex, hitCollPtr);
    m_RPCHitCollList.push_back(hitCollPtr);

  }

  return StatusCode::SUCCESS;
}

//--------------------------------------------
// Get next event and extract collection of hit collections:
StatusCode RpcDigitizationTool::getNextEvent(const EventContext& ctx)
{

  ATH_MSG_DEBUG ( "RpcDigitizationTool::getNextEvent()" );

  // initialize pointer
  m_thpcRPC = nullptr;

  //  get the container(s)
  typedef PileUpMergeSvc::TimedList<RPCSimHitCollection>::type TimedHitCollList;

  // In case of single hits container just load the collection using read handles
  if (!m_onlyUseContainerName) {
      SG::ReadHandle<RPCSimHitCollection> hitCollection(m_hitsContainerKey, ctx);
    if (!hitCollection.isValid()) {
      ATH_MSG_ERROR("Could not get RPCSimHitCollection container " << hitCollection.name() << " from store " << hitCollection.store());
      return StatusCode::FAILURE;
    }

    // create a new hits collection
    m_thpcRPC = new TimedHitCollection<RPCSimHit>{1};
    m_thpcRPC->insert(0, hitCollection.cptr());
    ATH_MSG_DEBUG("RPCSimHitCollection found with " << hitCollection->size() << " hits");

    return StatusCode::SUCCESS;
  }

  //this is a list<pair<time_t, DataLink<RPCSimHitCollection> > >
  TimedHitCollList hitCollList;

  if (!(m_mergeSvc->retrieveSubEvtsData(m_inputHitCollectionName, hitCollList).isSuccess()) ) {
    ATH_MSG_ERROR ( "Could not fill TimedHitCollList" );
    return StatusCode::FAILURE;
  }
  if (hitCollList.size()==0) {
    ATH_MSG_ERROR ( "TimedHitCollList has size 0" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG (  hitCollList.size() << " RPCSimHitCollections with key " << m_inputHitCollectionName << " found" );
  }

  // create a new hits collection
  m_thpcRPC = new TimedHitCollection<RPCSimHit>() ;

  //now merge all collections into one
  TimedHitCollList::iterator iColl(hitCollList.begin());
  TimedHitCollList::iterator endColl(hitCollList.end());
  while (iColl != endColl) {
    const RPCSimHitCollection* p_collection(iColl->second);
    m_thpcRPC->insert(iColl->first, p_collection);
    //if ( m_debug ) ATH_MSG_DEBUG ( "RPCSimHitCollection found with "
    //   << p_collection->size() << " hits" );    // loop on the hit collections
    ++iColl;
  }
  return StatusCode::SUCCESS;
}

//--------------------------------------------
StatusCode RpcDigitizationTool::mergeEvent(const EventContext& ctx) {

  StatusCode status = StatusCode::SUCCESS;

  ATH_MSG_DEBUG ( "RpcDigitizationTool::in mergeEvent()" );

  if(m_DumpFromDbFirst && m_RPCInfoFromDb){
    status = DumpRPCCalibFromCoolDB(ctx) ;
    if (status == StatusCode::FAILURE) return status ;
    m_DumpFromDbFirst = false ;
  }

  // create and record the Digit container in StoreGate
  SG::WriteHandle<RpcDigitContainer> digitContainer(m_outputDigitCollectionKey, ctx);
  ATH_CHECK(digitContainer.record(std::make_unique<RpcDigitContainer>(m_idHelper->module_hash_max())));
  ATH_MSG_DEBUG ( "RpcDigitContainer recorded in StoreGate." );

  // Create and record the SDO container in StoreGate
  SG::WriteHandle<MuonSimDataCollection> sdoContainer(m_outputSDO_CollectionKey, ctx);
  ATH_CHECK(sdoContainer.record(std::make_unique<MuonSimDataCollection>()));
  ATH_MSG_DEBUG ( "RpcSDOCollection recorded in StoreGate." );

  //////////////// TEMP////
  m_sdo_tmp_map.clear();
  /////////////////////////

  status = doDigitization(ctx, digitContainer.ptr(), sdoContainer.ptr());
  if (status.isFailure())  {
    ATH_MSG_ERROR ( "doDigitization Failed" );
  }

  // Clean-up
  std::list<RPCSimHitCollection*>::iterator RPCHitColl = m_RPCHitCollList.begin();
  std::list<RPCSimHitCollection*>::iterator RPCHitCollEnd = m_RPCHitCollList.end();
  while(RPCHitColl!=RPCHitCollEnd)
    {
      delete (*RPCHitColl);
      ++RPCHitColl;
    }
  m_RPCHitCollList.clear();

  return status;
}

//--------------------------------------------
StatusCode RpcDigitizationTool::processAllSubEvents(const EventContext& ctx) {

  StatusCode status = StatusCode::SUCCESS;

  //merging of the hit collection in getNextEvent method

  ATH_MSG_DEBUG ( "RpcDigitizationTool::in digitize()" );

  if(m_DumpFromDbFirst && m_RPCInfoFromDb){
    status = DumpRPCCalibFromCoolDB(ctx) ;
    if (status == StatusCode::FAILURE) return status ;
    m_DumpFromDbFirst = false ;
  }

  // create and record the Digit container in StoreGate
  SG::WriteHandle<RpcDigitContainer> digitContainer(m_outputDigitCollectionKey, ctx);
  ATH_CHECK(digitContainer.record(std::make_unique<RpcDigitContainer>(m_idHelper->module_hash_max())));
  ATH_MSG_DEBUG ( "RpcDigitContainer recorded in StoreGate." );

  // Create and record the SDO container in StoreGate
  SG::WriteHandle<MuonSimDataCollection> sdoContainer(m_outputSDO_CollectionKey, ctx);
  ATH_CHECK(sdoContainer.record(std::make_unique<MuonSimDataCollection>()));
  ATH_MSG_DEBUG ( "RpcSDOCollection recorded in StoreGate." );

  //////////////// TEMP////
  m_sdo_tmp_map.clear();
  /////////////////////////

  if (0 == m_thpcRPC ) {
    status = getNextEvent(ctx);
    if (StatusCode::FAILURE == status) {
      ATH_MSG_INFO ( "There are no RPC hits in this event" );
      return status; // there are no hits in this event
    }
  }

  ATH_CHECK(doDigitization(ctx, digitContainer.ptr(), sdoContainer.ptr()));

  return status;
}

//--------------------------------------------
StatusCode RpcDigitizationTool::doDigitization(const EventContext& ctx, RpcDigitContainer* digitContainer, MuonSimDataCollection* sdoContainer) {

  ATHRNG::RNGWrapper* rngWrapper = m_rndmSvc->getEngine(this);
  rngWrapper->setSeed( name(), ctx );
  CLHEP::HepRandomEngine *rndmEngine = rngWrapper->getEngine(ctx);

  //StatusCode status = StatusCode::SUCCESS;
  //status.ignore();
  
  int nKilledStrips = 0;
  int nToBeKilledStrips = 0;
  
  RPCSimHitCollection* inputSimHitColl=NULL;
  
 if (m_validationSetup)
    {
      inputSimHitColl = new RPCSimHitCollection("RPC_Hits");
      StatusCode status = evtStore()->record(inputSimHitColl,"InputRpcHits");
      if (status.isFailure())  {
        ATH_MSG_ERROR ( "Unable to record Input RPC HIT collection in StoreGate"  );
        return status;
      }
    }

  // get the iterator pairs for this DetEl
  //iterate over hits
  TimedHitCollection< RPCSimHit >::const_iterator i, e;
  
  // Perform null check on m_thpcRPC
  if(!m_thpcRPC) {
    ATH_MSG_ERROR ( "m_thpcRPC is null" );
    return StatusCode::FAILURE;
  }

  
  while( m_thpcRPC->nextDetectorElement(i, e) ) {

    // to store the a single 
    struct SimDataContent {
      Identifier channelId;
      std::vector<MuonSimData::Deposit> deposits;
      Amg::Vector3D gpos;
      float simTime;
    };
    std::map<Identifier,SimDataContent> channelSimDataMap; 

    // Loop over the hits:
    while (i != e) {
      ATH_MSG_DEBUG ( "RpcDigitizationTool::loop over the hits" );

      TimedHitPtr<RPCSimHit> phit(*i++);

      // the hit
      const RPCSimHit& hit(*phit);
      // the hit id
      const int idHit = hit.RPCid();
      // the global time (G4 time + bunch time)
      double globalHitTime(hitTime(phit));
      // the G4 time or TOF from IP
      double G4Time(hit.globalTime());
      // the bunch time
      double bunchTime(globalHitTime - hit.globalTime());


      ATH_MSG_DEBUG  ( "Global time " << globalHitTime << " G4 time " << G4Time  << " Bunch time " << bunchTime       );

      if (m_validationSetup){
        ATH_MSG_VERBOSE("Validation:  globalHitTime, G4Time, BCtime = "<<globalHitTime<<" "<<G4Time<<" "<<bunchTime);	  
        inputSimHitColl->Emplace(idHit, globalHitTime, hit.localPosition(), hit.trackNumber(), hit.postLocalPosition(), hit.energyDeposit(), hit.stepLength(), hit.particleEncoding(), hit.kineticEnergy());
      }

      // convert sim id helper to offline id
      m_muonHelper = RpcHitIdHelper::GetHelper(m_idHelper->gasGapMax());
      std::string stationName = m_muonHelper->GetStationName(idHit);
      int         stationEta  = m_muonHelper->GetZSector    (idHit);
      int         stationPhi  = m_muonHelper->GetPhiSector  (idHit);
      int         doubletR    = m_muonHelper->GetDoubletR   (idHit);
      int         doubletZ    = m_muonHelper->GetDoubletZ   (idHit);
      int         doubletPhi  = m_muonHelper->GetDoubletPhi (idHit);
      int         gasGap      = m_muonHelper->GetGasGapLayer(idHit);
      int         measphi     = m_muonHelper->GetMeasuresPhi(idHit);

      if( measphi!=0 ) continue; //Skip phi strip . To be created after efficiency evaluation

      //construct Atlas identifier from components
      ATH_MSG_DEBUG ( "creating id for hit in element:"
		      << " stationName "  <<    stationName.c_str()
		      << " stationEta "   <<    stationEta
		      << " stationPhi "   <<    stationPhi
		      << " doubletR "     <<    doubletR
		      << " doubletZ "     <<    doubletZ
		      << " doubletPhi "   <<    doubletPhi
		      << " gasGap "	 <<    gasGap
		      << " measphi "	 <<    measphi );//

      const Identifier idpaneleta = m_idHelper->channelID(stationName, stationEta, stationPhi, doubletR,
                                                          doubletZ, doubletPhi,gasGap, 0, 1);
      const Identifier idpanelphi = m_idHelper->channelID(stationName, stationEta, stationPhi, doubletR,
                                                          doubletZ, doubletPhi,gasGap, 1, 1);

      //loop on eta and phi to apply correlated efficiency between the two views
      //      Identifier atlasRpcId;
      //      Identifier atlasRpcIdphi;
      
      double corrtimejitter    =  0 ;
      if(m_CorrJitter>0.01) corrtimejitter    = CLHEP::RandGaussZiggurat::shoot(rndmEngine,0.,m_CorrJitter); //correlated jitter
      //      std::cout<<" Correlated eta / phi jitter = "<<corrtimejitter<<std::endl;
      // handle here the special case where eta panel is dead => phi strip status (dead or eff.) cannot be resolved; 
      // measured panel eff. will be used in that case and no phi strip killing will happen
      bool undefPhiStripStat = false;

      std::vector<int> pcseta    = PhysicalClusterSize(ctx, &idpaneleta, &hit, rndmEngine); //set to one for new algorithms
      ATH_MSG_DEBUG ( "Simulated cluster on eta panel: size/first/last= "<<pcseta[0]   <<"/"<<pcseta[1]   <<"/"<<pcseta[2] );
      std::vector<int> pcsphi = PhysicalClusterSize(ctx, &idpanelphi, &hit, rndmEngine); //set to one for new algorithms
      ATH_MSG_DEBUG ( "Simulated cluster on phi panel: size/first/last= "<<pcsphi[0]<<"/"<<pcsphi[1]<<"/"<<pcsphi[2] );

      // create Identifiers
      Identifier atlasRpcIdeta = m_idHelper->channelID(stationName, stationEta, stationPhi, doubletR,
                                                       doubletZ, doubletPhi,gasGap, 0, pcseta[1] );
      Identifier atlasRpcIdphi = m_idHelper->channelID(stationName, stationEta, stationPhi, doubletR,
                                                       doubletZ, doubletPhi,gasGap, 1, pcsphi[1] );

      const RpcReadoutElement* ele= m_GMmgr->getRpcReadoutElement(atlasRpcIdeta);// first add time jitter to the time:
      const EBC_EVCOLL evColl = EBC_MAINEVCOLL;
      const HepMcParticleLink::PositionFlag idxFlag = (phit.eventId()==0) ? HepMcParticleLink::IS_POSITION: HepMcParticleLink::IS_INDEX;
      const HepMcParticleLink particleLink(phit->trackNumber(),phit.eventId(),evColl,idxFlag);

      // as long there is no proper implementation of the BIS78 cabling and digitisation,
      // skip this method to avoid hard crash of digitisation
      if (stationName.find("BIS")!=std::string::npos && std::abs(stationEta)>6) {
        ATH_MSG_WARNING("skipping DetectionEfficiency for BIS78");
      } else {
        ATH_CHECK(DetectionEfficiency(ctx, &atlasRpcIdeta,&atlasRpcIdphi, undefPhiStripStat, rndmEngine, particleLink));
      }

      ATH_MSG_DEBUG ( "SetPhiOn " << m_SetPhiOn << " SetEtaOn " <<  m_SetEtaOn );

      for( int imeasphi=0 ;  imeasphi!=2;  ++imeasphi){

        // get Identifier and list of clusters for this projection 
        const Identifier atlasId = (imeasphi==0) ? atlasRpcIdeta : atlasRpcIdphi;
        std::vector<int> pcs = (imeasphi==0) ? pcseta : pcsphi;

	ATH_MSG_DEBUG ( "SetOn: stationName "  <<    stationName.c_str()
                        << " stationEta "   <<    stationEta
                        << " stationPhi "   <<    stationPhi
                        << " doubletR "     <<    doubletR
                        << " doubletZ "     <<    doubletZ
                        << " doubletPhi "   <<    doubletPhi
                        << " gasGap "       <<    gasGap
                        << " measphi "      <<    imeasphi ) ;
        
	//pcs contains the cluster size, the first strip number and the last strip number of the cluster
	pcs = TurnOnStrips(pcs, &atlasId, rndmEngine);
        if(pcs[2]<0) return StatusCode::FAILURE;

	ATH_MSG_DEBUG ( "Simulated cluster1: size/first/last= "<<pcs[0]<<"/"<<pcs[1]<<"/"<<pcs[2] );

	//Adjust absolute position and local position
	Amg::Vector3D pos=hit.localPosition();
	pos=adjustPosition(&atlasId, pos); //
	pos=posInPanel(&atlasId, pos);     //This is what we want to save in deposit?

	//Calculate propagation time along readout strip in seconds
        Amg::Vector3D gpos = ele->localToGlobalCoords(pos,atlasId);
	double proptime=PropagationTimeNew(&atlasId,gpos);

	double tns = G4Time + proptime + corrtimejitter; //the time is in nanoseconds
	ATH_MSG_VERBOSE ( "TOF+propagation time  "<< tns <<" /s where proptime "<< proptime << "/s" );

	double time =tns + bunchTime;
	ATH_MSG_VERBOSE ( "final time in ns: BC+TOF+prop "<<time<<" /ns" );

	//pack propagation time along strip, bunch time and local hit position
	long long int  packedMCword=PackMCTruth(proptime  ,
						bunchTime ,
						pos.y()   ,
						pos.z())  ;

	double *b=reinterpret_cast<double*>(&packedMCword);


	//////////////////////////////////////////////////////////////////////////////////
	// create here deposit for MuonSimData
	// ME unused: const HepMcParticleLink & particleLink = hit.particleLink();
	// MuonMCData first  word is the packing of    : proptime, bunchTime, posy, posz
	// MuonMCData second word is the total hit time: bunchcTime+tof+proptime+correlatedJitter / ns
	MuonSimData::Deposit deposit(particleLink,
				     MuonMCData((*b),time)); // store tof+strip_propagation+corr.jitter
	//				     MuonMCData((*b),G4Time+bunchTime+proptime          )); // store tof+strip_propagation

	//Do not store pile-up truth information
	if (m_includePileUpTruth ||
		!((phit->trackNumber() == 0) || (phit->trackNumber() == m_vetoThisBarcode))) {

        if( abs(hit.particleEncoding())== 13 || hit.particleEncoding() == 0){

          auto channelSimDataMapPos = channelSimDataMap.find(atlasId);
          if( channelSimDataMapPos == channelSimDataMap.end() ){
            Amg::Vector3D ppos=hit.postLocalPosition();
            Amg::Vector3D gppos = ele->localToGlobalCoords(ppos,atlasId);
            Amg::Vector3D gdir = gppos - gpos;
            Trk::Intersection intersection = ele->surface(atlasId).straightLineIntersection(gpos,gdir,false,false); 
            SimDataContent& content = channelSimDataMap[atlasId];
            content.channelId = atlasId;
            content.deposits.push_back(deposit);
            content.gpos = intersection.position;
            content.simTime = hitTime(phit);
            ATH_MSG_VERBOSE("adding SDO entry: r " << content.gpos.perp() << " z " << content.gpos.z() );
          }
        }
	}

	if( imeasphi==0 && m_SetEtaOn==0) continue ;
	if( imeasphi==1 && m_SetPhiOn==0) continue ;


	//---------------------------------------------------------------------
	// construct new digit and store it in the respective digit collection
	// --------------------------------------------------------------------

	// we create one digit-vector/deposit for each strip in the cluster

	for(int clus=pcs[1];clus<=pcs[2];clus++){

	  Identifier newId = m_idHelper->channelID(m_idHelper->elementID(stationName, stationEta,
									 stationPhi,doubletR), doubletZ, doubletPhi,gasGap, imeasphi, clus);
	  // here count and maybe kill dead strips if using COOL input for the detector status 
	  if (m_Efficiency_fromCOOL) {
            SG::ReadCondHandle<RpcCondDbData> readHandle{m_readKey, ctx};
            const RpcCondDbData* readCdo{*readHandle};
	    if ( !(undefPhiStripStat && imeasphi==1) )
	      {
		if(readCdo->getDeadStripIntMap().find( newId )  != readCdo->getDeadStripIntMap().end()) 
		  {
		    ATH_MSG_DEBUG ( "After DetectionEfficiency: strip "<< m_idHelper->show_to_string(newId) <<" in a cluster of size "<< pcs[2]-pcs[1]+1<<" is dead - kill it ");
		    ++nToBeKilledStrips;
		    if (m_kill_deadstrips){
		      ++nKilledStrips;
		      continue;//gabriele
		    }
		  }
	      }
	  }



	  if(!m_idHelper->valid(newId)){
	    ATH_MSG_ERROR ( "Created an invalid id, aborting!" );
	    m_idHelper->print(newId);
	    return StatusCode::FAILURE;
	  }

	  ///////////////////////////////////////////////////////////////////
	  /////////////// TEMP, waiting for Reco to learn using clusters...
	  ///////////////////////////////////////////////////////////////////
	  //One identifier but several deposits // name m_sdo_tmp_map is wrong call it m_sdo_map
	  if(m_sdo_tmp_map.find(newId)==m_sdo_tmp_map.end()){
	    std::vector<MuonSimData::Deposit> newdeps;
	    newdeps.push_back(deposit);
	    m_sdo_tmp_map.insert(std::map<Identifier, std::vector<MuonSimData::Deposit> >::value_type(newId,newdeps));
	  } 
	  else
	    {
	      m_sdo_tmp_map[newId].push_back(deposit);
	    }
	} // end for cluster
      } //loop on eta and phi
    } // end loop hits

    if( m_muonOnlySDOs ){
      
      for( auto it = channelSimDataMap.begin(); it!=channelSimDataMap.end();++it ){
        
        MuonSimData simData(it->second.deposits,0);
        simData.setPosition(it->second.gpos);
        simData.setTime(it->second.simTime);
        auto insertResult = sdoContainer->insert(std::make_pair( it->first,simData) );
        if (!insertResult.second) ATH_MSG_ERROR ( "Attention: this sdo is not recorded, since the identifier already exists in the sdoContainer map" );
      }
    }

  } // end loop detector elements


  ///////// TEMP for Reco not able to use clusterization


  std::map<Identifier, std::vector<MuonSimData::Deposit> >::iterator map_iter=m_sdo_tmp_map.begin();
  ATH_MSG_DEBUG ( "Start the digit map loop" );

  for(;map_iter!=m_sdo_tmp_map.end();map_iter++){

    //Identifier
    const Identifier theId=(*map_iter).first;
    ATH_MSG_DEBUG ( "in the map loop: id "<<m_idHelper->show_to_string(theId) );
    //Deposit
    const std::vector<MuonSimData::Deposit> theDeps=(*map_iter).second;
  
    // store the SDO from the muon
    MuonSimData::Deposit theMuon;                      // useful beacuse it sorts the digits in ascending time.
    std::multimap<double, MuonSimData::Deposit> times; // extract here time info from deposits.

    //loop on the vector deposit
    for(unsigned int k=0;k<theDeps.size();k++){
      double time = theDeps[k].second.secondEntry();
      times.insert(std::multimap<double, MuonSimData::Deposit>::value_type(time,theDeps[k]));
    }

    // now iterate again over the multimap entries and store digits after dead time applied

    IdContext rpcContext = m_idHelper->module_context();//work on chamber context

    std::multimap<double, MuonSimData::Deposit>::iterator map_dep_iter=times.begin();

    // loop to suppress digits too close in time (emulate Front-End and CMA dead time)
    double last_time=-10000; // init to high value
    for(;map_dep_iter!=times.end();map_dep_iter++){
      double currTime =(*map_dep_iter).first;
      ATH_MSG_VERBOSE ( "deposit with time " << currTime  );

      if (!m_muonOnlySDOs && !m_sdoAreOnlyDigits)
	{
	  // store (before any cut: all G4 hits) in the SDO container
	  //Identifier sdo and digit are the same
	  if (sdoContainer->find(theId) != sdoContainer->end()) // Identifier exist -> increase deposit
	    {
	      std::map<Identifier,MuonSimData>::const_iterator it = sdoContainer->find(theId);
	      std::vector<MuonSimData::Deposit> deps = ((*it).second).getdeposits();
	      deps.push_back((*map_dep_iter).second);
	    }
	  else // Identifier does not exist -> create (Id,deposit)
	    {
	      std::vector<MuonSimData::Deposit> deposits;
	      deposits.push_back((*map_dep_iter).second);
	      std::pair<std::map<Identifier,MuonSimData>::iterator, bool> insertResult =
		sdoContainer->insert(std::make_pair( theId, MuonSimData(deposits,0) ) );
	      if (!insertResult.second) ATH_MSG_ERROR ( "Attention: this sdo is not recorded, since the identifier already exists in the sdoContainer map" );
	    }
	}
      //apply dead time
      if(fabs(currTime-last_time)>(m_deadTime))
	{


	  ATH_MSG_DEBUG ( "deposit with time "<< currTime <<" is distant enough from previous (if any) hit on teh same strip" );
	  last_time=(*map_dep_iter).first;

	  // first add time jitter to the time:
          double uncorrjitter    = 0 ;
	  if(m_UncorrJitter>0.01) uncorrjitter = CLHEP::RandGaussZiggurat::shoot(rndmEngine,0.,m_UncorrJitter);
	  //	  std::cout<<" uncorrelated jitter = "<<uncorrjitter<<std::endl;

	  //Historically patch for the cavern background
	  //Now we subtract TOF from IP to assume full time calibrated detector (t=0 for particle from IP at light speed)
	  //We add a time shift to emulate FE global offset

	  const RpcReadoutElement* ele= m_GMmgr->getRpcReadoutElement(theId);
	  Amg::Vector3D posi = ele->stripPos(theId);
	  double tp = time_correction(posi.x(),posi.y(),posi.z());
	  tp = (m_patch_for_rpc_time)?  tp: 0;
	  //Calculate propagation time for a hit at the center of the strip, to be subtructed as well as the nominal TOF 
	  double propTimeFromStripCenter=PropagationTimeNew(&theId,posi);
	  double newDigit_time =	currTime+uncorrjitter+m_rpc_time_shift-tp-propTimeFromStripCenter;

	  ATH_MSG_VERBOSE ( "last_time=currTime " << last_time << " jitter " <<uncorrjitter << " TOFcorrection " <<tp << " shift " << m_rpc_time_shift << "  newDigit_time " << newDigit_time );

	  //Apply readout window (sensitive detector time window)
	  bool outsideDigitizationWindow = outsideWindow(newDigit_time);
	  if (outsideDigitizationWindow)
	    {
	      ATH_MSG_VERBOSE ( "hit outside digitization window - do not produce digits" );
	      ATH_MSG_DEBUG   ( "Hit outside time window!!" << " hit time (ns) = " << newDigit_time << " timeWindow  = " << m_timeWindowLowerOffset << " / " << m_timeWindowUpperOffset  );

	      continue;
	    }
	  // ok, let's store this digit
	  // this is an accepted hit to become digit
	  last_time=(*map_dep_iter).first;

	  RpcDigit*                newDigit = new RpcDigit(theId, newDigit_time); // RpcDigit::time MUST be a double, or we will lose the precision we need
	  Identifier               elemId   = m_idHelper->elementID(theId);
	  RpcDigitCollection*      digitCollection = 0;

	  IdentifierHash coll_hash;
	  if (m_idHelper->get_hash(elemId, coll_hash, &rpcContext)) {
	    ATH_MSG_ERROR ( "Unable to get RPC hash id from RPC Digit collection "
			    << "context begin_index = " << rpcContext.begin_index()
			    << " context end_index  = " << rpcContext.end_index()
			    << " the identifier is "
			    );
	    elemId.show();
	  }

	  // make new digit
	  ATH_MSG_DEBUG ( "Digit Id = " << m_idHelper->show_to_string(theId)<<" digit time "<<newDigit_time );
	  //std::cout << "Digit Id = " << m_idHelper->show_to_string(theId)<<" digit time "<<newDigit_time << std::endl;

	  // put new collection in storegate
	  const RpcDigitCollection* coll = digitContainer->indexFindPtr(coll_hash);
	  if (nullptr ==  coll) {
	    digitCollection = new RpcDigitCollection(elemId,coll_hash);
	    digitCollection->push_back(newDigit);
	    StatusCode status = digitContainer->addCollection(digitCollection, coll_hash);
	    if (status.isFailure()){
	      ATH_MSG_ERROR  ( "Couldn't record RpcDigitCollection with key=" << coll_hash << " in StoreGate!" );
	      //else
	      delete  digitCollection; 
	      digitCollection    = 0 ;
	      return StatusCode::RECOVERABLE; // consistent with ERROR message above. 
	    } else {
	      ATH_MSG_DEBUG ( "New RpcHitCollection with key=" << coll_hash << " recorded in StoreGate." );
	    }
	  } else {
	    digitCollection = const_cast<RpcDigitCollection*>( coll );
	    digitCollection->push_back(newDigit);
	  }

	  if (!m_muonOnlySDOs && m_sdoAreOnlyDigits)
	    {

	      // put SDO collection in StoreGate
	      if (sdoContainer->find(theId) != sdoContainer->end())
		{
		  std::map<Identifier,MuonSimData>::const_iterator it = sdoContainer->find(theId);
		  std::vector<MuonSimData::Deposit> deps = ((*it).second).getdeposits();
		  deps.push_back((*map_dep_iter).second);
		}
	      else
		{
		  std::vector<MuonSimData::Deposit> deposits;
		  deposits.push_back((*map_dep_iter).second);
		  std::pair<std::map<Identifier,MuonSimData>::iterator, bool> insertResult =
		    sdoContainer->insert(std::make_pair( theId, MuonSimData(deposits,0) ) );
		  if (!insertResult.second) ATH_MSG_ERROR("Attention: this sdo is not recorded, since teh identifier already exists in the sdoContainer map");
		}
	    }

	} else
        ATH_MSG_DEBUG ("discarding digit due to dead time: "<< (*map_dep_iter).first<< " "<<last_time);
    }

  }// loop to suppress digits too close in time ended

  // reset the pointer if it not null
  if (m_thpcRPC) {
    delete m_thpcRPC;
    m_thpcRPC=0;
  }

  ATH_MSG_DEBUG ("EndOf Digitize() n. of strips Killed (dead) in the DB = "<<nKilledStrips<<" ("<<nToBeKilledStrips<<")");
  return StatusCode::SUCCESS;
}

//--------------------------------------------
std::vector<int> RpcDigitizationTool::PhysicalClusterSize(const EventContext& ctx, const Identifier* id, const RPCSimHit* theHit, CLHEP::HepRandomEngine* rndmEngine)
{

  int stationEta  = m_idHelper->stationEta(*id);
  float pitch;
  int measuresPhi = m_idHelper->measuresPhi(*id);
  std::vector<int> result(3,0);

  const RpcReadoutElement* ele= m_GMmgr->getRpcReadoutElement(*id);

  pitch = ele->StripPitch(measuresPhi);

  int nstrip;
  double xstrip;

  std::vector<double> cs=m_csPara; // read from file
  double cs1[5]; //the contributions to the observed cluster size due to physical cluster size 1 and 2
  double cs2[5];

  Amg::Vector3D position=adjustPosition(id, theHit->localPosition());

  nstrip=findStripNumber(position,*id, xstrip);

  xstrip=xstrip*30./pitch;

  cs1[0]=cs[0];
  cs2[0]=0;

  double cs1_tot, cs2_tot;
  double pcs1_prob=0; // the probability to have physical cluster size 1
  int pcsIs1=0;

  // NOTE: standard identifiers require nstrip eta increasing with |eta|
  // while now it is increasing with eta. Here we fix this problem.

  if(!measuresPhi){
    if(stationEta<0){ // fix needed only for negative half-barrel
      int totEtaStrips=ele->Nstrips(measuresPhi);
      nstrip=totEtaStrips-nstrip+1;
      // if stationEta<0, invert the numbering AND invert the position of the hit in the strip 
      // s.spagnolo 20/10/2015; this fixes a small bias in the digit positions for clusters with size > 1  
      xstrip=30.-xstrip;
    }
  }

  if(measuresPhi) nstrip=adjustStripNumber(id,nstrip);


  result[1]=nstrip;
  result[2]=nstrip;

  //testbeam algorithm
  if(m_testbeam_clustersize){

    // code to decide if the physical cluster size is 1 or 2;
    // this is based on a distribution shown in the muon TDR, representing the
    // fraction cs1/cs2 as a function of the impact point.
    // this distribution was fitted with a composite function (gaus_const_gaus)

    if(xstrip<8) pcs1_prob=m_rgausPara[0]*exp(-(xstrip-m_rgausPara[1])*(xstrip-m_rgausPara[1])*0.5/(m_rgausPara[2]*m_rgausPara[2]))/100.;
    else if(xstrip>8&&xstrip<22) pcs1_prob=m_constPara[0];
    else if(xstrip>22&&xstrip<30) pcs1_prob=m_fgausPara[0]*exp(-(xstrip-m_fgausPara[1])*(xstrip-m_fgausPara[1])*0.5/(m_fgausPara[2]*m_fgausPara[2]))/100.;

    if(CLHEP::RandFlat::shoot(rndmEngine)<pcs1_prob) pcsIs1=1;

    for(int i=1;i<5;i++){
      cs1[i]=pcs1_prob*cs[i];
      cs2[i]=(1-pcs1_prob)*cs[i];

    }

    // FIXME: there are too many pcs2, i.e. the two distributions we use (pcs and observed cs) were obtained with different experimental setups. The following lines convert some of the pcs2 to pcs1. This will be eliminated with new experimental distributions

    double pcs1_av=0.6688;
    double pcs1_tot=cs[0]+cs[1]*pcs1_av+cs[2]*pcs1_av+cs[2]*pcs1_av+cs[4]*pcs1_av;
    double pcs2_tot=100.-pcs1_tot;
    double pcs1_missing=pcs1_tot-pcs1_av*100;
    double pcs2_to_convert=pcs1_missing/pcs2_tot;

    if(!pcsIs1&&CLHEP::RandFlat::shoot(rndmEngine)<pcs2_to_convert){
      pcsIs1=1;
      for(int i=1;i<5;i++){
	cs1[i]=cs1[i]+pcs2_to_convert*cs2[i]; // recover
	cs2[i]=cs2[i]-pcs2_to_convert*cs2[i]; // recover
      }
    }

    // end recover

    // normalization of the distributions

    cs1_tot=cs1[1]+cs1[2]+cs1[3]+cs1[4]; // count here only cs>1, thus not cs1[0]
    cs2_tot=cs2[0]+cs2[1]+cs2[2]+cs2[3]+cs2[4];

    // if pcs is 2, decide which strip is activated

    if(!pcsIs1&&xstrip>pitch/2.) result[2]++;
    if(!pcsIs1&&xstrip<pitch/2.) result[1]--;

    // now assign cs 'not physical', according to the distributions cs1 and cs2

    if(pcsIs1){
      double rand1=CLHEP::RandFlat::shoot(rndmEngine,100.);
      if(rand1>cs1_tot+cs2_tot){ // it means that cs is 1
	result[0]=1;
      } else {
	double rand=CLHEP::RandFlat::shoot(rndmEngine,cs1_tot);
	if(rand<cs1[1]) result[0]=2;
	else if(rand<cs1[1]+cs1[2]) result[0]=3;
	else if(rand<cs1[1]+cs1[2]+cs1[3]) result[0]=4;
	else result[0]=4;
      }

    } else {
      double rand=CLHEP::RandFlat::shoot(rndmEngine,cs2_tot);
      if(rand<cs2[1]) result[0]=2;
      else if(rand<cs2[1]+cs2[2]) result[0]=3;
      else if(rand<cs2[1]+cs2[2]+cs2[3]) result[0]=4;
      else result[0]=4;

    }
  }//testbeam algorithm
  else{

    float xstripnorm=xstrip/30.;
    result[0] = ClusterSizeEvaluation(ctx, id, xstripnorm, rndmEngine );

    int nstrips = ele->Nstrips(measuresPhi);
    //
    if (result[1] <       1 ) result[1] =       1 ;
    if (result[2] <       1 ) result[2] =       1 ;
    if (result[1] > nstrips ) result[1] = nstrips ;
    if (result[2] > nstrips ) result[2] = nstrips ;

  }


  if( m_turnON_clustersize == false ) result[0] = 1 ;

  return result;

}

//--------------------------------------------
std::vector<int> RpcDigitizationTool::TurnOnStrips(std::vector<int> pcs, const Identifier* id, CLHEP::HepRandomEngine* rndmEngine)
{

  int nstrips;
  int measuresPhi = m_idHelper->measuresPhi(*id);

  const RpcReadoutElement* ele= m_GMmgr->getRpcReadoutElement(*id);

  nstrips = ele->Nstrips(measuresPhi);

  //testbeam algorithm
  if(m_testbeam_clustersize){
    int stripsAlreadyTurnedOn=1-pcs[1]+pcs[2];

    // turn on strips according to spread distribution obtained from data

    if(stripsAlreadyTurnedOn==1){

      if(pcs[0]==2){
	if(CLHEP::RandFlat::shoot(rndmEngine)<0.5) pcs[1]--;
	else pcs[2]++;
      } else if(pcs[0]==3){
	//  out3 << pcs[1]<< " ";
	if(CLHEP::RandFlat::shoot(rndmEngine)<m_cs3Para){
	  pcs[1]--;   // -+-
	  pcs[2]++;
	} else {
	  if(CLHEP::RandFlat::shoot(rndmEngine)<0.5) pcs[2]+=2; // +--
	  else pcs[1]-=2;  // --+
	}
	// out3 << pcs[1] <<std::endl;
      } else if(pcs[0]==4){
	// out4 << pcs[1]<< std::endl;
	double rand=CLHEP::RandFlat::shoot(rndmEngine);
	if(rand<m_cs4Para[0]){ pcs[2]+=3;} // +---
	else if(rand<m_cs4Para[0]+m_cs4Para[1]){  // -+--
	  pcs[1]--;
	  pcs[2]+=2;
	} else if(rand<m_cs4Para[0]+m_cs4Para[1]+m_cs4Para[2]){ // --+-
	  pcs[1]-=2;
	  pcs[2]++;
	} else pcs[1]-=3; //  ---+
	// out4 << pcs[1]<< std::endl;
      }
    }

    if(stripsAlreadyTurnedOn==2){

      if(pcs[0]==3){
	double rand_norm=m_cs3Para+0.5*(1-m_cs3Para);
	if(CLHEP::RandFlat::shoot(rndmEngine)<0.5){   //  +-- or -+-
	  //out3 << pcs[1]<< " ";
	  if(CLHEP::RandFlat::shoot(rndmEngine)<m_cs3Para/rand_norm) pcs[1]--; // -+-
	  else pcs[2]++; // +--
	} else { // -+- or --+
	  //out3 << pcs[1]<<" ";
	  if(CLHEP::RandFlat::shoot(rndmEngine)<m_cs3Para/rand_norm) pcs[2]++; // -+-
	  else pcs[1]--; // --+
	}
	//out3 << pcs[1]<< " "<< std::endl;
      } else if(pcs[0]==4){
	if(CLHEP::RandFlat::shoot(rndmEngine)<0.5){ // strip crossed is the first of the two
	  //out4<<pcs[1]<<" ";
	  double rand=CLHEP::RandFlat::shoot(rndmEngine,2*m_cs4Para[0]+m_cs4Para[1]+m_cs4Para[2]);
	  if(rand<2*m_cs4Para[0]){ pcs[2]+=2;} // the '2*' is to compensate for the 0.5
	  else if(rand<2*m_cs4Para[0]+m_cs4Para[1]){ pcs[1]--;pcs[2]++;}
	  else { pcs[1]-=2;}
	  //out4<<pcs[1]<<std::endl;
	} else {  // strip crossed is the second of the two
	  //out4<<pcs[2]<<" ";
	  double rand=CLHEP::RandFlat::shoot(rndmEngine,m_cs4Para[1]+m_cs4Para[2]+2*m_cs4Para[3]);
	  if(rand<2*m_cs4Para[3]){ pcs[1]-=2;}
	  else if(rand<2*m_cs4Para[3]+m_cs4Para[2]){ pcs[1]--;pcs[2]++;}
	  else { pcs[2]+=2;}
	  //out4<<pcs[1]<<std::endl;
	}
      }
    }
  }//testbeam algorithm
  else{
    if(pcs[0]==-2)
      {
	pcs[1]=pcs[2]-1;
      }
    else if(pcs[0]==2)
      {
	pcs[2]=pcs[1]+1;
      }
    else if(pcs[0]>2)
      {
	pcs[1] = pcs[1] - pcs[0] / 2  ;
	if(fmod(pcs[0],2)==0) pcs[1] = pcs[1] + 1 ;
	pcs[2] = pcs[1] + pcs[0]  - 1 ;
      }
    else if(pcs[0]<-2)
      {
	pcs[1] = pcs[1] + pcs[0] / 2  ;
	pcs[2] = pcs[1] - pcs[0] - 1  ;
      }

  }

  // cut the clusters at the beginning and at the end of the chamber

  if(pcs[1]<1) pcs[1]=1;
  if(pcs[2]<1) pcs[2]=1; // could be 0, for some imprecisions in the case of hits at the border of the chamber
  if(pcs[1]>nstrips) pcs[1]=(int)nstrips;
  if(pcs[2]>nstrips) pcs[2]=(int)nstrips;

  pcs[0] = pcs[2] - pcs[1] + 1 ;

  return pcs;

}

//--------------------------------------------
double RpcDigitizationTool::PropagationTime(const Identifier* id, const Amg::Vector3D pos){

  float length;
  float impact;
  float distance = 0;
  int doubletZ    = m_idHelper->doubletZ(*id);
  int doubletPhi  = m_idHelper->doubletPhi(*id);
  int measuresPhi = m_idHelper->measuresPhi(*id);
  int stEta       = m_idHelper->stationEta(*id);

  const RpcReadoutElement* ele= m_GMmgr->getRpcReadoutElement(*id);


  // length of the strip
  length = ele->StripLength(measuresPhi);


  if(measuresPhi){
    // position along the strip
    // offset necessary because pos is given wrt the center of the strip panel

    double offset=ele->stripPanelZsize(measuresPhi);
    impact = pos.z()+0.5*offset;

    if(stEta>=0){
      if(doubletZ==1) distance=impact;
      if(doubletZ==2||doubletZ==3) distance=length-impact; // dZ=3 for rib chambers. probably not correct...
    } else {
      if(doubletZ==2||doubletZ==3) distance=impact;
      if(doubletZ==1) distance=length-impact;
    }
  } else {

    double offset=ele->stripPanelSsize(measuresPhi);
    impact = pos.y()+0.5*offset;
    if(doubletPhi==1) distance=impact;
    if(doubletPhi==2) distance=length-impact;
  }

  // distance in mm, SIG_VEL in ns/m
  return distance*SIG_VEL/1000;

}

//--------------------------------------------
double RpcDigitizationTool::PropagationTimeNew(const Identifier* id, const Amg::Vector3D globPos){

  float distance;
  int measuresPhi = m_idHelper->measuresPhi(*id);

  const RpcReadoutElement* ele= m_GMmgr->getRpcReadoutElement(*id);
  const Amg::Vector3D point=Amg::Vector3D(globPos);

  if(measuresPhi){

    distance=ele->distanceToPhiReadout(point,*id);

  } else {

    distance=ele->distanceToEtaReadout(point,*id);

  }


  // distance in mm, SIG_VEL in ns/m
  return fabs(distance*SIG_VEL/1000);

}

//--------------------------------------------
Amg::Vector3D RpcDigitizationTool::adjustPosition(const Identifier* id, const Amg::Vector3D hitPos){

  // code to change local axis orientation taking into account geometrical rotations

  const RpcReadoutElement* ele= m_GMmgr->getRpcReadoutElement(*id);

  bool flipEta=false; // both are false if MuonDetDescr is used, because axis re-oriented in RPCSD
  // ME unused: bool flipPhi=false;

  // calculate flipEta
  int rotate=1;
  //  if(ele->rotatedGasGap(*id)) rotate=-rotate;
  if(ele->rotatedRpcModule()) rotate=-rotate;
  if(ele->isMirrored()) rotate=-rotate;
  if(rotate<0) flipEta=true;

  // calculate flipPhi
  //  flipPhi=ele->isMirrored();

  Amg::Vector3D result=hitPos;

  if(flipEta) result.z() = -result.z();
  //  if(flipPhi) result.y() = -result.y();

  return result;

}

//--------------------------------------------
int RpcDigitizationTool::adjustStripNumber(const Identifier* id,int nstrip){

  // code to change local axis orientation taking into account geometrical rotations

  const RpcReadoutElement* ele= m_GMmgr->getRpcReadoutElement(*id);

  int result=nstrip;

  // bool flipEta=false; // not used
  bool flipPhi=false;

  // calculate flipEta
  // int rotate=1; // not used
  //  if(ele->rotatedGasGap(*id)) rotate=-rotate;
  // if(ele->rotatedRpcModule()) rotate=-rotate; // not used
  // if(ele->isMirrored()) rotate=-rotate; // not used
  // if(rotate<0) flipEta=true; // not used

  // calculate flipPhi
  flipPhi=ele->isMirrored();

  //  if(flipEta) result.z() = -result.z();

  if(flipPhi){

    int totStrips=ele->Nstrips(1);
    result=totStrips-nstrip+1;

  }

  return result;

}

//--------------------------------------------
Amg::Vector3D RpcDigitizationTool::posInPanel(const Identifier* id, const Amg::Vector3D posInGap){ // the hit has the position in the gap. we need the position in the panel

  int stationName = m_idHelper->stationName(*id);
  // ME unused: int gasGap      = m_idHelper->gasGap(*id);
  int measuresPhi = m_idHelper->measuresPhi(*id);
  std::string namestring=m_idHelper->stationNameString(stationName);


  const RpcReadoutElement* ele= m_GMmgr->getRpcReadoutElement(*id);

  float gaplength=ele->gasGapSsize();
  // correction needed only in X direction
  float panelXlength=ele->stripPanelSsize(measuresPhi);
  Amg::Vector3D result=posInGap;

  if(ele->Nphigasgaps()!=1) return result; //all but BMS/F and ribs chambers

  else if(ele->NphiStripPanels()==1) return result; // for rib chambers no correction needed
  else {

    //std::cout<<"position in gap is "<<posInGap<<std::endl;

    if(result.y()<0) result.y() = gaplength/2.-fabs(result.y()); // wrt the beginning of the panel

    result.y() = result.y()-panelXlength/2.; // wrt the center of the panel

    //std::cout<<"position in panel is"<< result<<std::endl;

    return result;
  }
}

//--------------------------------------------
int RpcDigitizationTool::findStripNumber(Amg::Vector3D posInGap, Identifier digitId, double& posinstrip){

  const RpcReadoutElement* ele= m_GMmgr->getRpcReadoutElement(digitId);

  Amg::Vector3D posInElement=ele->SDtoModuleCoords(posInGap, digitId);

  //std::cout << "posx,y,z " << posInGap.x() << " " << posInElement.x()<< " " << posInGap.y()<< " " << posInElement.y()<< " " <<posInGap.z() << " " << posInElement.z()<< std::endl;

  // extract from digit id the relevant info

  int measuresPhi = m_idHelper->measuresPhi(digitId);
  int doubletZ = m_idHelper->doubletZ(digitId);
  int doubletPhi = m_idHelper->doubletPhi(digitId);
  int gasGap = m_idHelper->gasGap(digitId);
  double stripWidth = ele->StripWidth(measuresPhi);

  //const Identifier& id,  int doubletZ, int doubletPhi,
  //                       int gasGap, int measuresPhi, int strip

  // find position of first and last strip

  int nstrips=ele->Nstrips(measuresPhi);

  Identifier firstStrip = m_idHelper->channelID(digitId, doubletZ, doubletPhi, gasGap, measuresPhi, 1);
  Identifier lastStrip = m_idHelper->channelID(digitId, doubletZ, doubletPhi, gasGap, measuresPhi, nstrips);

  Amg::Vector3D firstPos(0.,0.,0);
  try {
    firstPos=ele->localStripPos(firstStrip);
  }
  catch (...) {
    ATH_MSG_ERROR("RpcReadoutElement::localStripPos call failed.");
    ATH_MSG_WARNING("firstPos determination failed.");
  }
  Amg::Vector3D lastPos(0.,0.,0);
  try {
    lastPos=ele->localStripPos(lastStrip);
  }
  catch (...) {
    ATH_MSG_ERROR("RpcReadoutElement::localStripPos call failed.");
    ATH_MSG_WARNING("lastPos determination failed.");
  }

  // if(ele->Nphigasgaps()!=1) return result; //all but BMS/F and ribs chambers

  //   else if(ele->NphiStripPanels()==1) return result; // for rib chambers no correction needed
  //   else {

  //     //std::cout<<"position in gap is "<<posInGap<<std::endl;

  //     if(result.y()<0) result.y() = gaplength/2.-fabs(result.y()); // wrt the beginning of the panel

  //     result.y() = result.y()-panelXlength/2.; // wrt the center of the panel

  //     //std::cout<<"position in panel is"<< result<<std::endl;

  //     return result;
  //   }

  double start, stop, impact;
  double pitch = ele->StripPitch(measuresPhi);
  double dead=pitch-stripWidth;

  if(measuresPhi){
    impact =  (posInElement.y());  //std::cout<<"position in element is y "<< posInElement.y() << " "<< impact << std::endl;
    start  =  (firstPos.y());
    stop   =  (lastPos.y());
  } else {
    impact =  (posInElement.z());  //std::cout<<"position in element is z "<< posInElement.z() << " " << impact << std::endl;
    start  =  (firstPos.z());
    stop   =  (lastPos.z());
  }

  double min_=std::min(start,stop);
  double max_=std::max(start,stop);

  min_=min_-pitch/2.-dead/2.*0;
  max_=max_+pitch/2.+dead/2.*0;

  //  std::cout<<" Nphigasgaps, NphiStripPanels "<< ele->Nphigasgaps()<< " "<< ele->NphiStripPanels()<<std::endl;


  //  int result= abs(int((min_-impact)/pitch))+1; // the original 
  //  int result= abs(int((min_-impact)/pitch)+1);
  int result= int((impact-min_)/pitch)+1;
  if (result<1 || result>nstrips) 
    {
      ATH_MSG_DEBUG("WARNING: strip closest to hit is outside the strip panel boundaries: impact, min_, max_ "
		    <<impact<<" ["<<min_<<", "<<max_<<"]  strip # "<<result<<" [1, "<<nstrips<<"]   pitch = "<<pitch
		    <<" stripID="<< m_idHelper->show_to_string(digitId));
      if (result>nstrips) result=nstrips;
      else if (result<1) result=1;
    }
  

  //  std::cout<<"\t start stop length impact pitch strip are "<<min_<< " "<<max_<< " "<<nstrips*pitch+2<< " "<< impact<< " "<<pitch<<" " << result<<std::endl;

  posinstrip=fabs(min_-impact)-(result-1)*pitch;

  //std::cout<<  "posinstrip " <<  fmod(fabs(min_-impact),pitch)/pitch<< std::endl;

  return result;

}

//--------------------------------------------
long long int RpcDigitizationTool::PackMCTruth(float proptime, float bctime, float posy, float posz){

  // start with proptime: it is usually ~ns. It comes in ns. We express it in ns/10. use only 8 bits
  if (proptime<0)
    {
      std::cerr<<"A poblem: packing a propagation time <0 "<<proptime<<" redefine it as 0"<<std::endl;
      proptime = 0.;
    }
  long long int new_proptime=int(proptime*10)&0xff;

  // now tof. it is ~100ns. comes in ns. express it in ns/10. 16 bits needed (0-32768)
  // now BC time: it is ~100ns. comes in ns. express it in ns/10. 16 bits needed (0-32768)
  // can be negative (=> add 300 ns)

  long long int new_bctime=int((bctime+300.)*10.)&0xffff;

  // posy: ~1000mm comes in mm, write it in mm*10. need 16 bits (0-32768)
  // can be negative (=>add 1500 mm)

  long long int new_posy=int((posy+1500.)*10.)&0xffff;

  // posz: ~1000mm comes in mm, write it in mm*10. need 16 bits (0-32768)
  // can be negative (=>add 1500 mm)

  long long int new_posz=int((posz+1500.)*10.)&0xffff;

  //std::cout<<"   result is "<<(new_proptime+(new_tof<<8)+(new_posy<<24)+(new_posz<<40))<<std::endl;

  return (new_proptime+(new_bctime<<8)+(new_posy<<24)+(new_posz<<40));

}

//--------------------------------------------
void RpcDigitizationTool::UnPackMCTruth(double theWord, float& proptime, float& bctime, float& posy, float& posz){
  //int64_t is just a shorter way of writing long long int
  typedef union
  {
    double  dWord;
    int64_t iWord;
  }
  Repacker;
  Repacker MCTruth;
  MCTruth.dWord=theWord;
  proptime=((MCTruth.iWord) & 0x00000000000000ffLL)/10.;
  bctime  =(((MCTruth.iWord) & 0x0000000000ffff00LL)>>8)/10.;
  posy    =(((MCTruth.iWord) & 0x000000ffff000000LL)>>24)/10.;
  posz    =(((MCTruth.iWord) & 0x00ffff0000000000LL)>>40)/10.;

  //   long long int *a=reinterpret_cast<long long int *>(&theWord);
  //
  //   //std::cout<<"   word at unpacking is "<<(*a)<<std::endl;
  //
  //   proptime=((*a) & 0x00000000000000ffLL)/10.;
  //   bctime  =(((*a) & 0x0000000000ffff00LL)>>8)/10.;
  //   posy=   (((*a) & 0x000000ffff000000LL)>>24)/10.;
  //   posz=   (((*a) & 0x00ffff0000000000LL)>>40)/10.;
  //
  bctime = bctime - 300.;
  posy   = posy  - 1500.;
  posz   = posz  - 1500.;

  //std::cout<<"unpacked from word "<<proptime << " " <<tof<< std::endl;

}

//--------------------------------------------
StatusCode RpcDigitizationTool::fillTagInfo() const {

  if (m_tagInfoMgr==0) return StatusCode::FAILURE;

  std::string RpctimeSchema="";
  std::stringstream RpctimeShift ;
  RpctimeShift << (int)m_rpc_time_shift ;

  if ( m_patch_for_rpc_time ) {
    RpctimeSchema="Datalike_TOFoff_TimeShift"+RpctimeShift.str()+"nsec";
  }
  else {
    RpctimeSchema="G4like_TOFon_TimeShift"+RpctimeShift.str()+"nsec";
  }

  StatusCode sc = m_tagInfoMgr->addTag(m_RPC_TimeSchema,RpctimeSchema);

  if(sc.isFailure()) {
    ATH_MSG_WARNING ( m_RPC_TimeSchema << " " << RpctimeSchema  << " not added to TagInfo " );
    return sc;
  } else {
    ATH_MSG_DEBUG ( m_RPC_TimeSchema << " " << RpctimeSchema  << " added to TagInfo " );
  }

  return StatusCode::SUCCESS;

}

//--------------------------------------------
StatusCode RpcDigitizationTool::readParameters(){

  std::string fileName = m_paraFile.value().c_str();
  std::string file = PathResolver::find_file (fileName, "DATAPATH");
  std::ifstream filein(file.c_str(), std::ios::in);

  if ( !filein.good() ) {
    ATH_MSG_FATAL ( "Failed to open file - check file name! " << fileName );
    return StatusCode::FAILURE;
  }

  char linebuffer[200];
  while(!filein.eof()){
    //std::cout << "we are inthe loop now ... " << std::endl;
    filein.getline(linebuffer, 200);
    std::string s=linebuffer;
    std::string tag;
    std::istringstream str(s.c_str());
    str>>tag;
    //std::cout << "thetag is " << tag << std::endl;
    ATH_MSG_DEBUG ( "read tag " <<tag );
    if(tag=="cs"){ // read cs distribution
      if(m_csPara.size()==0) m_csPara.resize(5);
      str>>m_csPara[0]>>m_csPara[1]>>m_csPara[2]>>m_csPara[3]>>m_csPara[4];
    } else if(tag=="rising_gaus"){
      if(m_rgausPara.size()==0) m_rgausPara.resize(3);
      str>>m_rgausPara[0]>>m_rgausPara[1]>>m_rgausPara[2];
    } else if(tag=="falling_gaus"){
      if(m_fgausPara.size()==0) m_fgausPara.resize(3);
      str>>m_fgausPara[0]>>m_fgausPara[1]>>m_fgausPara[2];
    } else if(tag=="const_value"){
      if(m_constPara.size()==0) m_constPara.resize(1);
      str>>m_constPara[0];
    } else if(tag=="cs_3_par"){
      str>>m_cs3Para;
    } else if(tag=="cs_4_par"){
      if(m_cs4Para.size()==0) m_cs4Para.resize(4);
      str>>m_cs4Para[0]>>m_cs4Para[1]>>m_cs4Para[2]>>m_cs4Para[3];
    }
  }

  if(m_csPara.size()==0||m_rgausPara.size()==0||m_fgausPara.size()==0||m_constPara.size()==0){

    return StatusCode::FAILURE; // something didn't work properly
  }
  // if reading was successful, we print the vaues for debugging

  ATH_MSG_DEBUG ( "Read from file the following parameters:");
  ATH_MSG_DEBUG ( "Cluster size distribution: "<<m_csPara[0]<<" " <<m_csPara[1]<<" " << m_csPara[2]<<" " <<m_csPara[3]<<" "<<m_csPara[4]);
  ATH_MSG_DEBUG ( "Fit parameters: "<<m_rgausPara[0]<< " "<<m_rgausPara[1]<< " "<<m_rgausPara[2]<<" " <<m_fgausPara[0]<< " "<<m_fgausPara[1]<< " "<<m_fgausPara[2]<<" " <<m_constPara[0]);


  // read dead panel list from file


  std::string Id_s1 ;
  std::string file1 = PathResolver::find_file(m_FileName_DeadPanels, "DATAPATH");
  std::ifstream filein1(file1.c_str() );
  if( !filein1.good() ) {
    ATH_MSG_FATAL ( "Failed to open file - check file name! " << m_FileName_DeadPanels  );
    return StatusCode::FAILURE;
  }
  while( getline( filein1, Id_s1) ) {
    ATH_MSG_DEBUG( "Dead Panel Id:" << Id_s1 ) ;
    int Id_rpc = atoi (Id_s1.c_str()) ;
    Identifier rpc = Identifier( Id_rpc )   ;
    m_DeadPanel_fromlist.insert(std::make_pair(rpc,1));
  }

  ATH_MSG_INFO( "Number of RPC Dead Panel from list:" << m_DeadPanel_fromlist.size() ) ;


  std::string Id_s2 ;
  std::string file2 = PathResolver::find_file(m_FileName_GoodPanels, "DATAPATH");
  std::ifstream filein2(file2.c_str() );
  if( !filein2.good() ) {
    ATH_MSG_FATAL ( "Failed to open file - check file name! " << m_FileName_GoodPanels  );
    return StatusCode::FAILURE;
  }
  while( getline( filein2, Id_s2) ) {
    ATH_MSG_DEBUG( "Good Panel Id:" << Id_s2 ) ;
    int Id_rpc = atoi (Id_s2.c_str()) ;
    Identifier rpc = Identifier( Id_rpc )   ;
    m_GoodPanel_fromlist.insert(std::make_pair(rpc,1));
  }

  ATH_MSG_INFO( "Number of RPC Good Panel from list:" << m_GoodPanel_fromlist.size() ) ;
  return StatusCode::SUCCESS;

}

//--------------------------------------------
StatusCode RpcDigitizationTool::DetectionEfficiency(const EventContext& ctx, const Identifier* IdEtaRpcStrip, const Identifier* IdPhiRpcStrip, bool& undefinedPhiStripStatus, CLHEP::HepRandomEngine* rndmEngine, const HepMcParticleLink& trkParticle) {

  ATH_MSG_DEBUG ( "RpcDigitizationTool::in DetectionEfficiency" );

  ATH_MSG_DEBUG ( "DetEff:Digit IdEta = " << m_idHelper->show_to_string(*IdEtaRpcStrip)  );
  ATH_MSG_DEBUG ( "DetEff:Digit IdPhi = " << m_idHelper->show_to_string(*IdPhiRpcStrip)  );

  
  if (IdEtaRpcStrip==0) return StatusCode::FAILURE;
  if (IdPhiRpcStrip==0) return StatusCode::FAILURE;
  undefinedPhiStripStatus = false;

  // dead spacers are not simulated in GEANT4  => their effect must be emulated in the digitizer as an effective max. efficiency = 99% 
  // (spacers are 1x1cm^2 over a grid of 10x10cm^2 =? geometrical ineff. introduced is 1% for normal incidence) 
  float maxGeomEff   = 0.99;
  float PhiAndEtaEff = maxGeomEff; // 1.00 ;
  float OnlyEtaEff   = 0.00 ;
  float OnlyPhiEff   = 0.00 ;

  //2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG
  int stationName = m_idHelper->stationName(*IdEtaRpcStrip);
  int stationEta  = m_idHelper->stationEta (*IdEtaRpcStrip);
  int doubletR    = m_idHelper->doubletR   (*IdEtaRpcStrip);

  //remove feet extension. driven by joboption
  if(m_BOG_BOF_DoubletR2_OFF && (stationName==9||stationName==10) && doubletR == 2){

    m_SetPhiOn = false ;
    m_SetEtaOn = false ;
    return StatusCode::SUCCESS;

  }

  m_SetPhiOn = true ;
  m_SetEtaOn = true ;

  if(m_turnON_efficiency==false)return StatusCode::SUCCESS;

  // int stripetadead = 0 ; // not used
  // int stripphidead = 0 ; // not used 
  
  int stripetagood = 0 ;
  int stripphigood = 0 ;

  if(!m_Efficiency_fromCOOL){
    unsigned int index = stationName - 2 ;
    // BML and BMS, BOL and BOS  come first (stationName= 2 and 3, 4 and 5 -> index 0-3)
    if(stationName>5 && stationName<50) index = index - 2 ;
    // BMF, BOF and BOG are 8,9,10 => must be 4,5 and 6
    else if(stationName>50) index = index - 44 ;
    // BME and BOE 53 and 54 are at indices 7 and 8 

    if( index>m_PhiAndEtaEff_A.size() || index>m_OnlyEtaEff_A.size() || index>m_OnlyPhiEff_A.size()) {
      ATH_MSG_ERROR ( "Index out of array in Detection Efficiency SideA " << index <<" stationName = "<<stationName) ;
      return StatusCode::FAILURE;
    }

    PhiAndEtaEff = m_PhiAndEtaEff_A [index];
    OnlyEtaEff   = m_OnlyEtaEff_A   [index];
    OnlyPhiEff   = m_OnlyPhiEff_A   [index];

    if(stationEta<0){
      if( index>m_PhiAndEtaEff_C.size() || index>m_OnlyEtaEff_C.size() || index>m_OnlyPhiEff_C.size()) {
	ATH_MSG_ERROR ( "Index out of array in Detection Efficiency SideC " << index <<" stationName = "<<stationName) ;
	return StatusCode::FAILURE;
      }
      PhiAndEtaEff = m_PhiAndEtaEff_C [index];
      OnlyEtaEff   = m_OnlyEtaEff_C   [index];
      OnlyPhiEff   = m_OnlyPhiEff_C   [index];
    }
  }
  else{

    SG::ReadCondHandle<RpcCondDbData> readHandle{m_readKey, ctx};
    const RpcCondDbData* readCdo{*readHandle};

    ATH_MSG_DEBUG("Efficiencies and cluster size + dead strips will be extracted from COOL");
    
    Identifier IdEta  = m_idHelper->panelID(*IdEtaRpcStrip);
    Identifier IdPhi  = m_idHelper->panelID(*IdPhiRpcStrip);
    ATH_MSG_DEBUG( "EtaPanelId to look for Eff is "<<m_idHelper->show_to_string(IdEta));
    ATH_MSG_DEBUG( "PhiPanelId to look for Eff is "<<m_idHelper->show_to_string(IdPhi));
    

    float  FracDeadStripEta        = 0. ;
    float  FracDeadStripPhi        = 0. ;
    int    RPC_ProjectedTracksEta  = 0  ;
    double EtaPanelEfficiency      = 0. ;
    double PhiPanelEfficiency      = 0. ;
    double GapEfficiency           = 0. ;

    bool noEntryInDb=false;
    
    if( readCdo->getFracDeadStripMap()  .find(IdEta) == readCdo->getFracDeadStripMap()  .end()){
      ATH_MSG_DEBUG ( "Not In CoolDB the Panel IdEtaRpcStrip :  "  << IdEta << " i.e. "<<m_idHelper->show_to_string(IdEta));
      noEntryInDb=true;      
    }
    else
      {
	ATH_MSG_DEBUG ( "Found In CoolDB the Panel IdEtaRpcStrip :  "  << IdEta << " i.e. "<<m_idHelper->show_to_string(IdEta));
      }
    if( readCdo->getFracDeadStripMap()  .find(IdPhi) == readCdo->getFracDeadStripMap()  .end()) {
      ATH_MSG_DEBUG ( "Not In CoolDB the Panel IdPhiRpcStrip :  "  << IdPhi << " i.e. "<<m_idHelper->show_to_string(IdPhi));
      noEntryInDb=true;
    }
    else
      {
	ATH_MSG_DEBUG ( "Found In CoolDB the Panel IdPhiRpcStrip :  "  << IdPhi << " i.e. "<<m_idHelper->show_to_string(IdPhi));
      }
 
    if( readCdo->getFracDeadStripMap()  .find(IdEta) != readCdo->getFracDeadStripMap()  .end()) FracDeadStripEta        = readCdo->getFracDeadStripMap().find(IdEta)->second ;
    if( readCdo->getFracDeadStripMap()  .find(IdPhi) != readCdo->getFracDeadStripMap()  .end()) FracDeadStripPhi        = readCdo->getFracDeadStripMap().find(IdPhi)->second ;
    if( readCdo->getProjectedTracksMap().find(IdEta) != readCdo->getProjectedTracksMap().end()) RPC_ProjectedTracksEta  = readCdo->getProjectedTracksMap().find(IdEta)->second ;

    if( readCdo->getEfficiencyMap()     .find(IdEta) != readCdo->getEfficiencyMap()     .end()) EtaPanelEfficiency      = readCdo->getEfficiencyMap().find(IdEta)->second ;
    if( readCdo->getEfficiencyMap()     .find(IdPhi) != readCdo->getEfficiencyMap()     .end()) PhiPanelEfficiency      = readCdo->getEfficiencyMap().find(IdPhi)->second ;
    if( readCdo->getEfficiencyGapMap()  .find(IdEta) != readCdo->getEfficiencyGapMap()  .end()) GapEfficiency	        = readCdo->getEfficiencyGapMap().find(IdEta)->second ;

    if (fabs(FracDeadStripEta-1.)<0.001) 
      {
	ATH_MSG_DEBUG ("Watch out: SPECIAL CASE: Read from Cool: FracDeadStripEta/Phi "<<FracDeadStripEta<<"/"<<FracDeadStripPhi
			 <<" RPC_ProjectedTracksEta "<<RPC_ProjectedTracksEta<<" Eta/PhiPanelEfficiency "<<EtaPanelEfficiency<<"/"<<PhiPanelEfficiency
			 <<" gapEff "<<GapEfficiency<<" for gas gap "<<m_idHelper->show_to_string(IdEta)<<" id "<<IdEta.get_identifier32().get_compact());
	// dead eta panel => cannot determine the strip status for phi strips 
	// FracDeadStripPhi must be reset to 0. and undefinedPhiStripStatus = true
	FracDeadStripPhi = 0.;
	undefinedPhiStripStatus = true;
	ATH_MSG_VERBOSE ("Watch out: SPECIAL CASE: Resetting FracDeadStripPhi "<<FracDeadStripPhi<<" ignoring phi dead strips ");
      }


    // special test 
    // here redefining the efficiencies: 
    // EtaPanelEfficiency = 0.92;
    // PhiPanelEfficiency = 0.85;
    // GapEfficiency      = 0.97;
    bool changing = false;
    ATH_MSG_DEBUG ("Read from Cool: FracDeadStripEta/Phi "<<FracDeadStripEta<<"/"<<FracDeadStripPhi<<" RPC_ProjectedTracksEta "<<RPC_ProjectedTracksEta<<" Eta/PhiPanelEfficiency "<<EtaPanelEfficiency<<"/"<<PhiPanelEfficiency<<" gapEff "<<GapEfficiency);
    //if ((1.-FracDeadStripEta)<EtaPanelEfficiency) 
    if ((maxGeomEff-FracDeadStripEta)-EtaPanelEfficiency<-0.011) 
      {
	  ATH_MSG_DEBUG("Ineff. from dead strips on Eta Panel larger that measured efficiency: deadFrac="<<FracDeadStripEta<<" Panel Eff="<<EtaPanelEfficiency<<" for Panel "<<m_idHelper->show_to_string(IdEta));
	  ATH_MSG_DEBUG("... see the corresponding report from RpcDetectorStatusDbTool");
	//EtaPanelEfficiency = 1.-FracDeadStripEta;	
	EtaPanelEfficiency = maxGeomEff-FracDeadStripEta;	
	changing = true;
      }
      //if ((1.-FracDeadStripPhi)<PhiPanelEfficiency) 
    if ((maxGeomEff-FracDeadStripPhi)-PhiPanelEfficiency<-0.011) 
    {
      ATH_MSG_DEBUG("Ineff. from dead strips on Phi Panel larger that measured efficiency: deadFrac="<<FracDeadStripPhi<<" Panel Eff="<<PhiPanelEfficiency<<" for Panel "<<m_idHelper->show_to_string(IdPhi));
      ATH_MSG_DEBUG("... see the corresponding report among the warnings of RpcDetectorStatusDbTool");
      //PhiPanelEfficiency = 1.-FracDeadStripPhi;
      PhiPanelEfficiency = maxGeomEff-FracDeadStripPhi;
      changing = true;
    }
    //if ((1.-FracDeadStripEta*FracDeadStripPhi)<GapEfficiency) 
    if ((maxGeomEff-FracDeadStripEta*FracDeadStripPhi)-GapEfficiency<-0.011) 
    {
      ATH_MSG_DEBUG("Ineff. from dead strips on Eta/Phi Panels larger that measured EtaORPhi efficiency: deadFrac="<<FracDeadStripEta*FracDeadStripPhi<<" EtaORPhi Eff="<<GapEfficiency<<" for GasGap "<<m_idHelper->show_to_string(IdEta));
      ATH_MSG_DEBUG("... see the corresponding report among the warnings of RpcDetectorStatusDbTool");
      //GapEfficiency = 1.-FracDeadStripEta*FracDeadStripPhi;
      GapEfficiency = maxGeomEff-FracDeadStripEta*FracDeadStripPhi;
      changing = true;
    }
    if (changing) ATH_MSG_DEBUG ("Rinormalized Values from Cool: FracDeadStripEta/Phi "<<FracDeadStripEta<<"/"<<FracDeadStripPhi<<" RPC_ProjectedTracksEta "<<RPC_ProjectedTracksEta<<" Eta/PhiPanelEfficiency "<<EtaPanelEfficiency<<"/"<<PhiPanelEfficiency<<" gapEff "<<GapEfficiency);

    
    //std::cout<<"   m_turnON_clustersize="<<  m_turnON_clustersize<<" effEta/Phi/GG="<<EtaPanelEfficiency<<"/"<<PhiPanelEfficiency<<"/"<<GapEfficiency<<std::endl;

    /*
      int stripetadead = 0 ;
      int stripphidead = 0 ;

      int stripetagood = 0 ;
      int stripphigood = 0 ;
    */

    // if(readCdo->getDeadStripIntMap().find( *IdEtaRpcStrip)  != readCdo->getDeadStripIntMap().end()) stripetadead = readCdo->getDeadStripIntMap().find( *IdEtaRpcStrip)->second; // not used
    // if(readCdo->getDeadStripIntMap().find( *IdPhiRpcStrip)  != readCdo->getDeadStripIntMap().end()) stripphidead = readCdo->getDeadStripIntMap().find( *IdPhiRpcStrip)->second; // not used

    // if( m_PanelId_OFF_fromlist && ( m_DeadPanel_fromlist.find(IdEta) != m_DeadPanel_fromlist.end()) )  stripetadead =  m_DeadPanel_fromlist.find(IdEta)->second; // not used
    // if( m_PanelId_OFF_fromlist && ( m_DeadPanel_fromlist.find(IdPhi) != m_DeadPanel_fromlist.end()) )  stripphidead =  m_DeadPanel_fromlist.find(IdPhi)->second; // not used

    if( m_PanelId_OK_fromlist && ( m_GoodPanel_fromlist.find(IdEta) != m_GoodPanel_fromlist.end()) )  stripetagood =  m_GoodPanel_fromlist.find(IdEta)->second;
    if( m_PanelId_OK_fromlist && ( m_GoodPanel_fromlist.find(IdPhi) != m_GoodPanel_fromlist.end()) )  stripphigood =  m_GoodPanel_fromlist.find(IdPhi)->second;
    

    //gabriele //..stefania - if there are dead strips renormalize the eff. to the active area  
    if(m_kill_deadstrips){
     if ((FracDeadStripEta>0.0&&FracDeadStripEta<1.0) || (FracDeadStripPhi>0.0&&FracDeadStripPhi<1.0) || (noEntryInDb))
    {
      EtaPanelEfficiency= EtaPanelEfficiency/(maxGeomEff-FracDeadStripEta);
      PhiPanelEfficiency= PhiPanelEfficiency/(maxGeomEff-FracDeadStripPhi);
      GapEfficiency     = GapEfficiency/(maxGeomEff-FracDeadStripEta*FracDeadStripPhi);

      if (EtaPanelEfficiency>maxGeomEff) EtaPanelEfficiency=maxGeomEff;
      if (PhiPanelEfficiency>maxGeomEff) PhiPanelEfficiency=maxGeomEff;
      if (GapEfficiency     >maxGeomEff) GapEfficiency     =maxGeomEff;


      if (EtaPanelEfficiency>GapEfficiency) GapEfficiency=EtaPanelEfficiency;
      if (PhiPanelEfficiency>GapEfficiency) GapEfficiency=PhiPanelEfficiency;
      ATH_MSG_DEBUG ("Eff Redefined (to correct for deadfrac): FracDeadStripEta/Phi "<<" Eta/PhiPanelEfficiency "<<EtaPanelEfficiency<<"/"<<PhiPanelEfficiency<<" gapEff "<<GapEfficiency);
    }
    }				    

    //values from COOLDB (eventually overwritten later)
    PhiAndEtaEff =  float(EtaPanelEfficiency + PhiPanelEfficiency - GapEfficiency) ;
    if (PhiAndEtaEff<0.) PhiAndEtaEff=0.;
    OnlyEtaEff   =  float(EtaPanelEfficiency - PhiAndEtaEff) ;
    if (OnlyEtaEff<0.) OnlyEtaEff=0.;
    OnlyPhiEff   =  float(PhiPanelEfficiency - PhiAndEtaEff) ;
    if (OnlyPhiEff<0.) OnlyPhiEff=0.;

   
    
    /* // here is where originally dead strips were killed - including the protection (on phi strips) for dead eta panel
       if(FracDeadStripEta>0.0&&FracDeadStripEta<1.0){
       OnlyEtaEff=OnlyEtaEff/(1.-FracDeadStripEta);
       PhiAndEtaEff=PhiAndEtaEff/(1.-FracDeadStripEta);
       }
       if(FracDeadStripPhi>0.0&&FracDeadStripPhi<1.0){
       OnlyPhiEff=OnlyPhiEff/(1.-FracDeadStripPhi);
       PhiAndEtaEff=PhiAndEtaEff/(1.-FracDeadStripPhi);
       }
    */
    
    //  special patch to be true only when m_Efficiency_fromCOOL=true and /RPC/DQMF/ELEMENT_STATUS tag is RPCDQMFElementStatus_2012_Jaunuary_26
    bool applySpecialPatch=false;
    if (m_EfficiencyPatchForBMShighEta && m_Efficiency_fromCOOL)
    {
      if (m_idHelper->stationName(*IdEtaRpcStrip)==3) ///// BMS  
      {
        if (abs(m_idHelper->stationEta(*IdEtaRpcStrip))==6 && m_idHelper->doubletR(*IdEtaRpcStrip)==1 && m_idHelper->doubletZ(*IdEtaRpcStrip)==2 && m_idHelper->doubletPhi(*IdEtaRpcStrip)==1) 
        {
          applySpecialPatch=true;
          ATH_MSG_WARNING ( "Applying special patch for BMS at |eta|=6 lowPt plane -dbbZ=2 and dbPhi=1 ... will use default eff. for Id "<<m_idHelper->show_to_string(*IdEtaRpcStrip));
          ATH_MSG_WARNING ( "Applying special patch: THIS HAS TO BE DONE IF /RPC/DQMF/ELEMENT_STATUS tag is RPCDQMFElementStatus_2012_Jaunuary_2") ;
        }
      }
    }

    //if projected tracks number too low or inconsistent values get efficiencies from joboption and overwrite previous values
    if(applySpecialPatch || RPC_ProjectedTracksEta<m_CutProjectedTracks || RPC_ProjectedTracksEta>10000000 || EtaPanelEfficiency>1 || EtaPanelEfficiency<0 || PhiPanelEfficiency>1 || PhiPanelEfficiency<0|| GapEfficiency>1 || GapEfficiency<0 || stripetagood == 1 || stripphigood == 1){

      //std::cout<<" do we ever enter here ? "<<std::endl;
      unsigned int index = stationName - 2 ;
      // BML and BMS, BOL and BOS  come first (stationName= 2 and 3, 4 and 5 -> index 0-3)
      if(stationName>5 && stationName<50) index = index - 2 ;
      // BMF, BOF and BOG are 8,9,10 => must be 4,5 and 6
      else if(stationName>50) index = index - 44 ;
      // BME and BOE 53 and 54 are at indices 7 and 8 
      ATH_MSG_DEBUG ( "Some special condition met here - resetting eff.s to python values at index=" << index << " i.e. StName="<<stationName) ;

      if( index>m_PhiAndEtaEff_A.size() || index>m_OnlyEtaEff_A.size() || index>m_OnlyPhiEff_A.size()) {
	ATH_MSG_ERROR ( "Index out of array in Detection Efficiency SideA COOLDB" << index <<" stationName = "<<stationName) ;
	return StatusCode::FAILURE;
      }
      if (RPC_ProjectedTracksEta<m_CutProjectedTracks) ATH_MSG_DEBUG ( "# of proj tracks = "<<RPC_ProjectedTracksEta<<" < cut = "<<m_CutProjectedTracks
								       <<" resetting eff. from cool with default(python) values ");

      PhiAndEtaEff = m_PhiAndEtaEff_A [index];
      OnlyEtaEff   = m_OnlyEtaEff_A   [index];
      OnlyPhiEff   = m_OnlyPhiEff_A   [index];

      if(stationEta<0){
	if( index>m_PhiAndEtaEff_C.size() || index>m_OnlyEtaEff_C.size() || index>m_OnlyPhiEff_C.size()) {
          ATH_MSG_ERROR ( "Index out of array in Detection Efficiency SideC COOLDB" << index <<" stationName = "<<stationName) ;
	  return StatusCode::FAILURE;
	}
	PhiAndEtaEff = m_PhiAndEtaEff_C [index];
	OnlyEtaEff   = m_OnlyEtaEff_C	[index];
	OnlyPhiEff   = m_OnlyPhiEff_C	[index];
      }
      
      
      //if (m_applyEffThreshold) { 
      //gabriele Set efficiency from dead strip fraction instead of nominal value
      float effgap = PhiAndEtaEff+OnlyEtaEff+OnlyPhiEff;
      float s_EtaPanelEfficiency=1.-FracDeadStripEta;
      float s_PhiPanelEfficiency=1.-FracDeadStripPhi;
      float s_PhiAndEtaEff      =  s_EtaPanelEfficiency        *   s_PhiPanelEfficiency  /effgap;
      if(s_PhiAndEtaEff<PhiAndEtaEff)PhiAndEtaEff=s_PhiAndEtaEff;
      float s_OnlyEtaEff        =  s_EtaPanelEfficiency        -   PhiAndEtaEff        ;
      float s_OnlyPhiEff        =  s_PhiPanelEfficiency        -   PhiAndEtaEff        ;  
      
      if(s_OnlyEtaEff  <OnlyEtaEff  )OnlyEtaEff  =s_OnlyEtaEff  ;
      if(s_OnlyPhiEff  <OnlyPhiEff  )OnlyPhiEff  =s_OnlyPhiEff  ;
      //      }
     
    }
    /* 
    //if strip dead overwrite previous values
    if(stripetadead==1) {
    ATH_MSG_DEBUG ( "Dead Strip For Id Eta " <<*IdEtaRpcStrip );
    PhiAndEtaEff =  0. ;
    OnlyEtaEff   =  0. ;
    }
    if(stripphidead==1&&FracDeadStripEta<1) {
    ATH_MSG_DEBUG ( "Dead Strip For Id Phi " <<*IdEtaRpcStrip );
    PhiAndEtaEff =  0. ;
    OnlyPhiEff   =  0. ;
    }
    //protect againts anomalous values
    if(PhiAndEtaEff>1)PhiAndEtaEff=1.;
    if(OnlyEtaEff  >1)  OnlyEtaEff=1.;
    if(OnlyPhiEff  >1)  OnlyPhiEff=1.;
    */
    float VolEff = PhiAndEtaEff+OnlyEtaEff+OnlyPhiEff ;
    if(VolEff>maxGeomEff){
      
      PhiAndEtaEff = (PhiAndEtaEff/VolEff)*maxGeomEff;
      OnlyEtaEff   = (OnlyEtaEff  /VolEff)*maxGeomEff;
      OnlyPhiEff   = (OnlyPhiEff  /VolEff)*maxGeomEff;
      /*
        PhiAndEtaEff = PhiAndEtaEff/VolEff;
        OnlyEtaEff   = OnlyEtaEff  /VolEff;
        OnlyPhiEff   = OnlyPhiEff  /VolEff;
      */
    }

  }


  if (m_applyEffThreshold) {
      //gabriele //apply minimum allowed efficiency criteria

      if(OnlyEtaEff+PhiAndEtaEff<m_Minimum_efficiency && OnlyPhiEff+PhiAndEtaEff<m_Minimum_efficiency){
	// eta panel eff < Minimum AND phi panel eff < Minimum and 
	PhiAndEtaEff   =m_Minimum_efficiency; 
	OnlyEtaEff     =0.;
	OnlyPhiEff     =0.; 
      } 
      else if(OnlyEtaEff+PhiAndEtaEff<m_Minimum_efficiency && OnlyPhiEff+PhiAndEtaEff>m_Minimum_efficiency){
	// eta panel eff < Minimum AND phi panel eff > Minimum and 
	double phiEff = OnlyPhiEff+PhiAndEtaEff;
	PhiAndEtaEff   =m_Minimum_efficiency; 
	OnlyEtaEff     =0.;
	OnlyPhiEff     =phiEff-PhiAndEtaEff; // Phi Panel Efficiency stays unchanged
      }
      else if(OnlyEtaEff+PhiAndEtaEff>m_Minimum_efficiency && OnlyPhiEff+PhiAndEtaEff<m_Minimum_efficiency){
	// eta panel eff > Minimum AND phi panel eff < Minimum and 
	double etaEff = OnlyEtaEff+PhiAndEtaEff;
	PhiAndEtaEff   =m_Minimum_efficiency; 
	OnlyPhiEff     =0.;
	OnlyEtaEff     =etaEff-PhiAndEtaEff; // Eta Panel Efficiency stays unchanged
      }

    }

  //Efficiency correction factor for fractional-charged particles(added by Quanyin Li: quli@cern.ch)
  //link to truth particles and calculate the charge and betagamma
  const HepMC::GenParticle* genparticle = trkParticle.cptr();
  if(genparticle){
    const int particlePdgId = genparticle->pdg_id();
    //only apply efficiency correction to fractional-charged particles based on pdgId betagamma
    if((static_cast<int>(std::abs(particlePdgId)/10000000) == 2) && (static_cast<int>(std::abs(particlePdgId)/100000) == 200)) {
      const double eff_sf = FCPEfficiency(genparticle);
      //Apply scale factor to the 3 Eff.
      PhiAndEtaEff = PhiAndEtaEff * eff_sf;
      OnlyEtaEff = OnlyEtaEff * eff_sf;
      OnlyPhiEff = OnlyPhiEff * eff_sf;

    }
  }



  float I0	  = PhiAndEtaEff			    ;
  float I1	  = PhiAndEtaEff +  OnlyEtaEff  	    ;
  float ITot   = PhiAndEtaEff +  OnlyEtaEff + OnlyPhiEff    ;

  float GapEff = PhiAndEtaEff + OnlyEtaEff + OnlyPhiEff;
  float PhiEff = PhiAndEtaEff + OnlyPhiEff             ;
  float EtaEff = PhiAndEtaEff + OnlyEtaEff             ;

  ATH_MSG_DEBUG ( "DetectionEfficiency: Final Efficiency Values applied for "<<m_idHelper->show_to_string(*IdEtaRpcStrip)<<" are " << PhiAndEtaEff << "=PhiAndEtaEff " <<OnlyEtaEff << "=OnlyEtaEff " <<OnlyPhiEff << "=OnlyPhiEff " << GapEff << "=GapEff " << EtaEff << "=EtaEff " << PhiEff << "=PhiEff ");

  float rndmEff = CLHEP::RandFlat::shoot(rndmEngine, 1) ;

  if(rndmEff < I0){
    m_SetPhiOn = true ;
    m_SetEtaOn = true ;
  }
  else if( ( I0 <= rndmEff ) && ( rndmEff < I1 )  ){
    m_SetPhiOn = false  ;
    m_SetEtaOn = true	;
  }
  else if( ( I1 <= rndmEff ) && ( rndmEff <= ITot ) ){
    m_SetPhiOn = true  ;
    m_SetEtaOn = false ;
  }
  else{
    m_SetPhiOn = false  ;
    m_SetEtaOn = false  ;
  }


  // //stefania 
  // if(stripetadead==1) m_SetEtaOn=false;
  // if(stripphidead==1) m_SetPhiOn=false;


  return StatusCode::SUCCESS;

}

//--------------------------------------------
int RpcDigitizationTool::ClusterSizeEvaluation(const EventContext& ctx, const Identifier* IdRpcStrip, float xstripnorm, CLHEP::HepRandomEngine* rndmEngine) {

  ATH_MSG_DEBUG ( "RpcDigitizationTool::in ClusterSizeEvaluation" );

  ATH_MSG_DEBUG ( "Digit Id = " << m_idHelper->show_to_string(*IdRpcStrip)  );

  if (IdRpcStrip==0) return 1;

  int ClusterSize = 1 ;

  float FracClusterSize1    = 1.00 ;
  float FracClusterSize2    = 0.00 ;
  float MeanClusterSize     = 1.50 ;
  float FracClusterSizeTail = 0.00 ;
  float MeanClusterSizeTail = 1.00 ;
  // float FracClusterSize1norm  = 1  ; // not used
  float FracClusterSize2norm  = 0  ;

  //2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG
  int stationName  = m_idHelper->stationName(*IdRpcStrip);
  int stationEta   = m_idHelper->stationEta (*IdRpcStrip);
  int measuresPhi  = m_idHelper->measuresPhi(*IdRpcStrip);

  unsigned int index = stationName - 2 ;
  // BML and BMS, BOL and BOS  come first (stationName= 2 and 3, 4 and 5 -> index 0-3)
  if(stationName>5 && stationName<50) index = index - 2 ;
  // BMF, BOF and BOG are 8,9,10 => must be 4,5 and 6
  else if(stationName>50) index = index - 44 ;
  // BME and BOE 53 and 54 are at indices 7 and 8 

  if( !m_ClusterSize_fromCOOL ){
    index += m_FracClusterSize1_A.size()/2*measuresPhi ;
    if( index>m_FracClusterSize1_A.size()    || index>m_FracClusterSize2_A.size() ||
	index>m_FracClusterSizeTail_A.size() || index>m_MeanClusterSizeTail_A.size() ) {
      ATH_MSG_WARNING ( "Index out of array in ClusterSizeEvaluation SideA " << index <<" statName "<<stationName) ;
      return 1;
    }
    FracClusterSize1    = m_FracClusterSize1_A      [index];
    FracClusterSize2    = m_FracClusterSize2_A      [index];
    FracClusterSizeTail = m_FracClusterSizeTail_A   [index];
    MeanClusterSizeTail = m_MeanClusterSizeTail_A   [index];

    if(stationEta<0){
      index += m_FracClusterSize1_C.size()/2*measuresPhi - m_FracClusterSize1_A.size()/2*measuresPhi ;
      if( index>m_FracClusterSize1_C.size()    || index>m_FracClusterSize2_C.size() ||
	  index>m_FracClusterSizeTail_C.size() || index>m_MeanClusterSizeTail_C.size() ) {
	ATH_MSG_WARNING ( "Index out of array in ClusterSizeEvaluation SideC " << index <<" statName "<<stationName) ;
	return 1;
      }

      FracClusterSize1    = m_FracClusterSize1_C      [index];
      FracClusterSize2    = m_FracClusterSize2_C      [index];
      FracClusterSizeTail = m_FracClusterSizeTail_C   [index];
      MeanClusterSizeTail = m_MeanClusterSizeTail_C   [index];
    }
  }
  else{
    SG::ReadCondHandle<RpcCondDbData> readHandle{m_readKey, ctx};
    const RpcCondDbData* readCdo{*readHandle};

    Identifier Id  = m_idHelper->panelID(*IdRpcStrip);

    int    RPC_ProjectedTracks = 0;

    if( readCdo->getProjectedTracksMap().find(Id) != readCdo->getProjectedTracksMap().end()) RPC_ProjectedTracks = readCdo->getProjectedTracksMap().find(Id)->second ;

    if(readCdo->getFracClusterSize1Map().find(Id) != readCdo->getFracClusterSize1Map().end()) FracClusterSize1	= float(readCdo->getFracClusterSize1Map().find(Id)->second) ;
    else ATH_MSG_INFO ( "FracClusterSize1 entry not found for id = " <<m_idHelper->show_to_string(*IdRpcStrip)<<" default will be used") ;
    if(readCdo->getFracClusterSize2Map().find(Id) != readCdo->getFracClusterSize2Map().end()) FracClusterSize2	= float(readCdo->getFracClusterSize2Map().find(Id)->second) ;
    else ATH_MSG_INFO ( "FracClusterSize2 entry not found for id = " <<m_idHelper->show_to_string(*IdRpcStrip)<<" default will be used") ;

    ATH_MSG_DEBUG ( "FracClusterSize1 and 2 "<< FracClusterSize1 << " " << FracClusterSize2  );

    FracClusterSizeTail = 1. - FracClusterSize1 - FracClusterSize2 ;

    if(readCdo->getMeanClusterSizeMap().find(Id) != readCdo->getMeanClusterSizeMap().end()) MeanClusterSize     = float(readCdo->getMeanClusterSizeMap().find(Id)->second)  ;
    else ATH_MSG_INFO ( "MeanClusterSize entry not found for id = " <<m_idHelper->show_to_string(*IdRpcStrip)<<" default will be used") ;

    MeanClusterSizeTail = MeanClusterSize - FracClusterSize1 - 2*FracClusterSize2 ;

    ATH_MSG_DEBUG ( "MeanClusterSizeTail and FracClusterSizeTail "<< MeanClusterSizeTail << " " << FracClusterSizeTail  );

    //if clustersize have anomalous values set to the average cluster size from joboption
    if(RPC_ProjectedTracks<m_CutProjectedTracks ||  RPC_ProjectedTracks>10000000 || MeanClusterSize>m_CutMaxClusterSize || MeanClusterSize<=1 || FracClusterSizeTail < 0 || FracClusterSize1 < 0 || FracClusterSize2 < 0  || FracClusterSizeTail > 1 || FracClusterSize1 > 1 || FracClusterSize2 >1){
      index += m_FracClusterSize1_A.size()/2*measuresPhi ;
      if( index>m_FracClusterSize1_A.size()    || index>m_FracClusterSize2_A.size() ||
	  index>m_FracClusterSizeTail_A.size() || index>m_MeanClusterSizeTail_A.size() ) {
	ATH_MSG_WARNING ( "Index out of array in ClusterSizeEvaluation SideA " << index << " statName "<<stationName) ;
	return 1;
      }
      FracClusterSize1	= m_FracClusterSize1_A      [index];
      FracClusterSize2	= m_FracClusterSize2_A      [index];
      FracClusterSizeTail = m_FracClusterSizeTail_A   [index];
      MeanClusterSizeTail = m_MeanClusterSizeTail_A   [index];

      if(stationEta<0){
	index += m_FracClusterSize1_C.size()/2*measuresPhi - m_FracClusterSize1_A.size()/2*measuresPhi ;
	if( index>m_FracClusterSize1_C.size()    || index>m_FracClusterSize2_C.size() ||
	    index>m_FracClusterSizeTail_C.size() || index>m_MeanClusterSizeTail_C.size() ) {
	  ATH_MSG_WARNING ( "Index out of array in ClusterSizeEvaluation SideC " << index << " statName "<<stationName ) ;
	  return 1;
	}

	FracClusterSize1	 = m_FracClusterSize1_C      [index];
	FracClusterSize2	 = m_FracClusterSize2_C      [index];
	FracClusterSizeTail = m_FracClusterSizeTail_C   [index];
	MeanClusterSizeTail = m_MeanClusterSizeTail_C   [index];
      }
    }

  }

  if(FracClusterSize1>1   )FracClusterSize1   =1.;
  if(FracClusterSize2>1   )FracClusterSize2   =1.;
  if(FracClusterSizeTail>1)FracClusterSizeTail=1.;
  float FracTot = FracClusterSize1+FracClusterSize2+FracClusterSizeTail;
  if(FracTot!=1.&&FracTot>0) {
    FracClusterSize1    =FracClusterSize1    / FracTot ;
    FracClusterSize2    =FracClusterSize2    / FracTot ;
    FracClusterSizeTail =FracClusterSizeTail / FracTot ;
  }
  if(MeanClusterSizeTail<0 || MeanClusterSizeTail>10 )MeanClusterSizeTail=1;

  ATH_MSG_VERBOSE ( "ClusterSize Final " << FracClusterSize1 << " FracClusterSize1 " << FracClusterSize2 << " FracClusterSize2  " << FracClusterSizeTail << "   " << FracClusterSizeTail  << " MeanClusterSizeTail  " <<  MeanClusterSizeTail );

  float FracClusterSize1plus2  = FracClusterSize1 + FracClusterSize2   	                    ;
  float ITot                   = FracClusterSize1 + FracClusterSize2 +  FracClusterSizeTail ;

  if( FracClusterSize1plus2!= 0 ){
    //FracClusterSize1norm = FracClusterSize1 / FracClusterSize1plus2 ; // not used
    FracClusterSize2norm = FracClusterSize2 / FracClusterSize1plus2 ;
  }

  float rndmCS = CLHEP::RandFlat::shoot(rndmEngine, ITot ) ;

  //Expanded CS2 of 1.3 to match average CS1 and CS2 (to be investigate)
  if(rndmCS < FracClusterSize1plus2){
    //deterministic assignment of CS 1 or 2
    if(xstripnorm <= FracClusterSize2norm/2.*1.3){
      ClusterSize = -2 ;
    }
    else if( (1.0-FracClusterSize2norm/2.*1.3) <= xstripnorm ){
      ClusterSize =  2 ;
    }
    else {
      ClusterSize =  1 ;
    }
    if(m_ClusterSize1_2uncorr==1){
      float rndmCS1_2 = CLHEP::RandFlat::shoot(rndmEngine, 1 ) ;
      ClusterSize =  1 ;
      if(rndmCS1_2<FracClusterSize2norm)ClusterSize =  2 ;
    }

  }
  else if( ( FracClusterSize1plus2 <= rndmCS ) && ( rndmCS <= ITot ) ){
    ClusterSize = m_FirstClusterSizeInTail  ;
    ClusterSize += int(CLHEP::RandExponential::shoot(rndmEngine, MeanClusterSizeTail )) ;
    float rndmLR = CLHEP::RandFlat::shoot(rndmEngine, 1.0 ) ;
    if(rndmLR>0.5)ClusterSize = - ClusterSize ;
  }
  else{
    ClusterSize = 1  ;
  }

  //std::cout << "Cluster1_2 " << rndmCS <<" " << ITot<< " " << ClusterSize << " " << xstripnorm << " " << FracClusterSize2norm/2. <<" " <<  (1.0-FracClusterSize2norm/2.)<< std::endl ;

  //negative CS correspond to left asymmetric cluster with respect to nstrip
  return ClusterSize;

}

//--------------------------------------------
StatusCode RpcDigitizationTool::PrintCalibrationVector() {

  ATH_MSG_INFO( "RpcDigitizationTool::in PrintCalibrationVector" );

  StatusCode sc = StatusCode::SUCCESS;
  int vec_size = 0 ;

  vec_size = m_PhiAndEtaEff_A.size() ;
  for(int i=0 ; i!= vec_size ; i++ ){
    ATH_MSG_INFO ( "size of RPC calib vector PhiAndEtaEff_A: " << m_PhiAndEtaEff_A.value().at(i) );
  }
  vec_size = m_OnlyPhiEff_A.size() ;
  for(int i=0 ; i!= vec_size ; i++ ){
    ATH_MSG_INFO ( "size of RPC calib vector OnlyPhi_A: " << m_OnlyPhiEff_A.value().at(i) );
  }
  vec_size = m_OnlyEtaEff_A.size() ;
  for(int i=0 ; i!= vec_size ; i++ ){
    ATH_MSG_INFO ( "size of RPC calib vector OnlyEta_A: " << m_OnlyEtaEff_A.value().at(i) );
  }

  vec_size = m_PhiAndEtaEff_C.size() ;
  for(int i=0 ; i!= vec_size ; i++ ){
    ATH_MSG_INFO ( "size of RPC calib vector PhiAndEtaEff_C: " << m_PhiAndEtaEff_C.value().at(i) );
  }
  vec_size = m_OnlyPhiEff_C.size() ;
  for(int i=0 ; i!= vec_size ; i++ ){
    ATH_MSG_INFO ( "size of RPC calib vector OnlyPhi_C: " << m_OnlyPhiEff_C.value().at(i) );
  }
  vec_size = m_OnlyEtaEff_C.size() ;
  for(int i=0 ; i!= vec_size ; i++ ){
    ATH_MSG_INFO ( "size of RPC calib vector OnlyEta_C: " << m_OnlyPhiEff_C.value().at(i) );
  }
  vec_size = m_FracClusterSize1_A.size() ;
  for(int i=0 ; i!= vec_size ; i++ ){
    ATH_MSG_INFO ( "size of RPC calib vector FracClusterSize1_A: " << m_FracClusterSize1_A.value().at(i) );
  }
  vec_size = m_FracClusterSize2_A.size() ;
  for(int i=0 ; i!= vec_size ; i++ ){
    ATH_MSG_INFO ( "size of RPC calib vector FracClusterSize2_A: " << m_FracClusterSize2_A.value().at(i) );
  }
  vec_size = m_FracClusterSizeTail_A.size() ;
  for(int i=0 ; i!= vec_size ; i++ ){
    ATH_MSG_INFO ( "size of RPC calib vector FracClusterSizeTail_A: " << m_FracClusterSizeTail_A.value().at(i) );
  }
  vec_size = m_MeanClusterSizeTail_A.size() ;
  for(int i=0 ; i!= vec_size ; i++ ){
    ATH_MSG_INFO ( "size of RPC calib vector MeanClusterSizeTail_A: " << m_MeanClusterSizeTail_A.value().at(i) );
  }
  vec_size = m_FracClusterSize1_C.size() ;
  for(int i=0 ; i!= vec_size ; i++ ){
    ATH_MSG_INFO ( "size of RPC calib vector FracClusterSize1_C: " << m_FracClusterSize1_C.value().at(i) );
  }
  vec_size = m_FracClusterSize2_C.size() ;
  for(int i=0 ; i!= vec_size ; i++ ){
    ATH_MSG_INFO ( "size of RPC calib vector FracClusterSize2_C: " << m_FracClusterSize2_C.value().at(i) );
  }
  vec_size = m_FracClusterSizeTail_C.size() ;
  for(int i=0 ; i!= vec_size ; i++ ){
    ATH_MSG_INFO ( "size of RPC calib vector FracClusterSizeTail_C: " << m_FracClusterSizeTail_C.value().at(i) );
  }
  vec_size = m_MeanClusterSizeTail_C.size() ;
  for(int i=0 ; i!= vec_size ; i++ ){
    ATH_MSG_INFO ( "size of RPC calib vector MeanClusterSizeTail_C: " << m_MeanClusterSizeTail_C.value().at(i) );
  }

  return sc;

}

//--------------------------------------------
StatusCode RpcDigitizationTool::DumpRPCCalibFromCoolDB(const EventContext& ctx) {

  ATH_MSG_DEBUG ( "RpcDigitizationTool::in DumpRPCCalibFromCoolDB" );

  SG::ReadCondHandle<RpcCondDbData> readHandle{m_readKey, ctx};
  const RpcCondDbData* readCdo{*readHandle};

  StatusCode sc = StatusCode::SUCCESS;

  int NpanelEff0       = 0 ;
  int NpanelEffLess05  = 0 ;
  int NpanelEffLess1   = 0 ;
  int NpanelEff1       = 0 ;

  int NpanelFracDead0       = 0 ;
  int NpanelFracDeadLess05  = 0 ;
  int NpanelFracDeadLess1   = 0 ;
  int NpanelFracDead1       = 0 ;

  int NpanelTrackLess500    = 0 ;
  int NpanelTrackLess100    = 0 ;
  int NpanelTrackLess50     = 0 ;
  int NpanelTrackLess10     = 0 ;
  int NpanelTrack0          = 0 ;

  int NpanelWith8Strip       = 0 ;
  int NpanelWith16Strip      = 0 ;
  int NpanelWith24Strip      = 0 ;
  int NpanelWith32Strip      = 0 ;
  int NpanelWith40Strip      = 0 ;
  int NpanelWith48Strip      = 0 ;
  int NpanelWith56Strip      = 0 ;
  int NpanelWith64Strip      = 0 ;
  int NpanelWith80Strip      = 0 ;
  int NpanelWithXStrip       = 0 ;

  int NpanelCSEq1          = 0 ;
  int NpanelCSLess15       = 0 ;
  int NpanelCSLess20       = 0 ;
  int NpanelCSLess25       = 0 ;
  int NpanelCSMore5        = 0 ;

  std::map   <Identifier,     double>::const_iterator  itr;
  std::map   <Identifier,std::string>::const_iterator itrs;
  std::map   <Identifier,      float>::const_iterator itrf;
  std::map   <Identifier,        int>::const_iterator itri;

  ATH_MSG_DEBUG ( "Size Summary RPC_EfficiencyMap: " << readCdo->getEfficiencyMap().size() );
  for (itr=readCdo->getEfficiencyMap().begin(); itr!=readCdo->getEfficiencyMap().end(); ++itr) {
    if( itr->second == 0.   )NpanelEff0++       ;
    if( itr->second  < 0.5  )NpanelEffLess05++  ;
    if( itr->second  < 1.   )NpanelEffLess1++   ;
    if( itr->second == 1.   )NpanelEff1++       ;

    ATH_MSG_DEBUG ( "Summary Id/RPC_EfficiencyMap: " << itr->first <<" i.e. "
		    <<(itr->first).get_identifier32().get_compact()<<" "
		    <<m_idHelper->show_to_string(itr->first)<< " "<< itr->second );
  }


  ATH_MSG_DEBUG ( "Size Summary RPC_EfficiencyGapMap: " << readCdo->getEfficiencyGapMap().size() );
  for (itr=readCdo->getEfficiencyGapMap().begin(); itr!=readCdo->getEfficiencyGapMap().end(); ++itr) {

    ATH_MSG_DEBUG ( "Summary Id/RPC_EfficiencyGapMap: " << itr->first <<" i.e. "
		    <<(itr->first).get_identifier32().get_compact()<<" "
		    <<m_idHelper->show_to_string(itr->first)<< " " << itr->second );
  }


  ATH_MSG_DEBUG ( "Size Summary RPC_MeanClusterSizeMap: " << readCdo->getMeanClusterSizeMap().size() );
  for (itr=readCdo->getMeanClusterSizeMap().begin(); itr!=readCdo->getMeanClusterSizeMap().end(); ++itr) {
    if( itr->second ==  1.0  )NpanelCSEq1++     ;
    if( itr->second  <  1.5  )NpanelCSLess15++  ;
    if( itr->second  <  2.0  )NpanelCSLess20++  ;
    if( itr->second  <  2.5  )NpanelCSLess25++  ;
    if( itr->second >=  5.0  )NpanelCSMore5++   ;

    ATH_MSG_DEBUG ( "Summary Id/RPC_MeanClusterSizeMap: " << itr->first <<" i.e. "
		    <<(itr->first).get_identifier32().get_compact()<<" "
		    <<m_idHelper->show_to_string(itr->first)<< " " << itr->second );
  }


  ATH_MSG_DEBUG ( "Size Summary RPC_FracClusterSize1Map: " << readCdo->getFracClusterSize1Map().size() );
  for (itr=readCdo->getFracClusterSize1Map().begin(); itr!=readCdo->getFracClusterSize1Map().end(); ++itr) {

    ATH_MSG_DEBUG ( "Summary Id/RPC_FracClusterSize1Map: " << itr->first <<" i.e. "
		    <<(itr->first).get_identifier32().get_compact()<<" "
		    <<m_idHelper->show_to_string(itr->first)<< " " << itr->second );
  }


  ATH_MSG_DEBUG ( "Size Summary RPC_FracClusterSize2Map: " << readCdo->getFracClusterSize2Map().size() );
  for (itr=readCdo->getFracClusterSize2Map().begin(); itr!=readCdo->getFracClusterSize2Map().end(); ++itr) {

    ATH_MSG_DEBUG ( "Summary Id/RPC_FracClusterSize2Map: " << itr->first <<" i.e. "
		    <<(itr->first).get_identifier32().get_compact()<<" "
		    <<m_idHelper->show_to_string(itr->first)<< " " << itr->second );
  }


  ATH_MSG_DEBUG ( "Size Summary RPC_DeadStripListMap: " << readCdo->getDeadStripMap().size() );
  for (itrs=readCdo->getDeadStripMap().begin(); itrs!=readCdo->getDeadStripMap().end(); ++itrs) {
    if(itrs->second.size()== 8){  NpanelWith8Strip++; }
    else if(itrs->second.size()==16){ NpanelWith16Strip++; }
    else if(itrs->second.size()==24){ NpanelWith24Strip++; }
    else if(itrs->second.size()==32){ NpanelWith32Strip++; }
    else if(itrs->second.size()==40){ NpanelWith40Strip++; }
    else if(itrs->second.size()==48){ NpanelWith48Strip++; }
    else if(itrs->second.size()==56){ NpanelWith56Strip++; }
    else if(itrs->second.size()==64){ NpanelWith64Strip++; }
    else if(itrs->second.size()==80){ NpanelWith80Strip++; }
    else { NpanelWithXStrip++; ATH_MSG_DEBUG ( "Anomalous "<< itrs->second.size() );}
    ATH_MSG_DEBUG ( "Summary Id/RPC_DeadStripListMap: " << itrs->first << " i.e. " 
		    <<(itrs->first).get_identifier32().get_compact()<<" "
		    <<m_idHelper->show_to_string(itrs->first)<< " " 
		    << itrs->second );
  }


  ATH_MSG_DEBUG ( "Size Summary RPC_FracDeadStripMap: " << readCdo->getFracDeadStripMap().size() );
  for (itr=readCdo->getFracDeadStripMap().begin(); itr!=readCdo->getFracDeadStripMap().end(); ++itr) {
    if( itr->second == 0.   )NpanelFracDead0++       ;
    if( itr->second  < 0.5  )NpanelFracDeadLess05++  ;
    if( itr->second  < 1.   )NpanelFracDeadLess1++   ;
    if( itr->second == 1.   )NpanelFracDead1++       ;

    ATH_MSG_DEBUG ( "Summary Id/RPC_FracDeadStripMap: " << itr->first << " i.e. " 
		    <<(itr->first).get_identifier32().get_compact()<<" "
		    <<m_idHelper->show_to_string(itr->first)<< " " 
		    << itr->second );
    if (itr->second > 0.50) {
      if (itr->second <= 0.75)
	ATH_MSG_DEBUG ( "Summary Id/RPC_FracDeadStripMap: panel with fraction of dead strips >50% and <=75% " << itr->first << " i.e. " 
			<<(itr->first).get_identifier32().get_compact()<<" "
			<<m_idHelper->show_to_string(itr->first)<< " " 
			<< itr->second );
      else if (itr->second > 0.75) {
	if (itr->second <= 0.90)
	  ATH_MSG_DEBUG ( "Summary Id/RPC_FracDeadStripMap: panel with fraction of dead strips >75% and <=90% " << itr->first << " i.e. " 
			  <<(itr->first).get_identifier32().get_compact()<<" "
			  <<m_idHelper->show_to_string(itr->first)<< " " 
			  << itr->second );
	else if (itr->second > 0.90) ATH_MSG_DEBUG ( "Summary Id/RPC_FracDeadStripMap: panel with fraction of dead strips >90% " << itr->first << " i.e. " 
						      <<(itr->first).get_identifier32().get_compact()<<" "
						      <<m_idHelper->show_to_string(itr->first)<< " " 
						      << itr->second );
      }
    }
  }


  ATH_MSG_DEBUG ( "Size Summary RPC_ProjectedTracksMap: " << readCdo->getProjectedTracksMap().size() );
  for (itri=readCdo->getProjectedTracksMap().begin(); itri!=readCdo->getProjectedTracksMap().end(); ++itri) {
    if(itri->second< 500) NpanelTrackLess500++;
    if(itri->second< 100) NpanelTrackLess100++;
    if(itri->second<  50)  NpanelTrackLess50++;
    if(itri->second<  10)  NpanelTrackLess10++;
    if(itri->second==  0)       NpanelTrack0++;

    ATH_MSG_DEBUG ( "Summary Id/RPC_ProjectedTracksMap: " << itri->first << " i.e. " 
		    <<(itri->first).get_identifier32().get_compact()<<" "
		    <<m_idHelper->show_to_string(itri->first)<< " " 
		    << itri->second );
  }
  ATH_MSG_DEBUG ( "Size Summary RPC_DeadStripList: " << readCdo->getDeadStripIntMap().size() );
  for (itri=readCdo->getDeadStripIntMap().begin(); itri!=readCdo->getDeadStripIntMap().end(); ++itri) {
    ATH_MSG_DEBUG ( "Summary Id/RPC_DeadStripList: " << itri->first << " i.e. " 
		    <<(itri->first).get_identifier32().get_compact()<<" "
		    <<m_idHelper->show_to_string(itri->first)<< " " 
		    << itri->second );
  }

  ATH_MSG_INFO ( "********************SUMMARY RPC CONDITIONS********************" )	    ;
  ATH_MSG_INFO ( "# Panel with Eff =0,<0.5,<1,=1: " << NpanelEff0 << " " << NpanelEffLess05 << " " << NpanelEffLess1 << " " << NpanelEff1 )	    ;
  ATH_MSG_INFO ( "# Panel with CS =1,<1.5,<2.0,<2.5,=>5: " << NpanelCSEq1 << " " << NpanelCSLess15 << " " << NpanelCSLess20  << " " <<  NpanelCSLess25 << " " << NpanelCSMore5  )	    ;
  ATH_MSG_INFO ( "# Panel with Fraction of dead strip =0,<0.5,<1,=1: " << NpanelFracDead0 << " " << NpanelFracDeadLess05 << " " << NpanelFracDeadLess1 << " " << NpanelFracDead1 )	    ;
  ATH_MSG_INFO ( "# Panel with 8,16,24,32,40,48,56,64,80,X strips: " << " " <<  NpanelWith8Strip << " " << NpanelWith16Strip << " " << NpanelWith24Strip << " " << NpanelWith32Strip <<" " <<
		 NpanelWith40Strip <<" " << NpanelWith48Strip <<" "<< NpanelWith56Strip <<" " <<NpanelWith64Strip << " " << NpanelWith80Strip << " " << NpanelWithXStrip )	    ;
  ATH_MSG_INFO ( "# Panel with Extrapolated Tracks =0,<10,<50,<100,<500: " << NpanelTrack0  << " " << NpanelTrackLess10  << " " << NpanelTrackLess50 << " " << NpanelTrackLess100 << " " <<  NpanelTrackLess500 )	    ;
  ATH_MSG_INFO ( "********************SUMMARY RPC CONDITIONS********************" )	    ;

  //Evaluate mean values from COOLDB

  std::vector<float> COOLDB_PhiAndEtaEff_A(7,0.0);
  std::vector<float> COOLDB_OnlyPhiEff_A  (7,0.0);
  std::vector<float> COOLDB_OnlyEtaEff_A  (7,0.0);
  std::vector<int>   COOLDB_CountEff_A      (7,0);

  std::vector<float> COOLDB_PhiAndEtaEff_C(7,0.0);
  std::vector<float> COOLDB_OnlyPhiEff_C  (7,0.0);
  std::vector<float> COOLDB_OnlyEtaEff_C  (7,0.0);
  std::vector<int>   COOLDB_CountEff_C      (7,0);

  std::vector<double> COOLDB_FracClusterSize1_A   (14,0.0);
  std::vector<double> COOLDB_FracClusterSize2_A   (14,0.0);
  std::vector<double> COOLDB_FracClusterSizeTail_A(14,0.0);
  std::vector<double> COOLDB_MeanClusterSizeTail_A(14,0.0);
  std::vector<int>    COOLDB_CountCS_A               (7,0);

  std::vector<double> COOLDB_FracClusterSize1_C   (14,0.0);
  std::vector<double> COOLDB_FracClusterSize2_C   (14,0.0);
  std::vector<double> COOLDB_FracClusterSizeTail_C(14,0.0);
  std::vector<double> COOLDB_MeanClusterSizeTail_C(14,0.0);
  std::vector<int>    COOLDB_CountCS_C               (7,0);

  std::vector<float> COOLDB_GapEff_A  (7,0.0);
  std::vector<float> COOLDB_PhiEff_A  (7,0.0);
  std::vector<float> COOLDB_EtaEff_A  (7,0.0);

  std::vector<float> COOLDB_GapEff_C  (7,0.0);
  std::vector<float> COOLDB_PhiEff_C  (7,0.0);
  std::vector<float> COOLDB_EtaEff_C  (7,0.0);

  std::vector<double> COOLDB_MeanClusterSize_A(14,0.0);
  std::vector<double> COOLDB_MeanClusterSize_C(14,0.0);


  int indexSName  = 0;
  int countGasGap = 0;

  for( int stationName =  2 ;  stationName !=11;  stationName++){
    for( int stationEta  = -7 ;  stationEta  != 8;  stationEta++ ){
      for( int stationPhi  =  1 ;  stationPhi  != 9;  stationPhi++ ){
	for( int doubletR    =  1 ;  doubletR    != 3;  doubletR++   ){
	  for( int doubletZ    =  1 ;  doubletZ    != 4;  doubletZ++   ){
	    for( int doubletPhi  =  1 ;  doubletPhi  != 3;  doubletPhi++ ){
	      for( int gasGap      =  1 ;  gasGap	  != 3;  gasGap++     ){

    bool isValid=false;
    Identifier rpcId = m_idHelper->channelID(stationName, stationEta, stationPhi, doubletR, doubletZ, doubletPhi, 1, 1, 1, true, &isValid); // last 5 arguments are: int doubletPhi, int gasGap, int measuresPhi, int strip, bool check, bool* isValid
    if (!isValid) continue;
		const RpcReadoutElement* rpc = m_GMmgr->getRpcReadoutElement(rpcId);
		if(rpc == 0 )continue;
		Identifier idr = rpc->identify();
		if(idr == 0 )continue;
		Identifier atlasIdEta = m_idHelper->channelID(idr, doubletZ,doubletPhi , gasGap, 0, 1)     ;
		if(atlasIdEta == 0 )continue;
		Identifier atlasIdPhi = m_idHelper->channelID(idr, doubletZ,doubletPhi , gasGap, 1, 1)     ;
		if(atlasIdPhi == 0 )continue;

		countGasGap++;

		indexSName = stationName - 2 ;
		if(indexSName>3) indexSName = indexSName - 2 ;

		float efficiencyEta     = 0.  ;
		float averageCSEta	     = 0.  ;
		float FracCS1Eta	     = 0.  ;
		float FracCS2Eta	     = 0.  ;
		float FracCStailEta     = 0.  ;

		int   ProjectedTracksEta=  0  ;
		float efficiencygapEta  =  0. ;

		float efficiencyPhi     =  0. ;
		float averageCSPhi	     =  0. ;
		float FracCS1Phi	     =  0. ;
		float FracCS2Phi	     =  0. ;
		float FracCStailPhi     =  0. ;



		if( readCdo->getEfficiencyMap            ().find(atlasIdEta)!= readCdo->getEfficiencyMap().end()       ) efficiencyEta     = readCdo->getEfficiencyMap      ().find(atlasIdEta)->second ;
		if( readCdo->getMeanClusterSizeMap       ().find(atlasIdEta)!= readCdo->getMeanClusterSizeMap().end()  ) averageCSEta      = readCdo->getMeanClusterSizeMap ().find(atlasIdEta)->second ;
		if( readCdo->getFracClusterSize1Map      ().find(atlasIdEta)!= readCdo->getFracClusterSize1Map().end() ) FracCS1Eta        = readCdo->getFracClusterSize1Map().find(atlasIdEta)->second ;
		if( readCdo->getFracClusterSize2Map      ().find(atlasIdEta)!= readCdo->getFracClusterSize2Map().end() ) FracCS2Eta        = readCdo->getFracClusterSize2Map().find(atlasIdEta)->second ;
		FracCStailEta     = 1.-FracCS1Eta-FracCS2Eta ;
		if( readCdo->getProjectedTracksMap       ().find(atlasIdEta)!= readCdo->getProjectedTracksMap().end()  ) ProjectedTracksEta= readCdo->getProjectedTracksMap ().find(atlasIdEta)->second ;
		if( readCdo->getEfficiencyGapMap         ().find(atlasIdEta)!= readCdo->getEfficiencyGapMap().end()    ) efficiencygapEta  = readCdo->getEfficiencyGapMap   ().find(atlasIdEta)->second ;

		if( readCdo->getEfficiencyMap            ().find(atlasIdPhi)!= readCdo->getEfficiencyMap().end()       ) efficiencyPhi     = readCdo->getEfficiencyMap      ().find(atlasIdPhi)->second ;
		if( readCdo->getMeanClusterSizeMap       ().find(atlasIdPhi)!= readCdo->getMeanClusterSizeMap().end()  ) averageCSPhi      = readCdo->getMeanClusterSizeMap ().find(atlasIdPhi)->second ;
		if( readCdo->getFracClusterSize1Map      ().find(atlasIdPhi)!= readCdo->getFracClusterSize1Map().end() ) FracCS1Phi        = readCdo->getFracClusterSize1Map().find(atlasIdPhi)->second ;
		if( readCdo->getFracClusterSize2Map      ().find(atlasIdPhi)!= readCdo->getFracClusterSize2Map().end() ) FracCS2Phi        = readCdo->getFracClusterSize2Map().find(atlasIdPhi)->second ;
		FracCStailPhi     	= 1.-FracCS1Phi-FracCS2Phi ;

		if((ProjectedTracksEta>=m_CutProjectedTracks)&&(ProjectedTracksEta<10000000)&&(ProjectedTracksEta>=m_CutProjectedTracks)&&(efficiencyEta>0)&&(efficiencyEta<=1)&&(efficiencygapEta>0)&&(efficiencygapEta<=1)&&(efficiencyPhi>0)&&(efficiencyPhi<=1)){

		  if( stationEta >=  0){
		    COOLDB_PhiAndEtaEff_A[indexSName] = COOLDB_PhiAndEtaEff_A[indexSName]  + efficiencyEta +  efficiencyPhi  -  efficiencygapEta ;
		    COOLDB_GapEff_A      [indexSName] = COOLDB_GapEff_A[indexSName]    + efficiencygapEta ;
		    COOLDB_PhiEff_A      [indexSName] = COOLDB_PhiEff_A  [indexSName]  +    efficiencyPhi ;
		    COOLDB_EtaEff_A      [indexSName] = COOLDB_EtaEff_A  [indexSName]  +    efficiencyEta ;
		    COOLDB_CountEff_A    [indexSName] = COOLDB_CountEff_A    [indexSName]  +            1 ;

		  }
		  else{
		    COOLDB_PhiAndEtaEff_C[indexSName] = COOLDB_PhiAndEtaEff_C[indexSName]  + efficiencyEta +  efficiencyPhi  -  efficiencygapEta ;
		    COOLDB_GapEff_C	 [indexSName] = COOLDB_GapEff_C[indexSName]	 + efficiencygapEta ;
		    COOLDB_PhiEff_C	 [indexSName] = COOLDB_PhiEff_C  [indexSName]  +    efficiencyPhi ;
		    COOLDB_EtaEff_C	 [indexSName] = COOLDB_EtaEff_C  [indexSName]  +    efficiencyEta ;
		    COOLDB_CountEff_C	 [indexSName] = COOLDB_CountEff_C    [indexSName]  +  	  1 ;

		  }
		}

		if((ProjectedTracksEta>=m_CutProjectedTracks)&&(ProjectedTracksEta<10000000)&&(averageCSEta>1)&&(averageCSPhi>1)&&(averageCSEta<=m_CutMaxClusterSize)&&(averageCSPhi<=m_CutMaxClusterSize)){
		  if((FracCStailEta>=0)&&(FracCS1Eta>=0)&&(FracCS2Eta>=0)&&(FracCStailEta<=1)&&(FracCS1Eta<=1)&&(FracCS2Eta<=1)&&(FracCStailPhi>=0)&&(FracCS1Phi>=0)&&(FracCS2Phi>=0)&&(FracCStailPhi<=1)&&(FracCS1Phi<=1)&&(FracCS2Phi<=1)){
		    if( stationEta >=  0){

		      COOLDB_FracClusterSize1_A   [indexSName] = COOLDB_FracClusterSize1_A   [indexSName] +  FracCS1Eta   ;
		      COOLDB_FracClusterSize2_A   [indexSName] = COOLDB_FracClusterSize2_A   [indexSName] +  FracCS2Eta   ;
		      COOLDB_MeanClusterSize_A    [indexSName] = COOLDB_MeanClusterSize_A    [indexSName] +  averageCSEta ;
		      COOLDB_CountCS_A	        [indexSName] = COOLDB_CountCS_A	     [indexSName] +             1 ;

		      COOLDB_FracClusterSize1_A   [indexSName+7] = COOLDB_FracClusterSize1_A   [indexSName+7] +  FracCS1Phi   ;
		      COOLDB_FracClusterSize2_A   [indexSName+7] = COOLDB_FracClusterSize2_A   [indexSName+7] +  FracCS2Phi   ;
		      COOLDB_MeanClusterSize_A    [indexSName+7] = COOLDB_MeanClusterSize_A    [indexSName+7] +  averageCSPhi ;

		    }
		    else{

		      COOLDB_FracClusterSize1_C   [indexSName] = COOLDB_FracClusterSize1_C   [indexSName] +  FracCS1Eta   ;
		      COOLDB_FracClusterSize2_C   [indexSName] = COOLDB_FracClusterSize2_C   [indexSName] +  FracCS2Eta   ;
		      COOLDB_MeanClusterSize_C    [indexSName] = COOLDB_MeanClusterSize_C    [indexSName] +  averageCSEta ;
		      COOLDB_CountCS_C	        [indexSName] = COOLDB_CountCS_C	     [indexSName] +             1 ;

		      COOLDB_FracClusterSize1_C   [indexSName+7] = COOLDB_FracClusterSize1_C   [indexSName+7] +  FracCS1Phi   ;
		      COOLDB_FracClusterSize2_C   [indexSName+7] = COOLDB_FracClusterSize2_C   [indexSName+7] +  FracCS2Phi   ;
		      COOLDB_MeanClusterSize_C    [indexSName+7] = COOLDB_MeanClusterSize_C    [indexSName+7] +  averageCSPhi ;

		    }
		  }}
	      }}}}}}}

  for( unsigned int index =  0 ;  index != COOLDB_PhiAndEtaEff_A.size() + 1; index++){
    int Count = COOLDB_CountEff_A [index] ;

    if(Count){
      COOLDB_PhiAndEtaEff_A[index]          = COOLDB_PhiAndEtaEff_A[index]  / Count ;
      COOLDB_GapEff_A      [index]          = COOLDB_GapEff_A      [index]  / Count ;
      COOLDB_EtaEff_A      [index]	   = COOLDB_EtaEff_A      [index]  / Count ;
      COOLDB_PhiEff_A      [index]	   = COOLDB_PhiEff_A      [index]  / Count ;
      COOLDB_OnlyPhiEff_A  [index]          = COOLDB_PhiEff_A      [index]  -  COOLDB_PhiAndEtaEff_A[index] ;
      COOLDB_OnlyEtaEff_A  [index]          = COOLDB_EtaEff_A      [index]  -  COOLDB_PhiAndEtaEff_A[index] ;
    }

    Count = COOLDB_CountEff_C [index] ;
    if(Count){
      COOLDB_PhiAndEtaEff_C[index]          = COOLDB_PhiAndEtaEff_C[index]  / Count ;
      COOLDB_GapEff_C      [index]          = COOLDB_GapEff_C      [index]  / Count ;
      COOLDB_EtaEff_C      [index]	   = COOLDB_EtaEff_C      [index]  / Count ;
      COOLDB_PhiEff_C      [index]	   = COOLDB_PhiEff_C      [index]  / Count ;
      COOLDB_OnlyPhiEff_C  [index]          = COOLDB_PhiEff_C      [index]  -  COOLDB_PhiAndEtaEff_C[index] ;
      COOLDB_OnlyEtaEff_C  [index]          = COOLDB_EtaEff_C      [index]  -  COOLDB_PhiAndEtaEff_C[index] ;
    }

    Count =  COOLDB_CountCS_A [index] ;
    if(Count){
      COOLDB_FracClusterSize1_A   [index]   = COOLDB_FracClusterSize1_A   [index] / Count	 ;
      COOLDB_FracClusterSize2_A   [index]   = COOLDB_FracClusterSize2_A   [index] / Count 	 ;
      COOLDB_MeanClusterSize_A    [index]   = COOLDB_MeanClusterSize_A    [index] / Count       ;
      COOLDB_MeanClusterSizeTail_A[index]   = COOLDB_MeanClusterSize_A[index]  -  COOLDB_FracClusterSize1_A   [index] - 2*COOLDB_FracClusterSize2_A   [index] ;
      COOLDB_FracClusterSizeTail_A[index]   = 1.  -  COOLDB_FracClusterSize1_A   [index] - COOLDB_FracClusterSize2_A   [index] ;
      COOLDB_FracClusterSize1_A   [index+7]   = COOLDB_FracClusterSize1_A   [index+7] / Count	 ;
      COOLDB_FracClusterSize2_A   [index+7]   = COOLDB_FracClusterSize2_A   [index+7] / Count 	 ;
      COOLDB_MeanClusterSize_A    [index+7]   = COOLDB_MeanClusterSize_A    [index+7] / Count       ;
      COOLDB_MeanClusterSizeTail_A[index+7]   = COOLDB_MeanClusterSize_A[index+7]  -  COOLDB_FracClusterSize1_A   [index+7] - 2*COOLDB_FracClusterSize2_A   [index+7] ;
      COOLDB_FracClusterSizeTail_A[index+7]   = 1.  -  COOLDB_FracClusterSize1_A   [index+7] - COOLDB_FracClusterSize2_A   [index+7] ;
    }

    Count =  COOLDB_CountCS_C [index] ;
    if(Count){
      COOLDB_FracClusterSize1_C   [index]   = COOLDB_FracClusterSize1_C   [index] / Count	 ;
      COOLDB_FracClusterSize2_C   [index]   = COOLDB_FracClusterSize2_C   [index] / Count 	 ;
      COOLDB_MeanClusterSize_C    [index]   = COOLDB_MeanClusterSize_C    [index] / Count       ;
      COOLDB_MeanClusterSizeTail_C[index]   = COOLDB_MeanClusterSize_C[index]  -  COOLDB_FracClusterSize1_C   [index] - 2*COOLDB_FracClusterSize2_C   [index] ;
      COOLDB_FracClusterSizeTail_C[index]   = 1.  -  COOLDB_FracClusterSize1_C   [index] - COOLDB_FracClusterSize2_C   [index] ;
      COOLDB_FracClusterSize1_C   [index+7]   = COOLDB_FracClusterSize1_C   [index+7] / Count	 ;
      COOLDB_FracClusterSize2_C   [index+7]   = COOLDB_FracClusterSize2_C   [index+7] / Count 	 ;
      COOLDB_MeanClusterSize_C    [index+7]   = COOLDB_MeanClusterSize_C    [index+7] / Count       ;
      COOLDB_MeanClusterSizeTail_C[index+7]   = COOLDB_MeanClusterSize_C[index+7]  -  COOLDB_FracClusterSize1_C   [index+7] - 2*COOLDB_FracClusterSize2_C   [index+7] ;
      COOLDB_FracClusterSizeTail_C[index+7]   = 1.  -  COOLDB_FracClusterSize1_C   [index+7] - COOLDB_FracClusterSize2_C   [index+7] ;
    }

  }

  //Dump average COOL for Joboptions Input
  ATH_MSG_VERBOSE( "Total Number of GasGap and Readout Panel "<<countGasGap<<" "<< 2*countGasGap);
  ATH_MSG_VERBOSE( "RpcDigitizationTool.PhiAndEtaEff_A=["<<COOLDB_PhiAndEtaEff_A[0]<<","<<COOLDB_PhiAndEtaEff_A[1]<<","<<COOLDB_PhiAndEtaEff_A[2]<<","<<COOLDB_PhiAndEtaEff_A[3]<<","<<COOLDB_PhiAndEtaEff_A[4]<<","<<COOLDB_PhiAndEtaEff_A[5]<<","<< COOLDB_PhiAndEtaEff_A[6]<<"]");
  ATH_MSG_VERBOSE( "RpcDigitizationTool.OnlyPhiEff_A=["<<COOLDB_OnlyPhiEff_A[0]<<","<<COOLDB_OnlyPhiEff_A[1]<<","<<COOLDB_OnlyPhiEff_A[2]<<","<<COOLDB_OnlyPhiEff_A[3]<<","<<COOLDB_OnlyPhiEff_A[4]<<","<<COOLDB_OnlyPhiEff_A[5]<<","<< COOLDB_OnlyPhiEff_A[6]<<"]");
  ATH_MSG_VERBOSE( "RpcDigitizationTool.OnlyEtaEff_A=["<<COOLDB_OnlyEtaEff_A[0]<<","<<COOLDB_OnlyEtaEff_A[1]<<","<<COOLDB_OnlyEtaEff_A[2]<<","<<COOLDB_OnlyEtaEff_A[3]<<","<<COOLDB_OnlyEtaEff_A[4]<<","<<COOLDB_OnlyEtaEff_A[5]<<","<< COOLDB_OnlyEtaEff_A[6]<<"]");
  ATH_MSG_VERBOSE( " ");
  ATH_MSG_VERBOSE( "RpcDigitizationTool.PhiAndEtaEff_C=["<<COOLDB_PhiAndEtaEff_C[0]<<","<<COOLDB_PhiAndEtaEff_C[1]<<","<<COOLDB_PhiAndEtaEff_C[2]<<","<<COOLDB_PhiAndEtaEff_C[3]<<","<<COOLDB_PhiAndEtaEff_C[4]<<","<<COOLDB_PhiAndEtaEff_C[5]<<","<< COOLDB_PhiAndEtaEff_C[6]<<"]");
  ATH_MSG_VERBOSE( "RpcDigitizationTool.OnlyPhiEff_C=["<<COOLDB_OnlyPhiEff_C[0]<<","<<COOLDB_OnlyPhiEff_C[1]<<","<<COOLDB_OnlyPhiEff_C[2]<<","<<COOLDB_OnlyPhiEff_C[3]<<","<<COOLDB_OnlyPhiEff_C[4]<<","<<COOLDB_OnlyPhiEff_C[5]<<","<< COOLDB_OnlyPhiEff_C[6]<<"]");
  ATH_MSG_VERBOSE( "RpcDigitizationTool.OnlyEtaEff_C=["<<COOLDB_OnlyEtaEff_C[0]<<","<<COOLDB_OnlyEtaEff_C[1]<<","<<COOLDB_OnlyEtaEff_C[2]<<","<<COOLDB_OnlyEtaEff_C[3]<<","<<COOLDB_OnlyEtaEff_C[4]<<","<<COOLDB_OnlyEtaEff_C[5]<<","<< COOLDB_OnlyEtaEff_C[6]<<"]");
  ATH_MSG_VERBOSE( " ");
  ATH_MSG_VERBOSE( "RpcDigitizationTool.FracClusterSize1_A=["<<COOLDB_FracClusterSize1_A[0]<<","<<COOLDB_FracClusterSize1_A[1]<<","<<COOLDB_FracClusterSize1_A[2]<<","<<COOLDB_FracClusterSize1_A[3]<<","<<COOLDB_FracClusterSize1_A[4]<<","<<COOLDB_FracClusterSize1_A[5]<<","<< COOLDB_FracClusterSize1_A[6]);
  ATH_MSG_VERBOSE( ","<<COOLDB_FracClusterSize1_A[7]<<","<<COOLDB_FracClusterSize1_A[8]<<","<<COOLDB_FracClusterSize1_A[9]<<","<<COOLDB_FracClusterSize1_A[10]<<","<<COOLDB_FracClusterSize1_A[11]<<","<<COOLDB_FracClusterSize1_A[12]<<","<< COOLDB_FracClusterSize1_A[13]<<"]");
  ATH_MSG_VERBOSE( "RpcDigitizationTool.FracClusterSize2_A=["<<COOLDB_FracClusterSize2_A[0]<<","<<COOLDB_FracClusterSize2_A[1]<<","<<COOLDB_FracClusterSize2_A[2]<<","<<COOLDB_FracClusterSize2_A[3]<<","<<COOLDB_FracClusterSize2_A[4]<<","<<COOLDB_FracClusterSize2_A[5]<<","<< COOLDB_FracClusterSize2_A[6]);
  ATH_MSG_VERBOSE( ","<<COOLDB_FracClusterSize2_A[7]<<","<<COOLDB_FracClusterSize2_A[8]<<","<<COOLDB_FracClusterSize2_A[9]<<","<<COOLDB_FracClusterSize2_A[10]<<","<<COOLDB_FracClusterSize2_A[11]<<","<<COOLDB_FracClusterSize2_A[12]<<","<< COOLDB_FracClusterSize2_A[13]<<"]");
  ATH_MSG_VERBOSE( "RpcDigitizationTool.FracClusterSizeTail_A=["<<COOLDB_FracClusterSizeTail_A[0]<<","<<COOLDB_FracClusterSizeTail_A[1]<<","<<COOLDB_FracClusterSizeTail_A[2]<<","<<COOLDB_FracClusterSizeTail_A[3]<<","<<COOLDB_FracClusterSizeTail_A[4]<<","<<COOLDB_FracClusterSizeTail_A[5]<<","<< COOLDB_FracClusterSizeTail_A[6]);
  ATH_MSG_VERBOSE( ","<<COOLDB_FracClusterSizeTail_A[7]<<","<<COOLDB_FracClusterSizeTail_A[8]<<","<<COOLDB_FracClusterSizeTail_A[9]<<","<<COOLDB_FracClusterSizeTail_A[10]<<","<<COOLDB_FracClusterSizeTail_A[11]<<","<<COOLDB_FracClusterSizeTail_A[12]<<","<< COOLDB_FracClusterSizeTail_A[13]<<"]");
  ATH_MSG_VERBOSE( "RpcDigitizationTool.MeanClusterSizeTail_A=["<<COOLDB_MeanClusterSizeTail_A[0]<<","<<COOLDB_MeanClusterSizeTail_A[1]<<","<<COOLDB_MeanClusterSizeTail_A[2]<<","<<COOLDB_MeanClusterSizeTail_A[3]<<","<<COOLDB_MeanClusterSizeTail_A[4]<<","<<COOLDB_MeanClusterSizeTail_A[5]<<","<< COOLDB_MeanClusterSizeTail_A[6]);
  ATH_MSG_VERBOSE( ","<<COOLDB_MeanClusterSizeTail_A[7]<<","<<COOLDB_MeanClusterSizeTail_A[8]<<","<<COOLDB_MeanClusterSizeTail_A[9]<<","<<COOLDB_MeanClusterSizeTail_A[10]<<","<<COOLDB_MeanClusterSizeTail_A[11]<<","<<COOLDB_MeanClusterSizeTail_A[12]<<","<< COOLDB_MeanClusterSizeTail_A[13]<<"]");
  ATH_MSG_VERBOSE( " ");
  ATH_MSG_VERBOSE( "RpcDigitizationTool.FracClusterSize1_C=["<<COOLDB_FracClusterSize1_C[0]<<","<<COOLDB_FracClusterSize1_C[1]<<","<<COOLDB_FracClusterSize1_C[2]<<","<<COOLDB_FracClusterSize1_C[3]<<","<<COOLDB_FracClusterSize1_C[4]<<","<<COOLDB_FracClusterSize1_C[5]<<","<< COOLDB_FracClusterSize1_C[6]);
  ATH_MSG_VERBOSE( ","<<COOLDB_FracClusterSize1_C[7]<<","<<COOLDB_FracClusterSize1_C[8]<<","<<COOLDB_FracClusterSize1_C[9]<<","<<COOLDB_FracClusterSize1_C[10]<<","<<COOLDB_FracClusterSize1_C[11]<<","<<COOLDB_FracClusterSize1_C[12]<<","<< COOLDB_FracClusterSize1_C[13]<<"]");
  ATH_MSG_VERBOSE( "RpcDigitizationTool.FracClusterSize2_C=["<<COOLDB_FracClusterSize2_C[0]<<","<<COOLDB_FracClusterSize2_C[1]<<","<<COOLDB_FracClusterSize2_C[2]<<","<<COOLDB_FracClusterSize2_C[3]<<","<<COOLDB_FracClusterSize2_C[4]<<","<<COOLDB_FracClusterSize2_C[5]<<","<< COOLDB_FracClusterSize2_C[6]);
  ATH_MSG_VERBOSE( ","<<COOLDB_FracClusterSize2_C[7]<<","<<COOLDB_FracClusterSize2_C[8]<<","<<COOLDB_FracClusterSize2_C[9]<<","<<COOLDB_FracClusterSize2_C[10]<<","<<COOLDB_FracClusterSize2_C[11]<<","<<COOLDB_FracClusterSize2_C[12]<<","<< COOLDB_FracClusterSize2_C[13]<<"]");
  ATH_MSG_VERBOSE( "RpcDigitizationTool.FracClusterSizeTail_C=["<<COOLDB_FracClusterSizeTail_C[0]<<","<<COOLDB_FracClusterSizeTail_C[1]<<","<<COOLDB_FracClusterSizeTail_C[2]<<","<<COOLDB_FracClusterSizeTail_C[3]<<","<<COOLDB_FracClusterSizeTail_C[4]<<","<<COOLDB_FracClusterSizeTail_C[5]<<","<< COOLDB_FracClusterSizeTail_C[6]);
  ATH_MSG_VERBOSE( ","<<COOLDB_FracClusterSizeTail_C[7]<<","<<COOLDB_FracClusterSizeTail_C[8]<<","<<COOLDB_FracClusterSizeTail_C[9]<<","<<COOLDB_FracClusterSizeTail_C[10]<<","<<COOLDB_FracClusterSizeTail_C[11]<<","<<COOLDB_FracClusterSizeTail_C[12]<<","<< COOLDB_FracClusterSizeTail_C[13]<<"]");
  ATH_MSG_VERBOSE( "RpcDigitizationTool.MeanClusterSizeTail_C=["<<COOLDB_MeanClusterSizeTail_C[0]<<","<<COOLDB_MeanClusterSizeTail_C[1]<<","<<COOLDB_MeanClusterSizeTail_C[2]<<","<<COOLDB_MeanClusterSizeTail_C[3]<<","<<COOLDB_MeanClusterSizeTail_C[4]<<","<<COOLDB_MeanClusterSizeTail_C[5]<<","<< COOLDB_MeanClusterSizeTail_C[6]);
  ATH_MSG_VERBOSE( ","<<COOLDB_MeanClusterSizeTail_C[7]<<","<<COOLDB_MeanClusterSizeTail_C[8]<<","<<COOLDB_MeanClusterSizeTail_C[9]<<","<<COOLDB_MeanClusterSizeTail_C[10]<<","<<COOLDB_MeanClusterSizeTail_C[11]<<","<<COOLDB_MeanClusterSizeTail_C[12]<<","<< COOLDB_MeanClusterSizeTail_C[13]<<"]");
  ATH_MSG_VERBOSE( " ");
  ATH_MSG_VERBOSE( "RpcDigitizationTool.GapEff_A=["<<COOLDB_GapEff_A[0]<<","<<COOLDB_GapEff_A[1]<<","<<COOLDB_GapEff_A[2]<<","<<COOLDB_GapEff_A[3]<<","<<COOLDB_GapEff_A[4]<<","<<COOLDB_GapEff_A[5]<<","<< COOLDB_GapEff_A[6]<<"]");
  ATH_MSG_VERBOSE( "RpcDigitizationTool.PhiEff_A=["<<COOLDB_PhiEff_A[0]<<","<<COOLDB_PhiEff_A[1]<<","<<COOLDB_PhiEff_A[2]<<","<<COOLDB_PhiEff_A[3]<<","<<COOLDB_PhiEff_A[4]<<","<<COOLDB_PhiEff_A[5]<<","<< COOLDB_PhiEff_A[6]<<"]");
  ATH_MSG_VERBOSE( "RpcDigitizationTool.EtaEff_A=["<<COOLDB_EtaEff_A[0]<<","<<COOLDB_EtaEff_A[1]<<","<<COOLDB_EtaEff_A[2]<<","<<COOLDB_EtaEff_A[3]<<","<<COOLDB_EtaEff_A[4]<<","<<COOLDB_EtaEff_A[5]<<","<< COOLDB_EtaEff_A[6]<<"]");
  ATH_MSG_VERBOSE( " ");
  ATH_MSG_VERBOSE( "RpcDigitizationTool.GapEff_C=["<<COOLDB_GapEff_C[0]<<","<<COOLDB_GapEff_C[1]<<","<<COOLDB_GapEff_C[2]<<","<<COOLDB_GapEff_C[3]<<","<<COOLDB_GapEff_C[4]<<","<<COOLDB_GapEff_C[5]<<","<< COOLDB_GapEff_C[6]<<"]");
  ATH_MSG_VERBOSE( "RpcDigitizationTool.PhiEff_C=["<<COOLDB_PhiEff_C[0]<<","<<COOLDB_PhiEff_C[1]<<","<<COOLDB_PhiEff_C[2]<<","<<COOLDB_PhiEff_C[3]<<","<<COOLDB_PhiEff_C[4]<<","<<COOLDB_PhiEff_C[5]<<","<< COOLDB_PhiEff_C[6]<<"]");
  ATH_MSG_VERBOSE( "RpcDigitizationTool.EtaEff_C=["<<COOLDB_EtaEff_C[0]<<","<<COOLDB_EtaEff_C[1]<<","<<COOLDB_EtaEff_C[2]<<","<<COOLDB_EtaEff_C[3]<<","<<COOLDB_EtaEff_C[4]<<","<<COOLDB_EtaEff_C[5]<<","<< COOLDB_EtaEff_C[6]<<"]");
  ATH_MSG_VERBOSE( " ");
  ATH_MSG_VERBOSE( "RpcDigitizationTool.MeanClusterSize_A=["<<COOLDB_MeanClusterSize_A[0]<<","<<COOLDB_MeanClusterSize_A[1]<<","<<COOLDB_MeanClusterSize_A[2]<<","<<COOLDB_MeanClusterSize_A[3]<<","<<COOLDB_MeanClusterSize_A[4]<<","<<COOLDB_MeanClusterSize_A[5]<<","<< COOLDB_MeanClusterSize_A[6]);
  ATH_MSG_VERBOSE( ","<<COOLDB_MeanClusterSize_A[7]<<","<<COOLDB_MeanClusterSize_A[8]<<","<<COOLDB_MeanClusterSize_A[9]<<","<<COOLDB_MeanClusterSize_A[10]<<","<<COOLDB_MeanClusterSize_A[11]<<","<<COOLDB_MeanClusterSize_A[12]<<","<< COOLDB_MeanClusterSize_A[13]<<"]");
  ATH_MSG_VERBOSE( " ");
  ATH_MSG_VERBOSE( "RpcDigitizationTool.MeanClusterSize_C=["<<COOLDB_MeanClusterSize_C[0]<<","<<COOLDB_MeanClusterSize_C[1]<<","<<COOLDB_MeanClusterSize_C[2]<<","<<COOLDB_MeanClusterSize_C[3]<<","<<COOLDB_MeanClusterSize_C[4]<<","<<COOLDB_MeanClusterSize_C[5]<<","<< COOLDB_MeanClusterSize_C[6]);
  ATH_MSG_VERBOSE( ","<<COOLDB_MeanClusterSize_C[7]<<","<<COOLDB_MeanClusterSize_C[8]<<","<<COOLDB_MeanClusterSize_C[9]<<","<<COOLDB_MeanClusterSize_C[10]<<","<<COOLDB_MeanClusterSize_C[11]<<","<<COOLDB_MeanClusterSize_C[12]<<","<< COOLDB_MeanClusterSize_C[13]<<"]");



  for( int stationName =  2 ;  stationName !=11;  stationName++){
    for( int stationEta  = -7 ;  stationEta  != 8;  stationEta++ ){
      for( int stationPhi  =  1 ;  stationPhi  != 9;  stationPhi++ ){
	for( int doubletR    =  1 ;  doubletR    != 3;  doubletR++   ){
	  for( int doubletZ    =  1 ;  doubletZ    != 4;  doubletZ++   ){
	    for( int doubletPhi  =  1 ;  doubletPhi  != 3;  doubletPhi++ ){
	      for( int gasGap      =  1 ;  gasGap	  != 3;  gasGap++     ){
		for( int measphi     =  0 ;  measphi	  != 2;  measphi++    ){

      bool isValid=false;
      Identifier rpcId = m_idHelper->channelID(stationName, stationEta, stationPhi, doubletR, doubletZ, doubletPhi, 1, 1, 1, true, &isValid); // last 5 arguments are: int doubletPhi, int gasGap, int measuresPhi, int strip, bool check, bool* isValid
      if (!isValid) continue;
		  const RpcReadoutElement* rpc = m_GMmgr->getRpcReadoutElement(rpcId);
		  if(!rpc)continue;
		  Identifier idr = rpc->identify();
		  if(idr == 0 )continue;
		  Identifier atlasId = m_idHelper->channelID(idr, doubletZ,doubletPhi , gasGap, measphi, 1)     ;
		  if(atlasId == 0 )continue;

		  if(readCdo->getEfficiencyMap().find(atlasId)->second == 1){
		    ATH_MSG_VERBOSE ( "Effi RPC panel = 1: " <<readCdo->getDeadStripMap().find(atlasId)->second<<" "<<readCdo->getProjectedTracksMap().find(atlasId)->second<<" sName "<<stationName<<" sEta " <<stationEta<<" sPhi "<<stationPhi<<" dR "<<doubletR<<" dZ "<<doubletZ<<" dPhi "<<doubletPhi<<" Gap "<<gasGap<<" view "<<measphi);
		  }
		  if(readCdo->getEfficiencyMap().find(atlasId)->second == 0){
		    ATH_MSG_VERBOSE ( "Effi RPC panel = 0: " <<readCdo->getDeadStripMap().find(atlasId)->second<<" "<<readCdo->getProjectedTracksMap().find(atlasId)->second<<" sName "<<stationName<<" sEta " <<stationEta<<" sPhi "<<stationPhi<<" dR "<<doubletR<<" dZ "<<doubletZ<<" dPhi "<<doubletPhi<<" Gap "<<gasGap<<" view "<<measphi);
		  }
		  if(readCdo->getEfficiencyGapMap().find(atlasId)->second == 0){
		    ATH_MSG_VERBOSE ( "EffiGap RPC panel = 0: " <<readCdo->getDeadStripMap().find(atlasId)->second<<" "<<readCdo->getProjectedTracksMap().find(atlasId)->second<<" sName "<<stationName<<" sEta " <<stationEta<<" sPhi "<<stationPhi<<" dR "<<doubletR<<" dZ "<<doubletZ<<" dPhi "<<doubletPhi<<" Gap "<<gasGap<<" view "<<measphi);
		  }
		  if(readCdo->getMeanClusterSizeMap().find(atlasId)->second > 3 ){
		    ATH_MSG_VERBOSE ( "MeanClusterSize RPC panel > 3: " <<readCdo->getMeanClusterSizeMap().find(atlasId)->second<<" "<<readCdo->getProjectedTracksMap().find(atlasId)->second<<" sName "<<stationName<<" sEta " <<stationEta<<" sPhi "<<stationPhi<<" dR "<<doubletR<<" dZ "<<doubletZ<<" dPhi "<<doubletPhi<<" Gap "<<gasGap<<" view "<<measphi);
		  }
		  if(readCdo->getFracDeadStripMap().find(atlasId)->second == 1){
		    ATH_MSG_VERBOSE ( "Dead RPC panel: " <<readCdo->getDeadStripMap().find(atlasId)->second<<" "<<readCdo->getProjectedTracksMap().find(atlasId)->second<<" sName "<<stationName<<" sEta " <<stationEta<<" sPhi "<<stationPhi<<" dR "<<doubletR<<" dZ "<<doubletZ<<" dPhi "<<doubletPhi<<" Gap "<<gasGap<<" view "<<measphi);
		  }

		  if(m_DeadPanel_fromlist.find(atlasId)->second == 1){
		    ATH_MSG_VERBOSE ( atlasId <<" Dead RPC panel from file list: " <<" sName "<<stationName<<" sEta " <<stationEta<<" sPhi "<<stationPhi<<" dR "<<doubletR<<" dZ "<<doubletZ<<" dPhi "<<doubletPhi<<" Gap "<<gasGap<<" view "<<measphi);
		  }

		}}}}}}}}


  for( int stationName =  2 ;  stationName !=11;  stationName++){
    for( int stationEta  = -7 ;  stationEta  != 8;  stationEta++ ){
      for( int stationPhi  =  1 ;  stationPhi  != 9;  stationPhi++ ){
	for( int doubletR    =  1 ;  doubletR    != 3;  doubletR++   ){
	  for( int doubletZ    =  1 ;  doubletZ    != 4;  doubletZ++   ){
	    for( int doubletPhi  =  1 ;  doubletPhi  != 3;  doubletPhi++ ){
	      for( int gasGap      =  1 ;  gasGap	  != 3;  gasGap++     ){
		for( int measphi     =  0 ;  measphi	  != 2;  measphi++    ){

      bool isValid=false;
      Identifier rpcId = m_idHelper->channelID(stationName, stationEta, stationPhi, doubletR, doubletZ, doubletPhi, 1, 1, 1, true, &isValid); // last 5 arguments are: int doubletPhi, int gasGap, int measuresPhi, int strip, bool check, bool* isValid
      if (!isValid) continue;
		  const RpcReadoutElement* rpc = m_GMmgr->getRpcReadoutElement(rpcId);
		  if(rpc == 0 )continue;
		  Identifier idr = rpc->identify();
		  if(idr == 0 )continue;
		  Identifier atlasId = m_idHelper->channelID(idr, doubletZ,doubletPhi , gasGap, measphi, 1)     ;
		  if(atlasId == 0 )continue;

		  indexSName = stationName - 2 ;
		  if(indexSName>3) indexSName = indexSName - 2 ;

		  float efficiency          = 0. ;
		  float efficiencygap       = 0. ;
		  float averageCS           = 0. ;
		  float FracCS1             = 0. ;
		  float FracCS2     	       = 0. ;
		  float FracCStail          = 0. ;
		  float averageCStail       = 0. ;
		  int   ProjectedTracks     = 0. ;
		  float FracDeadStrip       = 0. ;

		  if( readCdo->getEfficiencyMap            ().find(atlasId)!= readCdo->getEfficiencyMap().end()       ) efficiency     = readCdo->getEfficiencyMap      ().find(atlasId)->second ;
		  if( readCdo->getEfficiencyGapMap         ().find(atlasId)!= readCdo->getEfficiencyGapMap().end()    ) efficiencygap  = readCdo->getEfficiencyGapMap   ().find(atlasId)->second ;
		  if( readCdo->getMeanClusterSizeMap       ().find(atlasId)!= readCdo->getMeanClusterSizeMap().end()  ) averageCS      = readCdo->getMeanClusterSizeMap ().find(atlasId)->second ;
		  if( readCdo->getFracClusterSize1Map      ().find(atlasId)!= readCdo->getFracClusterSize1Map().end() ) FracCS1        = readCdo->getFracClusterSize1Map().find(atlasId)->second ;
		  if( readCdo->getFracClusterSize2Map      ().find(atlasId)!= readCdo->getFracClusterSize2Map().end() ) FracCS2        = readCdo->getFracClusterSize2Map().find(atlasId)->second ;
		  FracCStail     = 1.0-FracCS1-FracCS2 ;
		  averageCStail  = averageCS - FracCS1 - 2*FracCS2                                ;
		  if( readCdo->getProjectedTracksMap       ().find(atlasId)!= readCdo->getProjectedTracksMap().end()  ) ProjectedTracks= readCdo->getProjectedTracksMap ().find(atlasId)->second ;
		  if( readCdo->getFracDeadStripMap         ().find(atlasId)!= readCdo->getFracDeadStripMap  ().end() ) FracDeadStrip  = readCdo->getFracDeadStripMap   ().find(atlasId)->second ;

		  if(ProjectedTracks<m_CutProjectedTracks  ||  ProjectedTracks>10000000 || efficiency<=0 || efficiency>1 || efficiencygap<=0 || efficiencygap>1 ){
		    if( stationEta >=  0){
		      efficiencygap       = COOLDB_GapEff_A             [indexSName]  ;
		      efficiency	      = COOLDB_PhiEff_A             [indexSName]  ;
		      if(measphi == 0){
			efficiency	      = COOLDB_EtaEff_A             [indexSName]  ;
		      }
		    }
		    else{
		      efficiencygap	    = COOLDB_GapEff_A 	    [indexSName]  ;
		      efficiency	    = COOLDB_PhiEff_A 	    [indexSName]  ;
		      if(measphi == 0){
			efficiency	    = COOLDB_EtaEff_A 	    [indexSName]  ;
		      }
		    }
		  }
		  if(ProjectedTracks<m_CutProjectedTracks ||  ProjectedTracks>10000000 || averageCS>m_CutMaxClusterSize || averageCS<=1 || FracCStail<0 || FracCS1<0 || FracCS2<0  || FracCStail>1 || FracCS1>1 || FracCS2>1){
		    if( stationEta >=  0){
		      averageCS	      = COOLDB_MeanClusterSize_A    [indexSName+7];
		      FracCS1	      = COOLDB_FracClusterSize1_A   [indexSName+7];
		      FracCS2	      = COOLDB_FracClusterSize2_A   [indexSName+7];
		      averageCStail       = COOLDB_MeanClusterSizeTail_A[indexSName+7];
		      if(measphi == 0){
			averageCS	      = COOLDB_MeanClusterSize_A    [indexSName]  ;
			FracCS1	      = COOLDB_FracClusterSize1_A   [indexSName]  ;
			FracCS2	      = COOLDB_FracClusterSize2_A   [indexSName]  ;
			averageCStail      = COOLDB_MeanClusterSizeTail_A[indexSName]  ;
		      }
		    }
		    else{
		      averageCS	    = COOLDB_MeanClusterSize_A    [indexSName+7];
		      FracCS1 	    = COOLDB_FracClusterSize1_A   [indexSName+7];
		      FracCS2 	    = COOLDB_FracClusterSize2_A   [indexSName+7];
		      averageCStail	    = COOLDB_MeanClusterSizeTail_A[indexSName+7];
		      if(measphi == 0){
			averageCS	    = COOLDB_MeanClusterSize_A    [indexSName]  ;
			FracCS1	    = COOLDB_FracClusterSize1_A   [indexSName]  ;
			FracCS2	    = COOLDB_FracClusterSize2_A   [indexSName]  ;
			averageCStail      = COOLDB_MeanClusterSizeTail_A[indexSName]  ;
		      }
		    }
		  }
		  ATH_MSG_VERBOSE( "Identifier " << atlasId << " sName "<<stationName<<" sEta " <<stationEta<<" sPhi "<<stationPhi<<" dR "<<doubletR<<" dZ "<<doubletZ<<" dPhi "<<doubletPhi<<" Gap "<<gasGap<<" view "<<measphi);
		  ATH_MSG_VERBOSE( " Eff "<<efficiency<<" EffGap " <<efficiencygap<<" MeanCS "<<averageCS<<" FracCS1 "<<FracCS1<<" FracCS2 "<<FracCS2<<" MeanCSTail "<<averageCStail<< " FracDead "<<FracDeadStrip<<" Tracks "<< ProjectedTracks);

		}}}}}}}}

  ATH_MSG_VERBOSE( "Number of dead strip " << readCdo->getDeadStripMap().size() );
  for( int stationName =  2 ;  stationName !=11;  stationName++){
    for( int stationEta  = -7 ;  stationEta  != 8;  stationEta++ ){
      for( int stationPhi  =  1 ;  stationPhi  != 9;  stationPhi++ ){
	for( int doubletR    =  1 ;  doubletR    != 3;  doubletR++   ){
	  for( int doubletZ    =  1 ;  doubletZ    != 4;  doubletZ++   ){
	    for( int doubletPhi  =  1 ;  doubletPhi  != 3;  doubletPhi++ ){
	      for( int gasGap      =  1 ;  gasGap	  != 3;  gasGap++     ){
		for( int measphi     =  0 ;  measphi	  != 2;  measphi++    ){
		  for( int strip       =  1 ;  strip	  !=81;  strip++      ){

        bool isValid=false;
        Identifier rpcId = m_idHelper->channelID(stationName, stationEta, stationPhi, doubletR, doubletZ, doubletPhi, 1, 1, 1, true, &isValid); // last 5 arguments are: int doubletPhi, int gasGap, int measuresPhi, int strip, bool check, bool* isValid
        if (!isValid) continue;
		    const RpcReadoutElement* rpc = m_GMmgr->getRpcReadoutElement(rpcId);
		    if(rpc == 0 )continue;
		    Identifier idr = rpc->identify();
		    if(idr == 0 )continue;
		    Identifier atlasId = m_idHelper->channelID(idr, doubletZ,doubletPhi , gasGap, measphi, strip)     ;
		    if(atlasId == 0 )continue;
		    int stripstatus	      = readCdo->getDeadStripIntMap      ().find(atlasId)->second ;
		    if( stripstatus != 1 )continue;
		    ATH_MSG_VERBOSE( "Identifier " << atlasId << " sName "<<stationName<<" sEta " <<stationEta<<" sPhi "<<stationPhi<<" dR "<<doubletR<<" dZ "<<doubletZ<<" dPhi "<<doubletPhi<<" Gap "<<gasGap<<" view "<<measphi<<" strip "<<strip << " stripstatus "<<stripstatus );

		  }}}}}}}}}
  return sc;

}

double time_correction(double x, double y, double z)
{
  double speed_of_light = 299.792458;     // mm/ns
  return sqrt(x*x+y*y+z*z)/speed_of_light; //FIXME use CLHEP::c_light
}
double RpcDigitizationTool::FCPEfficiency(const HepMC::GenParticle* genParticle)
{
  double qcharge = 1.;
  double qbetagamma = -1.;
  const int particlePdgId = genParticle->pdg_id();
  //charge calculation
  qcharge = (static_cast<double>((std::abs(particlePdgId) / 1000) % 100)) / (static_cast<double>((std::abs(particlePdgId) / 10) % 100));
  qcharge = ((static_cast<double>((static_cast<int>(qcharge*100)))))/100;
  if(particlePdgId < 0.0) qcharge = -qcharge;
  //BetaGamma calculation
  const double QPx = genParticle->momentum().px();
  const double QPy  = genParticle->momentum().py();
  const double QPz  = genParticle->momentum().pz();
  const double QE  = genParticle->momentum().e();
  const double QM2  = std::pow(QE,2)-std::pow(QPx,2)-std::pow(QPy,2)-std::pow(QPz,2);
  const double QP = std::sqrt(std::pow(QPx,2)+std::pow(QPy,2)+std::pow(QPz,2));
  double QM;
  if(QM2>=0.){QM = std::sqrt(QM2);}
  else {QM = -1.0;}
  if(QM>0.){qbetagamma = QP/QM;}
  else {qbetagamma = -1.0;}

  //find the i in the array
  int i_e = -1;
  for(int i=0;i<12;i++){
    if(Charge[i] == std::fabs(qcharge)){i_e = i;break;}
  }
  int i_v = -99, j_v = 99;
  if(qbetagamma != -1){
    for(int i=0;i<15;i++){
      if(Velocity[i] <= qbetagamma){i_v = i;}
    }
    for(int i=14;i>=0;i--){
      if(Velocity[i] >= qbetagamma){j_v = i;}
    }
  }
  //calculate the efficiency according to charge and velocity. Using linear function to calculate efficiency of a specific velocity between velocity1 and velocity2
  double eff_fcp = 1.0 , eff_muon = 1.0;
  const double delta_v = Velocity[i_v] - Velocity[j_v] ;
  if(i_e >= 0 && i_e <=11){
    if(j_v>=0 && j_v<=14 && i_v>=0 && i_v<=14 && (j_v - i_v)==1){
      eff_fcp = ( Eff_garfield[i_e][i_v] - Eff_garfield[i_e][j_v] ) / delta_v * qbetagamma + ( Eff_garfield[i_e][j_v] * Velocity[i_v] - Eff_garfield[i_e][i_v] * Velocity[j_v] ) / delta_v ;
      eff_muon = ( Eff_garfield[11][i_v] - Eff_garfield[11][j_v] ) / delta_v * qbetagamma + ( Eff_garfield[11][j_v] * Velocity[i_v] - Eff_garfield[11][i_v] * Velocity[j_v] ) / delta_v ;
    }
    else if(i_v == 14 && j_v == 99){
      eff_fcp = Eff_garfield[i_e][14];
      eff_muon = Eff_garfield[11][14];
    }
    else if(i_v == -99 && j_v == 0){
      eff_fcp = Eff_garfield[i_e][0];
      eff_muon = Eff_garfield[11][0];
    }
    else {
      ATH_MSG_WARNING ( "Wrong particle with unknown velocity! Scale factor is set to be 1." );
    }
  }
  else {
    ATH_MSG_WARNING ( "Wrong particle with unknown charge! Scale factor is set to be 1." );
  }
  //A scale factor is calculated by efficiency of fcp / efficiency of muon(charge==1.0
  const double eff_SF = eff_fcp / eff_muon ;
  return eff_SF;
}
