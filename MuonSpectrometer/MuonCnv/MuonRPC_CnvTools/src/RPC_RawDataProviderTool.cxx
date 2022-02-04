/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "RPC_RawDataProviderTool.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

#include "Gaudi/Interfaces/IOptionsSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ThreadLocalContext.h"

// #include "valgrind/callgrind.h"

#define padMaxIndex 600
#define slMaxIndex  200


using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

Muon::RPC_RawDataProviderTool::RPC_RawDataProviderTool(
    const std::string& t,
    const std::string& n,
    const IInterface*  p) :
    base_class(t, n, p),
    m_AllowCreation(false)
{
}


Muon::RPC_RawDataProviderTool::~RPC_RawDataProviderTool()
{}


StatusCode Muon::RPC_RawDataProviderTool::initialize()
{
    ATH_CHECK(RPC_RawDataProviderToolCore::initialize());
    
    ServiceHandle<Gaudi::Interfaces::IOptionsSvc> joSvc("JobOptionsSvc", name());
    ATH_CHECK(joSvc.retrieve());

    // register the container only when the input from ByteStream is set up
    if (joSvc->has("EventSelector.ByteStreamInputSvc") || m_containerKey.key() != "RPCPAD")
    {
      m_AllowCreation = true;
    }
    else
    {
      ATH_MSG_DEBUG( "ByteStream conversion service not found.\n"<< "RPC PAD container not registered.");
    }
    
    ATH_MSG_INFO( "initialize() successful in " << name());
    
    return StatusCode::SUCCESS;
}


// the new one 
StatusCode Muon::RPC_RawDataProviderTool::convert() const
{
  return this->convert( Gaudi::Hive::currentContext() );
}

StatusCode Muon::RPC_RawDataProviderTool::convert(const EventContext& ctx) const
{
  SG::ReadCondHandle<RpcCablingCondData> readHandle{m_readKey, ctx};
  const RpcCablingCondData* readCdo{*readHandle};
//CALLGRIND_START_INSTRUMENTATION
  /// 
  //m_decoder->setSLdecodingRequest();
  // We used to set a variable in decoder to turn on SL decoding, but not allowed with const functions
  // So we will pass this through with EventContext and it was only active when running
  // full decoding it seems
  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf;
  std::vector<uint32_t> robIds = readCdo->giveFullListOfRobIds();
  m_robDataProvider->getROBData( robIds, vecOfRobf);
//CALLGRIND_STOP_INSTRUMENTATION
  return convert(vecOfRobf, ctx); // using the old one
}

// the old one 
StatusCode Muon::RPC_RawDataProviderTool::convert(const ROBFragmentList& vecRobs) const
{
  return this->convert( vecRobs, Gaudi::Hive::currentContext() );
}

StatusCode Muon::RPC_RawDataProviderTool::convert(const ROBFragmentList& vecRobs, const EventContext& ctx) const
{
 //CALLGRIND_START_INSTRUMENTATION
    std::vector<IdentifierHash> collections;
 //CALLGRIND_STOP_INSTRUMENTATION
    return convert(vecRobs, collections, ctx); 
}

// the new one 
StatusCode Muon::RPC_RawDataProviderTool::convert(const std::vector<uint32_t>& robIds) const
{
  return this->convert( robIds, Gaudi::Hive::currentContext() );
}

StatusCode Muon::RPC_RawDataProviderTool::convert(const std::vector<uint32_t>& robIds, const EventContext& ctx) const
{
 //CALLGRIND_START_INSTRUMENTATION
    std::vector<IdentifierHash> collections;
    std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf;
    m_robDataProvider->getROBData(robIds, vecOfRobf);
 //CALLGRIND_STOP_INSTRUMENTATION
    return convert(vecOfRobf, collections, ctx); 
}

// the new one
StatusCode Muon::RPC_RawDataProviderTool::convert(const std::vector<IdentifierHash>& rdoIdhVect) const
{
  return this->convert( rdoIdhVect, Gaudi::Hive::currentContext() );
}

StatusCode Muon::RPC_RawDataProviderTool::convert(const std::vector<IdentifierHash>& rdoIdhVect, const EventContext& ctx) const
{
  SG::ReadCondHandle<RpcCablingCondData> readHandle{m_readKey, ctx};
  const RpcCablingCondData* readCdo{*readHandle};
 //CALLGRIND_START_INSTRUMENTATION
    std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf;
    std::vector<uint32_t> robIds;
    CHECK( readCdo->giveROB_fromRDO(rdoIdhVect, robIds) );
    m_robDataProvider->getROBData(robIds, vecOfRobf);
//CALLGRIND_STOP_INSTRUMENTATION
    return convert(vecOfRobf, rdoIdhVect, ctx); // using the old one 
}

// the old one 
StatusCode Muon::RPC_RawDataProviderTool::convert(const ROBFragmentList& vecRobs, const std::vector<IdentifierHash>& collections) const
{
  return this->convert( vecRobs, collections, Gaudi::Hive::currentContext() );
}

StatusCode Muon::RPC_RawDataProviderTool::convert(const ROBFragmentList& vecRobs, const std::vector<IdentifierHash>& collections, const EventContext& ctx) const
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

  SG::WriteHandle<RpcPadContainer> rdoContainerHandle(m_containerKey, ctx);
  SG::WriteHandle<RpcSectorLogicContainer> logicHandle(m_sec, ctx);

  RpcPadContainer* pad = nullptr;
  RpcSectorLogicContainer* logic = nullptr;

  // here we have to check if the container is already present and if it is we retrieve from SG
  if (rdoContainerHandle.isPresent()) {
    const RpcPadContainer* pad_c = nullptr;
    ATH_CHECK( evtStore()->retrieve (pad_c, m_containerKey.key()) );
    pad = const_cast<RpcPadContainer*>(pad_c);
  } else {
    ATH_CHECK( rdoContainerHandle.record(std::make_unique<RpcPadContainer> (padMaxIndex) ) );
    pad = rdoContainerHandle.ptr();
  }

  if (logicHandle.isPresent()) {
    const RpcSectorLogicContainer* logic_c = nullptr;
    ATH_CHECK( evtStore()->retrieve (logic_c, m_sec.key()) );
    logic = const_cast<RpcSectorLogicContainer*>(logic_c);                  
  } else {
    ATH_MSG_DEBUG("Writing out RpcSectorLogicContainer");
    ATH_CHECK( logicHandle.record (std::make_unique<RpcSectorLogicContainer>()) );
    logic = logicHandle.ptr();
  }
    
  // pass the containers to the convertIntoContainers function in the RPC_RawDataProviderToolCore base class
  ATH_CHECK( convertIntoContainers(vecRobs, collections, pad, logic, true) );

  return StatusCode::SUCCESS;
}







