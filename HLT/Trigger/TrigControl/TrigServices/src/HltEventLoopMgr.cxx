/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi includes
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/IssueSeverity.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ThreadGaudi.h"
#include "GaudiKernel/Timing.h"
#include "GaudiKernel/IAlgContextSvc.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/ServiceLocatorHelper.h"
#include "GaudiKernel/SmartIF.h"

// Athena includes
#include "StoreGate/StoreGateSvc.h"
#include "EventInfo/EventIncident.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"
#include "xAODEventInfo/EventAuxInfo.h"
#include "ByteStreamData/ByteStreamMetadata.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CLIDSvc/CLASS_DEF.h"

// omni redefines this macro, so we save it
namespace {
static std::string CMT_PACKAGE_VERSION = PACKAGE_VERSION;
}

// Trigger includes
#include "TrigServices/HltEventLoopMgr.h"
#include "TrigServices/TrigISHelper.h"
#include "TrigServices/TrigHLTIssues.h"
#include "TrigPreFlightCheck.h"
#include "TrigCOOLUpdateHelper.h"
#include "TrigSORFromPtreeHelper.h"
#include "TrigConfInterfaces/IHLTConfigSvc.h"
#include "TrigInterfaces/Incidents.h"
#include "TrigMonitorBase/TrigLockedHist.h"
#include "TrigKernel/HltAcceptFlag.h"
#include "TrigKernel/HltResultStatusCode.h"
#include "TrigKernel/HltExtraStatusCode.h"
#include "TrigKernel/IHltTHistSvc.h"
#include "TrigSteeringEvent/HLTResult.h"
#include "TrigSteeringEvent/HLTExtraData.h"
#include "TrigNavigation/TrigEDMSizes.h"

// TDAQ includes
#include "ers/ers.h"
#include "eformat/eformat.h"
#include "eformat/write/ROBFragment.h"
#include "TTCInfo/LumiBlock.h"
#include "ddc/DdcFloatInfoNamed.h"
#include "hltinterface/HLTInterface.h"
#include "hltinterface/EventId.h"
#include "hltinterface/HLTResult.h"
#include "CTPfragment/CTPfragment.h"
#include "CTPfragment/CTPExtraWordsFormat.h"
#include "CTPfragment/Issue.h"

// ROOT includes
#include "TH1F.h"
#include "TH2I.h"
#include "TProfile.h"
#include "TClass.h"

// Other includes
#include <set>
#include <string>
#include <algorithm>
#include <ctime>
#include <mutex>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#undef PACKAGE_VERSION      // use the CMT_PACKAGE_VERSION std::string instead

#define ST_WHERE "HltEventLoopMgr::" << __func__ << "(): "

using namespace boost::property_tree;
using std::string;
using std::function;
using std::begin;
using std::end;
using SOR = TrigSORFromPtreeHelper::SOR;

namespace
{
  //=========================================================================
  const eformat::helper::Status SW_UNCLASSIFIED(eformat::UNCLASSIFIED, 0),
                                SW_FAIL(eformat::DATA_CORRUPTION, 0),
                                SW_TRUNCATION(eformat::INTERNAL_OVERFLOW, 0);
  constexpr unsigned 
      INITIAL_ROB_SIZE_MARGIN = 4; /* margin for the rob size, to account for 
                                      possible additional status words that may
                                      still be unknonw */

  //=========================================================================
  constexpr std::array<uint32_t, 7> L1R_MANDATORY_ROBS =
    {{
      0x7300a8, 0x7300a9, 0x7300aa, 0x7300ab, // TDAQ_CALO_CLUSTER_PROC_ROI ROBs
      0x7500ac, 0x7500ad,                     // TDAQ_CALO_JET_PROC_ROI ROBs
      0x760001                                // TDAQ_MUON_CTP_INTERFACE ROB
    }}; // default list of IDs of ROBs that must come in the L1R from the RoIB

  //=========================================================================
  constexpr std::array<uint32_t, 12> L1R_SKIP_ROB_CHECK =
    {{
      0x7300a8, 0x7300a9, 0x7300aa, 0x7300ab, // TDAQ_CALO_CLUSTER_PROC_ROI ROBs
      0x7500ac, 0x7500ad,                     // TDAQ_CALO_JET_PROC_ROI ROBs
      0x760001,                               // TDAQ_MUON_CTP_INTERFACE ROB
      0x770001,                               // TDAQ_CTP ROB
      0x910081, 0x910091, 0x910082, 0x910092  // TDAQ_CALO_TOPO_PROC ROBs
    }}; //concrete IDs still unknown for TDAQ_FTK and TDAQ_CALO_FEAT_EXTRACT_ROI

  //=========================================================================
  constexpr std::array<eformat::SubDetector, 7> L1R_SKIP_SD_CHECK =
    {{
      eformat::TDAQ_CALO_CLUSTER_PROC_ROI,
      eformat::TDAQ_CALO_JET_PROC_ROI,
      eformat::TDAQ_HLT,
      eformat::TDAQ_FTK,
      eformat::TDAQ_CALO_TOPO_PROC,
      eformat::TDAQ_CALO_DIGITAL_PROC,
      eformat::TDAQ_CALO_FEAT_EXTRACT_ROI
    }};

  //=========================================================================
  constexpr std::array<eformat::SubDetector, 8> L1R_BINS =
    {{
      eformat::TDAQ_CALO_CLUSTER_PROC_ROI,
      eformat::TDAQ_CALO_JET_PROC_ROI,
      eformat::TDAQ_MUON_CTP_INTERFACE,
      eformat::TDAQ_CTP,
      eformat::TDAQ_FTK,
      eformat::TDAQ_CALO_TOPO_PROC,
      eformat::TDAQ_CALO_DIGITAL_PROC,
      eformat::TDAQ_CALO_FEAT_EXTRACT_ROI
    }};

  //=========================================================================
  constexpr const char * UNEXPECTED_L1R_ROB_LABEL = "Unexpected_ROB";
  constexpr const char * MISSING_L1R_CTP_LABEL = "Missing_CTP_Fragment";

  //=========================================================================
  std::mutex timeout_mutex; // To synchronize main and time out threads

  //=========================================================================
  bool skipEnabledCheck(eformat::SubDetector sd)
  {
    // Skip check for HLT Result ROBs (for DataScouting)
    if (sd == eformat::TDAQ_HLT) return true;
    
    // Skip check for certain L1 input ROBs
    return std::find(begin(L1R_SKIP_SD_CHECK),
                     end(L1R_SKIP_SD_CHECK),
                     sd) != end(L1R_SKIP_SD_CHECK);
  }

  //=========================================================================
  bool skipEnabledCheck(uint32_t robid)
  {
    // Skip check for HLT Result ROBs (for DataScouting)
    if (eformat::helper::SourceIdentifier(robid).subdetector_id() == eformat::TDAQ_HLT) return true;
    
    // Skip check for certain L1 input ROBs
    return std::find(begin(L1R_SKIP_ROB_CHECK),
                     end(L1R_SKIP_ROB_CHECK),
                     robid) != end(L1R_SKIP_ROB_CHECK);
  }

  //=========================================================================
  std::string
  missingL1RobsMsg(const std::vector<uint32_t>& rob_ids)
  {
    std::ostringstream ost;
    if(!rob_ids.empty())
    {
      ost << "L1 result ROBs missing: ";

      ost << "0x" << std::hex << std::setfill('0') << std::setw(6);
      std::copy(std::begin(rob_ids),
                std::end(rob_ids) - 1, // leave one to avoid delimiter
                std::ostream_iterator<uint32_t>{ost, ", 0x"});
      ost << rob_ids.back() << "."; // append without delimiter
    }

    return ost.str();
  }

  //=========================================================================
  void fillEdmHist(TProfile * hist, const std::string& colname,
                   const HLT::TrigEDMSizes::EDMObjectInfo& info)
  {
    if(hist)
      hist->Fill(colname.c_str(), static_cast<double>(info.words));
  }

  //=========================================================================
  void addDebugStreamTag(hltinterface::HLTResult& hlt_result,
                         const std::string& tagname)
  {
    hlt_result.stream_tag.emplace_back(tagname, eformat::DEBUG_TAG, false);
  }

  //=========================================================================
  std::unique_ptr<eformat::write::ROBFragment>
  initRob(uint16_t modid, uint32_t rn, uint32_t l1id, uint32_t bcid,
          uint32_t l1tt, uint32_t dett, uint32_t * status_words)
  {
    using namespace eformat;
    using write::ROBFragment;

    auto src = helper::SourceIdentifier{TDAQ_HLT, modid}.code();
    auto robp = std::unique_ptr<ROBFragment>{
        new ROBFragment{src, rn, l1id, bcid, l1tt, dett, 0, 0, STATUS_FRONT}};

    status_words[0] = SW_UNCLASSIFIED.code();
    status_words[1] = hltonl::NORMAL_HLT_RESULT;
    status_words[2] = hltonl::PSC_ERROR_UNCLASSIFIED;

    robp->status(1, status_words);
    robp->rod_status(1, status_words);

    return robp;
  }

  //=========================================================================
  // Class that resets a pointer upon destruction, locking given mutex
  template <typename T, typename MUTEX>
  class PtrResetter
  {
  public:
    explicit PtrResetter(T*& ptr, MUTEX& mut)
      : m_ptr{ptr}
      , m_mut(mut) // cannot use uniform init due to gcc4.8 bug
    {}
    ~PtrResetter() // this is where the work happens
    {
      std::lock_guard<MUTEX>{m_mut};
      m_ptr = nullptr;
    }
    PtrResetter(const PtrResetter&) = delete;
    PtrResetter operator=(const PtrResetter&) = delete;
    // no need to delete move ops explicitly in this case - not generated

  private:
    T*& m_ptr;
    MUTEX& m_mut;
  };
}


//=========================================================================
// Standard Constructor
//=========================================================================
HltEventLoopMgr::HltEventLoopMgr(const std::string& nam,
    ISvcLocator* svcLoc)
: MinimalEventLoopMgr(nam, svcLoc),
  m_incidentSvc ( "IncidentSvc",  nam ),
  m_evtStore( "StoreGateSvc", nam ),
  m_detectorStore( "DetectorStore", nam ),
  m_inputMetaDataStore( "StoreGateSvc/InputMetaDataStore", nam ),
  m_robDataProviderSvc( "ROBDataProviderSvc",nam ),
  m_THistSvc( "THistSvc", nam ),
  m_isHelper( "TrigISHelper", this),
  m_coolHelper( "TrigCOOLUpdateHelper", this),
  m_sorTime_stamp(2,0),
  m_detector_mask(0xffffffff, 0xffffffff, 0, 0),
  m_l1_hltPrescaleUpdateLB(0xffffffff),
  m_mandatoryL1ROBs{{begin(L1R_MANDATORY_ROBS), end(L1R_MANDATORY_ROBS)}},
  m_histProp_Hlt_result_size(Gaudi::Histo1DDef("HltResultSize",0,500000,100)),
  m_histProp_Hlt_result_size_physics(Gaudi::Histo1DDef("HltResultSize-(Stream (Main_physics))",0,500000,100)),
  m_histProp_Hlt_result_size_express(Gaudi::Histo1DDef("HltResultSize-(Stream (express_express))",0,250000,100)),
  m_histProp_Hlt_result_size_DataScouting(Gaudi::Histo1DDef("HltResultSize-(Streams (DataScouting_*_calibration))",0,125000,100)),
  m_histProp_numStreamTags(Gaudi::Histo1DDef("NumberOfStreamTags",-.5,19.5,20)),
  m_histProp_streamTagNames(Gaudi::Histo1DDef("StreamTagNames",-.5,.5,1)),
  m_histProp_num_partial_eb_robs(Gaudi::Histo1DDef("NumberROBsPartialEB",-.5,199.5,200)),
  m_histProp_Hlt_Edm_Sizes(Gaudi::Histo1DDef("HltEDMSizes",0.,10000.,100))
{
  // General properties for event loop managers
  declareProperty("predefinedLumiBlock",      m_predefinedLumiBlock=0);
  declareProperty("Lvl1CTPROBid",             m_lvl1CTPROBid=0x770001);
  declareProperty("ApplicationName",          m_applicationName="None");
  declareProperty("PartitionName",            m_partitionName="None");
  declareProperty("setMagFieldFromPtree",     m_setMagFieldFromPtree=false);
  declareProperty("enabledROBs",              m_enabledROBs);
  declareProperty("enabledSubDetectors",      m_enabledSubDetectors);
  declareProperty("MandatoryL1ROBs",          m_mandatoryL1ROBs, "List of mandatory ROB IDs coming from the RoIB (must come in L1R seed)");
  declareProperty("HltEDMCollectionNames",    m_hltEdmCollectionNames, "Names of all HLT EDM Collections");
  declareProperty("JobOptionsType",           m_jobOptionsType="NONE");
  declareProperty("doMonitoring",             m_doMonitoring=true, "Produce framework monitoring histograms");
  declareProperty("histHltResultSize",        m_histProp_Hlt_result_size, "Histogram for HLT result size in words (overall)");
  declareProperty("histHltResultSizePhysics", m_histProp_Hlt_result_size_physics, "Histogram for HLT result size in words for physics stream ");
  declareProperty("histHltResultSizeExpress", m_histProp_Hlt_result_size_express, "Histogram for HLT result size in words for express stream ");
  declareProperty("histHltResultSizeDS",      m_histProp_Hlt_result_size_DataScouting, "Histogram for HLT result size in words for DataScouting stream ");
  declareProperty("histNumberOfStreamTags",   m_histProp_numStreamTags, "Histogram with number of stream tags");
  declareProperty("histStreamTagNames",       m_histProp_streamTagNames,"Histogram with stream tag names");  
  declareProperty("histNumberROBsPartialEB",  m_histProp_num_partial_eb_robs, "Histogram with number of ROBs for PEB" );
  declareProperty("histHltEdmSizes",          m_histProp_Hlt_Edm_Sizes, "Histogram with sizes of HLT EDM collections" );
  declareProperty("ForceHltReject",           m_forceHltReject=false);
  declareProperty("ForceHltAccept",           m_forceHltAccept=false);
  declareProperty("HltResultName",            m_HltResultName="HLTResult_HLT");
  declareProperty("HltDebugStreamName",       m_HltDebugStreamName ="HLTMissingData");
  declareProperty("HltForcedStreamName",      m_HltForcedStreamName ="HLTEventAcceptForced");
  declareProperty("CoolUpdateTool",           m_coolHelper);
  declareProperty("maxPrepareForRunSleepSec", m_prepareForRunSleep = 0, "Max number of seconds to sleep at the beginning of prepareForRun");
  declareProperty("Lvl1CTPROBcheck",          m_lvl1CTPROBcheck=true);
  declareProperty("WriteTruncatedHLTtoDebug", m_writeHltTruncationToDebug=true);
  declareProperty("HltTruncationDebugStreamName",  m_HltTruncationDebugStreamName ="TruncatedHLTResult");
  declareProperty("ExcludeFromHltTruncationDebugStream",  m_excludeFromHltTruncationDebugStream );

  m_excludeFromHltTruncationDebugStream = {"CostMonitoring"};
}

//=========================================================================
// Standard Destructor
//=========================================================================
HltEventLoopMgr::~HltEventLoopMgr()
{
}

//=========================================================================
// implementation of IInterface: queryInterface
//=========================================================================
StatusCode HltEventLoopMgr::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if(!ppvInterface)
      return StatusCode::FAILURE;

  if(ITrigEventLoopMgr::interfaceID().versionMatch(riid))
    *ppvInterface = static_cast<ITrigEventLoopMgr*>(this);
  else
    return MinimalEventLoopMgr::queryInterface(riid, ppvInterface);

  addRef();
  return StatusCode::SUCCESS;
}

//=========================================================================
// implementation of IService::sysInitalize
//=========================================================================
StatusCode HltEventLoopMgr::sysInitialize()
{

  // initialize the base class
  StatusCode sc = MinimalEventLoopMgr::sysInitialize();
  sc.setChecked();
  return sc;
}

