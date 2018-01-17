/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "PileUpTools/PileUpMergeSvc.h"

//Inputs
#include "MuonSimEvent/RPCSimHit.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
//Outputs
#include "MuonDigitContainer/RpcDigitContainer.h"
#include "MuonSimData/MuonSimDataCollection.h"
//Geometry
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonCondInterface/IRPCConditionsSvc.h"
#include "MuonSimEvent/RpcHitIdHelper.h"


//run n. from geometry DB 
#include "GeoModelInterfaces/IGeoModelSvc.h" 
#include "GeometryDBSvc/IGeometryDBSvc.h" 
#include "RDBAccessSvc/IRDBAccessSvc.h" 
#include "RDBAccessSvc/IRDBRecordset.h" 
#include "RDBAccessSvc/IRDBRecord.h" 

//Truth
#include "GeneratorObjects/HepMcParticleLink.h"
#include "HepMC/GenParticle.h"

//Random Numbers
#include "AthenaKernel/IAtRndmGenSvc.h"
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


using namespace MuonGM;
#define SIG_VEL 4.8  // ns/m
static double time_correction(double, double, double);

static constexpr unsigned int crazyParticleBarcode(
    std::numeric_limits<int32_t>::max());
// Barcodes at the HepMC level are int

RpcDigitizationTool::RpcDigitizationTool(const std::string& type,
					 const std::string& name,
					 const IInterface* pIID)
  : PileUpToolBase(type, name, pIID)
  , m_digitContainer(0)
  , m_sdoContainer(0)
  , m_GMmgr(0)
  , m_idHelper(0)
  , m_muonHelper(0)
  , m_thpcRPC(0)
  , m_rSummarySvc("RPCCondSummarySvc", name)
  , m_cs3Para(0)
  , m_validationSetup(false)
  , m_vetoThisBarcode(crazyParticleBarcode) 
  , m_SetPhiOn(false)
  , m_SetEtaOn(false)
  , m_mergeSvc(0)
  , m_inputHitCollectionName("RPC_Hits")
  , m_outputDigitCollectionName("RPC_DIGITS")
  , m_outputSDO_CollectionName("RPC_SDO")
  , m_rndmSvc("AtRndmGenSvc", name )
  , m_rndmEngine(0)
  , m_rndmEngineName("RPC_Digitization")
  , m_tagInfoMgr(0)
{

  declareInterface<IMuonDigitizationTool>(this);

  declareProperty("Parameters"           ,  m_paraFile = "G4RPC_Digitizer.txt");  // File with cluster distributions
  declareProperty("InputObjectName"      ,  m_inputHitCollectionName    = "RPC_Hits",  "name of the input object");
  declareProperty("OutputObjectName"     ,  m_outputDigitCollectionName = "RPC_DIGITS","name of the output object");
  declareProperty("OutputSDOsName"       ,  m_outputSDO_CollectionName  = "RPC_SDO",   "name of the output object");
  declareProperty("WindowLowerOffset"    ,  m_timeWindowLowerOffset = -100.       , "digitization window lower limit");
  declareProperty("WindowUpperOffset"    ,  m_timeWindowUpperOffset = +150.       , "digitization window lower limit");
  declareProperty("DeadTime"             ,  m_deadTime              = 100.        , "dead time"                      );
  declareProperty("UncorrJitter"         ,  m_UncorrJitter          = 1.5         , "jitter uncorrelated"            );
  declareProperty("CorrJitter"           ,  m_CorrJitter            = 0.0         , "jitter correlated"              );
  declareProperty("RndmSvc"              ,  m_rndmSvc                  , "Random Number Service used in Muon digitization" );
  declareProperty("RndmEngine"           ,  m_rndmEngineName           , "Random engine name");
  declareProperty("PatchForRpcTime"      ,  m_patch_for_rpc_time       = false );
  declareProperty("PatchForRpcTimeShift" ,  m_rpc_time_shift           = 12.5   ); // shift rpc digit time to match hardware time calibration: Zmumu muons are at the center of BC0, i.e. at 12.5ns+BC0shift w.r.t. RPC readout (BC0shift=2x3.125)
  declareProperty("RPC_TimeSchema"       ,  m_RPC_TimeSchema           = "RPC_TimeSchema"); // RPC time Info tag name
  declareProperty("RPCSDOareRPCDigits"   ,  m_sdoAreOnlyDigits         = true  );
  declareProperty("MuonOnlySDOs"         ,  m_muonOnlySDOs             = true  );
  declareProperty("PanelId_OFF_fromlist" ,  m_PanelId_OFF_fromlist     = false );
  declareProperty("FileName_DeadPanels"  ,  m_FileName_DeadPanels= "PermanentDeadPanels.txt"); //File with deadpanel list
  declareProperty("PanelId_OK_fromlist"  ,  m_PanelId_OK_fromlist      = false );
  declareProperty("FileName_GoodPanels"  ,  m_FileName_GoodPanels= "PermanentGoodPanels.txt"); //File with goodpanel list

  declareProperty("PhiAndEtaEff_A"            ,  m_PhiAndEtaEff_A                   );
  declareProperty("OnlyPhiEff_A"              ,  m_OnlyPhiEff_A                     );
  declareProperty("OnlyEtaEff_A"              ,  m_OnlyEtaEff_A                     );
  declareProperty("PhiAndEtaEff_C"            ,  m_PhiAndEtaEff_C                   );
  declareProperty("OnlyPhiEff_C"              ,  m_OnlyPhiEff_C                     );
  declareProperty("OnlyEtaEff_C"              ,  m_OnlyEtaEff_C                     );
  declareProperty("FracClusterSize1_A"        ,  m_FracClusterSize1_A		    );
  declareProperty("FracClusterSize2_A"        ,  m_FracClusterSize2_A		    );
  declareProperty("FracClusterSizeTail_A"     ,  m_FracClusterSizeTail_A	    );
  declareProperty("MeanClusterSizeTail_A"     ,  m_MeanClusterSizeTail_A	    );
  declareProperty("FracClusterSize1_C"        ,  m_FracClusterSize1_C		    );
  declareProperty("FracClusterSize2_C"        ,  m_FracClusterSize2_C		    );
  declareProperty("FracClusterSizeTail_C"     ,  m_FracClusterSizeTail_C	    );
  declareProperty("MeanClusterSizeTail_C"     ,  m_MeanClusterSizeTail_C	    );
  declareProperty("turnON_efficiency"         ,  m_turnON_efficiency    = true      );
  declareProperty("Minimum_efficiency"        ,  m_Minimum_efficiency   = 0.5       );
  declareProperty("ApplyEfficiencyThreshold"  ,  m_applyEffThreshold    = false     );
  declareProperty("Efficiency_fromCOOL"       ,  m_Efficiency_fromCOOL  = false     );
  declareProperty("KillDeadStrips"            ,  m_kill_deadstrips      = false     );
  declareProperty("EfficiencyPatchForBMShighEta", m_EfficiencyPatchForBMShighEta = false );
  declareProperty("turnON_clustersize"        ,  m_turnON_clustersize   = true      );
  declareProperty("ClusterSize_fromCOOL"      ,  m_ClusterSize_fromCOOL = false     );
  declareProperty("testbeam_clustersize"      ,  m_testbeam_clustersize   = 1       );
  declareProperty("ClusterSize1_2uncorr"      ,  m_ClusterSize1_2uncorr   = 0       );
  declareProperty("FirstClusterSizeInTail"    ,  m_FirstClusterSizeInTail = 3       );
  declareProperty("PrintCalibrationVector"    ,  m_PrintCalibrationVector = 0       );
  declareProperty("Force_BOG_BOF_DoubletR2_OFF",  m_BOG_BOF_DoubletR2_OFF  = false  );
  declareProperty("IgnoreRunDependentConfig"  ,  m_ignoreRunDepConfig     = false   );
  declareProperty("RPCInfoFromDb"             ,  m_RPCInfoFromDb          = false   );
  declareProperty("DumpFromDbFirst"           ,  m_DumpFromDbFirst        = false   );
  declareProperty("CutMaxClusterSize"         ,  m_CutMaxClusterSize      = 5.0     );
  declareProperty("CutProjectedTracks"        ,  m_CutProjectedTracks     = 100     );
  declareProperty("RPCCondSummarySvc"         ,  m_rSummarySvc                      );
  declareProperty("ValidationSetup"           ,  m_validationSetup        = false   );
  declareProperty("IncludePileUpTruth"        ,  m_includePileUpTruth     = true    );
  declareProperty("ParticleBarcodeVeto"       ,  m_vetoThisBarcode = crazyParticleBarcode);
}

