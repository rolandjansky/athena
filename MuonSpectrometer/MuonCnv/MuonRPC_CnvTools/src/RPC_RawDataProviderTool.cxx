/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "RPC_RawDataProviderTool.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ThreadLocalContext.h"

// #include "valgrind/callgrind.h"

#define padMaxIndex 600
#define slMaxIndex  200


using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

Muon::RPC_RawDataProviderTool::RPC_RawDataProviderTool(
    const std::string& t,
    const std::string& n,
    const IInterface*  p) :
    RPC_RawDataProviderToolCore(t, n, p),
    m_AllowCreation(false)
{
  declareInterface<IMuonRawDataProviderTool>(this);
}


Muon::RPC_RawDataProviderTool::~RPC_RawDataProviderTool()
{}


StatusCode Muon::RPC_RawDataProviderTool::initialize()
{
    ATH_CHECK(RPC_RawDataProviderToolCore::initialize());
    
    // Check if EventSelector has the ByteStreamCnvSvc
    bool has_bytestream = false;
    IJobOptionsSvc* jobOptionsSvc;
    StatusCode sc = service("JobOptionsSvc", jobOptionsSvc, false);
    if (sc.isFailure()) {
      ATH_MSG_DEBUG( "Could not find JobOptionsSvc");
      jobOptionsSvc = 0;
    } else {
      IService* svc = dynamic_cast<IService*>(jobOptionsSvc);
      if(svc != 0 ) {
        ATH_MSG_INFO( " Tool = " << name() 
           << " is connected to JobOptionsSvc Service = "
           << svc->name() );
      } else return StatusCode::FAILURE;
    }
    
    IJobOptionsSvc* TrigConfSvc;
    sc = service("TrigConf::HLTJobOptionsSvc", TrigConfSvc, false);
    if (sc.isFailure()) {
      msg(MSG::DEBUG) << "Could not find TrigConf::HLTJobOptionsSvc" << endmsg;
      TrigConfSvc = 0;
    } else {
      IService* svc = dynamic_cast<IService*>(TrigConfSvc);
      if(svc != 0 ) {
        ATH_MSG_INFO( " Tool = " << name() 
           << " is connected to JobOptionsSvc Service = "
           << svc->name() );
      } else return StatusCode::FAILURE;
    }
    
    if(jobOptionsSvc==0 && TrigConfSvc==0)
    {
      ATH_MSG_FATAL( "Bad job configuration" );
      return StatusCode::FAILURE;  
    }
    
    const std::vector<const Gaudi::Details::PropertyBase*>* dataFlowProps 
        = (jobOptionsSvc)?  jobOptionsSvc->getProperties("DataFlowConfig") : 0;

    const std::vector<const Gaudi::Details::PropertyBase*>* eventSelProps 
        = (jobOptionsSvc)? jobOptionsSvc->getProperties("EventSelector") :
                     TrigConfSvc->getProperties("EventSelector"); 
    
    if     ( dataFlowProps != 0 ) has_bytestream = true;
    else if( eventSelProps != 0 )
    {
        for (std::vector<const Gaudi::Details::PropertyBase*>::const_iterator 
            cur  = eventSelProps->begin();
      cur != eventSelProps->end(); cur++) {
      
          if( (*cur)->name() == "ByteStreamInputSvc" ) has_bytestream = true;
        }
    } 
    else has_bytestream = true;
    
    // register the container only when the imput from ByteStream is set up     
    if( has_bytestream || m_containerKey.key() != "RPCPAD" )
    {
        m_AllowCreation= true;
    }
    else
    {
      ATH_MSG_DEBUG( "ByteStream conversion service not found.\n"<< "RPC PAD container not registered.");
    }
    
    ATH_MSG_INFO( "initialize() successful in " << name());
    
    return StatusCode::SUCCESS;
}


