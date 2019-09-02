/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MDT_RawDataProviderTool.h"
#include "MuonRDO/MdtCsmContainer.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

// using namespace OFFLINE_FRAGMENTS_NAMESPACE;
Muon::MDT_RawDataProviderTool::MDT_RawDataProviderTool(const std::string& t,
						       const std::string& n,
						       const IInterface*  p )
  :
  MDT_RawDataProviderToolCore(t,n,p)
{
  declareInterface<Muon::IMuonRawDataProviderTool>(this);
  
}


Muon::MDT_RawDataProviderTool::~MDT_RawDataProviderTool()
{}

StatusCode Muon::MDT_RawDataProviderTool::initialize()
{    
    ATH_MSG_VERBOSE("Starting init");

    ATH_CHECK( MDT_RawDataProviderToolCore::initialize() );
  
    ATH_MSG_DEBUG("initialize() successful in " << name());
    return StatusCode::SUCCESS;
}

StatusCode Muon::MDT_RawDataProviderTool::finalize()
{
  return StatusCode::SUCCESS;
}

// the new one 
StatusCode Muon::MDT_RawDataProviderTool::convert() //call decoding function using list of all detector ROBId's
{
  SG::ReadCondHandle<MuonMDT_CablingMap> readHandle{m_readKey};
  const MuonMDT_CablingMap* readCdo{*readHandle};
  if(readCdo==nullptr){
    ATH_MSG_ERROR("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }
  return convert(readCdo->getAllROBId());
}

StatusCode Muon::MDT_RawDataProviderTool::convert(const std::vector<IdentifierHash>& HashVec)
{
  SG::ReadCondHandle<MuonMDT_CablingMap> readHandle{m_readKey};
  const MuonMDT_CablingMap* readCdo{*readHandle};
  if(readCdo==nullptr){
    ATH_MSG_ERROR("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }
  return convert(readCdo->getROBId(HashVec));
}

StatusCode Muon::MDT_RawDataProviderTool::convert(const std::vector<uint32_t>& robIds)
{
  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf;
  m_robDataProvider->getROBData( robIds, vecOfRobf);
   return convert(vecOfRobf); // using the old one
 }
StatusCode Muon::MDT_RawDataProviderTool::convert( const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs,
 const std::vector<IdentifierHash>&)
{
  return convert(vecRobs);
}

StatusCode Muon::MDT_RawDataProviderTool::convert( const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs)
{
  ATH_MSG_VERBOSE("convert(): " << vecRobs.size()<<" ROBFragments.");
    
  // logic for run-2 (not thread safe mode)
  if (Gaudi::Hive::currentContext().slot() > 1) {
    ATH_MSG_FATAL ( "MDT_RawDataProviderTool is not thread safe, but you are trying to run with > 1 thread. You must switch to either 1 thread or use MDT_RawProviderToolMT" );
    return StatusCode::FAILURE;
  }

  SG::WriteHandle<MdtCsmContainer> rdoContainerHandle(m_rdoContainerKey);

  MdtCsmContainer* rdoContainer = 0;

  // here we have to check if the container is already present and if it is we retrieve from SG
  if (rdoContainerHandle.isPresent()) {

    const MdtCsmContainer* rdoContainer_c = 0;
    ATH_CHECK( evtStore()->retrieve( rdoContainer_c, m_rdoContainerKey.key() ) );
    rdoContainer = const_cast<MdtCsmContainer*>(rdoContainer_c);
      
  } else {
    
    ATH_CHECK(rdoContainerHandle.record (std::make_unique<MdtCsmContainer>(m_maxhashtoUse)) );
    ATH_MSG_DEBUG("Created container");
    rdoContainer = rdoContainerHandle.ptr();
  }

  // this should never happen, but since we dereference the pointer, we should check
  if(!rdoContainer) {
    ATH_MSG_ERROR("MdtCsmContainer is null, cannot decode MDT data");
    return StatusCode::FAILURE;
  }
  
  // use the convert function in the MDT_RawDataProviderToolCore class
  ATH_CHECK( convertIntoContainer( vecRobs, *rdoContainer ) );

  return StatusCode::SUCCESS;
}