void HltEventLoopMgr::updateDFProps()
{
  ServiceHandle<IJobOptionsSvc> p_jobOptionsSvc("JobOptionsSvc", name());
  if ((p_jobOptionsSvc.retrieve()).isFailure()) {
    msgStream() << MSG::WARNING << "Could not find JobOptionsSvc to set "
                << "DataFlow properties" << endmsg;
  } else {
    auto dfprops = p_jobOptionsSvc->getProperties("DataFlowConfig");

    // Application name
    auto pname = "DF_ApplicationName";
    const auto * prop = Gaudi::Utils::getProperty(dfprops, pname);
    if(prop && m_applicationName.assign(*prop)) {
      msgStream() << MSG::DEBUG << " ---> Read from DataFlow configuration: "
                  << m_applicationName << endmsg;
    } else {
      msgStream() << MSG::WARNING << "Could not set Property '" << pname
                  << "' from DataFlow." << endmsg;
    }

    // Partition name
    pname = "DF_PartitionName";
    prop = Gaudi::Utils::getProperty(dfprops, pname);
    if (prop && m_partitionName.assign(*prop)) {
      msgStream() << MSG::DEBUG << " ---> Read from DataFlow configuration: "
                  << m_partitionName << endmsg;
    } else {
      msgStream() << MSG::WARNING << "Could not set Property '" << pname
                  << "' from DataFlow." << endmsg;
    }

    // get the list of enabled ROBs
    pname = "DF_Enabled_ROB_IDs";
    prop = Gaudi::Utils::getProperty(dfprops, pname);
    if (prop && m_enabledROBs.assign(*prop)) {
      msgStream() << MSG::DEBUG << " ---> Read from DataFlow configuration: "
                  << m_enabledROBs.value().size() << " enabled ROB IDs."
                  << endmsg;
    } else {
      // this is only info, because it is normal in athenaHLT
      msgStream() << MSG::INFO << "Could not set Property '" << pname
                  << "' from DataFlow." << endmsg;
    }

    // get the list of enabled Sub Detectors
    pname = "DF_Enabled_SubDet_IDs";
    prop = Gaudi::Utils::getProperty(dfprops, pname);
    if (prop && m_enabledSubDetectors.assign(*prop)) {
      msgStream() << MSG::DEBUG << " ---> Read from DataFlow configuration: "
                  << m_enabledSubDetectors.value().size()
                  << " enabled Sub Detector IDs." << endmsg;
    } else {
      // this is only info, because it is normal in athenaHLT
      msgStream() << MSG::INFO << "Could not set Property '"
                  << pname << "' from DataFlow." << endmsg;
    }
  }

  p_jobOptionsSvc.release().ignore();
}