// the new one 
StatusCode Muon::RPC_RawDataProviderTool::convert()
{
  SG::ReadCondHandle<RpcCablingCondData> readHandle{m_readKey};
  const RpcCablingCondData* readCdo{*readHandle};
//CALLGRIND_START_INSTRUMENTATION
  /// 
  m_decoder->setSLdecodingRequest();
  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf;
  std::vector<uint32_t> robIds = readCdo->giveFullListOfRobIds();
  m_robDataProvider->getROBData( robIds, vecOfRobf);
//CALLGRIND_STOP_INSTRUMENTATION
  return convert(vecOfRobf); // using the old one
}
// the old one 
StatusCode Muon::RPC_RawDataProviderTool::convert(const ROBFragmentList& vecRobs)
{
 //CALLGRIND_START_INSTRUMENTATION
    std::vector<IdentifierHash> collections;
 //CALLGRIND_STOP_INSTRUMENTATION
    return convert(vecRobs,collections); 
}

// the new one 
StatusCode Muon::RPC_RawDataProviderTool::convert(const std::vector<uint32_t>& robIds)
{
 //CALLGRIND_START_INSTRUMENTATION
    std::vector<IdentifierHash> collections;
    std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf;
    m_robDataProvider->getROBData(robIds, vecOfRobf);
 //CALLGRIND_STOP_INSTRUMENTATION
    return convert(vecOfRobf,collections); 
}

// the new one
StatusCode Muon::RPC_RawDataProviderTool::convert(const std::vector<IdentifierHash>& rdoIdhVect)
{
  SG::ReadCondHandle<RpcCablingCondData> readHandle{m_readKey};
  const RpcCablingCondData* readCdo{*readHandle};
 //CALLGRIND_START_INSTRUMENTATION
    std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf;
    std::vector<uint32_t> robIds;
    CHECK( readCdo->giveROB_fromRDO(rdoIdhVect, robIds) );
    m_robDataProvider->getROBData(robIds, vecOfRobf);
//CALLGRIND_STOP_INSTRUMENTATION
    return convert(vecOfRobf, rdoIdhVect); // using the old one 
}
// the old one 
StatusCode Muon::RPC_RawDataProviderTool::convert(const ROBFragmentList& vecRobs,
  const std::vector<IdentifierHash>& collections)
{
 //CALLGRIND_START_INSTRUMENTATION

  if(m_AllowCreation == false)
  {
    ATH_MSG_WARNING( "Container create disabled due to byte stream");

        return StatusCode::SUCCESS; // Maybe it should be false to stop the job
                                    // because the convert method should not
                                    // have been called .... but this depends
                                    // on the user experience
  }

  SG::WriteHandle<RpcPadContainer> rdoContainerHandle(m_containerKey);
  SG::WriteHandle<RpcSectorLogicContainer> logicHandle(m_sec);

  RpcPadContainer* pad = 0;
  RpcSectorLogicContainer* logic = 0;

  // here we have to check if the container is already present and if it is we retrieve from SG
  if (rdoContainerHandle.isPresent()) {
    const RpcPadContainer* pad_c;
    ATH_CHECK( evtStore()->retrieve (pad_c, m_containerKey.key()) );
    pad = const_cast<RpcPadContainer*>(pad_c);
  } else {
    ATH_CHECK( rdoContainerHandle.record(std::make_unique<RpcPadContainer> (padMaxIndex) ) );
    pad = rdoContainerHandle.ptr();
  }

  if (logicHandle.isPresent()) {
    const RpcSectorLogicContainer* logic_c;
    ATH_CHECK( evtStore()->retrieve (logic_c, m_sec.key()) );
    logic = const_cast<RpcSectorLogicContainer*>(logic_c);                  
  } else {
    ATH_MSG_DEBUG("Writing out RpcSectorLogicContainer");
    ATH_CHECK( logicHandle.record (std::make_unique<RpcSectorLogicContainer>()) );
    logic = logicHandle.ptr();
  }
    
  // pass the containers to the convertIntoContainers function in the RPC_RawDataProviderToolCore base class
  ATH_CHECK( convertIntoContainers(vecRobs, collections, pad, logic) );

  return StatusCode::SUCCESS;
}







