/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigServices/HltEventLoopMgr.h"

using namespace boost::property_tree;

HltEventLoopMgr::HltEventLoopMgr(const std::string& name, ISvcLocator* svcLoc)
: MinimalEventLoopMgr(name, svcLoc)
{
  verbose() << "start of " << __FUNCTION__ << endmsg;
  
  // declareProperty("predefinedLumiBlock",      m_predefinedLumiBlock=0);
  // declareProperty("Lvl1CTPROBid",             m_lvl1CTPROBid=0x770001);
  // declareProperty("ApplicationName",          m_applicationName="None");
  // declareProperty("PartitionName",            m_partitionName="None");
  // declareProperty("setMagFieldFromPtree",     m_setMagFieldFromPtree=false);
  // declareProperty("enabledROBs",              m_enabledROBs);
  // declareProperty("enabledSubDetectors",      m_enabledSubDetectors);
  // declareProperty("MandatoryL1ROBs",          m_mandatoryL1ROBs,
                  // "List of mandatory ROB IDs coming from the RoIB (must come in L1R seed)");
  // declareProperty("HltEDMCollectionNames",    m_hltEdmCollectionNames,
                  // "Names of all HLT EDM Collections");
  // declareProperty("JobOptionsType",           m_jobOptionsType="NONE");
  // declareProperty("doMonitoring",             m_doMonitoring=true,
                  // "Produce framework monitoring histograms");
/*
  declareProperty("histHltResultSize",        m_histProp_Hlt_result_size,
                  "Histogram for HLT result size in words (overall)");
  declareProperty("histHltResultSizePhysics", m_histProp_Hlt_result_size_physics,
                  "Histogram for HLT result size in words for physics stream ");
  declareProperty("histHltResultSizeExpress", m_histProp_Hlt_result_size_express,
                  "Histogram for HLT result size in words for express stream ");
  declareProperty("histHltResultSizeDS",      m_histProp_Hlt_result_size_DataScouting,
                  "Histogram for HLT result size in words for DataScouting stream ");
  declareProperty("histNumberOfStreamTags",   m_histProp_numStreamTags,
                  "Histogram with number of stream tags");
  declareProperty("histStreamTagNames",       m_histProp_streamTagNames,
                  "Histogram with stream tag names");  
  declareProperty("histNumberROBsPartialEB",  m_histProp_num_partial_eb_robs,
                  "Histogram with number of ROBs for PEB" );
  declareProperty("histHltEdmSizes",          m_histProp_Hlt_Edm_Sizes,
                  "Histogram with sizes of HLT EDM collections" );
*/
  // declareProperty("ForceHltReject",           m_forceHltReject=false);
  // declareProperty("ForceHltAccept",           m_forceHltAccept=false);
  // declareProperty("HltResultName",            m_HltResultName="HLTResult_HLT");
  // declareProperty("HltDebugStreamName",       m_HltDebugStreamName ="HLTMissingData");
  // declareProperty("HltForcedStreamName",      m_HltForcedStreamName ="HLTEventAcceptForced");
  // declareProperty("CoolUpdateTool",           m_coolHelper);
  // declareProperty("maxPrepareForRunSleepSec", m_prepareForRunSleep = 0,
                  // "Max number of seconds to sleep at the beginning of prepareForRun");
  // declareProperty("Lvl1CTPROBcheck",          m_lvl1CTPROBcheck=true);
  // declareProperty("WriteTruncatedHLTtoDebug", m_writeHltTruncationToDebug=true);
  // declareProperty("HltTruncationDebugStreamName", m_HltTruncationDebugStreamName="TruncatedHLTResult");
  // declareProperty("ExcludeFromHltTruncationDebugStream", m_excludeFromHltTruncationDebugStream );

  
  verbose() << "end of " << __FUNCTION__ << endmsg;
}

HltEventLoopMgr::~HltEventLoopMgr()
{
}

StatusCode HltEventLoopMgr::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  verbose() << "start of " << __FUNCTION__ << endmsg;
  if(!ppvInterface)
      return StatusCode::FAILURE;

  if(ITrigEventLoopMgr::interfaceID().versionMatch(riid))
    *ppvInterface = static_cast<ITrigEventLoopMgr*>(this);
  else
    return MinimalEventLoopMgr::queryInterface(riid, ppvInterface);

  addRef();
  verbose() << "end of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}


StatusCode HltEventLoopMgr::initialize()
{
  verbose() << "start of " << __FUNCTION__ << endmsg;
  
  // initialize the base class
  StatusCode sc = MinimalEventLoopMgr::initialize();
  
  
  info() << "---> HltEventLoopMgr = " << name()
         << " initialize - package version " << PACKAGE_VERSION << endmsg;

  if (sc.isFailure()) {
    error() << "Failed to initialize base class MinimalEventLoopMgr" << endmsg;
    return sc;
  }
  
  verbose() << "end of " << __FUNCTION__ << endmsg;
  
  return sc;
}

StatusCode HltEventLoopMgr::prepareForRun(const ptree & pt)
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode HltEventLoopMgr::hltUpdateAfterFork(const ptree & pt)
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode HltEventLoopMgr::processRoIs(
             const std::vector<eformat::ROBFragment<const uint32_t*> >& l1_result,
             hltinterface::HLTResult& hlt_result,
             const hltinterface::EventId& evId)
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode HltEventLoopMgr::timeOutReached(const boost::property_tree::ptree& pt)
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}