//=========================================================================
// implementation of IService::initalize
//=========================================================================
StatusCode HltEventLoopMgr::initialize()
{
  // initialize the base class
  StatusCode sc = MinimalEventLoopMgr::initialize();

  // leave this after initialization of base class, otherwise msgSvc is not correctly set up
  //            -----
  msgStream() << MSG::INFO << " ---> HltEventLoopMgr = " << name() << " initialize "
      << " - package version " << CMT_PACKAGE_VERSION << endmsg ;
  if (sc.isFailure()) {
    msgStream() << MSG::ERROR << "Failed to initialize base class MinimalEventLoopMgr" << endmsg;
    return sc;
  }

  updateDFProps();

  // JobOptions type
  SmartIF<IProperty> propMgr ( Gaudi::createApplicationMgr() );
  if( propMgr.isValid() ) {
    try {
      if (m_jobOptionsType.assign( propMgr->getProperty("JobOptionsType") ))
        msgStream() << MSG::DEBUG << " ---> Read from DataFlow configuration: " << m_jobOptionsType << endmsg;
    }
    catch (...) {
      msgStream() << MSG::WARNING << "Could not set Property '" << m_jobOptionsType.name() << "' from DataFlow." << endmsg;
    }
  }
  else {
    msgStream() << MSG::WARNING << "Error retrieving IProperty interface of ApplicationMgr" << endmsg;
  }

  // print properties
  msgStream() << MSG::INFO << " ---> predefinedLumiBlock    = " << m_predefinedLumiBlock << endmsg ;
  msgStream() << MSG::INFO << " ---> Lvl1 CTP ROB Id        = " << m_lvl1CTPROBid 
      << " SourceID in hex = 0x" << MSG::hex << m_lvl1CTPROBid.value() << MSG::dec << endmsg ;
  msgStream() << MSG::INFO << " ---> ApplicationName        = " << m_applicationName << endmsg ;
  msgStream() << MSG::INFO << " ---> PartitionName          = " << m_partitionName << endmsg ;
  msgStream() << MSG::INFO << " ---> JobOptionsType         = " << m_jobOptionsType << endmsg ;

  msgStream() << MSG::INFO << " ---> Enabled ROBs: size = "
              << m_enabledROBs.value().size();
  if (m_enabledROBs.value().size() == 0)
  {
    msgStream() << MSG::INFO << ". No check will be performed ";
  }
  msgStream() << endmsg;

  msgStream() << MSG::INFO << " ---> Enabled Sub Detectors: size = "
              << m_enabledSubDetectors.value().size();
  if (m_enabledSubDetectors.value().size() == 0)
  {
     msgStream() << ". No check will be performed " << endmsg;
  }
  msgStream() << endmsg;

  msgStream() << MSG::INFO << " ---> Fill monitoring histograms             = " << m_doMonitoring << endmsg ;
  msgStream() << MSG::INFO << " ---> Hist: histHltResultSize (overall)      = " << m_histProp_Hlt_result_size << endmsg ;
  msgStream() << MSG::INFO << " ---> Hist: histHltResultSize (physics)      = " << m_histProp_Hlt_result_size_physics << endmsg ;
  msgStream() << MSG::INFO << " ---> Hist: histHltResultSize (express)      = " << m_histProp_Hlt_result_size_express << endmsg ;
  msgStream() << MSG::INFO << " ---> Hist: histHltResultSize (DataScouting) = " << m_histProp_Hlt_result_size_DataScouting << endmsg ;
  msgStream() << MSG::INFO << " ---> Hist: histNumberOfStreamTags           = " << m_histProp_numStreamTags << endmsg ;
  msgStream() << MSG::INFO << " ---> Hist: histStreamTagNames               = " << m_histProp_streamTagNames << endmsg ;
  msgStream() << MSG::INFO << " ---> Hist: histNumberROBsPartialEB          = " << m_histProp_num_partial_eb_robs << endmsg ;
  msgStream() << MSG::INFO << " ---> Hist: histHltEdmSizes                  = " << m_histProp_Hlt_Edm_Sizes << endmsg;
  msgStream() << MSG::INFO << " ---> HLT EDM Collection Names               = " << m_hltEdmCollectionNames << endmsg;
  msgStream() << MSG::INFO << " ---> HltResult SG key                       = " << m_HltResultName << endmsg ;
  msgStream() << MSG::INFO << " ---> HLT debug stream name                  = " << m_HltDebugStreamName << endmsg ;
  msgStream() << MSG::INFO << " ---> HLT stream for forced events           = " << m_HltForcedStreamName << endmsg ;
  msgStream() << MSG::INFO << " ---> ForceHltReject                         = " << m_forceHltReject << endmsg ;
  msgStream() << MSG::INFO << " ---> ForceHltAccept                         = " << m_forceHltAccept << endmsg;

  if (m_forceHltReject.value()) {
    msgStream() << MSG::INFO << " +------------------------------------------+ "  << endmsg ;
    msgStream() << MSG::INFO << " | >>>   ForceHltReject is enabled !    <<< | "  << endmsg ;
    msgStream() << MSG::INFO << " | It takes precedence over ForceHltAccept  | "  << endmsg ;
    msgStream() << MSG::INFO << " +------------------------------------------+ "  << endmsg ;
  }

  msgStream() << MSG::INFO << " ---> Write events with truncated HLT result to debug stream  = " << m_writeHltTruncationToDebug << endmsg;
  msgStream() << MSG::INFO << " ---> Debug stream name for events with truncated HLT result  = " << m_HltTruncationDebugStreamName << endmsg;
  msgStream() << MSG::INFO << " ---> Stream names of events with a truncated HLT result which will not be send to the debug stream  = " << m_excludeFromHltTruncationDebugStream << endmsg;

  //-------------------------------------------------------------------------
  // Setup the StoreGateSvc
  //-------------------------------------------------------------------------
  sc = m_evtStore.retrieve();
  if(sc.isFailure())
  {
    msgStream() << MSG::FATAL << "Error retrieving StoreGateSvc "+m_evtStore << endmsg;
    return sc;
  }

  //-------------------------------------------------------------------------
  // Setup the DetectorStore
  //-------------------------------------------------------------------------
  sc = m_detectorStore.retrieve();
  if(sc.isFailure())
  {
    msgStream() << MSG::FATAL << "Error retrieving DetectorStore "+m_detectorStore << endmsg;
    return sc;
  }

  //-------------------------------------------------------------------------
  // Setup the InputMetaDataStore
  //-------------------------------------------------------------------------
  sc = m_inputMetaDataStore.retrieve();
  if(sc.isFailure())
  {
    msgStream() << MSG::FATAL << "Error retrieving InputMetaDataStore"+m_inputMetaDataStore << endmsg;
    return sc;
  }

  //--------------------------------------------------------------------------
  // Setup the IncidentSvc
  //--------------------------------------------------------------------------
  sc = m_incidentSvc.retrieve();
  if(sc.isFailure())
  {
    msgStream() << MSG::FATAL << "Error retrieving IncidentSvc "+m_incidentSvc << endmsg;
    return sc;
  }

  //--------------------------------------------------------------------------
  // Setup the ROBDataProviderSvc 
  //--------------------------------------------------------------------------
  sc = m_robDataProviderSvc.retrieve();
  if(sc.isFailure())
  {
    msgStream() << MSG::FATAL << "Error retrieving ROBDataProviderSvc "+m_robDataProviderSvc << endmsg;
    return sc;
  }

  //--------------------------------------------------------------------------
  // Setup the Histogram Service
  //--------------------------------------------------------------------------
  sc = m_THistSvc.retrieve();
  if(sc.isFailure())
  {
    msgStream() << MSG::FATAL << "Error retrieving THistSvc "+m_THistSvc << endmsg;
    return sc;
  }

  //--------------------------------------------------------------------------
  // Setup the TrigISHelper
  //--------------------------------------------------------------------------
  sc = m_isHelper.retrieve();
  if(sc.isFailure())
  {
    msgStream() << MSG::FATAL << "Error retrieving TrigISHelper "+m_isHelper << endmsg;
    return sc;
  }
  sc = m_isHelper->setProperty(m_partitionName);
  if(sc.isFailure())
  {
    msgStream() << MSG::FATAL << "Error setting " << m_partitionName.name()
                    << " of " << m_isHelper << endmsg;
    return sc;
  }

  //--------------------------------------------------------------------------
  // COOL helper
  //--------------------------------------------------------------------------
  if (m_coolHelper.retrieve().isFailure()) {
    msgStream() << MSG::FATAL << "Error retrieving" << m_coolHelper << endmsg;
    return StatusCode::FAILURE;
  }

  //--------------------------------------------------------------------------
  // Setup optional services/tools
  //--------------------------------------------------------------------------
  if (service("AlgContextSvc", m_algContextSvc, /*createIf=*/ false).isFailure()) {
    m_algContextSvc = 0;
    msgStream() << MSG::DEBUG << "No AlgContextSvc available" << endmsg;
  }

  //--------------------------------------------------------------------------
  // Pre flight check
  //--------------------------------------------------------------------------
  ToolHandle<TrigPreFlightCheck> preFlightCheck;
  if (preFlightCheck.retrieve().isFailure()) {
    msgStream() << MSG::FATAL << "Error retrieving TrigPreFlightCheck "+preFlightCheck << endmsg;
    return StatusCode::FAILURE;
  }

  // A failed pre-flight check is fatal in a partition
  if ( validPartition() ) {
    if ( preFlightCheck->check(MSG::ERROR).isFailure() ) {
      msgStream() << MSG::FATAL << "Pre-flight check for HLT failed." << endmsg;
      return StatusCode::FAILURE;
    }
  }
  else {
    if ( preFlightCheck->check(MSG::WARNING).isFailure() )
      msgStream() << MSG::WARNING << "Pre-flight check for HLT failed." << endmsg;
  }    
  preFlightCheck->release();

  // The remainder of this method used to be in the L2/EF specialization

  // fill CTP ROB id vector
  m_ctpRobIdVec.clear();
  if (m_lvl1CTPROBid.value() != 0) {
    m_ctpRobIdVec.push_back( m_lvl1CTPROBid.value() );
  }

  // print properties
  msgStream() << MSG::INFO << " ---> HltEventLoopMgr            = " << name() << " special properties <--- " << endmsg ;
  msgStream() << MSG::INFO << " ---> Lvl1 CTP ROB Id vec.        = 0x" << MSG::hex << m_ctpRobIdVec[0] << MSG::dec
      << " size of vector = " << m_ctpRobIdVec.size() << endmsg ;
  msgStream() << MSG::INFO << " ---> Check for invalid CTP ROBs  = " << m_lvl1CTPROBcheck << endmsg ;

//-------------------------------------------------------------------------
// Reset counters
//-------------------------------------------------------------------------
  m_total_evt                         = 0;
  m_failed_evt                        = 0;
  m_invalid_lvl1_result               = 0;
  m_invalid_hlt_result                = 0;
  m_truncated_hlt_result              = 0;
  m_truncated_hlt_result_to_debug     = 0;
  m_truncated_hlt_result_not_to_debug = 0;

//--------------------------------------------------------------------------
// Setup the HLT Histogram Service when configured
//--------------------------------------------------------------------------
  if ( &*m_THistSvc ) {
    m_hltTHistSvc = SmartIF<IHltTHistSvc>( &*m_THistSvc );
    if (m_hltTHistSvc.isValid()) {
      msgStream() << MSG::INFO << "A THistSvc implementing the HLT interface IHltTHistSvc was found."
    << endmsg;
    } else {
      msgStream() << MSG::INFO << "No THistSvc implementing the HLT interface IHltTHistSvc was found."
    << endmsg;
    }
  }

//--------------------------------------------------------------------------
// Setup the HLT ROB Data Provider Service when configured
//--------------------------------------------------------------------------
  if ( &*m_robDataProviderSvc ) {
    m_hltROBDataProviderSvc = SmartIF<ITrigROBDataProviderSvc>( &*m_robDataProviderSvc );
    if (m_hltROBDataProviderSvc.isValid()) {
      msgStream() << MSG::INFO << "A ROBDataProviderSvc implementing the HLT interface ITrigROBDataProviderSvc was found."
    << endmsg;
    } else {
      msgStream() << MSG::INFO << "No ROBDataProviderSvc implementing the HLT interface ITrigROBDataProviderSvc was found."
    << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
// implementation of IService::sysFinalize
//=========================================================================
StatusCode HltEventLoopMgr::sysFinalize()
{
  // initialize the base class
  StatusCode sc = MinimalEventLoopMgr::sysFinalize();
  sc.setChecked();
  return sc;
}

//=========================================================================
// implementation of IService::finalize
//=========================================================================
StatusCode HltEventLoopMgr::finalize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << " ---> HltEventLoopMgr = " << name() << " finalize " << endmsg;
  log << MSG::INFO << " Total number of events processed :                                     " << m_total_evt << endmsg;
  log << MSG::INFO << "    Events with error in event processing                               " << m_failed_evt << endmsg;
  log << MSG::INFO << "    Events with invalid Lvl1 Result                                     " << m_invalid_lvl1_result << endmsg;
  log << MSG::INFO << "    Events with invalid Hlt Result                                      " << m_invalid_hlt_result << endmsg;
  log << MSG::INFO << "    Events with truncated Hlt Result payload                            " << m_truncated_hlt_result << endmsg;   
  log << MSG::INFO << "    Events with truncated Hlt Result payload (send to debug stream)     " << m_truncated_hlt_result_to_debug << endmsg;   
  log << MSG::INFO << "    Events with truncated Hlt Result payload (not send to debug stream) " << m_truncated_hlt_result_not_to_debug << endmsg;   

  // Need to release now. Automatic release in destructor is too late since services are already gone.
  m_hltTHistSvc.reset();
  m_hltROBDataProviderSvc.reset();

  StatusCode sc = MinimalEventLoopMgr::finalize();
  if (sc.isFailure()) {
    msgStream() << MSG::ERROR << "Error in MinimalEventLoopMgr Finalize" << endmsg;
  }

  // Release all interfaces
  m_incidentSvc.release().ignore();
  m_robDataProviderSvc.release().ignore();
  m_evtStore.release().ignore();
  m_detectorStore.release().ignore();
  m_inputMetaDataStore.release().ignore();
  m_THistSvc.release().ignore();
  m_isHelper.release().ignore();

  return sc;
}

//=========================================================================
// implementation of IService::sysReinitialize
//=========================================================================
StatusCode HltEventLoopMgr::sysReinitialize()
{
  // initialize the base class
  StatusCode sc = MinimalEventLoopMgr::sysReinitialize();
  sc.setChecked();

  return sc;
}

//=========================================================================
// implementation of IService::reinitialize
//=========================================================================
StatusCode HltEventLoopMgr::reinitialize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << " ---> HltEventLoopMgr = " << name() << " reinitialize " << endmsg;

//-------------------------------------------------------------------------
// Reset counters
//-------------------------------------------------------------------------
  m_total_evt                         = 0;
  m_failed_evt                        = 0;
  m_invalid_lvl1_result               = 0;
  m_invalid_hlt_result                = 0;
  m_truncated_hlt_result              = 0;
  m_truncated_hlt_result_to_debug     = 0;
  m_truncated_hlt_result_not_to_debug = 0;

  StatusCode sc = MinimalEventLoopMgr::reinitialize();
  if (sc.isFailure()) {
    msgStream() << MSG::ERROR << "Error in MinimalEventLoopMgr Reinitialize" << endmsg;
  }

  return sc;
}

StatusCode HltEventLoopMgr::executeAlgorithms()
{
  // Call the resetExecuted() method of ALL "known" algorithms
  // (before we were reseting only the topalgs)
  SmartIF<IAlgManager> algMan(serviceLocator());
  if ( algMan.isValid() ) {
    const auto& allAlgs = algMan->getAlgorithms() ;
    for( auto ialg = allAlgs.begin() ; allAlgs.end() != ialg ; ++ialg ) {
      if ( 0 != *ialg ) (*ialg)->resetExecuted();
    }
  }

  // Call the execute() method of all top algorithms
  return callOnAlgs(&IAlgorithm::sysExecute, "sysExecute", true);
}


//=========================================================================
// implementation of IEventProcessor::executeEvent(void* par) ---> for "online" running
//=========================================================================
StatusCode HltEventLoopMgr::executeEvent(void* par)
{
  StatusCode  sc;
  if (msgLevel() <= MSG::DEBUG) {  
    msgStream() << MSG::DEBUG << " ---> HltEventLoopMgr = " << name()
		    << " executeEvent(par): par = 0x" << MSG::hex << par << MSG::dec << endmsg;
  }

  {
    std::lock_guard<std::mutex>{timeout_mutex};

    resetTimeout(Athena::Timeout::instance());
    sc = m_evtStore->retrieve(m_currentEvent);
  }

  if( sc.isFailure() ) {
    msgStream() << MSG::ERROR << "Expected an EventInfo object." << endmsg;
    return StatusCode::FAILURE;
  } else {
    const auto eventrn = m_currentEvent->event_ID()->run_number();
    if ( (m_currentRun != eventrn))
      msgStream() << (m_currentRun ? MSG::WARNING : MSG::DEBUG)
                  << "Run number changed from " << m_currentRun << " to "
                  << eventrn << " Complete EventID   = "
                  << *(m_currentEvent->event_ID()) << endmsg;
  }

  //-----------------------------------------------------------------------
  // obtain the HLT conditions update counters from the CTP fragment
  //-----------------------------------------------------------------------
  std::vector<uint32_t> l1_hltCounters(1,0);    // HLT counters from CTP fragment
  std::vector<uint32_t> l1_extraPayload;        // extra paylaod from CTP fragment

  // find the CTP ROB and get the HLT conditions update counters
  bool b_invalidCTPRob = false;
  std::vector<uint32_t> ctpRobIdVec(1,m_lvl1CTPROBid.value());
  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> ctpRobFragmentVec;
  m_robDataProviderSvc->getROBData(ctpRobIdVec,ctpRobFragmentVec);
  if( ctpRobFragmentVec.size() == 1 ) {
    try {
      // get the HLT Counter
      l1_hltCounters[0] = CTPfragment::hltCounter( ctpRobFragmentVec[0] );
      l1_extraPayload = CTPfragment::extraPayloadWords( ctpRobFragmentVec[0] );
    }
    catch (CTPfragment::NullFragmentPointer& ex) {
      b_invalidCTPRob=true;
      ISSUE(IssueSeverity::ERROR,ex.what());
    }
    catch (CTPfragment::Inconsistency& ex) {
      b_invalidCTPRob=true;
      ISSUE(IssueSeverity::ERROR,ex.what());
    }
    catch (CTPfragment::TimeOutOfRange& ex) {
      b_invalidCTPRob=true;
      ISSUE(IssueSeverity::ERROR,ex.what());
    }
    catch (CTPfragment::TriggerWordsOutOfRange& ex) {
      b_invalidCTPRob=true;
      ISSUE(IssueSeverity::ERROR,ex.what());
    }
    catch (CTPfragment::GenericIssue& ex) {
      b_invalidCTPRob=true;
      ISSUE(IssueSeverity::ERROR,ex.what());
    }
    catch (eformat::Issue& ex) {
      std::string issue_msg = std::string("Uncaught eformat issue:    ")+std::string(ex.what());
      b_invalidCTPRob=true;
      ISSUE(IssueSeverity::ERROR, issue_msg);
    }
    catch (ers::Issue& ex) {
      std::string issue_msg = std::string("Uncaught ERS issue:        ")+std::string(ex.what());
      b_invalidCTPRob=true;
      ISSUE(IssueSeverity::ERROR, issue_msg);
    }
    catch (std::exception& ex) {
      std::string issue_msg = std::string("Uncaught std exception:    ")+std::string(ex.what());
      b_invalidCTPRob=true;
      ISSUE(IssueSeverity::ERROR, issue_msg);
    }
    catch (...) {
      b_invalidCTPRob=true;
      ISSUE(IssueSeverity::ERROR,"Uncaught unknown exception.");
    }
  } else {
    // no valid CTP fragment found
    std::string issue_msg = std::string("No valid CTP fragment found.  ") ;
    b_invalidCTPRob=true;
    ISSUE(IssueSeverity::ERROR, issue_msg);
  }

  //-----------------------------------------------------------------------
  // create an empty HLT Result Object and register it in StoreGate
  // (if this fails, steering will try to create a HLT result object)
  //-----------------------------------------------------------------------
  auto pHltResult = new HLT::HLTResult;
  pHltResult->setLvl1Id(m_currentEvent->event_ID()->event_number());
  HLT::HLTExtraData& extraData = pHltResult->getExtraData();
  extraData.appName = applicationName();
  extraData.statusCode |= hltonl::HLT_PRESCALE_UPDATE;  // OK

  // Record it in StoreGate (object can be modified)
  sc = m_evtStore->record(pHltResult, m_HltResultName, true);
  if(sc.isFailure()) {
    msgStream() << MSG::ERROR << "Error declaring HLT Result object in SG" << endmsg;
  }

  //-----------------------------------------------------------------------
  // Decode CTP extra payload words
  //-----------------------------------------------------------------------
  CTPfragment::ExtraPayload ctp_payload;
  try {
    ctp_payload = CTPfragment::ExtraPayload(l1_extraPayload);
  }
  catch (CTPfragment::ExtraPayloadTooLong& ex) {
    b_invalidCTPRob=true;
    ISSUE(IssueSeverity::ERROR, std::string("Invalid CTP fragment. Exception = ")+ex.what());
  }

  if ( msgLevel() <= MSG::DEBUG ) {
    msgStream() << MSG::DEBUG << "CTP extra payload (" << l1_extraPayload.size() << " words): ";
    for (std::size_t i=0; i<l1_extraPayload.size(); ++i) msgStream() << " " << l1_extraPayload[i];

    msgStream() << ctp_payload << endmsg;
  }

  // Schedule COOL updates
  m_coolHelper->setFolderUpdates(ctp_payload.getFolderUpdates());

  //-----------------------------------------------------------------------
  // Check if an HLT conditions update is required
  //-----------------------------------------------------------------------
  if (!b_invalidCTPRob) {
    sc = checkHltPrescaleUpdate(l1_hltCounters);
    if(sc.isFailure()) {
      std::vector<uint32_t>& vExtraData = pHltResult->getExtras();
      // set the HLT PSK flag to 0 to indicate error
      vExtraData[vExtraData.size()-2] = 0;       // one word for prescale counter (=1 ok, =0 error)
      msgStream() << MSG::FATAL << "HLT Conditions update failed" << endmsg;
      throw ers::HLTAbort(ERS_HERE, name()+": HLT Conditions update failed");
    }
  }

  // Fire BeginEvent "Incident"
  m_incidentSvc->fireIncident(EventIncident(*m_currentEvent, name(),"BeginEvent"));

  //-----------------------------------------------------------------------
  // COOL updates for LB changes
  //-----------------------------------------------------------------------
  if ( m_currentLB != m_currentEvent->event_ID()->lumi_block() ) {  // LB change
    m_currentLB = m_currentEvent->event_ID()->lumi_block();

    if ( m_coolHelper->hltCoolUpdate(m_currentLB, m_currentRun,
                                     m_sorTime_stamp[0], m_sorTime_stamp[1]).isFailure() ) {
      msgStream() << MSG::FATAL << "COOL update failed. Aborting." << endmsg;
      throw ers::HLTAbort(ERS_HERE, name()+": Failure during COOL update");
    }
  }

  // Execute Algorithms
  bool eventFailed = false;
  try {
    sc = executeAlgorithms();
  }
  catch ( const std::exception& e ) {
    msgStream() << MSG::ERROR << "Caught a standard exception "
                << e.what() << endmsg;
    sc = StatusCode::FAILURE;
  }
  catch (...) {
    msgStream() << MSG::ERROR << "Unknown exception" << endmsg;
    sc = StatusCode::FAILURE;
  }

  if (sc.isSuccess()) {
    // Call the execute() method of all output streams
    for (auto o : m_outStreamList ) {
      o->resetExecuted();
      sc = o->sysExecute();
      if(sc.isFailure())  {
        msgStream() << MSG::WARNING << "Execution of output stream " << o->name() << " failed" << endmsg;
        eventFailed = true;
      }
    }
  }
  else {
    eventFailed = true;
  }     

  // Fire EndEvent "Incident"
  m_incidentSvc->fireIncident(EventIncident(*m_currentEvent, name(),"EndEvent"));

  return eventFailed ? StatusCode::FAILURE : StatusCode::SUCCESS;
}

//=========================================================================
// prepare for run step
//=========================================================================
StatusCode HltEventLoopMgr::prepareForRun(const ptree & pt)
{
  try
  {
    
    (void)TClass::GetClass("vector<unsigned short>"); // preload to overcome an issue with dangling references in serialization
    (void)TClass::GetClass("vector<unsigned long>");

    const SOR * sor;
    if(internalPrepareResets().isFailure() || // do the necessary resets and
       !(sor = processRunParams(pt))) // update SOR in det store and get it back
      return StatusCode::FAILURE;

    auto & soral = getSorAttrList(sor);
    updInternal(soral);     // update internally kept info
    updMetadaStore(soral);  // update metadata store

    const EventInfo * evinfo;
    if(updMagField(pt).isFailure() ||     // update mag field when appropriate
       updHLTConfigSvc().isFailure() ||   // update config svc when appropriate
       resetCoolValidity().isFailure() || // reset selected proxies/IOV folders
       prepXAODEventInfo().isFailure() || // update xAOD event data in SG
       !(evinfo = prepEventInfo()))       // update old event data in SG
      return StatusCode::FAILURE;

    bookAllHistograms();

    if(prepareAlgs(*evinfo).isSuccess())
      return StatusCode::SUCCESS;
  }
  catch(const ptree_bad_path & e)
  {
    msgStream() << MSG::ERROR << ST_WHERE
                << "Bad ptree path: \""
                << e.path<ptree::path_type>().dump() << "\" - " << e.what()
                << endmsg;
  }
  catch(const ptree_bad_data & e)
  {
    msgStream() << MSG::ERROR << ST_WHERE
                << "Bad ptree data: \""
                << e.data<ptree::data_type>() << "\" - " << e.what() << endmsg;
  }
  catch(const std::runtime_error& e)
  {
    msgStream() << MSG::ERROR << ST_WHERE
                << "Runtime error: " << e.what() << endmsg;
  }

  return StatusCode::FAILURE;
}

//=========================================================================
// allow for updates after forking the workers and issue an incident
// to inform the clients
//=========================================================================
StatusCode HltEventLoopMgr::hltUpdateAfterFork(const ptree & pt)
{
  updateDFProps();

  //-----------------------------------------------------------------------
  // Fire "UpdateAfterFork" "Incident"
  //-----------------------------------------------------------------------
  int worker_id  = atoi( (pt.get_child("workerId").data()).c_str() );
  int process_id = getpid();
  m_incidentSvc->fireIncident(HLT::Incidents::UpdateAfterFork(worker_id,process_id,name()));

  readyMsg(); // The event processing will start now

  return StatusCode::SUCCESS;
}

//=========================================================================
// Hlt: process one event/RoI  ---> for "online" running
//=========================================================================
StatusCode HltEventLoopMgr::processRoIs (
            const std::vector<eformat::ROBFragment<const uint32_t*>>& l1_result,
            hltinterface::HLTResult& hlt_result,
            const hltinterface::EventId& evId)
{
  PtrResetter<const EventInfo, std::mutex> currentEventResetter{m_currentEvent,
                                                                timeout_mutex};
  StatusCode sc;

  //-----------------------------------------------------------------------
  // increase event counter
  //-----------------------------------------------------------------------
  m_total_evt++;

  if ( msgLevel() <= MSG::DEBUG ) {
    msgStream() << MSG::DEBUG << " " << endmsg;
    msgStream() << MSG::DEBUG << " +------------+" << endmsg;
    msgStream() << MSG::DEBUG << " | processRoI | for " << name()
                << " and event number = " << m_total_evt  << " called."
                << endmsg;
    msgStream() << MSG::DEBUG << " +------------+" << endmsg;
  }

  //-----------------------------------------------------------------------
  // If HLT THistSvc flush N-tuple buffer
  //-----------------------------------------------------------------------
  if ( m_hltTHistSvc.isValid() ) {
    (m_hltTHistSvc->send()).ignore() ;
    if ( msgLevel() <= MSG::DEBUG ) {
      msgStream() << MSG::DEBUG << " ---> THistSvc->send(): m_hltTHistSvc = "
                  << m_hltTHistSvc << endmsg;
    }
  }

  //-----------------------------------------------------------------------
  // reset event qualifiers
  //-----------------------------------------------------------------------
  m_lvl1id               = 0; // Level-1 ID=event no., 32 bit unsigned
  m_run_no               = 0; // run number,           32 bit unsigned
  m_bunch_crossing_id    = 0; // bunch crossing ID,    32 bit unsigned
  m_l1_Status_Element    = 0;
  m_l1_Trigger_Type      = 0;
  m_l1_detev_type        = 0;
  m_time_stamp           = 0; // time stamp - posix time in seconds from 1970,   32 bit unsigned
  m_time_stamp_ns_offset = 0; // time stamp ns - ns time offset for time_stamp,  32 bit unsigned
  m_lumi_block           = 0; // luminosity block identifier,                    32 bit unsigned
  m_l1_Trigger_Info.assign(16,0);  // triggerInfo

  // clear hlt_result structure
  // -------------------------
  hlt_result.trigger_info.clear();
  hlt_result.stream_tag.clear();
  hlt_result.psc_errors.clear();
  (*hlt_result.fragment_pointer) = 0;

  //-----------------------------------------------------------------------
  // Check received Lvl1 result (Integrity is checked by HLTPU)
  // get basic event parameters
  // Store LVL1 Result in ROBDataProviderSvc cache
  // Get extended information from the CTP ROB
  //-----------------------------------------------------------------------
  if( l1_result.size() == 0 ) {   // no Lvl1 result robs
    m_invalid_lvl1_result++;

    failedEvent(hlt_result,
                hltonl::PSC_ERROR_NO_L1_RESULT,
                "No Lvl1 result ROBs received.");

    return StatusCode::RECOVERABLE;
  }

  // *-- SubDetectors in received L1 ROB list
  if (m_doMonitoring.value()) {
    scoped_lock_histogram lock;
    for(const auto& rob : l1_result)
    {
      auto sid = eformat::helper::SourceIdentifier(rob.rob_source_id());
      auto label = std::string{UNEXPECTED_L1R_ROB_LABEL};
      if(std::find(begin(L1R_BINS), end(L1R_BINS), sid.subdetector_id())
         != end(L1R_BINS))
        label = sid.human_detector();
      m_hist_l1_robs->Fill(label.c_str(), 1.);
    }
  }

  const OFFLINE_FRAGMENTS_NAMESPACE::DataType* buffer;
  // at least 1 Lvl1 rob, set basic parameters
  m_lvl1id               = l1_result[0].rod_lvl1_id();  // Level-1 ID=event no., 32 bit unsigned
  m_run_no               = l1_result[0].rod_run_no();   // run number,           32 bit unsigned
  m_bunch_crossing_id    = l1_result[0].rod_bc_id();    // bunch crossing ID,    32 bit unsigned
  l1_result[0].status(buffer);
  m_l1_Status_Element    = *buffer;
  m_l1_Trigger_Type      = l1_result[0].rod_lvl1_trigger_type();
  m_l1_detev_type        = l1_result[0].rod_detev_type();
  m_time_stamp           = 0; // time stamp - posix time in seconds from 1970,   32 bit unsigned
  m_time_stamp_ns_offset = 0; // time stamp ns - ns time offset for time_stamp,  32 bit unsigned
  m_lumi_block           = 0; // luminosity block identifier,                    32 bit unsigned
  m_l1_Trigger_Info.assign(16,0);  // triggerInfo can be obtained from CTP fragment

  // put Lvl1 result in ROBDataProviderSvc cache
  m_robDataProviderSvc->setNextEvent(l1_result);

  // find the CTP ROB and get the complete event info data
  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> ctpRobFragmentVec;
  m_robDataProviderSvc->getROBData(m_ctpRobIdVec,ctpRobFragmentVec);
  bool b_invalidCTPRob = false;
  std::ostringstream ost;
  if( ctpRobFragmentVec.size() == 1 ) {
    try {
      m_lvl1id            = ctpRobFragmentVec[0]->rod_lvl1_id();  // Level-1 ID=event no., 32 bit unsigned
      m_run_no            = ctpRobFragmentVec[0]->rod_run_no();   // run number,           32 bit unsigned
      m_bunch_crossing_id = ctpRobFragmentVec[0]->rod_bc_id();    // bunch crossing ID,    32 bit unsigned
      ctpRobFragmentVec[0]->status(buffer);
      m_l1_Status_Element = *buffer;
      m_l1_Trigger_Type   = ctpRobFragmentVec[0]->rod_lvl1_trigger_type();
      m_l1_detev_type     = ctpRobFragmentVec[0]->rod_detev_type();
      // get time_stamp - posix time in seconds from 1970, and time_stamp_ns - ns time offset for time_stamp
      CTPfragment::time( ctpRobFragmentVec[0], m_time_stamp, m_time_stamp_ns_offset );
      // luminosity block identifier, 32 bit unsigned
      m_lumi_block = CTPfragment::lumiBlockNumber( ctpRobFragmentVec[0] );
      // get the L1 trigger info
      m_l1_Trigger_Info   = CTPfragment::triggerDecision( ctpRobFragmentVec[0] );

      b_invalidCTPRob = !checkEventIdConsistency(evId);
     }
    catch (CTPfragment::NullFragmentPointer& ex) {
      m_invalid_lvl1_result++;
      if (m_lvl1CTPROBcheck) {
        b_invalidCTPRob=true;
        ost << " Invalid CTP fragment. Exception = " << ex.what();
      }
      ISSUE(IssueSeverity::ERROR,ex.what());
    }
    catch (CTPfragment::Inconsistency& ex) {
      m_invalid_lvl1_result++;
      if (m_lvl1CTPROBcheck) {
        b_invalidCTPRob=true;
        ost << " Invalid CTP fragment. Exception = " << ex.what();
      }
      ISSUE(IssueSeverity::ERROR,ex.what());
    }
    catch (CTPfragment::TimeOutOfRange& ex) {
      m_invalid_lvl1_result++;
      if (m_lvl1CTPROBcheck) {
        b_invalidCTPRob=true;
        ost << " Invalid CTP fragment. Exception = " << ex.what();
      }
      ISSUE(IssueSeverity::ERROR,ex.what());
    }
    catch (CTPfragment::TriggerWordsOutOfRange& ex) {
      m_invalid_lvl1_result++;
      if (m_lvl1CTPROBcheck) {
        b_invalidCTPRob=true;
        ost << " Invalid CTP fragment. Exception = " << ex.what();
      }
      ISSUE(IssueSeverity::ERROR,ex.what());
    }
    catch (CTPfragment::GenericIssue& ex) {
      m_invalid_lvl1_result++;
      if (m_lvl1CTPROBcheck) {
        b_invalidCTPRob=true;
        ost << " Invalid CTP fragment. Exception = " << ex.what();
      }
      ISSUE(IssueSeverity::ERROR,ex.what());
    }
    catch (eformat::Issue& ex) {
      m_invalid_lvl1_result++;
      std::string issue_msg = std::string("Uncaught eformat issue:    ")+std::string(ex.what());
      if (m_lvl1CTPROBcheck) {
        b_invalidCTPRob=true;
        ost << " Invalid CTP fragment. Exception = " << ex.what();
      }
      ISSUE(IssueSeverity::ERROR, issue_msg);
    }
    catch (ers::Issue& ex) {
      m_invalid_lvl1_result++;
      std::string issue_msg = std::string("Uncaught ERS issue:        ")+std::string(ex.what());
      if (m_lvl1CTPROBcheck) {
        b_invalidCTPRob=true;
        ost << " Invalid CTP fragment. Exception = " << ex.what();
      }
      ISSUE(IssueSeverity::ERROR, issue_msg);
    }
    catch (std::exception& ex) {
      m_invalid_lvl1_result++;
      std::string issue_msg = std::string("Uncaught std exception:    ")+std::string(ex.what());
      if (m_lvl1CTPROBcheck) {
        b_invalidCTPRob=true;
        ost << " Invalid CTP fragment. Exception = " << ex.what();
      }
      ISSUE(IssueSeverity::ERROR, issue_msg);
    }
    catch (...) {
      m_invalid_lvl1_result++;
      if (m_lvl1CTPROBcheck) {
        b_invalidCTPRob=true;
        ost << " Invalid CTP fragment. Uncaught unknown exception.";
      }
      ISSUE(IssueSeverity::ERROR,"Uncaught unknown exception.");
    }
  } else {
    // no valid CTP fragment found
    m_invalid_lvl1_result++;
    if (m_doMonitoring.value()) m_hist_l1_robs->Fill(MISSING_L1R_CTP_LABEL, 1.);

    std::string issue_msg = std::string("No valid CTP fragment found.  ") ;
    if (m_hltROBDataProviderSvc.isValid()) issue_msg = issue_msg + std::string("\n") +
                m_hltROBDataProviderSvc->dumpROBcache() ;
    if (m_lvl1CTPROBcheck) {
      b_invalidCTPRob=true;
      ost << " No valid CTP fragment found. " ;
    }
    ISSUE(IssueSeverity::ERROR, issue_msg);
  }

  // in case a check of the CTP Rob is requested and an invalid fragment was found
  // skip the event and put it on the debug stream
  // -----------------------------------------------------------------------------
  if (m_lvl1CTPROBcheck && b_invalidCTPRob) {
    ost << " Skip event requested. ";
    failedEvent(hlt_result,
                hltonl::PSC_ERROR_INVALID_CTP_RESULT,
                ost.str());

    return StatusCode::RECOVERABLE;
  }

  // Check for other missing L1 ROBs
  auto missing_l1_robs = missingL1Robs(l1_result);
  if(!missing_l1_robs.empty())
  {
    ++m_invalid_lvl1_result;
    failedEvent(hlt_result, hltonl::PSC_ERROR_NO_L1_RESULT,
                missingL1RobsMsg(missing_l1_robs));

    return StatusCode::RECOVERABLE;
  }

  //-----------------------------------------------------------------------
  // Clear the event store, if used in the event loop
  //-----------------------------------------------------------------------
  sc = m_evtStore->clearStore();
  if ( msgLevel() <= MSG::DEBUG ) {
    msgStream() << MSG::DEBUG << " ---> Clear of Event data store " << sc << endmsg;
  }
  if(sc.isFailure()) {
    failedEvent(hlt_result, hltonl::PSC_ERROR_SG_CLEAR_FAILED,
                "Clear of Event data store failed.");
    return sc;
  }

  // create an EventInfo Object to fire the incidents and register in evt store
  auto pEvent = new EventInfo(new EventID(m_run_no,
                                          evId.globalId,
                                          m_time_stamp,
                                          m_time_stamp_ns_offset,
                                          m_lumi_block,
                                          m_bunch_crossing_id,
                                          std::get<0>(m_detector_mask),
                                          std::get<1>(m_detector_mask),
                                          std::get<2>(m_detector_mask),
                                          std::get<3>(m_detector_mask)),
                              new EventType(),
                              new TriggerInfo(m_l1_Status_Element,
                                              m_lvl1id,
                                              m_l1_Trigger_Type,
                                              m_l1_Trigger_Info));

  if ( msgLevel() <= MSG::DEBUG ) {
    msgStream() << MSG::DEBUG << " +-----------+ " << endmsg;
    msgStream() << MSG::DEBUG << " | New Event | Run = " << m_run_no 
		<< " / Level-1 ID = "      << m_lvl1id 
		<< " / global Event ID = " << evId.globalId
		<< endmsg;
    msgStream() << MSG::DEBUG << " +-----------+ " << endmsg;
    msgStream() << MSG::DEBUG << " Complete EventID    = " << *(pEvent->event_ID()) << endmsg;
    msgStream() << MSG::DEBUG << "        EventType    = " << ((pEvent->event_type())->typeToString()) << endmsg;
    msgStream() << MSG::DEBUG << "      TriggerInfo    = " << *(pEvent->trigger_info()) << endmsg;
    msgStream() << MSG::DEBUG << " Current Run number  = " << m_currentRun << endmsg;
  }

  // Record it in StoreGate
  if(m_evtStore->record(pEvent, "EventInfo").isFailure())
  {
    failedEvent(hlt_result, hltonl::PSC_ERROR_NO_EVENTINFO,
        "Error recording EventInfo object in SG");

    return StatusCode::FAILURE;
  }

  //-----------------------------------------------------------------------
  // process the RoI(s)/Event
  //-----------------------------------------------------------------------
  sc = executeEvent(NULL);
  if(sc.isFailure()) {
    failedEvent(hlt_result, hltonl::PSC_ERROR_UNCLASSIFIED,
                "An unknown error occured during event processing",
                false /* no empty result */);
  }

  // Get the xAOD::EventInfo from SG
  xAOD::EventInfo * xev{nullptr};
  if(m_evtStore->retrieve(xev, "EventInfo").isFailure())
  {
    msgStream() << MSG::WARNING << ST_WHERE
                << "Could not retrieve xAOD::EventInfo in store gate. "
                << "If running in HelloWorld algorithms, this is OK."
                << endmsg;
  }

  if (msgLevel() <= MSG::DEBUG)
    msgStream() << MSG::DEBUG << " new xAOD::EventInfo: " << xev << endmsg;

  //-----------------------------------------------------------------------
  // build HLT result
  //-----------------------------------------------------------------------
  HltResult(hlt_result, pEvent, xev);
  // fill histograms
  fillHltResultHistograms(hlt_result);

  return sc;
}

//=========================================================================
// implementation of IService::stop() ---> for "online" running
//=========================================================================
StatusCode HltEventLoopMgr::stop()
{
  // Do nothing if we are already stopped
  if ( FSMState() != Gaudi::StateMachine::RUNNING )
    return StatusCode::SUCCESS;

  if (msgLevel() <= MSG::DEBUG) {  
    msgStream() << MSG::DEBUG << " ---> HltEventLoopMgr = " << name() << " stop() " << endmsg;
  }

  //-----------------------------------------------------------------------
  // Fire EndRun "Incident"
  //-----------------------------------------------------------------------
  m_incidentSvc->fireIncident(Incident(name(), IncidentType::EndRun));

  //-----------------------------------------------------------------------
  // Call endRun() and stop() of all algorithms
  //-----------------------------------------------------------------------
  auto suc1 = callOnAlgs(&IAlgorithm::sysEndRun, "sysEndRun").isSuccess();
  auto suc2 = callOnAlgs(&IAlgorithm::sysStop, "sysStop").isSuccess();

  return (suc1 && suc2) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

//=========================================================================
// implementation of ITrigEventLoopMgr::timeOutReached ---> for "online" running
//=========================================================================
/**
 * Called by the PSC once the HLTPU signals the timeout warning.
 *
 * !!! This method is executed in the context of the timeout thread !!!
 * !!! Make sure the code is thread-safe                            !!!
 *
 */
StatusCode HltEventLoopMgr::timeOutReached(const ptree& pt)
{
  IAlgorithm* alg = m_algContextSvc ? m_algContextSvc->currentAlg() : 0;

  // add current algorithm info
  std::ostringstream tmsg;
  tmsg << "Timeout warning received"
       << (alg ? (" while executing '"+alg->name()+"'. ") : ". ");

  // set timeout and add current event info
  {
    std::lock_guard<std::mutex>{timeout_mutex};

    setTimeout(Athena::Timeout::instance());
    if (m_currentEvent)
      tmsg << " On event " << *m_currentEvent->event_ID() << ". ";
    else
      tmsg << " Unknown event - outside of event processing? ";
  }

  // add time interval info
  auto starts = pt.get_optional<long long>("StartTime_s");
  if(starts)
  {
    auto ends = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    tmsg << (ends - *starts) << " seconds elapsed since ";
  }
  else
    tmsg << "No information of when ";

  tmsg << "the HLTPU last started counting.";

  // log timeout message
  ERS_HLT_WARNING(tmsg.str());

  // Also fire timeout incident (this seems to cause problems, see #66066)
  // m_incidentSvc->fireIncident( HLT::Incidents::EventTimeout(name()) );  

  return StatusCode::SUCCESS;
}



//=========================================================================
// implementation of ITrigEventLoopMgr::checkHltPrescaleUpdate() ---> for "online" running
//=========================================================================
StatusCode HltEventLoopMgr::checkHltPrescaleUpdate(std::vector<uint32_t>& hltCounter)
{
  if (msgLevel() <= MSG::DEBUG) {  
    msgStream() << MSG::DEBUG << " ---> HltEventLoopMgr = " << name()
		    << " hltConditionsUpdate(std::vector<uint32_t>&): size of input = " << hltCounter.size() << ": ";
    for (size_t i=0; i<hltCounter.size(); ++i) msgStream() << hltCounter[i] << " ";
    msgStream() << endmsg;
  }
  if ( hltCounter.empty() ) return StatusCode::SUCCESS;  

  // First (and only used) element is LB of the prescale update
  uint32_t new_hltPrescaleUpdateLB = hltCounter[0];

  // Only update if counter (lumiblock for update) changed (or first event)
  if ( m_l1_hltPrescaleUpdateLB != new_hltPrescaleUpdateLB ) {    
    m_l1_hltPrescaleUpdateLB = new_hltPrescaleUpdateLB;

    // Retrieve EventInfo object
    const EventInfo* pEvent(0);
    StatusCode sc = m_evtStore->retrieve(pEvent);
    if(sc.isFailure()) {
      msgStream() << MSG::ERROR << "Unable to retrieve EventInfo object" << endmsg;
      return sc;
    }

    msgStream() << MSG::INFO << "Prescale update requested for lumiblock "
        << m_l1_hltPrescaleUpdateLB << ". Current event: " << *pEvent->event_ID() << endmsg;

    // Perform prescale update right away (including on first event)
    sc = hltPrescaleUpdate(m_l1_hltPrescaleUpdateLB);
    return sc;
  }

  return StatusCode::SUCCESS;
}


//=========================================================================
// implementation of ITrigEventLoopMgr::hltPrescaleUpdate() ---> for "online" running
//=========================================================================
StatusCode HltEventLoopMgr::hltPrescaleUpdate(uint32_t lumiBlock)
{
  if ( !m_hltConfigSvc ) {
    return StatusCode::SUCCESS;
  }

  // ID must be unqiue across runs until DbProxy gets restarted
  unsigned int id = m_currentRun*10000 + lumiBlock;
  msgStream() << MSG::INFO << " ---> HltEventLoopMgr = " << name()
                  << " calling HLTConfigSvc::updatePrescaleSets(" << id << ")" << endmsg;
  StatusCode sc = m_hltConfigSvc->updatePrescaleSets(id);

  return sc;
}

//=========================================================================
// check that a ROB ID is enabled for readout in OKS  ---> for "online" running
//=========================================================================
bool HltEventLoopMgr::isRobEnabled(uint32_t robid) const
{
  bool b_enabled = true;

  // check if given ROB is actually enabled for readout
  // Consider some ROBs from SubDetectorGroup TDAQ as always enabled
  if ((m_enabledROBs.value().size() != 0) &&
      !skipEnabledCheck(robid))
  {
    auto rob_enabled_it = std::find(m_enabledROBs.value().begin(),
                                    m_enabledROBs.value().end(),
                                    robid);
    if(rob_enabled_it == m_enabledROBs.value().end())
    {
      if(msgLevel() <= MSG::DEBUG)
        msgStream() << MSG::DEBUG << "---> ROB Id : 0x" << MSG::hex << robid
                    << MSG::dec << " will not be retrieved, since it is not on "
                                   "the list of enabled ROBs." << endmsg;

      b_enabled = false;
    }
  }

  return b_enabled;
}

//=========================================================================
// check that a Sub Detector ID is enabled for readout in OKS  ---> for "online" running
//=========================================================================
bool HltEventLoopMgr::isSubDetectorEnabled(uint32_t subdetid) const
{
  bool b_enabled = true;

  // check if given subdetector is actually enabled for readout
  if ((m_enabledSubDetectors.value().size() != 0) &&
      !skipEnabledCheck(static_cast<eformat::SubDetector>(subdetid)))
  {
    auto sd_enabled_it = std::find(m_enabledSubDetectors.value().begin(),
                                   m_enabledSubDetectors.value().end(),
                                   subdetid);
    if(sd_enabled_it == m_enabledSubDetectors.value().end())
    {
      if(msgLevel() <= MSG::DEBUG)
        msgStream() << MSG::DEBUG << "---> Sub Detector Id : 0x" << MSG::hex
                    << subdetid << MSG::dec << " will not be retrieved, since "
                    "it is not on the list of enabled Sub Detectors." << endmsg;
      b_enabled = false;
    }
  }

  return b_enabled;
}

//=========================================================================
// filter a set of robs according to whether or not they are enabled
//=========================================================================
std::set<uint32_t>
HltEventLoopMgr::filterRobs(const std::set<uint32_t>& robs) const
{
  auto ret = decltype(robs){};
  for(const auto& r : robs)
    if(isRobEnabled(r))
      ret.insert(r);

  return ret;
}

//=========================================================================
// filter a set of dets according to whether or not they are enabled
//=========================================================================
std::set<eformat::SubDetector>
HltEventLoopMgr::filterDets(const std::set<uint32_t>& dets) const
{
  auto ret = std::set<eformat::SubDetector>{};
  for(const auto& d : dets)
    if(isSubDetectorEnabled(d))
      ret.insert(static_cast<eformat::SubDetector>(d));

  return ret;
}

//=========================================================================
// Obsolete methods for online running
//=========================================================================
StatusCode HltEventLoopMgr::nextEvent(int maxevt)
{
  msgStream() << MSG::ERROR << " +-------------------------------------------------------------------------+ " << endmsg;
  msgStream() << MSG::ERROR << " | method --->  HltEventLoopMgr::nextEvent(int maxevt) <--- called         | " << endmsg;
  msgStream() << MSG::ERROR << " | This method can not be used in trigger since the event loop is          | " << endmsg;
  msgStream() << MSG::ERROR << " | controlled by the data flow software.                                   | " << endmsg;
  msgStream() << MSG::ERROR << " | Method is not implemented                                               | " << endmsg;
  msgStream() << MSG::ERROR << " +-------------------------------------------------------------------------+ " << endmsg;
  msgStream() << MSG::ERROR << " maxevt = " << maxevt << endmsg;
  msgStream() << MSG::ERROR << " Type info = " << System::typeinfoName(typeid(*this)) << endmsg;
  return StatusCode::FAILURE;
}

StatusCode HltEventLoopMgr::executeRun(int)
{
  msgStream() << MSG::ERROR << " +-------------------------------------------------------------------------+ " << endmsg;
  msgStream() << MSG::ERROR << " | method --->  HltEventLoopMgr::executeRun(int) <--- called               | " << endmsg;
  msgStream() << MSG::ERROR << " | This method can not be used in trigger since the event loop is          | " << endmsg;
  msgStream() << MSG::ERROR << " | controlled by the data flow software.                                   | " << endmsg;
  msgStream() << MSG::ERROR << " | Method is not implemented                                               | " << endmsg;
  msgStream() << MSG::ERROR << " +-------------------------------------------------------------------------+ " << endmsg;
  msgStream() << MSG::ERROR << " Type info = " << System::typeinfoName(typeid(*this)) << endmsg;
  return StatusCode::FAILURE;
}

StatusCode HltEventLoopMgr::stopRun()
{
  msgStream() << MSG::ERROR << " +-------------------------------------------------------------------------+ " << endmsg;
  msgStream() << MSG::ERROR << " | method --->  HltEventLoopMgr::stopRun() <--- called                     | " << endmsg;
  msgStream() << MSG::ERROR << " | This method is not implemented for online usage. Call stop() instead.   | " << endmsg;
  msgStream() << MSG::ERROR << " +-------------------------------------------------------------------------+ " << endmsg;
  msgStream() << MSG::ERROR << " Type info = " << System::typeinfoName(typeid(*this)) << endmsg;
  return StatusCode::FAILURE;
}

//=========================================================================
// HltEmptyResultROB()
//=========================================================================
void HltEventLoopMgr::HltEmptyResultROB(hltinterface::HLTResult& hlt_result,
    uint32_t run_no, uint32_t lvl1_id, uint32_t bunch_crossing_id,
    uint32_t l1_Trigger_Type, uint32_t l1_detev_type,
    hltonl::PSCErrorCode pscErrorCode)
{
  if ( msgLevel() <= MSG::DEBUG ) {
    msgStream() << MSG::DEBUG << "---> HltEmptyResultROB() for " << name() 
		    << " and sub detector = " << eformat::helper::SubDetectorDictionary.string(eformat::TDAQ_HLT)
		    << " called " << endmsg;
  }

  //
  // Set the PSC error words
  // -----------------------
  //
  HltSetPscError(hlt_result, pscErrorCode);

  //
  // Create a dummy HLT result ROB
  // -----------------------------
  //

  // the second status element
  hltonl::ResultStatusCode hlt_status = hltonl::DUMMY_HLT_RESULT ;

  // Build the third status elements for the dummy HLT result
  // number of status words used (in case of PSC error = 3, otherwise = 2)
  uint32_t n_status_word = 3;
  m_status_words[0] = SW_FAIL.code();
  m_status_words[1] = hlt_status;
  m_status_words[2] = pscErrorCode; //this means "dummy HLT result, with no data due PSC error"

  if (!pscErrorCode) {
    m_status_words[0] = SW_UNCLASSIFIED.code(); //this means "dummy HLT result, with no data, no PSC error"
    n_status_word = 2;
  }

  // Build an empty HLT result
  eformat::helper::SourceIdentifier src(eformat::TDAQ_HLT, 0);
  eformat::write::ROBFragment rob(src.code(), run_no, lvl1_id, bunch_crossing_id, l1_Trigger_Type, l1_detev_type,
      0, 0, eformat::STATUS_FRONT);
  rob.status(n_status_word, m_status_words);
  rob.rod_status(n_status_word, m_status_words);

  // find the maximum allowed payload
  int hltr_max_payload = hlt_result.max_result_size - rob.size_word() ;
  if ( hlt_result.max_result_size <= rob.size_word() ) {
    msgStream() << MSG::ERROR
        << "Can not create empty HLT result ROB with available ROB buffer size ! Available buffer size = "
        << hlt_result.max_result_size << " Number of header words used by empty ROB = " << rob.size_word()
        << endmsg;
    return;
  }

  // Create a dummy HLT result and check data size
  uint32_t* hltr_data(0);
  int       hltr_data_size = 0;
  HLT::HLTResult emptyHLTResult;
  emptyHLTResult.setLvl1Id(lvl1_id);

  HLT::HLTExtraData& extraData = emptyHLTResult.getExtraData();
  extraData.appName = applicationName();
  extraData.statusCode &= ~hltonl::HLT_PRESCALE_UPDATE;  // error (clear bit)

  bool serializationOk = emptyHLTResult.serialize( hltr_data, hltr_data_size, hltr_max_payload);

  if ( !serializationOk ) {
    // Update status words for dummy HLT result
    hlt_status = hltonl::DUMMY_HLT_RESULT_TRUNCATED;
    m_status_words[0] = SW_TRUNCATION.code() ;
    m_status_words[1] = hlt_status ;
  }

  if (( hltr_data_size > hltr_max_payload)) { // consistency check
    msgStream() << MSG::ERROR 
        << "Inconsistency for created dummy HLT result ROB. Returned data size = " << hltr_data_size
        << " is greater than allowed max. payload size = " << hltr_max_payload << "."
        << endmsg;
    hltr_data_size = hltr_max_payload;

    // Update status words for dummy HLT result
    hlt_status = hltonl::DUMMY_HLT_RESULT_TRUNCATED;
    m_status_words[0] = SW_TRUNCATION.code();
    m_status_words[1] = hlt_status ;
  }

  // build the fragment
  rob.status(n_status_word, m_status_words);
  rob.rod_status(n_status_word, m_status_words);
  rob.rod_data(hltr_data_size, hltr_data);

  //
  // store the dummy HLT ROB fragment in the hlt_result structure
  // ------------------------------------------------------------ 
  //
  // The maximum possible space is already allocated by the HLTMPPU
  // copy the ROB contents into the allocated memory
  auto next_fragment = hlt_result.fragment_pointer;
  auto spaceleft = hlt_result.max_result_size;
  addRobToHLTResult(hlt_result, rob, next_fragment, spaceleft);

  // delete the data array
  if (hltr_data != 0) delete[] hltr_data;

  return;
} // end method HltEventLoopMgr::HltEmptyResultROB

//=========================================================================
// HltEmptyResultROB()  ---> for "online" running
//=========================================================================
void HltEventLoopMgr::HltEmptyResultROB(hltinterface::HLTResult& hlt_result, hltonl::PSCErrorCode pscErrorCode)
{
  if ( msgLevel() <= MSG::DEBUG ) {
    msgStream() << MSG::DEBUG << "---> HltEmptyResultROB() for "
                << name() << " called " << endmsg;
  }

  HltEmptyResultROB(hlt_result,
                    m_run_no, m_lvl1id, m_bunch_crossing_id,
                    m_l1_Trigger_Type, m_l1_detev_type,
                    pscErrorCode);
  return;
} //  end method HltEventLoopMgr::HltEmptyResult

//=========================================================================
// HltResultROBs()
//=========================================================================
hltonl::PSCErrorCode HltEventLoopMgr::HltResultROBs(
    hltinterface::HLTResult& hlt_result,
    uint32_t run_no, uint32_t lvl1_id, uint32_t bunch_crossing_id,
    uint32_t l1_Trigger_Type, uint32_t l1_detev_type,
    const std::vector<TriggerInfo::number_type>& trigger_info,
    const std::vector<TriggerInfo::StreamTag>& stream_tags,
    const std::vector<xAOD::EventInfo::StreamTag>& xsts)
{
  HLT::HLTResult* dobj{0};
  auto hlt_decision = hltonl::PSC_FORCED_REJECT;
  auto ecode = hltonl::PSC_ERROR_UNCLASSIFIED;
  if(!m_forceHltReject)
  {
    // preliminary hlt_result setup
    setStreamTags(hlt_result, stream_tags);
    mergeStreamTags(hlt_result, xsts);
    hlt_result.trigger_info = trigger_info;

    // Get Pesa HLT Result from Steering
    StatusCode sc;
    if(!(m_evtStore->transientContains<HLT::HLTResult>(m_HltResultName)) ||
       (sc = m_evtStore->retrieve(dobj,m_HltResultName)).isFailure())
    {
      hlt_decision = hltonl::PSC_DEBUG;
      msgStream() << MSG::ERROR << ST_WHERE
                  << "Error retrieving HLTResult from StoreGate with key = "
                  << m_HltResultName << endmsg;

      ecode = sc.isFailure() ? hltonl::PSC_ERROR_NO_HLTRESULT_RETRIEVED
                             : hltonl::PSC_ERROR_NO_HLTRESULT_FOUND;
    }
    else if((hlt_decision = processHltDecision(hlt_result)) !=
            hltonl::STEERING_REJECT)
    {
      // Got Steering's HLT Result and confirmed the event should be accepted
      auto serializationOk = true;
      ecode = serializeRobs(hlt_result, serializationOk, dobj, run_no, lvl1_id,
                            bunch_crossing_id, l1_Trigger_Type, l1_detev_type);
      if(!ecode)
        recordEDMSizeInfo(dobj->getNavigationResult().size(), serializationOk);

      // The HLT result got truncated, put the event on a special debug stream if requested 
      if (!serializationOk) {
	m_truncated_hlt_result++;
    if (m_doMonitoring.value()) m_hist_Hlt_truncated_result->Fill(1.);

 	if ((!m_HltTruncationDebugStreamName.value().empty()) &&
	    (m_writeHltTruncationToDebug.value())) {
      	  // check if event should be not send to the debug stream (e.g. Cost Monitoring)
	  bool sendToDebug(true);
	  for (auto it_st : hlt_result.stream_tag) {
	    auto p = std::find(m_excludeFromHltTruncationDebugStream.value().begin(), 
			       m_excludeFromHltTruncationDebugStream.value().end(),
			       (it_st).name);
	    if (p != m_excludeFromHltTruncationDebugStream.value().end()) sendToDebug=false;
	  }
	  if (sendToDebug) {
	    m_truncated_hlt_result_to_debug++;	 
        if (m_doMonitoring.value()) m_hist_Hlt_truncated_result->Fill(2.);
	    hlt_result.stream_tag.clear();
	    addDebugStreamTag(hlt_result, m_HltTruncationDebugStreamName);
	    msgStream() << MSG::ERROR << ST_WHERE
			<< "HLTResult was truncated. Event send to debug stream  = "
			<< m_HltTruncationDebugStreamName << endmsg;
	  } else {
	    m_truncated_hlt_result_not_to_debug++;	 
        if (m_doMonitoring.value()) m_hist_Hlt_truncated_result->Fill(3.);
	    msgStream() << MSG::WARNING << ST_WHERE
			<< "HLTResult was truncated. Event was NOT send to debug stream  = "
			<< m_HltTruncationDebugStreamName
			<< " because an exclusion stream tag name matched from = " << m_excludeFromHltTruncationDebugStream.value()  
			<< endmsg;
	  }
	} else {
	  m_truncated_hlt_result_not_to_debug++;
      if (m_doMonitoring.value()) m_hist_Hlt_truncated_result->Fill(3.);
	}
      }
    }
  }

  if (m_doMonitoring.value())
    m_hist_eventAcceptFlags->Fill(static_cast<float>(m_mapAccept.codeToHash(hlt_decision)));

  if(msgLevel() <= MSG::DEBUG) {
    msgStream() << MSG::DEBUG << ST_WHERE << "Decision = "
                << hltonl::PrintHltAcceptFlag(hlt_decision) << "\n"
                << hlt_result << endmsg;
    if(dobj) {
      const auto& extraData = dobj->getExtraData();
      msgStream() << MSG::DEBUG << ST_WHERE
                  << "HltResult extra data: Host name = " << extraData.appName
                  << ", status code = " << extraData.statusCode << endmsg;
    }
  }

  return ecode;
} // end method HltEventLoopMgr::HltResultROBs

//=========================================================================
// HltResult()  ---> for "online" running
//=========================================================================
void HltEventLoopMgr::HltResult(hltinterface::HLTResult& hlt_result,
                                const EventInfo* pEvent,
                                const xAOD::EventInfo * xev)
{
  if ( msgLevel() <= MSG::DEBUG ) {
    msgStream() << MSG::DEBUG << "---> HltResult() for " << name() << " called " << endmsg;
  }

  using xev_stags_t = decltype(xev->streamTags());
  auto xev_stags = xev ? xev->streamTags() : xev_stags_t{};

  hltonl::PSCErrorCode hlt_psc_error = HltResultROBs(hlt_result,
                         m_run_no, m_lvl1id, m_bunch_crossing_id,
                         m_l1_Trigger_Type, m_l1_detev_type,
                         pEvent->trigger_info()->eventFilterInfo(),
                         pEvent->trigger_info()->streamTags(),
                         xev_stags);
  // If there was an error when building the HLT result ROB,
  // put the event on the debug stream and try to create a dummy result ROB
  if (hlt_psc_error) {
    // put event on debug stream
    addDebugStreamTag(hlt_result, m_HltDebugStreamName.value());
    // make empty HLT result ROB
    HltEmptyResultROB(hlt_result,hlt_psc_error);
    // increase counter
    m_invalid_hlt_result++;
  }

  return;
} //  end method HltEventLoopMgr::HltResult

//=========================================================================
void HltEventLoopMgr::bookHistograms()
{
  if ( msgLevel() <= MSG::DEBUG ) {
    msgStream() << MSG::DEBUG << "---> bookHistograms() for " << name()
                    << " called. Do monitoring = " << m_doMonitoring.value() << endmsg;
  }

  // return if no monitoring is requested
  if ( !m_doMonitoring.value() ) { return; }

  std::vector<TH1F**> regHistsTH1F;
  regHistsTH1F.reserve(14);
  std::vector<TProfile**> regHistsTProfile;
  regHistsTProfile.reserve(4);

  // monitoring information root directory
  const std::string histPath = std::string("/EXPERT/") + getGaudiThreadGenericName(name()) + "/";

  //     +--------------------+
  // *-- | Event accept flags |
  //     +--------------------+
  uint32_t n_bins_eventAcceptFlags = hltonl::NUM_ACCEPTANCE_FLAGS;
  m_hist_eventAcceptFlags = new TH1F ("EventAcceptFlags",
                                      "EventAcceptFlags;;entries", 
                                      n_bins_eventAcceptFlags, 0.5, 
                                      n_bins_eventAcceptFlags+0.5);

  for (const auto& m : m_mapAccept) {
    m_hist_eventAcceptFlags->GetXaxis()->SetBinLabel( m.second.second , (m.second.first).c_str() );
  }
  regHistsTH1F.push_back(&m_hist_eventAcceptFlags);

  //     +-----------------------+
  // *-- | HLT result properties |
  //     +-----------------------+
  // *-- HLT result size plot
  m_hist_Hlt_result_size = new TH1F ((m_histProp_Hlt_result_size.value().title()).c_str(),
                                     (m_histProp_Hlt_result_size.value().title() + ";words;entries").c_str(),
                                     m_histProp_Hlt_result_size.value().bins(),
                                     m_histProp_Hlt_result_size.value().lowEdge(),
                                     m_histProp_Hlt_result_size.value().highEdge());

  regHistsTH1F.push_back(&m_hist_Hlt_result_size);

  // *-- HLT result status codes
  uint32_t n_bins_ResultStatus = hltonl::NUM_HLT_STATUS_CODES;
  m_hist_Hlt_result_status = new TH1F ("HltResultStatusCodes", "HltResultStatusCodes;;entries", 
                                       n_bins_ResultStatus, 0.5, n_bins_ResultStatus+0.5);

  // do not print label for normal HLT result with no errors, it is not filled
  for (hltonl::MapResultStatusCode::EnumMap::const_iterator map_it = m_mapResultStatus.begin(); map_it != m_mapResultStatus.end(); ++map_it) {
    if ( (*map_it).first == hltonl ::NORMAL_HLT_RESULT ) {
      m_hist_Hlt_result_status->GetXaxis()->SetBinLabel( ((*map_it).second).second, (((*map_it).second).first+" (bin not filled)").c_str() );
    } else {
      m_hist_Hlt_result_status->GetXaxis()->SetBinLabel( ((*map_it).second).second, (((*map_it).second).first).c_str() );
    }
  }
  regHistsTH1F.push_back(&m_hist_Hlt_result_status);

  // *-- HLT result truncation
  uint32_t n_bins_ResultTruncation = 3;
  m_hist_Hlt_truncated_result = new TH1F ("HltResultTruncation",
                                          "HltResultTruncation;;entries", 
                                          n_bins_ResultTruncation, 0.5, n_bins_ResultTruncation+0.5);

  m_hist_Hlt_truncated_result->GetXaxis()->SetBinLabel( 1, std::string("Truncated HLT result").c_str() );
  m_hist_Hlt_truncated_result->GetXaxis()->SetBinLabel( 2, std::string("Truncated HLT result (send to debug stream)").c_str() );
  m_hist_Hlt_truncated_result->GetXaxis()->SetBinLabel( 3, std::string("Truncated HLT result (not send to debug stream)").c_str() );
  regHistsTH1F.push_back(&m_hist_Hlt_truncated_result);

  // *-- HLT result size plot (Stream Physics Main)
  m_hist_Hlt_result_size_physics = 
    new TH1F ((m_histProp_Hlt_result_size_physics.value().title()).c_str(),
              (m_histProp_Hlt_result_size_physics.value().title() + ";words;entries").c_str(),
              m_histProp_Hlt_result_size_physics.value().bins(),
              m_histProp_Hlt_result_size_physics.value().lowEdge(),
              m_histProp_Hlt_result_size_physics.value().highEdge());

  regHistsTH1F.push_back(&m_hist_Hlt_result_size_physics);


  // *-- HLT result size plot (Stream Express)
  m_hist_Hlt_result_size_express = 
    new TH1F ((m_histProp_Hlt_result_size_express.value().title()).c_str(),
              (m_histProp_Hlt_result_size_express.value().title() + ";words;entries").c_str(),
              m_histProp_Hlt_result_size_express.value().bins(),
              m_histProp_Hlt_result_size_express.value().lowEdge(),
              m_histProp_Hlt_result_size_express.value().highEdge());

  regHistsTH1F.push_back(&m_hist_Hlt_result_size_express);


  // *-- HLT result size plot (Stream calibration, DataScouting results)
  m_hist_Hlt_result_size_DataScouting = 
    new TH1F ((m_histProp_Hlt_result_size_DataScouting.value().title()).c_str(),
              (m_histProp_Hlt_result_size_DataScouting.value().title() + ";words;entries").c_str(),
              m_histProp_Hlt_result_size_DataScouting.value().bins(),
              m_histProp_Hlt_result_size_DataScouting.value().lowEdge(),
              m_histProp_Hlt_result_size_DataScouting.value().highEdge());

  regHistsTH1F.push_back(&m_hist_Hlt_result_size_DataScouting);


  // *-- HLT result size profile plot for all stream types "physics"
  m_hist_HltResultSizes_Stream_physics = new TProfile( "Average Hlt Result size for physics streams",
                                                       "Average Hlt Result size for physics streams;Stream Name;Average size in words",
                                                       1,
                                                       (double) 0., (double) 1.,
                                                       (double) 0., (double) 3000000.);

  m_hist_HltResultSizes_Stream_physics->GetXaxis()->SetBinLabel(1, "NoTag");
  m_hist_HltResultSizes_Stream_physics->SetCanExtend(TH1::kAllAxes);
  regHistsTProfile.push_back(&m_hist_HltResultSizes_Stream_physics);

  // *-- HLT result size profile plot for all stream names "DataScouting"
  m_hist_HltResultSizes_Stream_DataScouting = new TProfile( "Average Hlt Result size for data scouting streams",
                                                            "Average Hlt Result size for data scouting streams;Stream Name;Average size in words",
                                                            1,
                                                            (double) 0., (double) 1.,
                                                            (double) 0., (double) 3000000.);

  m_hist_HltResultSizes_Stream_DataScouting->GetXaxis()->SetBinLabel(1, "NoTag");
  m_hist_HltResultSizes_Stream_DataScouting->SetCanExtend(TH1::kAllAxes);
  regHistsTProfile.push_back(&m_hist_HltResultSizes_Stream_DataScouting);

  //     +-----------------------+
  // *-- | framework error codes |
  //     +-----------------------+
  uint32_t n_bins_error = hltonl::NUM_PSC_ERROR_CODES;
  m_hist_frameworkErrorCodes = new TH1F ("FrameworkErrorCodes",
                                         "FrameworkErrorCodes;;entries", 
                                         n_bins_error, 0.5, n_bins_error+0.5);

  for (const auto& m : m_mapPscError) {
    m_hist_frameworkErrorCodes->GetXaxis()->SetBinLabel( m.second.second , (m.second.first).c_str() );
  }
  regHistsTH1F.push_back(&m_hist_frameworkErrorCodes);
  
  //     +-------------+
  // *-- | Stream Tags |
  //     +-------------+
  // *-- number of set stream tags
  m_hist_numStreamTags = new TH1F ((m_histProp_numStreamTags.value().title()).c_str(),
                                   (m_histProp_numStreamTags.value().title() + ";tags;entries").c_str(),
                                   m_histProp_numStreamTags.value().bins(),
                                   m_histProp_numStreamTags.value().lowEdge(),
                                   m_histProp_numStreamTags.value().highEdge());
  regHistsTH1F.push_back(&m_hist_numStreamTags);


  // *-- stream tag types
  uint32_t n_bins_tag = eformat::helper::TagTypeDictionary.size() + 1;
  m_hist_streamTagTypes = new TH1F ("StreamTagTypes",
                                    "StreamTagTypes;tags;entries", n_bins_tag, -0.5 , n_bins_tag-0.5);
  
  using eformat::helper::tagtype_to_string;
  for(uint32_t i=0; i < n_bins_tag; i++ ) { // StreamTag labels
    uint32_t bit = (1u << i);
    m_hist_streamTagTypes->GetXaxis()->SetBinLabel( i+1, tagtype_to_string( static_cast<eformat::TagType>(bit) ).c_str() );
  }
  m_hist_streamTagTypes->GetXaxis()->SetBinLabel(n_bins_tag, "NoTag(=Rejected)");
  regHistsTH1F.push_back(&m_hist_streamTagTypes);


  // *-- stream tag names
  m_hist_streamTagNames = new TH1F ((m_histProp_streamTagNames.value().title()).c_str(),
                                    (m_histProp_streamTagNames.value().title() + ";tags;entries").c_str(),
                                    m_histProp_streamTagNames.value().bins(),
                                    m_histProp_streamTagNames.value().lowEdge(),
                                    m_histProp_streamTagNames.value().highEdge());
  if (m_histProp_streamTagNames.value().bins()>0) {
    m_hist_streamTagNames->GetXaxis()->SetBinLabel(1,std::string("NoTag").c_str() );
  }
  m_hist_streamTagNames->SetCanExtend(TH1::kAllAxes);
  regHistsTH1F.push_back(&m_hist_streamTagNames);

  //     +------------------------+
  // *-- | Partial event building |
  //     +------------------------+
  // *-- number of bins for sub detector plots (55 SubDet max.)
  uint32_t n_bins_partEBSubDet = eformat::helper::SubDetectorDictionary.size();

  // *-- number of ROBs for partial event building
  m_hist_num_partial_eb_robs = new TH1F ((m_histProp_num_partial_eb_robs.value().title()).c_str(),
                                         (m_histProp_num_partial_eb_robs.value().title() + ";robs;entries").c_str(),
                                         m_histProp_num_partial_eb_robs.value().bins(),
                                         m_histProp_num_partial_eb_robs.value().lowEdge(),
                                         m_histProp_num_partial_eb_robs.value().highEdge());

  regHistsTH1F.push_back(&m_hist_num_partial_eb_robs);


  // *-- number of SubDetectors used in partial event building
  m_hist_num_partial_eb_SubDetectors = new TH1F ("NumberSubDetectorsPartialEB",
                                                 "NumberSubDetectorsPartialEB;subdet;entries",
                                                 n_bins_partEBSubDet,-0.5,(float) n_bins_partEBSubDet-0.5);
  regHistsTH1F.push_back(&m_hist_num_partial_eb_SubDetectors);

  // *-- SubDetectors used in partial event building: ROB list
  m_hist_partial_eb_SubDetectors_ROBs = new TH1F ("SubDetectorsPartialEBFromROBList",
                                                  "SubDetectorsPartialEBFromROBList;;entries",
                                                  n_bins_partEBSubDet,0.,(float) n_bins_partEBSubDet);
  uint32_t n_tmp_bin = 1;
  for (const auto& sub : eformat::helper::SubDetectorDictionary) {
    m_hist_partial_eb_SubDetectors_ROBs->GetXaxis()->SetBinLabel( n_tmp_bin++, sub.second.c_str() );
  }
  regHistsTH1F.push_back(&m_hist_partial_eb_SubDetectors_ROBs);

  // *-- SubDetectors used in partial event building: SD list
  m_hist_partial_eb_SubDetectors_SDs = new TH1F ("SubDetectorsPartialEBFromSDList",
                                                 "SubDetectorsPartialEBFromSDList;;entries",
                                                 n_bins_partEBSubDet,0.,(float) n_bins_partEBSubDet);

  n_tmp_bin = 1;
  for (const auto& sub: eformat::helper::SubDetectorDictionary) {
    m_hist_partial_eb_SubDetectors_SDs->GetXaxis()->SetBinLabel( n_tmp_bin++, sub.second.c_str() );
  }
  regHistsTH1F.push_back(&m_hist_partial_eb_SubDetectors_SDs);


  //     +---------------+
  // *-- | HLT EDM Sizes |
  //     +---------------+
  // *-- EDM sizes for all events without a truncated HLT result
  m_hist_HltEdmSizes_No_Truncation = 
    new TProfile( (m_histProp_Hlt_Edm_Sizes.value().title()+":Events_Without_Truncation").c_str(),
                  (m_histProp_Hlt_Edm_Sizes.value().title()+":Events_Without_Truncation;;Average size in words").c_str(),
                  m_histProp_Hlt_Edm_Sizes.value().bins(), 0., m_histProp_Hlt_Edm_Sizes.value().bins(),
                  m_histProp_Hlt_Edm_Sizes.value().lowEdge(), m_histProp_Hlt_Edm_Sizes.value().highEdge());

  n_tmp_bin = 1;
  for (const std::string& col_name : m_hltEdmCollectionNames.value()) {
    m_hist_HltEdmSizes_No_Truncation->GetXaxis()->SetBinLabel( n_tmp_bin++, col_name.c_str() );
  }
  regHistsTProfile.push_back(&m_hist_HltEdmSizes_No_Truncation);

  // *-- EDM sizes for all events with a truncated HLT result
  m_hist_HltEdmSizes_With_Truncation = 
    new TProfile( (m_histProp_Hlt_Edm_Sizes.value().title()+":Events_With_Truncation").c_str(),
                  (m_histProp_Hlt_Edm_Sizes.value().title()+":Events_With_Truncation;;Average size in words").c_str(),
                  m_histProp_Hlt_Edm_Sizes.value().bins(), 0., m_histProp_Hlt_Edm_Sizes.value().bins(),
                  m_histProp_Hlt_Edm_Sizes.value().lowEdge(), m_histProp_Hlt_Edm_Sizes.value().highEdge());

  n_tmp_bin = 1;
  for (const std::string& col_name : m_hltEdmCollectionNames.value()) {
    m_hist_HltEdmSizes_With_Truncation->GetXaxis()->SetBinLabel( n_tmp_bin++, col_name.c_str() );
  }
  regHistsTProfile.push_back(&m_hist_HltEdmSizes_With_Truncation);

  // *-- Sizes of collections which were retained for events with a truncated HLT result
  m_hist_HltEdmSizes_TruncatedResult_Retained_Collections = 
    new TProfile( "Events_With_HLTResult_Truncation:Size_of_Not_Truncated_Collections",
                  "Events_With_HLTResult_Truncation:Size_of_Not_Truncated_Collections;;Average size in words",
                  m_histProp_Hlt_Edm_Sizes.value().bins(), 0., m_histProp_Hlt_Edm_Sizes.value().bins(),
                  m_histProp_Hlt_Edm_Sizes.value().lowEdge(), m_histProp_Hlt_Edm_Sizes.value().highEdge());

  n_tmp_bin = 1;
  for (const std::string& col_name : m_hltEdmCollectionNames.value()) {
    m_hist_HltEdmSizes_TruncatedResult_Retained_Collections->GetXaxis()->SetBinLabel( n_tmp_bin++, col_name.c_str() );
  }
  regHistsTProfile.push_back(&m_hist_HltEdmSizes_TruncatedResult_Retained_Collections);

  // *-- Sizes of collections which were truncated for events with a truncated HLT result
  m_hist_HltEdmSizes_TruncatedResult_Truncated_Collections = 
    new TProfile( "Events_With_HLTResult_Truncation:Size_of_Truncated_Collections",
                  "Events_With_HLTResult_Truncation:Size_of_Truncated_Collections;;Average size in words",
                  m_histProp_Hlt_Edm_Sizes.value().bins(), 0., m_histProp_Hlt_Edm_Sizes.value().bins(),
                  m_histProp_Hlt_Edm_Sizes.value().lowEdge(),
                  m_histProp_Hlt_Edm_Sizes.value().highEdge());

  n_tmp_bin = 1;
  for (const std::string& col_name : m_hltEdmCollectionNames.value()) {
    m_hist_HltEdmSizes_TruncatedResult_Truncated_Collections->GetXaxis()->SetBinLabel( n_tmp_bin++, col_name.c_str() );
  }
  regHistsTProfile.push_back(&m_hist_HltEdmSizes_TruncatedResult_Truncated_Collections);

  //     +---------------------+
  // *-- | register histograms |
  //     +---------------------+
  // *-- TH1F
  for (TH1F** hist : regHistsTH1F) {
    if ( *hist && m_THistSvc->regHist(histPath + (*hist)->GetName(), (*hist)).isFailure() ) {
      msgStream() << MSG::WARNING << "Cannot register histogram " << (*hist)->GetName() << endmsg;
      delete (*hist);
      *hist = 0;
    }
  }
  // *-- TProfile
  for (TProfile** hist : regHistsTProfile) {
    if ( *hist && m_THistSvc->regHist(histPath + (*hist)->GetName(), (*hist)).isFailure() ) {
      msgStream() << MSG::WARNING << "Cannot register histogram " << (*hist)->GetName() << endmsg;
      delete (*hist);
      *hist = 0;
    }
  }
}

//=========================================================================
void HltEventLoopMgr::HltBookHistograms()
{
  if ( msgLevel() <= MSG::DEBUG ) {
    msgStream() << MSG::DEBUG << "---> HltBookHistograms() for " << name()
        << " called. Do monitoring = " << m_doMonitoring.value() << endmsg;
  }

  // return if no monitoring is requested
  if ( !m_doMonitoring.value() ) { return; }

  // monitoring information root directory
  std::string path = std::string("/EXPERT/")+getGaudiThreadGenericName(name())+"/";

  // *-- SubDetectors from l1 ROBs
  auto nbins = L1R_BINS.size() + 2;
  m_hist_l1_robs = new TH1F ("HLTROBsReceivedFromL1",
                             "HLTROBsReceivedFromL1;;entries",
                             nbins,
                             0.,
                             static_cast<double>(nbins));

  // Set labels from L1R_BINS
  unsigned short i = 0;
  while(i < L1R_BINS.size())
  {
    auto sid = eformat::helper::SourceIdentifier(L1R_BINS[i], 0);
    m_hist_l1_robs->GetXaxis()->SetBinLabel(++i, sid.human_detector().c_str());
  }

  // Add a label for unexpected robs and another for missing CTP fragment
  m_hist_l1_robs->GetXaxis()->SetBinLabel(++i, UNEXPECTED_L1R_ROB_LABEL);
  m_hist_l1_robs->GetXaxis()->SetBinLabel(++i, MISSING_L1R_CTP_LABEL);

  // Register it
  if(m_THistSvc->regHist(path + m_hist_l1_robs->GetName(), m_hist_l1_robs).isFailure())
  {
    msgStream() << MSG::WARNING << "Can not register monitoring histogram: "
                << m_hist_l1_robs->GetName() << endmsg;
  }
} //  end method HltEventLoopMgr::HltBookHistograms

//=========================================================================
void HltEventLoopMgr::fillHltResultHistograms(const hltinterface::HLTResult& hlt_result)
{
  if ( msgLevel() <= MSG::DEBUG ) {
    msgStream() << MSG::DEBUG << "---> fillHltResultHistograms(hltinterface::HLTResult& hlt_result) for " << name() << " called " << endmsg;
  }

  // return if no monitoring is requested
  if ( !m_doMonitoring.value() ) { return; }

  //     +-----------------------+
  // *-- | HLT result properties |
  //     +-----------------------+
  if ( (hlt_result.fragment_pointer != 0) && ( (*hlt_result.fragment_pointer) != 0 ) ) {
    eformat::ROBFragment<uint32_t*> hltrob(hlt_result.fragment_pointer);
    uint16_t hltrob_moduleID = eformat::helper::SourceIdentifier( hltrob.rob_source_id() ).module_id();

    // *-- HLT result size plots
    if (hltrob_moduleID == 0) {
      lock_histogram_operation<TH1F>(m_hist_Hlt_result_size)->Fill( (float) hltrob.fragment_size_word() ) ;
    }

    int xbins_physics = m_hist_HltResultSizes_Stream_physics->GetNbinsX();
    int xbins_DS = m_hist_HltResultSizes_Stream_DataScouting->GetNbinsX();
    {
      scoped_lock_histogram lock;
      for (const eformat::read::ROBFragment& rob : hlt_result.hltResult_robs) {
        uint16_t moduleID = eformat::helper::SourceIdentifier( rob.rob_source_id() ).module_id();
        for (const eformat::helper::StreamTag& st : hlt_result.stream_tag) {
          // only normal HLT Results
          if (moduleID == 0) {
            if (st.type == "physics") {
              if (st.name == "Main") m_hist_Hlt_result_size_physics->Fill( (float) rob.fragment_size_word() ) ;
              m_hist_HltResultSizes_Stream_physics->Fill( st.name.c_str(), (double) rob.fragment_size_word() ) ;
            }
            if (st.type == "express") {
              m_hist_Hlt_result_size_express->Fill( (float) rob.fragment_size_word() ) ;
            }
          }
          // DataScouting HLT ROBs
          if (moduleID != 0) {
            if ((st.type == "calibration") && ((st.name).find("DataScouting_") != std::string::npos)) {
              m_hist_Hlt_result_size_DataScouting->Fill( (float) rob.fragment_size_word() ) ;
              m_hist_HltResultSizes_Stream_DataScouting->Fill( st.name.c_str(), (double) rob.fragment_size_word() ) ;
            }
          }
        }
    }

      // deflate bins for profile histograms if needed
      if ( m_hist_HltResultSizes_Stream_physics->GetNbinsX()!=xbins_physics ) {
        m_hist_HltResultSizes_Stream_physics->LabelsDeflate("X");
      }
      if ( m_hist_HltResultSizes_Stream_DataScouting->GetNbinsX()!=xbins_DS ) {
        m_hist_HltResultSizes_Stream_DataScouting->LabelsDeflate("X");
      }
    }

    // *-- HLT result status codes
    if ((hltrob.nstatus() > 1) && (hltrob_moduleID == 0)) {
      scoped_lock_histogram lock;
      const uint32_t* it;
      hltrob.status(it);
      m_hist_Hlt_result_status->Fill( (float) m_mapResultStatus.codeToHash( (hltonl::ResultStatusCode) *(it+1)) );
    }
  }

  //     +-----------------------+
  // *-- | framework error codes |
  //     +-----------------------+
  if (hlt_result.psc_errors.size() > 0)  {
    scoped_lock_histogram lock;
    for (uint32_t ec : hlt_result.psc_errors) {
      m_hist_frameworkErrorCodes->Fill( (float) m_mapPscError.codeToHash( (hltonl::PSCErrorCode) ec) );
    }
  }

  //     +-------------+
  // *-- | Stream Tags |
  //     +-------------+
  //     number of set stream tags
  m_hist_numStreamTags->Fill( (float) hlt_result.stream_tag.size() );

  //     stream tag types
  if(hlt_result.stream_tag.empty()) {
    m_hist_streamTagTypes->Fill( (float) m_hist_streamTagTypes->GetXaxis()->GetNbins() - 1. );
  }
  using namespace eformat::helper;
  for(int32_t i=0; i < m_hist_streamTagTypes->GetXaxis()->GetNbins(); i++) {
    uint32_t bit = (1u<<i);
    if( contains_type(hlt_result.stream_tag, static_cast<eformat::TagType>(bit)) ) {
      m_hist_streamTagTypes->Fill( (float) i);
    }
  }

  //     stream tag names
  if(hlt_result.stream_tag.empty()) {
    m_hist_streamTagNames->Fill(0.);
  } else {
    int xbins = m_hist_streamTagNames->GetNbinsX();
    scoped_lock_histogram lock;      
    for(const eformat::helper::StreamTag& st: hlt_result.stream_tag) {
      m_hist_streamTagNames->Fill(st.name.c_str(),1.);
    }
    // Remove extra empty bins if needed
    if ( m_hist_streamTagNames->GetNbinsX()!=xbins )
      m_hist_streamTagNames->LabelsDeflate("X");
  }

  //     +------------------------+
  // *-- | Partial Event Building |
  //     +------------------------+
  // *-- loop over stream tags and get total number of ROBs and SDs
  uint32_t num_robs(0), num_sd(0);
  std::set<uint32_t> peb_robs;
  std::set<eformat::SubDetector> peb_sd;
  for (const eformat::helper::StreamTag& st : hlt_result.stream_tag) {
    num_robs = num_robs + (st.robs).size();
    num_sd = num_sd + (st.dets).size();
    peb_robs.insert( (st.robs).begin(),(st.robs).end() );
    peb_sd.insert(   (st.dets).begin(),(st.dets).end() );
  }

  // *-- number of ROBs for partial event building
  m_hist_num_partial_eb_robs->Fill( (float) num_robs);

  // *-- number of SubDetectors for partial event building
  m_hist_num_partial_eb_SubDetectors->Fill( (float) num_sd);

  // *-- SubDetectors for partial event building in ROB list
  for(uint32_t rob : peb_robs) {
    m_hist_partial_eb_SubDetectors_ROBs->Fill(eformat::helper::SourceIdentifier(rob).human_detector().c_str(),1.);
  }  

  // *-- SubDetectors for partial event building in SD list
  for (const eformat::SubDetector& sd : peb_sd) {
    m_hist_partial_eb_SubDetectors_SDs->Fill(eformat::helper::SourceIdentifier(sd,0).human_detector().c_str(),1.);
  }

  return;
} // end method HltEventLoopMgr::fillHltResultHistograms

//=========================================================================
void HltEventLoopMgr::bookAllHistograms()
{
  // Book the monitoring histograms
  HltBookHistograms(); // (histograms that were booked differently in l2/ef)
  // Book own histograms
  bookHistograms();
}

//=========================================================================
StatusCode
HltEventLoopMgr::callOnAlgs(const function<StatusCode(IAlgorithm&)> & func,
                            const string & fname, bool failureIsError)
{
  StatusCode sc;
  for(auto alg : m_topAlgList)
  {
    if(func(*alg).isFailure())
    {
      msgStream() << (failureIsError ? MSG::ERROR : MSG::WARNING) << ST_WHERE
                  << "Calling " << fname << "() on algorithm " << alg->name()
                  << " failed" << endmsg;
      sc = StatusCode::FAILURE;
      if(failureIsError)
        break;
    }
  }

  return sc;
}

//=========================================================================
StatusCode HltEventLoopMgr::prepareAlgs(const EventInfo& evinfo)
{
  auto suc1 = callOnAlgs(&IAlgorithm::sysStart, "sysStart").isSuccess();
  m_incidentSvc->fireIncident(EventIncident(evinfo, name(),
                                            IncidentType::BeginRun));
  auto suc2 = callOnAlgs(&IAlgorithm::sysBeginRun, "sysBeginRun").isSuccess();
  m_incidentSvc->fireIncident(EventIncident(evinfo, name(), "EndOfBeginRun"));

  return (suc1 && suc2) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

//=========================================================================
StatusCode HltEventLoopMgr::internalPrepareResets()
{
  m_currentEvent = nullptr; // no time out would make sense here, so not locking
  m_currentLB = 0;
  if (m_predefinedLumiBlock > 0)
  {
    m_currentLB = m_predefinedLumiBlock;
    msgStream() << MSG::DEBUG << ST_WHERE
                << "Using predefined lumi block " << m_currentLB << endmsg;
  }

  // The real CTP counter only has 16 bits and never reaches this value
  m_l1_hltPrescaleUpdateLB = 0xffffffff;

  return clearTemporaryStores();
}

//=========================================================================
const SOR * HltEventLoopMgr::processRunParams(const ptree & pt)
{
  // update the run number
  m_currentRun = pt.get<uint32_t>("RunParams.run_number");

  // Fill SOR parameters from the ptree
  TrigSORFromPtreeHelper sorhelp{msgStream()};
  auto sor = sorhelp.fillSOR(pt.get_child("RunParams"));
  if(!sor)
    msgStream() << MSG::ERROR << ST_WHERE
                << "setup of SOR from ptree failed" << endmsg;

  return sor;
}

//=========================================================================
void HltEventLoopMgr::updInternal(const coral::AttributeList & sor_attrlist)
{
  auto detMaskFst = sor_attrlist["DetectorMaskFst"].data<unsigned long long>();
  auto detMaskSnd = sor_attrlist["DetectorMaskSnd"].data<unsigned long long>();
  updDetMask({detMaskFst, detMaskSnd});

  auto sorTime = sor_attrlist["SORTime"].data<unsigned long long>();
  updSorTime(sorTime);

  if(msgLevel() <= MSG::DEBUG)
  {
    // save current stream flags for later reset
    // cast needed (stream thing returns long, but doesn't take it back)
    auto previous_stream_flags =
        static_cast<std::ios::fmtflags>(msgStream().flags());
    msgStream() << MSG::DEBUG << ST_WHERE
                << "Full detector mask (128 bits) = 0x"
                << MSG::hex << std::setfill('0')
                << std::setw(8) << std::get<3>(m_detector_mask)
                << std::setw(8) << std::get<2>(m_detector_mask)
                << std::setw(8) << std::get<1>(m_detector_mask)
                << std::setw(8) << std::get<0>(m_detector_mask) << endmsg;
    msgStream().flags(previous_stream_flags);

    msgStream() << MSG::DEBUG << ST_WHERE
                << "sorTimeStamp[0] [sec] = " << m_sorTime_stamp[0] << endmsg;
    msgStream() << MSG::DEBUG << ST_WHERE
                << "sorTimeStamp[1] [ns]  = " << m_sorTime_stamp[1] << endmsg;
  }
}

//=========================================================================
void HltEventLoopMgr::updMetadaStore(const coral::AttributeList & sor_attrlist)
{
  // least significant part is "snd" in sor but "fst" for ByteStreamMetadata
  auto bs_dm_fst = sor_attrlist["DetectorMaskSnd"].data<unsigned long long>();
  // most significant part is "fst" in sor but "snd" for ByteStreamMetadata
  auto bs_dm_snd = sor_attrlist["DetectorMaskFst"].data<unsigned long long>();

  auto metadata = new ByteStreamMetadata(
    sor_attrlist["RunNumber"].data<unsigned int>(),
    0,
    0,
    sor_attrlist["RecordingEnabled"].data<bool>(),
    0,
    bs_dm_fst,
    bs_dm_snd,
    0,
    0,
    "",
    "",
    "",
    0,
    std::vector<std::string>());

  // Record ByteStreamMetadata in MetaData Store
  if(m_inputMetaDataStore->record(metadata,"ByteStreamMetadata").isFailure())
  {
    msgStream() << MSG::WARNING << ST_WHERE
                << "Unable to record MetaData in InputMetaDataStore."
                << endmsg;
    delete metadata;
  }
  else
    msgStream() << MSG::DEBUG << ST_WHERE
                << "Recorded MetaData in InputMetaDataStore." << endmsg;
}

//=========================================================================
StatusCode HltEventLoopMgr::clearTemporaryStores()
{
  //-----------------------------------------------------------------------
  // Clear the event store, if used in the event loop
  //-----------------------------------------------------------------------
  auto sc = m_evtStore->clearStore();
  msgStream() << MSG::DEBUG << ST_WHERE
              << "clear of Event Store " << sc << endmsg;
  if(sc.isFailure()) {
    msgStream() << MSG::ERROR << ST_WHERE
                << "clear of Event Store failed" << endmsg;
    return sc;
  }

  //-----------------------------------------------------------------------
  // Clear the InputMetaDataStore
  //-----------------------------------------------------------------------
  sc = m_inputMetaDataStore->clearStore();
  msgStream() << MSG::DEBUG << ST_WHERE
              << "clear of InputMetaDataStore store " << sc << endmsg;
  if(sc.isFailure())
    msgStream() << MSG::ERROR << ST_WHERE
                << "clear of InputMetaDataStore failed" << endmsg;

  return sc;
}

//=========================================================================
void HltEventLoopMgr::updDetMask(const std::pair<uint64_t, uint64_t>& dm)
{
  m_detector_mask = std::make_tuple(
                        // least significant 4 bytes
                        static_cast<EventID::number_type>(dm.second),
                        // next least significant 4 bytes
                        static_cast<EventID::number_type>(dm.second >> 32),
                        // next least significant 4 bytes
                        static_cast<EventID::number_type>(dm.first),
                        // most significant 4 bytes
                        static_cast<EventID::number_type>(dm.first >> 32));
}

//=========================================================================
void HltEventLoopMgr::updSorTime(unsigned long long st)
{
  m_sorTime_stamp[0]=0;
  m_sorTime_stamp[1]=0;
  if(st)
  {
    m_sorTime_stamp[0] = (unsigned int)(st/1000000000);
    m_sorTime_stamp[1] = (unsigned int)(st-m_sorTime_stamp[0]*1000000000);
  }
}

namespace
{
  // Helper to print SOR record
  void printSORAttrList(const coral::AttributeList& atr,
                        MsgStream& log, const MSG::Level& lvl)
  {
    unsigned long long sorTime_ns(atr["SORTime"].data<unsigned long long>());

    // Human readable format of SOR time if available
    time_t sorTime_sec = sorTime_ns/1000000000;
    const auto sorTime_readable = OWLTime(sorTime_sec);

    log << lvl << "SOR parameters:" << endmsg;
    log << "   RunNumber        = "
        << atr["RunNumber"].data<unsigned int>() << endmsg;
    log << "   SORTime [ns]     = "
        << sorTime_ns << " (" << sorTime_readable << ") " << endmsg;

    // save current stream flags for later reset
    // cast needed (stream thing returns long, but doesn't take it back)
    auto previous_stream_flags = static_cast<std::ios::fmtflags>(log.flags());
    auto dmfst = atr["DetectorMaskFst"].data<unsigned long long>();
    auto dmsnd = atr["DetectorMaskSnd"].data<unsigned long long>();
    log << MSG::hex << std::setfill('0');
    log << "   DetectorMaskFst     = 0x" << std::setw(16) << dmfst << endmsg;
    log << "   DetectorMaskSnd     = 0x" << std::setw(16) << dmsnd << endmsg;
    log << "   (complete DetectorMask = 0x"
        << std::setw(16) << dmfst << std::setw(16) << dmsnd << ")" << endmsg;
    // reset stream flags
    log.flags(previous_stream_flags);

    log << "   RunType          = "
        << atr["RunType"].data<std::string>() << endmsg;
    log << "   RecordingEnabled = "
        << (atr["RecordingEnabled"].data<bool>() ? "true" : "false") << endmsg;
  }
}

//=========================================================================
const coral::AttributeList &
HltEventLoopMgr::getSorAttrList(const SOR * sor) const
{
  if(sor->size() != 1)
  {
    // This branch should never be entered (the CondAttrListCollection
    //corresponding to the SOR should contain one single AttrList). Since
    //that's required by code ahead but not checked at compile time, we
    //explicitly guard against any potential future mistake with this check
    msgStream() << MSG::ERROR << ST_WHERE
                << "Wrong SOR: size = " << sor->size() << endmsg;
    throw std::runtime_error("SOR record should have one and one only attribute"
                             " list, but it has " + sor->size());
  }

  const auto & soral = sor->begin()->second;
  printSORAttrList(soral, msgStream(), MSG::INFO);
  return soral;
}

//=========================================================================
StatusCode HltEventLoopMgr::updHLTConfigSvc()
{
  // Get HLTConfigSvc if available and do sanity check
  service("TrigConf::HLTConfigSvc/HLTConfigSvc",
          m_hltConfigSvc, /*createIf=*/false).ignore();

  if ( (m_hltConfigSvc==0) && (m_jobOptionsType.value()=="DB") ) {
    msgStream() << MSG::ERROR << ST_WHERE
                << "JobOptionsType==DB but could not retrieve HLTConfigSvc"
                << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
const EventInfo * HltEventLoopMgr::prepEventInfo() const
{
  const EventInfo * pEvent =
      new EventInfo(new EventID(m_currentRun,
                                0,
                                m_sorTime_stamp[0],
                                m_sorTime_stamp[1],
                                m_currentLB,
                                0,
                                std::get<0>(m_detector_mask),
                                std::get<1>(m_detector_mask),
                                std::get<2>(m_detector_mask),
                                std::get<3>(m_detector_mask)),
                    new EventType());

  // Record it in StoreGate
  if(m_evtStore->record(pEvent,"EventInfo").isFailure())
  {
    msgStream() << MSG::ERROR << ST_WHERE
                << "Could not record EventInfo object" << endmsg;
    return nullptr;
  }

  msgStream() << MSG::DEBUG << ST_WHERE
              << "Recorded EventInfo object: "
              << *pEvent->event_ID() << endmsg;

  return pEvent;
}

//=========================================================================
StatusCode HltEventLoopMgr::prepXAODEventInfo() const
{
  auto aux = new xAOD::EventAuxInfo;
  auto ev = new xAOD::EventInfo;
  ev->setStore(aux);
  ev->setRunNumber(m_currentRun);
  ev->setEventNumber(0);
  ev->setLumiBlock(m_currentLB);
  ev->setTimeStamp(m_sorTime_stamp[0]);
  ev->setTimeStampNSOffset(m_sorTime_stamp[1]);
  ev->setBCID(0);
  ev->setDetectorMask0(std::get<0>(m_detector_mask));
  ev->setDetectorMask1(std::get<1>(m_detector_mask));
  ev->setDetectorMask2(std::get<2>(m_detector_mask));
  ev->setDetectorMask3(std::get<3>(m_detector_mask));

  // Record it in StoreGate
  if(m_evtStore->record(aux, "EventInfoAux.").isFailure() ||
     m_evtStore->record(ev, "EventInfo").isFailure())
  {
    msgStream() << MSG::ERROR << ST_WHERE
                << "Could not record xAOD::EventInfo object" << endmsg;

    return StatusCode::FAILURE;
  }

  msgStream() << MSG::DEBUG << ST_WHERE
              << "Recorded xAOD::EventInfo object: "
              << *ev << endmsg;

  return StatusCode::SUCCESS;
}

//=========================================================================
StatusCode HltEventLoopMgr::updMagField(const ptree& pt) const
{
  if(m_setMagFieldFromPtree && validPartition())
  {
    try
    {
      auto tor_cur = pt.get<float>("Magnets.ToroidsCurrent.value");
      auto sol_cur = pt.get<float>("Magnets.SolenoidCurrent.value");

      IProperty* magfsvc(0);
      service("AtlasFieldSvc", magfsvc, /*createIf=*/false).ignore();
      if ( magfsvc==0 ) {
        msgStream() << MSG::ERROR << ST_WHERE
                    << "Cannot retrieve AtlasFieldSvc" << endmsg;
        return StatusCode::FAILURE;
      }

      auto sc = Gaudi::Utils::setProperty(magfsvc, "UseSoleCurrent", sol_cur);
      if ( sc.isFailure() ) {
        msgStream() << MSG::ERROR << ST_WHERE
                    << "Cannot set property AtlasFieldSvc.UseSoleCurrent"
                    << endmsg;
        return StatusCode::FAILURE;
      }

      sc = Gaudi::Utils::setProperty(magfsvc, "UseToroCurrent", tor_cur);
      if ( sc.isFailure() ) {
        msgStream() << MSG::ERROR << ST_WHERE
                    << "Cannot set property AtlasFieldSvc.UseToroCurrent"
                    << endmsg;
        return StatusCode::FAILURE;
      }

      msgStream() << MSG::INFO << "*****************************************" << endmsg;
      msgStream() << MSG::INFO << "  Auto-configuration of magnetic field:  " << endmsg;
      msgStream() << MSG::INFO << "    solenoid current from IS = " << sol_cur << endmsg;
      msgStream() << MSG::INFO << "     torroid current from IS = " << tor_cur << endmsg;
      msgStream() << MSG::INFO << "*****************************************" << endmsg;
    }
    catch(ptree_bad_path& e)
    {
      msgStream() << MSG::ERROR << ST_WHERE
                  << "Magnet auto-configuration failed: " << e.what() << endmsg;
      return StatusCode::FAILURE;
    }
  }
  else
  {
    msgStream() << MSG::DEBUG << ST_WHERE
                << "Magnetic fields not available" << endmsg;
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
StatusCode HltEventLoopMgr::resetCoolValidity()
{
  if ( m_coolHelper->resetBeginRunFolders(m_currentRun).isFailure() ) {
    msgStream() << MSG::ERROR << ST_WHERE
                << "Reset of at least one proxy failed" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
void HltEventLoopMgr::readyMsg() const
{
  if (msgLevel() <= MSG::INFO)
  {
    LumiBlock lb;
    lb.LumiBlockNumber = 0;
    if(validPartition() &&
       m_isHelper->findValue(TrigISHelper::LumiBlock, lb).isFailure())
    { // Lumiblock only used for information. Do not make this a failure.
      msgStream() << MSG::WARNING << ST_WHERE
                  << "Cannot read lumiblock number from IS" << endmsg;
    }

    const auto& ilbn = lb.LumiBlockNumber;
    msgStream() << MSG::INFO  << ST_WHERE
                << "Ready to start event processing. Run = "
                <<  m_currentRun << " (LB = "
                << (ilbn ? boost::lexical_cast<string>(ilbn) : string{"??"})
                << ")" << endmsg;
  }
}

//=========================================================================
void HltEventLoopMgr::
setStreamTags(hltinterface::HLTResult& hltr,
              const std::vector<TriggerInfo::StreamTag>& stream_tags) const
{
  hltr.stream_tag.reserve(stream_tags.size());
  for(const auto& st : stream_tags)
  {
    // get robs, filtering disabled ones
    auto robs = filterRobs(st.robs());
    // get subdetectors, filtering disabled ones
    auto dets = filterDets(st.dets());

    hltr.stream_tag.emplace_back(st.name(), st.type(), st.obeysLumiblock(),
                                 robs, dets);
  }
}

//=========================================================================
void HltEventLoopMgr::
mergeStreamTags(hltinterface::HLTResult& hltr,
                const std::vector<xAOD::EventInfo::StreamTag>& xsts) const
{
  auto& stags = hltr.stream_tag;
  for(const auto& st : xsts)
  {
    // get robs, filtering disabled ones
    auto robs = filterRobs(st.robs());
    // get subdetectors, filtering disabled ones
    auto dets = filterDets(st.dets());

    // is there already a tag with the same type, name and obeysLumi flag?
    auto it = find_if(begin(stags), end(stags),
                      [&st](const eformat::helper::StreamTag& current){
      return current.name == st.name() &&
             current.type == st.type() &&
             current.obeys_lumiblock == st.obeysLumiblock();
    });

    if(it != stags.end()) // if so, unite robs and dets
    {
      it->robs.insert(begin(robs), end(robs));
      it->dets.insert(begin(dets), end(dets));
    }
    else // otherwise, add the tag
      stags.emplace_back(st.name(), st.type(), st.obeysLumiblock(), robs, dets);
  }
}

//=========================================================================
hltonl::AcceptFlag
HltEventLoopMgr::processHltDecision(hltinterface::HLTResult& hltr) const
{
  if(m_forceHltAccept)
  {
    // we force accept independently of what the Steering decided for the event
    addDebugStreamTag(hltr, m_HltForcedStreamName.value());
    return hltonl::PSC_FORCED_ACCEPT;
  }

  return hltr.stream_tag.size() ? hltonl::STEERING_ACCEPT
                                : hltonl::STEERING_REJECT;
}

//=========================================================================
hltonl::PSCErrorCode
HltEventLoopMgr::serializeRobs(hltinterface::HLTResult& hltr, bool& serOk,
                               HLT::HLTResult* dobj, uint32_t rn, uint32_t l1id,
                               uint32_t bcid, uint32_t l1tt, uint32_t dett)
{
  auto fp = hltr.fragment_pointer;
  auto spaceleft = hltr.max_result_size;
  
  // obtain the module ids that need serialization
  auto modids = dobj->listOfModIDs(); // hope for copy ellision

  if(find(begin(modids), end(modids), 0) == end(modids))
  {
    const auto& sts = hltr.stream_tag;
    using ST = decltype(sts[0]);
    auto pred = [](const ST& st){ return st.type == "debug"; };

    if (m_forceHltAccept ||
        find_if(begin(sts), end(sts), pred) != end(sts))
    {
      modids.push_back(0);
    }
  }

  // loop over module ids and build a rob for each one
  for(const auto& modid : modids)
  {
    // Initialize a ROB
    auto robp = initRob(modid, rn, l1id, bcid, l1tt, dett, m_status_words);
    auto initial_robsize = robp->size_word() + INITIAL_ROB_SIZE_MARGIN;
    if(!checkRobSize(initial_robsize, spaceleft, hltr.max_result_size))
      return hltonl::PSC_ERROR_ROB_BUILD_FAILED;
    
    // serialize data and fill rob up
    uint32_t* tmp{nullptr};
    serOk &= serializeRob(tmp, *robp, *dobj, modid,
                          spaceleft - initial_robsize);

    // Hold this until we've copied it into the HLTResult
    auto tmpstor = std::unique_ptr<uint32_t[]>{tmp};

    // copy it in and delete temporary data, updating spaceleft
    addRobToHLTResult(hltr, *robp, fp, spaceleft);
  }

  return hltonl::PSC_ERROR_UNCLASSIFIED;
}

//=========================================================================
bool HltEventLoopMgr::serializeRob(uint32_t*& tmpstor,
                                   eformat::write::ROBFragment& rob,
                                   HLT::HLTResult& dobj,
                                   unsigned int modid,
                                   int payload_max)
{
  // find the remaining space for this rob's payload
  int payload = 0;

  // serialize current rob
  auto ret = dobj.serialize(tmpstor, payload, payload_max, modid);

  if(payload > payload_max)
  {
    msgStream() << MSG::ERROR << ST_WHERE
                << "Serialized ROB's size (" << payload << ") is bigger "
                   "than max allowed size (" << payload_max << ")"
                << endmsg;

    payload = payload_max;
    ret = false;
  }

  if(!ret) // otherwise, it's already correct
  {
    m_status_words[0] = SW_TRUNCATION.code();
    m_status_words[1] = hltonl::NORMAL_HLT_RESULT_TRUNCATED;
    rob.status(2, m_status_words);
    rob.rod_status(2, m_status_words);
  }

  rob.rod_data(payload, tmpstor);

  return ret;
}

//=========================================================================
void HltEventLoopMgr::addRobToHLTResult(hltinterface::HLTResult& hltr,
                                        eformat::write::ROBFragment& rob,
                                        uint32_t*& next_fragment, // don't pass original fragment pointer!
                                        uint32_t& spaceleft)
{
  // Store the HLT ROB fragment in the HLT result
  // (Maximum possible space is already allocated by the HLTMPPU)
  auto copied = eformat::write::copy(*rob.bind(), next_fragment, rob.size_word());
  hltr.hltResult_robs.emplace_back(next_fragment);

  next_fragment += copied;
  spaceleft -= copied;

  if(copied == 0 || copied != rob.size_word())  {
    msgStream() << MSG::ERROR
                << "Memory copy operation for HLT result ROB failed. "
                   "Returned number of copied words = " << copied
                << ". Number of words in ROB which should be copied = "
                << rob.size_word() << "." << endmsg;
  }
}

//=========================================================================
void HltEventLoopMgr::recordEDMSizeInfo(size_t nav_size,
                                        bool serializationOk) const
{
  if (m_evtStore->transientContains<HLT::TrigEDMSizes>("TrigEDMSizes")) {
    const HLT::TrigEDMSizes* dobjNavSizes(0);
    if ( m_evtStore->retrieve(dobjNavSizes).isSuccess() ) {
      for(const auto& navSizeInfo : dobjNavSizes->info())
      {
        std::string colname(navSizeInfo.collection+"__"+navSizeInfo.label);
        // Protection for invalid collection names in TProfile
        // (avoid bug in TProfile memory alloc)
        auto it_col_name = std::find(m_hltEdmCollectionNames.value().begin(),
                                     m_hltEdmCollectionNames.value().end(),
                                     colname);
        if(it_col_name == m_hltEdmCollectionNames.value().end())
        {
          if ( msgLevel() <= MSG::DEBUG )
            msgStream() << MSG::DEBUG << "EDM collection name = " << colname
                        << " is not in the configured list of EDM collections."
                        << endmsg;

          colname=m_hltEdmCollectionNames.value().back();
        }

        auto * edmhist = serializationOk ? m_hist_HltEdmSizes_No_Truncation
                                         : m_hist_HltEdmSizes_With_Truncation;
        fillEdmHist(edmhist, colname, navSizeInfo);

        if(!serializationOk)
        {
          edmhist = navSizeInfo.isTruncated(nav_size) ?
              m_hist_HltEdmSizes_TruncatedResult_Truncated_Collections :
              m_hist_HltEdmSizes_TruncatedResult_Retained_Collections;
          fillEdmHist(edmhist, colname, navSizeInfo);
        }
      }
    }
    else
      msgStream() << MSG::WARNING << "cannot access EDM sizes information"
                  << endmsg;
  }
}

//=========================================================================
bool HltEventLoopMgr::checkRobSize(uint32_t robsize, uint32_t spaceleft,
                                   uint32_t maxsize)
{
  if(spaceleft <= robsize)
  {
    msgStream() << MSG::ERROR << ST_WHERE
                << "Can not create HLT result ROB with available ROB "
                   "buffer size ! Total available buffer size = "
                << maxsize << "; buffer size left = " << spaceleft
                << "; Number of header words used by empty ROB = "
                << robsize << endmsg;

    return false;
  }

  return true;
}

//=========================================================================
bool HltEventLoopMgr::
checkEventIdConsistency(const hltinterface::EventId& evId) const
{
  if(evId.l1Id != m_lvl1id || evId.lbNumber != m_lumi_block)
  {
    msgStream() << MSG::WARNING << ST_WHERE
                << "EventId information does not match CTP fragment contents: "
                << "EventId = {globalId: " << evId.globalId << ", l1Id: "
                << evId.l1Id << ", lbNumber: " << evId.lbNumber
                << "}, but the CTP fragment included l1Id = " << m_lvl1id
                << " and lumiblock = " << m_lumi_block << "." << endmsg;
    return false;
  }

  return true;
}

//=========================================================================
void HltEventLoopMgr::failedEvent(hltinterface::HLTResult& hlt_result,
                                  hltonl::PSCErrorCode ecode,
                                  const std::string& emsg,
                                  bool empty_result)
{
  m_failed_evt++;

  addDebugStreamTag(hlt_result, m_HltDebugStreamName.value());
  if(empty_result)
  {
    HltEmptyResultROB(hlt_result, ecode);
    fillHltResultHistograms(hlt_result);
  }

  msgStream() << MSG::ERROR << ST_WHERE
              << emsg << " PSC error code = "
              << hltonl::PrintPscErrorCode(ecode)
              << "\n" << hlt_result << "\n" << endmsg;
}

//=========================================================================
std::vector<uint32_t> HltEventLoopMgr::
missingL1Robs(const std::vector<eformat::ROBFragment<const uint32_t*>>& l1r)
const
{
  using eformat::helper::SourceIdentifier;
  using ROB = eformat::ROBFragment<const uint32_t*>;

  std::vector<uint32_t> ret{};
  for(const auto& robid : m_mandatoryL1ROBs.value())
  {
    if(isSubDetectorIn(SourceIdentifier(robid).subdetector_id()))
    {
      auto it = std::find_if(begin(l1r), end(l1r), [robid](const ROB& rob){
        return rob.rob_source_id() == robid;
      });
      if(it == end(l1r))
        ret.push_back(robid);
    }
  }

  return ret;
}

//=========================================================================
bool HltEventLoopMgr::isSubDetectorIn(eformat::SubDetector sd) const
{
  uint64_t most = std::get<3>(m_detector_mask);
  most <<= 32;
  most |= std::get<2>(m_detector_mask);

  uint64_t least = std::get<1>(m_detector_mask);
  least <<= 32;
  least |= std::get<0>(m_detector_mask);

  return eformat::helper::DetectorMask{least, most}.is_set(sd);
}