// member function implementation
//--------------------------------------------
StatusCode RpcDigitizationTool::initialize() {

  ATH_MSG_DEBUG ( "RpcDigitizationTool:: in initialize()" );
  ATH_MSG_DEBUG ( "Configuration  RpcDigitizationTool " );

  ATH_MSG_DEBUG ( "Parameters             " << m_paraFile                  );
  ATH_MSG_DEBUG ( "InputObjectName        " << m_inputHitCollectionName    );
  ATH_MSG_DEBUG ( "OutputObjectName       " << m_outputDigitCollectionName );
  ATH_MSG_DEBUG ( "WindowLowerOffset      " << m_timeWindowLowerOffset     );
  ATH_MSG_DEBUG ( "WindowUpperOffset      " << m_timeWindowUpperOffset     );
  ATH_MSG_DEBUG ( "DeadTime               " << m_deadTime                  );
  ATH_MSG_DEBUG ( "RndmSvc                " << m_rndmSvc                   );
  ATH_MSG_DEBUG ( "RndmEngine             " << m_rndmEngineName            );
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
  ATH_MSG_DEBUG ( "RPCCondSummarySvc      " <<  m_rSummarySvc              );
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


  if (detStore()->retrieve( m_GMmgr,"Muon" ).isFailure()) {
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG ( "Retrieved GeoModel from DetectorStore." );

  m_idHelper = m_GMmgr->rpcIdHelper();
  if(!m_idHelper) {
    return StatusCode::FAILURE;
  }

  // check the input object name
  if (m_inputHitCollectionName=="") {
    ATH_MSG_FATAL ( "Property InputObjectName not set !" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( "Input objects: '" << m_inputHitCollectionName << "'" );
  }

  // check the output object name
  if (m_outputDigitCollectionName=="") {
    ATH_MSG_FATAL ( "Property OutputObjectName not set !" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( "Output digits: '" << m_outputDigitCollectionName << "'" );
  }

  // initialize digit container
  // m_digitContainer = new RpcDigitContainer(m_idHelper->module_hash_max());
  //  m_digitContainer->addRef();


  //set the configuration based on run1/run2
  // Retrieve geometry config information from the database (RUN1, RUN2, etc...) 
  IRDBAccessSvc* rdbAccess = 0; 
  StatusCode result = service("RDBAccessSvc",rdbAccess); 

  if(result.isFailure()) { 
    ATH_MSG_ERROR("Unable to get RDBAccessSvc"); 
    return result; 
  } 

  bool run1 = true;
  std::string configVal = "";
  const IGeoModelSvc* geoModel = 0; 
  result = service("GeoModelSvc", geoModel); 
  if (result.isFailure()) { 
    ATH_MSG_ERROR( "Could not locate GeoModelSvc"  ); 
  } 
  else { 
    // check the DetDescr version 
    std::string atlasVersion = geoModel->atlasVersion(); 

    IRDBRecordset_ptr atlasCommonRec = rdbAccess->getRecordsetPtr("AtlasCommon",atlasVersion,"ATLAS"); 
    if(atlasCommonRec->size()==0) { 
      run1 = true; 
    } else { 
      configVal = (*atlasCommonRec)[0]->getString("CONFIG"); 
      ATH_MSG_INFO( "From DD Database, Configuration is "<< configVal );
      std::string MSgeoVersion = m_GMmgr->geometryVersion().substr(0,4);
      ATH_MSG_INFO( "From DD Database, MuonSpectrometer geometry version is "<< MSgeoVersion );
      if(configVal=="RUN1" || MSgeoVersion=="R.06"){ 
        run1 = true; 
      } 
      else if(configVal=="RUN2" || MSgeoVersion=="R.07") { 
        run1 = false; 
      } 
      else { 
        ATH_MSG_FATAL("Unexpected value for geometry config read from the database: " << configVal); 
        return StatusCode::FAILURE; 
      } 
    } 
    // 
    if (run1) ATH_MSG_INFO("From Geometry DB: MuonSpectrometer configuration is: RUN1 or MuonGeometry = R.06"); 
    else        ATH_MSG_INFO("From Geometry DB: MuonSpectrometer configuration is: RUN2 or MuonGeometry = R.07"); 
  }
  if (m_ignoreRunDepConfig==false) 
    {
      m_BOG_BOF_DoubletR2_OFF = false;
      m_Efficiency_fromCOOL   = false;
      m_ClusterSize_fromCOOL  = false;
      m_RPCInfoFromDb         = false;
      m_kill_deadstrips       = false;
      m_applyEffThreshold     = false;
      if (run1)
	{
	  //m_BOG_BOF_DoubletR2_OFF = true 
	  //m_Efficiency_fromCOOL   = true 
	  //m_ClusterSize_fromCOOL  = true
	  m_BOG_BOF_DoubletR2_OFF = true;
	  if (configVal=="RUN1") 
	    {// MC12 setup 
	      m_Efficiency_fromCOOL   = true;
	      m_ClusterSize_fromCOOL  = true;
	      m_RPCInfoFromDb         = true;
	      m_kill_deadstrips       = true;
	      m_applyEffThreshold     = false;
	      m_CutProjectedTracks    = 50;
	    }
	}
      else 
	{
	  //m_BOG_BOF_DoubletR2_OFF = false # do not turn off at digitization the hits in the dbR=2 chambers in the feet
	  //m_Efficiency_fromCOOL   = false # use common average values in python conf. 
	  //m_ClusterSize_fromCOOL  = false # use common average values in python conf. 
	  m_BOG_BOF_DoubletR2_OFF = false;
	   if (configVal=="RUN2") 
	    {// MC15c setup 
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
  else
    {
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

  if(StatusCode::SUCCESS != readParameters() ){
    ATH_MSG_ERROR ("Cannot read parameters for cluster simulation. Exiting.");
    return StatusCode::FAILURE;
  }

  if (!m_rndmSvc.retrieve().isSuccess())
    {
      ATH_MSG_ERROR ( " Could not initialize Random Number Service" );
      return StatusCode::FAILURE;
    }

  // getting our random numbers stream
  ATH_MSG_DEBUG ( "Getting random number engine : <" << m_rndmEngineName << ">" );
  m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
  if (m_rndmEngine==0) {
    ATH_MSG_ERROR ( "Could not find RndmEngine : " << m_rndmEngineName );
    return StatusCode::FAILURE;
  }

  // get TagInfoMgr
  if ( service("TagInfoMgr", m_tagInfoMgr).isFailure() || m_tagInfoMgr==0) {
    ATH_MSG_WARNING ( " Unable to locate TagInfoMgr service" );
    return StatusCode::FAILURE;
  }

  // fill the taginfo information
  if (StatusCode::SUCCESS != fillTagInfo()) {
    ATH_MSG_WARNING ( "Could not fill the tagInfo for RPC Timing" );
    return StatusCode::FAILURE;
  }
  if (m_RPCInfoFromDb){
    if (StatusCode::SUCCESS != m_rSummarySvc.retrieve()) {
      ATH_MSG_WARNING ( "Could not retrieve RPC Info from Db" );
      return StatusCode::FAILURE;
    }
  }

  // initialize digit container
  m_digitContainer = new RpcDigitContainer(m_idHelper->module_hash_max());
  m_digitContainer->addRef();

  if(m_PrintCalibrationVector!=0){
    if  (StatusCode::SUCCESS != PrintCalibrationVector()) {
      ATH_MSG_WARNING ( "Could not PrintCalibrationVector" );
      return StatusCode::FAILURE;
    }
  }

  m_DeadStripPanel.clear();

  //////////////////// special test 
  //  m_turnON_clustersize=false;

  return StatusCode::SUCCESS;

}

//--------------------------------------------
StatusCode RpcDigitizationTool::prepareEvent(unsigned int) {

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

  SubEventIterator iEvt = bSubEvents;
  while(iEvt!=eSubEvents)
    {
      StoreGateSvc& seStore = *iEvt->ptr()->evtStore();
      PileUpTimeEventIndex thisEventIndex = PileUpTimeEventIndex(static_cast<int>(iEvt->time()),iEvt->index());
      ATH_MSG_VERBOSE("SubEvt StoreGate " << seStore.name() << " :"
                      << " bunch crossing : " << bunchXing
                      << " time offset : " << iEvt->time()
                      << " event number : " << iEvt->ptr()->eventNumber()
                      << " run number : " << iEvt->ptr()->runNumber());
      const RPCSimHitCollection* seHitColl(nullptr);
      if (!seStore.retrieve(seHitColl,m_inputHitCollectionName).isSuccess())
        {
          ATH_MSG_ERROR ("SubEvent RPCSimHitCollection not found in StoreGate " << seStore.name());
          return StatusCode::FAILURE;
        }
      ATH_MSG_VERBOSE ("RPCSimHitCollection found with " << seHitColl->size() << " hits");
      //Copy Hit Collection
      RPCSimHitCollection* RPCHitColl = new RPCSimHitCollection("RPC_Hits");
      RPCSimHitCollection::const_iterator i = seHitColl->begin();
      RPCSimHitCollection::const_iterator e = seHitColl->end();
      // Read hits from this collection
      for (; i!=e; ++i)
        {
          RPCHitColl->Emplace(*i);
        }
      m_thpcRPC->insert(thisEventIndex, RPCHitColl);
      //store these for deletion at the end of mergeEvent
      m_RPCHitCollList.push_back(RPCHitColl);

      ++iEvt;
    }

  return StatusCode::SUCCESS;
}

//--------------------------------------------
// Get next event and extract collection of hit collections:
StatusCode RpcDigitizationTool::getNextEvent()
{

  ATH_MSG_DEBUG ( "RpcDigitizationTool::getNextEvent()" );

  if(!m_mergeSvc) {
    //locate the PileUpMergeSvc and initialize our local ptr
    const bool CREATEIF(true);
    if (!(service("PileUpMergeSvc", m_mergeSvc, CREATEIF)).isSuccess() ||
	0 == m_mergeSvc) {
      ATH_MSG_ERROR ("Could not find PileUpMergeSvc" );
      return StatusCode::FAILURE;
    }
   
  }


  // initialize pointer
  m_thpcRPC = 0;

  //  get the container(s)
  typedef PileUpMergeSvc::TimedList<RPCSimHitCollection>::type TimedHitCollList;

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
StatusCode RpcDigitizationTool::mergeEvent() {

  StatusCode status = StatusCode::SUCCESS;

  ATH_MSG_DEBUG ( "RpcDigitizationTool::in mergeEvent()" );

  if(m_DumpFromDbFirst && m_RPCInfoFromDb){
    status = DumpRPCCalibFromCoolDB() ;
    if (status == StatusCode::FAILURE) return status ;
    m_DumpFromDbFirst = false ;
  }

  // Cleanup and record the Digit container in StoreGate
  m_digitContainer->cleanup();
  status = evtStore()->record(m_digitContainer,m_outputDigitCollectionName);
  if (status.isFailure())  {
    ATH_MSG_ERROR ( "Unable to record RPC digit container in StoreGate" );
    return status;
  }
  ATH_MSG_DEBUG ( "RpcDigitContainer recorded in StoreGate." );

  // Create and record the SDO container in StoreGate
  m_sdoContainer = new MuonSimDataCollection();
  status = evtStore()->record(m_sdoContainer,m_outputSDO_CollectionName);
  if (status.isFailure())  {
    ATH_MSG_ERROR ( "Unable to record RPC SDO collection in StoreGate" );
    return status;
  }
  ATH_MSG_DEBUG ( "RpcSDOCollection recorded in StoreGate." );

  //////////////// TEMP////
  m_sdo_tmp_map.clear();
  /////////////////////////

  status = doDigitization();
  if (status.isFailure())  {
    ATH_MSG_ERROR ( "doDigitization Failed" );
  }

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
StatusCode RpcDigitizationTool::digitize() {
  return this->processAllSubEvents();
}

//--------------------------------------------
StatusCode RpcDigitizationTool::processAllSubEvents() {

  StatusCode status = StatusCode::SUCCESS;

  //merging of the hit collection in getNextEvent method

  ATH_MSG_DEBUG ( "RpcDigitizationTool::in digitize()" );

  if(m_DumpFromDbFirst && m_RPCInfoFromDb){
    status = DumpRPCCalibFromCoolDB() ;
    if (status == StatusCode::FAILURE) return status ;
    m_DumpFromDbFirst = false ;
  }

  // Cleanup and record the Digit container in StoreGate
  m_digitContainer->cleanup();
  status = evtStore()->record(m_digitContainer, m_outputDigitCollectionName);
  if (status.isFailure())  {
    ATH_MSG_ERROR ( "Unable to record RPC digit container in StoreGate" );
    return status;
  }
  ATH_MSG_DEBUG ( "RpcDigitContainer recorded in StoreGate." );

  // Create and record the SDO container in StoreGate
  m_sdoContainer = new MuonSimDataCollection();
  status = evtStore()->record(m_sdoContainer,m_outputSDO_CollectionName);
  if (status.isFailure())  {
    ATH_MSG_ERROR ( "Unable to record RPC SDO collection in StoreGate" );
    return status;
  }
  ATH_MSG_DEBUG ( "RpcSDOCollection recorded in StoreGate." );

  //////////////// TEMP////
  m_sdo_tmp_map.clear();
  /////////////////////////

  if (0 == m_thpcRPC ) {
    status = getNextEvent();
    if (StatusCode::FAILURE == status) {
      ATH_MSG_INFO ( "There are no RPC hits in this event" );
      return status; // there are no hits in this event
    }
  }
  
  status = doDigitization();
  if (status.isFailure())  {
    ATH_MSG_ERROR ( "doDigitization Failed" );
  }

  return status;
}

//--------------------------------------------
StatusCode RpcDigitizationTool::doDigitization() {

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

      //std::cout << "Global time " << globalHitTime << " G4 time " << G4Time
      //          << " Bunch time " << bunchTime      <<  std::endl;

      if (m_validationSetup){
	  RPCSimHit* copyHit = new RPCSimHit(idHit, globalHitTime, hit.localPosition(), hit.trackNumber(),
					     hit.postLocalPosition(), hit.energyDeposit(), hit.stepLength(), 
					     hit.particleEncoding(), hit.kineticEnergy());
	  ATH_MSG_VERBOSE("Validation:  globalHitTime, G4Time, BCtime = "<<globalHitTime<<" "<<G4Time<<" "<<bunchTime);
	  ATH_MSG_VERBOSE("Validation:  "<<copyHit->print());
	  
	inputSimHitColl->Insert(*copyHit);
      }

      // convert sim id helper to offline id
      m_muonHelper = RpcHitIdHelper::GetHelper();
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
      if(m_CorrJitter>0.01) corrtimejitter    = CLHEP::RandGaussZiggurat::shoot(m_rndmEngine,0.,m_CorrJitter); //correlated jitter
      //      std::cout<<" Correlated eta / phi jitter = "<<corrtimejitter<<std::endl;
      // handle here the special case where eta panel is dead => phi strip status (dead or eff.) cannot be resolved; 
      // measured panel eff. will be used in that case and no phi strip killing will happen
      bool undefPhiStripStat = false;

      std::vector<int> pcseta    = PhysicalClusterSize(&idpaneleta, &hit); //set to one for new algorithms
      ATH_MSG_DEBUG ( "Simulated cluster on eta panel: size/first/last= "<<pcseta[0]   <<"/"<<pcseta[1]   <<"/"<<pcseta[2] );
      std::vector<int> pcsphi = PhysicalClusterSize(&idpanelphi, &hit); //set to one for new algorithms
      ATH_MSG_DEBUG ( "Simulated cluster on phi panel: size/first/last= "<<pcsphi[0]<<"/"<<pcsphi[1]<<"/"<<pcsphi[2] );

      // create Identifiers
      Identifier atlasRpcIdeta = m_idHelper->channelID(stationName, stationEta, stationPhi, doubletR,
                                                       doubletZ, doubletPhi,gasGap, 0, pcseta[1] );
      Identifier atlasRpcIdphi = m_idHelper->channelID(stationName, stationEta, stationPhi, doubletR,
                                                       doubletZ, doubletPhi,gasGap, 1, pcsphi[1] );

      const RpcReadoutElement* ele= m_GMmgr->getRpcReadoutElement(atlasRpcIdeta);// first add time jitter to the time:

      if (DetectionEfficiency(&atlasRpcIdeta,&atlasRpcIdphi, undefPhiStripStat).isFailure()) return StatusCode::FAILURE ;
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
	pcs = TurnOnStrips(pcs, &atlasId);
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
	MuonSimData::Deposit deposit(HepMcParticleLink(phit->trackNumber(),phit.eventId()),
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
	    if ( !(undefPhiStripStat && imeasphi==1) )
	      {
		if(m_rSummarySvc->RPC_DeadStripList().find( newId )  != m_rSummarySvc->RPC_DeadStripList().end()) 
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
        auto insertResult = m_sdoContainer->insert(std::make_pair( it->first,simData) );
        if (!insertResult.second) ATH_MSG_ERROR ( "Attention: this sdo is not recorded, since the identifier already exists in the m_sdoContainer map" );
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
	  if (m_sdoContainer->find(theId) != m_sdoContainer->end()) // Identifier exist -> increase deposit
	    {
	      std::map<Identifier,MuonSimData>::const_iterator it = m_sdoContainer->find(theId);
	      std::vector<MuonSimData::Deposit> deps = ((*it).second).getdeposits();
	      deps.push_back((*map_dep_iter).second);
	    }
	  else // Identifier does not exist -> create (Id,deposit)
	    {
	      std::vector<MuonSimData::Deposit> deposits;
	      deposits.push_back((*map_dep_iter).second);
	      std::pair<std::map<Identifier,MuonSimData>::iterator, bool> insertResult =
		m_sdoContainer->insert(std::make_pair( theId, MuonSimData(deposits,0) ) );
	      if (!insertResult.second) ATH_MSG_ERROR ( "Attention: this sdo is not recorded, since the identifier already exists in the m_sdoContainer map" );
	    }
	}
      //apply dead time
      if(fabs(currTime-last_time)>(m_deadTime))
	{


	  ATH_MSG_DEBUG ( "deposit with time "<< currTime <<" is distant enough from previous (if any) hit on teh same strip" );
	  last_time=(*map_dep_iter).first;

	  // first add time jitter to the time:
          double uncorrjitter    = 0 ;
	  if(m_UncorrJitter>0.01) uncorrjitter = CLHEP::RandGaussZiggurat::shoot(m_rndmEngine,0.,m_UncorrJitter);
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
	  RpcDigitContainer::const_iterator it_coll = m_digitContainer->indexFind(coll_hash);
	  if (m_digitContainer->end() ==  it_coll) {
	    digitCollection = new RpcDigitCollection(elemId,coll_hash);
	    digitCollection->push_back(newDigit);
	    StatusCode status = m_digitContainer->addCollection(digitCollection, coll_hash);
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
	    digitCollection = const_cast<RpcDigitCollection*>( *it_coll );
	    digitCollection->push_back(newDigit);
	  }

	  if (!m_muonOnlySDOs && m_sdoAreOnlyDigits)
	    {

	      // put SDO collection in StoreGate
	      if (m_sdoContainer->find(theId) != m_sdoContainer->end())
		{
		  std::map<Identifier,MuonSimData>::const_iterator it = m_sdoContainer->find(theId);
		  std::vector<MuonSimData::Deposit> deps = ((*it).second).getdeposits();
		  deps.push_back((*map_dep_iter).second);
		}
	      else
		{
		  std::vector<MuonSimData::Deposit> deposits;
		  deposits.push_back((*map_dep_iter).second);
		  std::pair<std::map<Identifier,MuonSimData>::iterator, bool> insertResult =
		    m_sdoContainer->insert(std::make_pair( theId, MuonSimData(deposits,0) ) );
		  if (!insertResult.second) ATH_MSG_ERROR("Attention: this sdo is not recorded, since teh identifier already exists in the m_sdoContainer map");
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
std::vector<int> RpcDigitizationTool::PhysicalClusterSize(const Identifier* id, const RPCSimHit* theHit)
{

  // ME unused: int stationName = m_idHelper->stationName(*id);
  int stationEta  = m_idHelper->stationEta(*id);
  // ME unused: int stationPhi  = m_idHelper->stationPhi(*id);
  // float length; // not used
  float pitch;
  // float stripWidth; // not used
  // ME unused: int doubletZ = m_idHelper->doubletZ(*id);
  // ME unused: int doubletPhi = m_idHelper->doubletPhi(*id);
  // ME unused: int doubletR = m_idHelper->doubletR(*id);
  // ME unused: int gasGap =    m_idHelper->gasGap(*id);
  int measuresPhi = m_idHelper->measuresPhi(*id);
  std::vector<int> result(3,0);


  //  CLHEP::HepRandomEngine*      engine  =       p_AtRndmGenSvc->GetEngine("RPCDIGI");
  //    std::cout<<"test1 "<< p_engine->getSeeds()[0]<<" " << p_engine->getSeeds()[1]<<std::endl;


  const RpcReadoutElement* ele= m_GMmgr->getRpcReadoutElement(*id);


  //ATH_MSG_DEBUG ( "in RpcDigitizationTool::PhysicalClusterSize, id is "<<*id) ;
  //ATH_MSG_DEBUG ( "RpcDigitizationTool::PhysicalClusterSize hit in element (stname, steta, stphi, dr, dZ, dPhi, measphi):
  //    "<<stationName<< " " <<stationEta<< " "<<stationPhi<< " "<< doubletR<<" " << doubletZ<< " "<<doubletPhi<< " "<<
  //    measuresPhi);
  //ATH_MSG_DEBUG ( "RpcDigitizationTool::PhysicalClusterSize, check for ID validity "<< m_idHelper->valid(*id)<< " " <<
  //  m_idHelper->validElement(*id));


  // stripWidth = ele->StripWidth(measuresPhi); // not used.

  pitch = ele->StripPitch(measuresPhi);


  // if(measuresPhi){
  //   length = ele->stripPanelSsize(measuresPhi); // not used
  // } else {
  //   length = ele->stripPanelZsize(measuresPhi); // not used
  // }

  int nstrip;
  double xstrip;

  std::vector<double> cs=m_csPara; // read from file
  double cs1[5]; //the contributions to the observed cluster size due to physical cluster size 1 and 2
  double cs2[5];

  // ME unused: double impact;
  // ME unused: double stripoffset;
  // ME unused: double dead = (pitch - stripWidth);


  Amg::Vector3D position=adjustPosition(id, theHit->localPosition());

  nstrip=findStripNumber(position,*id, xstrip);

  xstrip=xstrip*30./pitch;

  //std::cout <<"xstrip "<< xstrip/30. << std::endl;

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

    if(CLHEP::RandFlat::shoot(m_rndmEngine)<pcs1_prob) pcsIs1=1;

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

    if(!pcsIs1&&CLHEP::RandFlat::shoot(m_rndmEngine)<pcs2_to_convert){
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
      double rand1=CLHEP::RandFlat::shoot(m_rndmEngine,100.);
      if(rand1>cs1_tot+cs2_tot){ // it means that cs is 1
	result[0]=1;
      } else {
	double rand=CLHEP::RandFlat::shoot(m_rndmEngine,cs1_tot);
	if(rand<cs1[1]) result[0]=2;
	else if(rand<cs1[1]+cs1[2]) result[0]=3;
	else if(rand<cs1[1]+cs1[2]+cs1[3]) result[0]=4;
	else result[0]=4;
      }

    } else {
      double rand=CLHEP::RandFlat::shoot(m_rndmEngine,cs2_tot);
      if(rand<cs2[1]) result[0]=2;
      else if(rand<cs2[1]+cs2[2]) result[0]=3;
      else if(rand<cs2[1]+cs2[2]+cs2[3]) result[0]=4;
      else result[0]=4;

    }
  }//testbeam algorithm
  else{

    float xstripnorm=xstrip/30.;
    result[0] = ClusterSizeEvaluation(id, xstripnorm );

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
std::vector<int> RpcDigitizationTool::TurnOnStrips(std::vector<int> pcs, const Identifier* id)
{

  int nstrips;
  int measuresPhi = m_idHelper->measuresPhi(*id);

  //  HepRandomM_Engine* engine = p_AtRndmGenSvc->GetEngine("RPCDIGI");

  const RpcReadoutElement* ele= m_GMmgr->getRpcReadoutElement(*id);

  nstrips = ele->Nstrips(measuresPhi);

  //testbeam algorithm
  if(m_testbeam_clustersize){
    int stripsAlreadyTurnedOn=1-pcs[1]+pcs[2];

    // turn on strips according to spread distribution obtained from data

    if(stripsAlreadyTurnedOn==1){

      if(pcs[0]==2){
	if(CLHEP::RandFlat::shoot(m_rndmEngine)<0.5) pcs[1]--;
	else pcs[2]++;
      } else if(pcs[0]==3){
	//  out3 << pcs[1]<< " ";
	if(CLHEP::RandFlat::shoot(m_rndmEngine)<m_cs3Para){
	  pcs[1]--;   // -+-
	  pcs[2]++;
	} else {
	  if(CLHEP::RandFlat::shoot(m_rndmEngine)<0.5) pcs[2]+=2; // +--
	  else pcs[1]-=2;  // --+
	}
	// out3 << pcs[1] <<std::endl;
      } else if(pcs[0]==4){
	// out4 << pcs[1]<< std::endl;
	double rand=CLHEP::RandFlat::shoot(m_rndmEngine);
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
	if(CLHEP::RandFlat::shoot(m_rndmEngine)<0.5){   //  +-- or -+-
	  //out3 << pcs[1]<< " ";
	  if(CLHEP::RandFlat::shoot(m_rndmEngine)<m_cs3Para/rand_norm) pcs[1]--; // -+-
	  else pcs[2]++; // +--
	} else { // -+- or --+
	  //out3 << pcs[1]<<" ";
	  if(CLHEP::RandFlat::shoot(m_rndmEngine)<m_cs3Para/rand_norm) pcs[2]++; // -+-
	  else pcs[1]--; // --+
	}
	//out3 << pcs[1]<< " "<< std::endl;
      } else if(pcs[0]==4){
	if(CLHEP::RandFlat::shoot(m_rndmEngine)<0.5){ // strip crossed is the first of the two
	  //out4<<pcs[1]<<" ";
	  double rand=CLHEP::RandFlat::shoot(m_rndmEngine,2*m_cs4Para[0]+m_cs4Para[1]+m_cs4Para[2]);
	  if(rand<2*m_cs4Para[0]){ pcs[2]+=2;} // the '2*' is to compensate for the 0.5
	  else if(rand<2*m_cs4Para[0]+m_cs4Para[1]){ pcs[1]--;pcs[2]++;}
	  else { pcs[1]-=2;}
	  //out4<<pcs[1]<<std::endl;
	} else {  // strip crossed is the second of the two
	  //out4<<pcs[2]<<" ";
	  double rand=CLHEP::RandFlat::shoot(m_rndmEngine,m_cs4Para[1]+m_cs4Para[2]+2*m_cs4Para[3]);
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
StatusCode RpcDigitizationTool::finalize() {

  ATH_MSG_DEBUG ( "RpcDigitizationTool::in finalize()" );

  m_digitContainer->release();
 
  return StatusCode::SUCCESS;
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


  //std::cout<<"packing in word "<<proptime << " " <<tof<< std::endl;
  using namespace std;

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

  std::string fileName = m_paraFile.c_str();
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
StatusCode RpcDigitizationTool::DetectionEfficiency(const Identifier* IdEtaRpcStrip, const Identifier* IdPhiRpcStrip, bool& undefinedPhiStripStatus) {

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
    
    if( m_rSummarySvc->RPC_FracDeadStripMap()  .find(IdEta) == m_rSummarySvc->RPC_FracDeadStripMap()  .end()){
      ATH_MSG_DEBUG ( "Not In CoolDB the Panel IdEtaRpcStrip :  "  << IdEta << " i.e. "<<m_idHelper->show_to_string(IdEta));
      noEntryInDb=true;      
    }
    else
      {
	ATH_MSG_DEBUG ( "Found In CoolDB the Panel IdEtaRpcStrip :  "  << IdEta << " i.e. "<<m_idHelper->show_to_string(IdEta));
      }
    if( m_rSummarySvc->RPC_FracDeadStripMap()  .find(IdPhi) == m_rSummarySvc->RPC_FracDeadStripMap()  .end()) {
      ATH_MSG_DEBUG ( "Not In CoolDB the Panel IdPhiRpcStrip :  "  << IdPhi << " i.e. "<<m_idHelper->show_to_string(IdPhi));
      noEntryInDb=true;
    }
    else
      {
	ATH_MSG_DEBUG ( "Found In CoolDB the Panel IdPhiRpcStrip :  "  << IdPhi << " i.e. "<<m_idHelper->show_to_string(IdPhi));
      }
    
    if( m_rSummarySvc->RPC_FracDeadStripMap()  .find(IdEta) != m_rSummarySvc->RPC_FracDeadStripMap()  .end()) FracDeadStripEta        = m_rSummarySvc->RPC_FracDeadStripMap  ().find(IdEta)->second ;
    if( m_rSummarySvc->RPC_FracDeadStripMap()  .find(IdPhi) != m_rSummarySvc->RPC_FracDeadStripMap()  .end()) FracDeadStripPhi        = m_rSummarySvc->RPC_FracDeadStripMap  ().find(IdPhi)->second ;
    if( m_rSummarySvc->RPC_ProjectedTracksMap().find(IdEta) != m_rSummarySvc->RPC_ProjectedTracksMap().end()) RPC_ProjectedTracksEta  = m_rSummarySvc->RPC_ProjectedTracksMap().find(IdEta)->second ;
    

    if( m_rSummarySvc->RPC_EfficiencyMap()     .find(IdEta) != m_rSummarySvc->RPC_EfficiencyMap()     .end()) EtaPanelEfficiency      = m_rSummarySvc->RPC_EfficiencyMap     ().find(IdEta)->second ;
    if( m_rSummarySvc->RPC_EfficiencyMap()     .find(IdPhi) != m_rSummarySvc->RPC_EfficiencyMap()     .end()) PhiPanelEfficiency      = m_rSummarySvc->RPC_EfficiencyMap     ().find(IdPhi)->second ;
    if( m_rSummarySvc->RPC_EfficiencyGapMap()  .find(IdEta) != m_rSummarySvc->RPC_EfficiencyGapMap()  .end()) GapEfficiency	        = m_rSummarySvc->RPC_EfficiencyGapMap  ().find(IdEta)->second ;

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

    // if(m_rSummarySvc->RPC_DeadStripList().find( *IdEtaRpcStrip)  != m_rSummarySvc->RPC_DeadStripList().end()) stripetadead = m_rSummarySvc->RPC_DeadStripList().find( *IdEtaRpcStrip)->second; // not used
    // if(m_rSummarySvc->RPC_DeadStripList().find( *IdPhiRpcStrip)  != m_rSummarySvc->RPC_DeadStripList().end()) stripphidead = m_rSummarySvc->RPC_DeadStripList().find( *IdPhiRpcStrip)->second; // not used

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

  float I0	  = PhiAndEtaEff			    ;
  float I1	  = PhiAndEtaEff +  OnlyEtaEff  	    ;
  float ITot   = PhiAndEtaEff +  OnlyEtaEff + OnlyPhiEff    ;

  float GapEff = PhiAndEtaEff + OnlyEtaEff + OnlyPhiEff;
  float PhiEff = PhiAndEtaEff + OnlyPhiEff             ;
  float EtaEff = PhiAndEtaEff + OnlyEtaEff             ;

  ATH_MSG_DEBUG ( "DetectionEfficiency: Final Efficiency Values applied for "<<m_idHelper->show_to_string(*IdEtaRpcStrip)<<" are " << PhiAndEtaEff << "=PhiAndEtaEff " <<OnlyEtaEff << "=OnlyEtaEff " <<OnlyPhiEff << "=OnlyPhiEff " << GapEff << "=GapEff " << EtaEff << "=EtaEff " << PhiEff << "=PhiEff ");

  float rndmEff = CLHEP::RandFlat::shoot(m_rndmEngine, 1) ;

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
int RpcDigitizationTool::ClusterSizeEvaluation(const Identifier* IdRpcStrip, float xstripnorm) {

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
      ATH_MSG_ERROR ( "Index out of array in ClusterSizeEvaluation SideA " << index <<" statName "<<stationName) ;
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
	ATH_MSG_ERROR ( "Index out of array in ClusterSizeEvaluation SideC " << index <<" statName "<<stationName) ;
	return 1;
      }

      FracClusterSize1    = m_FracClusterSize1_C      [index];
      FracClusterSize2    = m_FracClusterSize2_C      [index];
      FracClusterSizeTail = m_FracClusterSizeTail_C   [index];
      MeanClusterSizeTail = m_MeanClusterSizeTail_C   [index];
    }
  }
  else{
    Identifier Id  = m_idHelper->panelID(*IdRpcStrip);

    int    RPC_ProjectedTracks = 0;

    if( m_rSummarySvc->RPC_ProjectedTracksMap().find(Id) != m_rSummarySvc->RPC_ProjectedTracksMap().end()) RPC_ProjectedTracks = m_rSummarySvc->RPC_ProjectedTracksMap().find(Id)->second ;

    if(m_rSummarySvc->RPC_FracClusterSize1Map().find(Id) != m_rSummarySvc->RPC_FracClusterSize1Map().end()) FracClusterSize1	= float(m_rSummarySvc->RPC_FracClusterSize1Map().find(Id)->second) ;
    else ATH_MSG_INFO ( "FracClusterSize1 entry not found for id = " <<m_idHelper->show_to_string(*IdRpcStrip)<<" default will be used") ;
    if(m_rSummarySvc->RPC_FracClusterSize2Map().find(Id) != m_rSummarySvc->RPC_FracClusterSize2Map().end()) FracClusterSize2	= float(m_rSummarySvc->RPC_FracClusterSize2Map().find(Id)->second) ;
    else ATH_MSG_INFO ( "FracClusterSize2 entry not found for id = " <<m_idHelper->show_to_string(*IdRpcStrip)<<" default will be used") ;

    ATH_MSG_DEBUG ( "FracClusterSize1 and 2 "<< FracClusterSize1 << " " << FracClusterSize2  );

    FracClusterSizeTail = 1. - FracClusterSize1 - FracClusterSize2 ;

    if(m_rSummarySvc->RPC_MeanClusterSizeMap().find(Id) != m_rSummarySvc->RPC_MeanClusterSizeMap().end()) MeanClusterSize     = float(m_rSummarySvc->RPC_MeanClusterSizeMap().find(Id)->second)  ;
    else ATH_MSG_INFO ( "MeanClusterSize entry not found for id = " <<m_idHelper->show_to_string(*IdRpcStrip)<<" default will be used") ;

    MeanClusterSizeTail = MeanClusterSize - FracClusterSize1 - 2*FracClusterSize2 ;

    ATH_MSG_DEBUG ( "MeanClusterSizeTail and FracClusterSizeTail "<< MeanClusterSizeTail << " " << FracClusterSizeTail  );

    //if clustersize have anomalous values set to the average cluster size from joboption
    if(RPC_ProjectedTracks<m_CutProjectedTracks ||  RPC_ProjectedTracks>10000000 || MeanClusterSize>m_CutMaxClusterSize || MeanClusterSize<=1 || FracClusterSizeTail < 0 || FracClusterSize1 < 0 || FracClusterSize2 < 0  || FracClusterSizeTail > 1 || FracClusterSize1 > 1 || FracClusterSize2 >1){
      index += m_FracClusterSize1_A.size()/2*measuresPhi ;
      if( index>m_FracClusterSize1_A.size()    || index>m_FracClusterSize2_A.size() ||
	  index>m_FracClusterSizeTail_A.size() || index>m_MeanClusterSizeTail_A.size() ) {
	ATH_MSG_ERROR ( "Index out of array in ClusterSizeEvaluation SideA " << index << " statName "<<stationName) ;
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
	  ATH_MSG_ERROR ( "Index out of array in ClusterSizeEvaluation SideC " << index << " statName "<<stationName ) ;
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

  float rndmCS = CLHEP::RandFlat::shoot(m_rndmEngine, ITot ) ;

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
      float rndmCS1_2 = CLHEP::RandFlat::shoot(m_rndmEngine, 1 ) ;
      ClusterSize =  1 ;
      if(rndmCS1_2<FracClusterSize2norm)ClusterSize =  2 ;
    }

  }
  else if( ( FracClusterSize1plus2 <= rndmCS ) && ( rndmCS <= ITot ) ){
    ClusterSize = m_FirstClusterSizeInTail  ;
    ClusterSize += int(CLHEP::RandExponential::shoot(m_rndmEngine, MeanClusterSizeTail )) ;
    float rndmLR = CLHEP::RandFlat::shoot(m_rndmEngine, 1.0 ) ;
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
    ATH_MSG_INFO ( "size of RPC calib vector PhiAndEtaEff_A: " << m_PhiAndEtaEff_A.at(i) );
  }
  vec_size = m_OnlyPhiEff_A.size() ;
  for(int i=0 ; i!= vec_size ; i++ ){
    ATH_MSG_INFO ( "size of RPC calib vector OnlyPhi_A: " << m_OnlyPhiEff_A.at(i) );
  }
  vec_size = m_OnlyEtaEff_A.size() ;
  for(int i=0 ; i!= vec_size ; i++ ){
    ATH_MSG_INFO ( "size of RPC calib vector OnlyEta_A: " << m_OnlyEtaEff_A.at(i) );
  }

  vec_size = m_PhiAndEtaEff_C.size() ;
  for(int i=0 ; i!= vec_size ; i++ ){
    ATH_MSG_INFO ( "size of RPC calib vector PhiAndEtaEff_C: " << m_PhiAndEtaEff_C.at(i) );
  }
  vec_size = m_OnlyPhiEff_C.size() ;
  for(int i=0 ; i!= vec_size ; i++ ){
    ATH_MSG_INFO ( "size of RPC calib vector OnlyPhi_C: " << m_OnlyPhiEff_C.at(i) );
  }
  vec_size = m_OnlyEtaEff_C.size() ;
  for(int i=0 ; i!= vec_size ; i++ ){
    ATH_MSG_INFO ( "size of RPC calib vector OnlyEta_C: " << m_OnlyPhiEff_C.at(i) );
  }
  vec_size = m_FracClusterSize1_A.size() ;
  for(int i=0 ; i!= vec_size ; i++ ){
    ATH_MSG_INFO ( "size of RPC calib vector FracClusterSize1_A: " << m_FracClusterSize1_A.at(i) );
  }
  vec_size = m_FracClusterSize2_A.size() ;
  for(int i=0 ; i!= vec_size ; i++ ){
    ATH_MSG_INFO ( "size of RPC calib vector FracClusterSize2_A: " << m_FracClusterSize2_A.at(i) );
  }
  vec_size = m_FracClusterSizeTail_A.size() ;
  for(int i=0 ; i!= vec_size ; i++ ){
    ATH_MSG_INFO ( "size of RPC calib vector FracClusterSizeTail_A: " << m_FracClusterSizeTail_A.at(i) );
  }
  vec_size = m_MeanClusterSizeTail_A.size() ;
  for(int i=0 ; i!= vec_size ; i++ ){
    ATH_MSG_INFO ( "size of RPC calib vector MeanClusterSizeTail_A: " << m_MeanClusterSizeTail_A.at(i) );
  }
  vec_size = m_FracClusterSize1_C.size() ;
  for(int i=0 ; i!= vec_size ; i++ ){
    ATH_MSG_INFO ( "size of RPC calib vector FracClusterSize1_C: " << m_FracClusterSize1_C.at(i) );
  }
  vec_size = m_FracClusterSize2_C.size() ;
  for(int i=0 ; i!= vec_size ; i++ ){
    ATH_MSG_INFO ( "size of RPC calib vector FracClusterSize2_C: " << m_FracClusterSize2_C.at(i) );
  }
  vec_size = m_FracClusterSizeTail_C.size() ;
  for(int i=0 ; i!= vec_size ; i++ ){
    ATH_MSG_INFO ( "size of RPC calib vector FracClusterSizeTail_C: " << m_FracClusterSizeTail_C.at(i) );
  }
  vec_size = m_MeanClusterSizeTail_C.size() ;
  for(int i=0 ; i!= vec_size ; i++ ){
    ATH_MSG_INFO ( "size of RPC calib vector MeanClusterSizeTail_C: " << m_MeanClusterSizeTail_C.at(i) );
  }

  return sc;

}

//--------------------------------------------
StatusCode RpcDigitizationTool::DumpRPCCalibFromCoolDB() {

  ATH_MSG_DEBUG ( "RpcDigitizationTool::in DumpRPCCalibFromCoolDB" );

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
  std::vector<Identifier>            ::const_iterator itrv;

  ATH_MSG_DEBUG ( "Size Summary RPC_EfficiencyMap: " << m_rSummarySvc->RPC_EfficiencyMap().size() );
  for (itr=m_rSummarySvc->RPC_EfficiencyMap().begin(); itr!=m_rSummarySvc->RPC_EfficiencyMap().end(); ++itr) {
    if( itr->second == 0.   )NpanelEff0++       ;
    if( itr->second  < 0.5  )NpanelEffLess05++  ;
    if( itr->second  < 1.   )NpanelEffLess1++   ;
    if( itr->second == 1.   )NpanelEff1++       ;

    ATH_MSG_DEBUG ( "Summary Id/RPC_EfficiencyMap: " << itr->first <<" i.e. "
		    <<(itr->first).get_identifier32().get_compact()<<" "
		    <<m_idHelper->show_to_string(itr->first)<< " "<< itr->second );
  }


  ATH_MSG_DEBUG ( "Size Summary RPC_EfficiencyGapMap: " << m_rSummarySvc->RPC_EfficiencyGapMap().size() );
  for (itr=m_rSummarySvc->RPC_EfficiencyGapMap().begin(); itr!=m_rSummarySvc->RPC_EfficiencyGapMap().end(); ++itr) {

    ATH_MSG_DEBUG ( "Summary Id/RPC_EfficiencyGapMap: " << itr->first <<" i.e. "
		    <<(itr->first).get_identifier32().get_compact()<<" "
		    <<m_idHelper->show_to_string(itr->first)<< " " << itr->second );
  }


  ATH_MSG_DEBUG ( "Size Summary RPC_MeanClusterSizeMap: " << m_rSummarySvc->RPC_MeanClusterSizeMap().size() );
  for (itr=m_rSummarySvc->RPC_MeanClusterSizeMap().begin(); itr!=m_rSummarySvc->RPC_MeanClusterSizeMap().end(); ++itr) {
    if( itr->second ==  1.0  )NpanelCSEq1++     ;
    if( itr->second  <  1.5  )NpanelCSLess15++  ;
    if( itr->second  <  2.0  )NpanelCSLess20++  ;
    if( itr->second  <  2.5  )NpanelCSLess25++  ;
    if( itr->second >=  5.0  )NpanelCSMore5++   ;

    ATH_MSG_DEBUG ( "Summary Id/RPC_MeanClusterSizeMap: " << itr->first <<" i.e. "
		    <<(itr->first).get_identifier32().get_compact()<<" "
		    <<m_idHelper->show_to_string(itr->first)<< " " << itr->second );
  }


  ATH_MSG_DEBUG ( "Size Summary RPC_FracClusterSize1Map: " << m_rSummarySvc->RPC_FracClusterSize1Map().size() );
  for (itr=m_rSummarySvc->RPC_FracClusterSize1Map().begin(); itr!=m_rSummarySvc->RPC_FracClusterSize1Map().end(); ++itr) {

    ATH_MSG_DEBUG ( "Summary Id/RPC_FracClusterSize1Map: " << itr->first <<" i.e. "
		    <<(itr->first).get_identifier32().get_compact()<<" "
		    <<m_idHelper->show_to_string(itr->first)<< " " << itr->second );
  }


  ATH_MSG_DEBUG ( "Size Summary RPC_FracClusterSize2Map: " << m_rSummarySvc->RPC_FracClusterSize2Map().size() );
  for (itr=m_rSummarySvc->RPC_FracClusterSize2Map().begin(); itr!=m_rSummarySvc->RPC_FracClusterSize2Map().end(); ++itr) {

    ATH_MSG_DEBUG ( "Summary Id/RPC_FracClusterSize2Map: " << itr->first <<" i.e. "
		    <<(itr->first).get_identifier32().get_compact()<<" "
		    <<m_idHelper->show_to_string(itr->first)<< " " << itr->second );
  }


  ATH_MSG_DEBUG ( "Size Summary RPC_DeadStripListMap: " << m_rSummarySvc->RPC_DeadStripListMap().size() );
  for (itrs=m_rSummarySvc->RPC_DeadStripListMap().begin(); itrs!=m_rSummarySvc->RPC_DeadStripListMap().end(); ++itrs) {
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


  ATH_MSG_DEBUG ( "Size Summary RPC_FracDeadStripMap: " << m_rSummarySvc->RPC_FracDeadStripMap().size() );
  for (itrf=m_rSummarySvc->RPC_FracDeadStripMap().begin(); itrf!=m_rSummarySvc->RPC_FracDeadStripMap().end(); ++itrf) {
    if( itrf->second == 0.   )NpanelFracDead0++       ;
    if( itrf->second  < 0.5  )NpanelFracDeadLess05++  ;
    if( itrf->second  < 1.   )NpanelFracDeadLess1++   ;
    if( itrf->second == 1.   )NpanelFracDead1++       ;

    ATH_MSG_DEBUG ( "Summary Id/RPC_FracDeadStripMap: " << itrf->first << " i.e. " 
		    <<(itrf->first).get_identifier32().get_compact()<<" "
		    <<m_idHelper->show_to_string(itrf->first)<< " " 
		    << itrf->second );
    if (itrf->second > 0.50) {
      if (itrf->second <= 0.75)
	ATH_MSG_DEBUG ( "Summary Id/RPC_FracDeadStripMap: panel with fraction of dead strips >50% and <=75% " << itrf->first << " i.e. " 
			<<(itrf->first).get_identifier32().get_compact()<<" "
			<<m_idHelper->show_to_string(itrf->first)<< " " 
			<< itrf->second );
      else if (itrf->second > 0.75) {
	if (itrf->second <= 0.90)
	  ATH_MSG_DEBUG ( "Summary Id/RPC_FracDeadStripMap: panel with fraction of dead strips >75% and <=90% " << itrf->first << " i.e. " 
			  <<(itrf->first).get_identifier32().get_compact()<<" "
			  <<m_idHelper->show_to_string(itrf->first)<< " " 
			  << itrf->second );
	else if (itrf->second > 0.90) ATH_MSG_DEBUG ( "Summary Id/RPC_FracDeadStripMap: panel with fraction of dead strips >90% " << itrf->first << " i.e. " 
						      <<(itrf->first).get_identifier32().get_compact()<<" "
						      <<m_idHelper->show_to_string(itrf->first)<< " " 
						      << itrf->second );
      }
    }
  }


  ATH_MSG_DEBUG ( "Size Summary RPC_ProjectedTracksMap: " << m_rSummarySvc->RPC_ProjectedTracksMap().size() );
  for (itri=m_rSummarySvc->RPC_ProjectedTracksMap().begin(); itri!=m_rSummarySvc->RPC_ProjectedTracksMap().end(); ++itri) {
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
  ATH_MSG_DEBUG ( "Size Summary RPC_DeadStripList: " << m_rSummarySvc->RPC_DeadStripList().size() );
  for (itri=m_rSummarySvc->RPC_DeadStripList().begin(); itri!=m_rSummarySvc->RPC_DeadStripList().end(); ++itri) {
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

		const RpcReadoutElement* rpc = m_GMmgr->getRpcRElement_fromIdFields(stationName, stationEta, stationPhi, doubletR, doubletZ, doubletPhi);
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



		if( m_rSummarySvc->RPC_EfficiencyMap            ().find(atlasIdEta)!= m_rSummarySvc->RPC_EfficiencyMap().end()       ) efficiencyEta     = m_rSummarySvc->RPC_EfficiencyMap      ().find(atlasIdEta)->second ;
		if( m_rSummarySvc->RPC_MeanClusterSizeMap       ().find(atlasIdEta)!= m_rSummarySvc->RPC_MeanClusterSizeMap().end()  ) averageCSEta      = m_rSummarySvc->RPC_MeanClusterSizeMap ().find(atlasIdEta)->second ;
		if( m_rSummarySvc->RPC_FracClusterSize1Map      ().find(atlasIdEta)!= m_rSummarySvc->RPC_FracClusterSize1Map().end() ) FracCS1Eta        = m_rSummarySvc->RPC_FracClusterSize1Map().find(atlasIdEta)->second ;
		if( m_rSummarySvc->RPC_FracClusterSize2Map      ().find(atlasIdEta)!= m_rSummarySvc->RPC_FracClusterSize2Map().end() ) FracCS2Eta        = m_rSummarySvc->RPC_FracClusterSize2Map().find(atlasIdEta)->second ;
		FracCStailEta     = 1.-FracCS1Eta-FracCS2Eta ;
		if( m_rSummarySvc->RPC_ProjectedTracksMap       ().find(atlasIdEta)!= m_rSummarySvc->RPC_ProjectedTracksMap().end()  ) ProjectedTracksEta= m_rSummarySvc->RPC_ProjectedTracksMap ().find(atlasIdEta)->second ;
		if( m_rSummarySvc->RPC_EfficiencyGapMap         ().find(atlasIdEta)!= m_rSummarySvc->RPC_EfficiencyGapMap().end()    ) efficiencygapEta  = m_rSummarySvc->RPC_EfficiencyGapMap   ().find(atlasIdEta)->second ;

		if( m_rSummarySvc->RPC_EfficiencyMap            ().find(atlasIdPhi)!= m_rSummarySvc->RPC_EfficiencyMap().end()       ) efficiencyPhi     = m_rSummarySvc->RPC_EfficiencyMap      ().find(atlasIdPhi)->second ;
		if( m_rSummarySvc->RPC_MeanClusterSizeMap       ().find(atlasIdPhi)!= m_rSummarySvc->RPC_MeanClusterSizeMap().end()  ) averageCSPhi      = m_rSummarySvc->RPC_MeanClusterSizeMap ().find(atlasIdPhi)->second ;
		if( m_rSummarySvc->RPC_FracClusterSize1Map      ().find(atlasIdPhi)!= m_rSummarySvc->RPC_FracClusterSize1Map().end() ) FracCS1Phi        = m_rSummarySvc->RPC_FracClusterSize1Map().find(atlasIdPhi)->second ;
		if( m_rSummarySvc->RPC_FracClusterSize2Map      ().find(atlasIdPhi)!= m_rSummarySvc->RPC_FracClusterSize2Map().end() ) FracCS2Phi        = m_rSummarySvc->RPC_FracClusterSize2Map().find(atlasIdPhi)->second ;
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

		  const RpcReadoutElement* rpc = m_GMmgr->getRpcRElement_fromIdFields(stationName, stationEta, stationPhi, doubletR, doubletZ, doubletPhi);
		  if(rpc == 0 )continue;
		  Identifier idr = rpc->identify();
		  if(idr == 0 )continue;
		  Identifier atlasId = m_idHelper->channelID(idr, doubletZ,doubletPhi , gasGap, measphi, 1)     ;
		  if(atlasId == 0 )continue;

		  if(m_rSummarySvc->RPC_EfficiencyMap().find(atlasId)->second == 1){
		    ATH_MSG_VERBOSE ( "Effi RPC panel = 1: " <<m_rSummarySvc->RPC_DeadStripListMap().find(atlasId)->second<<" "<<m_rSummarySvc->RPC_ProjectedTracksMap().find(atlasId)->second<<" sName "<<stationName<<" sEta " <<stationEta<<" sPhi "<<stationPhi<<" dR "<<doubletR<<" dZ "<<doubletZ<<" dPhi "<<doubletPhi<<" Gap "<<gasGap<<" view "<<measphi);
		  }
		  if(m_rSummarySvc->RPC_EfficiencyMap().find(atlasId)->second == 0){
		    ATH_MSG_VERBOSE ( "Effi RPC panel = 0: " <<m_rSummarySvc->RPC_DeadStripListMap().find(atlasId)->second<<" "<<m_rSummarySvc->RPC_ProjectedTracksMap().find(atlasId)->second<<" sName "<<stationName<<" sEta " <<stationEta<<" sPhi "<<stationPhi<<" dR "<<doubletR<<" dZ "<<doubletZ<<" dPhi "<<doubletPhi<<" Gap "<<gasGap<<" view "<<measphi);
		  }
		  if(m_rSummarySvc->RPC_EfficiencyGapMap().find(atlasId)->second == 0){
		    ATH_MSG_VERBOSE ( "EffiGap RPC panel = 0: " <<m_rSummarySvc->RPC_DeadStripListMap().find(atlasId)->second<<" "<<m_rSummarySvc->RPC_ProjectedTracksMap().find(atlasId)->second<<" sName "<<stationName<<" sEta " <<stationEta<<" sPhi "<<stationPhi<<" dR "<<doubletR<<" dZ "<<doubletZ<<" dPhi "<<doubletPhi<<" Gap "<<gasGap<<" view "<<measphi);
		  }
		  if(m_rSummarySvc->RPC_MeanClusterSizeMap().find(atlasId)->second > 3 ){
		    ATH_MSG_VERBOSE ( "MeanClusterSize RPC panel > 3: " <<m_rSummarySvc->RPC_MeanClusterSizeMap().find(atlasId)->second<<" "<<m_rSummarySvc->RPC_ProjectedTracksMap().find(atlasId)->second<<" sName "<<stationName<<" sEta " <<stationEta<<" sPhi "<<stationPhi<<" dR "<<doubletR<<" dZ "<<doubletZ<<" dPhi "<<doubletPhi<<" Gap "<<gasGap<<" view "<<measphi);
		  }
		  if(m_rSummarySvc->RPC_FracDeadStripMap().find(atlasId)->second == 1){
		    ATH_MSG_VERBOSE ( "Dead RPC panel: " <<m_rSummarySvc->RPC_DeadStripListMap().find(atlasId)->second<<" "<<m_rSummarySvc->RPC_ProjectedTracksMap().find(atlasId)->second<<" sName "<<stationName<<" sEta " <<stationEta<<" sPhi "<<stationPhi<<" dR "<<doubletR<<" dZ "<<doubletZ<<" dPhi "<<doubletPhi<<" Gap "<<gasGap<<" view "<<measphi);
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

		  const RpcReadoutElement* rpc = m_GMmgr->getRpcRElement_fromIdFields(stationName, stationEta, stationPhi, doubletR, doubletZ, doubletPhi);
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

		  if( m_rSummarySvc->RPC_EfficiencyMap            ().find(atlasId)!= m_rSummarySvc->RPC_EfficiencyMap().end()       ) efficiency     = m_rSummarySvc->RPC_EfficiencyMap      ().find(atlasId)->second ;
		  if( m_rSummarySvc->RPC_EfficiencyGapMap         ().find(atlasId)!= m_rSummarySvc->RPC_EfficiencyGapMap().end()    ) efficiencygap  = m_rSummarySvc->RPC_EfficiencyGapMap   ().find(atlasId)->second ;
		  if( m_rSummarySvc->RPC_MeanClusterSizeMap       ().find(atlasId)!= m_rSummarySvc->RPC_MeanClusterSizeMap().end()  ) averageCS      = m_rSummarySvc->RPC_MeanClusterSizeMap ().find(atlasId)->second ;
		  if( m_rSummarySvc->RPC_FracClusterSize1Map      ().find(atlasId)!= m_rSummarySvc->RPC_FracClusterSize1Map().end() ) FracCS1        = m_rSummarySvc->RPC_FracClusterSize1Map().find(atlasId)->second ;
		  if( m_rSummarySvc->RPC_FracClusterSize2Map      ().find(atlasId)!= m_rSummarySvc->RPC_FracClusterSize2Map().end() ) FracCS2        = m_rSummarySvc->RPC_FracClusterSize2Map().find(atlasId)->second ;
		  FracCStail     = 1.0-FracCS1-FracCS2 ;
		  averageCStail  = averageCS - FracCS1 - 2*FracCS2                                ;
		  if( m_rSummarySvc->RPC_ProjectedTracksMap       ().find(atlasId)!= m_rSummarySvc->RPC_ProjectedTracksMap().end()  ) ProjectedTracks= m_rSummarySvc->RPC_ProjectedTracksMap ().find(atlasId)->second ;
		  if( m_rSummarySvc->RPC_FracDeadStripMap         ().find(atlasId)!= m_rSummarySvc->RPC_FracDeadStripMap   ().end() ) FracDeadStrip  = m_rSummarySvc->RPC_FracDeadStripMap   ().find(atlasId)->second ;

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

  ATH_MSG_VERBOSE( "Number of dead strip " << m_rSummarySvc->RPC_DeadStripList ().size() );
  for( int stationName =  2 ;  stationName !=11;  stationName++){
    for( int stationEta  = -7 ;  stationEta  != 8;  stationEta++ ){
      for( int stationPhi  =  1 ;  stationPhi  != 9;  stationPhi++ ){
	for( int doubletR    =  1 ;  doubletR    != 3;  doubletR++   ){
	  for( int doubletZ    =  1 ;  doubletZ    != 4;  doubletZ++   ){
	    for( int doubletPhi  =  1 ;  doubletPhi  != 3;  doubletPhi++ ){
	      for( int gasGap      =  1 ;  gasGap	  != 3;  gasGap++     ){
		for( int measphi     =  0 ;  measphi	  != 2;  measphi++    ){
		  for( int strip       =  1 ;  strip	  !=81;  strip++      ){

		    const RpcReadoutElement* rpc = m_GMmgr->getRpcRElement_fromIdFields(stationName, stationEta, stationPhi, doubletR, doubletZ, doubletPhi);
		    if(rpc == 0 )continue;
		    Identifier idr = rpc->identify();
		    if(idr == 0 )continue;
		    Identifier atlasId = m_idHelper->channelID(idr, doubletZ,doubletPhi , gasGap, measphi, strip)     ;
		    if(atlasId == 0 )continue;
		    //if( m_rSummarySvc->RPC_DeadStripList().find(atlasId).empty() )continue;
		    int stripstatus	      = m_rSummarySvc->RPC_DeadStripList      ().find(atlasId)->second ;
		    if( stripstatus != 1 )continue;
		    ATH_MSG_VERBOSE( "Identifier " << atlasId << " sName "<<stationName<<" sEta " <<stationEta<<" sPhi "<<stationPhi<<" dR "<<doubletR<<" dZ "<<doubletZ<<" dPhi "<<doubletPhi<<" Gap "<<gasGap<<" view "<<measphi<<" strip "<<strip << " stripstatus "<<stripstatus );
		    //std::cout<<"Identifier " << atlasId << " sName "<<stationName<<" sEta " <<stationEta<<" sPhi "<<stationPhi<<" dR "<<doubletR<<" dZ "<<doubletZ<<" dPhi "<<doubletPhi<<" Gap "<<gasGap<<" view "<<measphi<<" strip "<<strip << " stripstatus "<<stripstatus << std::endl;;

		  }}}}}}}}}
  return sc;

}

double time_correction(double x, double y, double z)
{
  double speed_of_light = 299.792458;     // mm/ns
  return sqrt(x*x+y*y+z*z)/speed_of_light; //FIXME use CLHEP::c_light
}
