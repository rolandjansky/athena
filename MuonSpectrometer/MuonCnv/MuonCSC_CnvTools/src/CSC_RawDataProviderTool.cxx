/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CSC_RawDataProviderTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "CSC_RawDataProviderTool.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "ByteStreamData/ROBData.h"

#include "MuonIdHelpers/CscIdHelper.h"

#include "ByteStreamCnvSvcBase/ByteStreamCnvSvcBase.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "ByteStreamData/RawEvent.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;


//================ Constructor =================================================

Muon::CSC_RawDataProviderTool::CSC_RawDataProviderTool(const std::string& t,
                                                       const std::string& n,
                                                       const IInterface*  p) :
  CSC_RawDataProviderToolCore(t, n, p)
{
  declareInterface<IMuonRawDataProviderTool>(this);

}

//================ Destructor =================================================

Muon::CSC_RawDataProviderTool::~CSC_RawDataProviderTool()
{}

//================ Initialisation =================================================

StatusCode Muon::CSC_RawDataProviderTool::initialize()
{
  // call initialize from base class
  ATH_CHECK( CSC_RawDataProviderToolCore::initialize() );
  
  ATH_CHECK(m_idHelperSvc.retrieve());
  
  return StatusCode::SUCCESS;
}

//============================================================================================

// new one
StatusCode Muon::CSC_RawDataProviderTool::convert(const std::vector<IdentifierHash>& rdoIdhVect){

  IdContext cscContext = m_idHelperSvc->cscIdHelper().module_context();

  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf;
  std::vector< uint32_t > robIds;
  
  for (unsigned int i=0; i<rdoIdhVect.size(); ++i) {
    uint32_t rob_id = 0xffff;
    m_cabling->hash2RobFull(rdoIdhVect[i],rob_id);
    robIds.push_back(rob_id);
  }
  m_robDataProvider->getROBData(robIds, vecOfRobf);
  ATH_MSG_VERBOSE ( "Number of ROB fragments " << vecOfRobf.size() );
  return convert(vecOfRobf, rdoIdhVect);
}

StatusCode Muon::CSC_RawDataProviderTool::convert(const EventContext& ctx) const {
  
  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf;
  const std::vector< uint32_t >& robIds = m_hid2re.allRobIds();
  ATH_MSG_VERBOSE ( "Number of ROB ids " << robIds.size() );
  // ask ROBDataProviderSvc for the vector of ROBFragment for all MDT ROBIDs
  m_robDataProvider->getROBData( robIds, vecOfRobf);
  ATH_MSG_VERBOSE ( "Number of ROB fragments " << vecOfRobf.size() );

  return convert(vecOfRobf, ctx);
}


StatusCode Muon::CSC_RawDataProviderTool::convert(const ROBFragmentList& vecRobs,
                                                  const std::vector<IdentifierHash>& /* collections */){
  const CSC_RawDataProviderTool* cthis = this;
  return cthis->convert (vecRobs, Gaudi::Hive::currentContext());
}

StatusCode
Muon::CSC_RawDataProviderTool::convert(const ROBFragmentList& vecRobs,
                                       const EventContext& ctx) const
{

  // logic for run-2 (not thread safe mode)
  if (ctx.slot() > 1) {
    ATH_MSG_FATAL ( "CSC_RawDataProviderTool is not thread safe, but you are trying to run with > 1 thread. You must switch to 1 thread or use CSC_RawDataProviderToolMT" );
    return StatusCode::FAILURE;
  }

  CscRawDataContainer* container = 0;

  SG::WriteHandle<CscRawDataContainer> rdoContainerHandle(m_containerKey, ctx);
  // here we have to check if the container is already present and if it is we retrieve from SG and then add to it
  if (rdoContainerHandle.isPresent()) {
    const CscRawDataContainer* rdoContainer_c = 0;
    ATH_CHECK( evtStore()->retrieve( rdoContainer_c, m_containerKey.key() ) );
    container = const_cast<CscRawDataContainer*>(rdoContainer_c);

  } else {

    ATH_CHECK( rdoContainerHandle.record(std::make_unique<CscRawDataContainer>( m_idHelperSvc->cscIdHelper().module_hash_max() )));
    ATH_MSG_DEBUG( "Created CSCRawDataContainer" );
    container = rdoContainerHandle.ptr();
  }

  if(!container) {
    ATH_MSG_ERROR("CSC RDO container pointer is null, cannot decode data");
    return StatusCode::FAILURE;
  }

  // call conversion function from the base class
  ATH_CHECK( convertIntoContainer(vecRobs, ctx, *container) );

  return StatusCode::SUCCESS;
}
