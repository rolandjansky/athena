/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TGC_RawDataProviderTool.h"

#include "MuonRDO/TgcRdoContainer.h"

//================ Constructor =================================================

Muon::TGC_RawDataProviderTool::TGC_RawDataProviderTool(
						       const std::string& t,
						       const std::string& n,
						       const IInterface*  p) :
  TGC_RawDataProviderToolCore(t, n, p)
{
  declareInterface<IMuonRawDataProviderTool>(this);
}

//================ Initialisation =================================================

StatusCode Muon::TGC_RawDataProviderTool::initialize()
{
  // call base class initialise
  ATH_CHECK( TGC_RawDataProviderToolCore::initialize() );
  
  ATH_MSG_INFO( "initialize() successful in " << name() );
  return StatusCode::SUCCESS;
}

//============================================================================================

StatusCode Muon::TGC_RawDataProviderTool::convert(const ROBFragmentList& vecRobs) 
{    

  SG::WriteHandle<TgcRdoContainer> rdoContainerHandle(m_rdoContainerKey); 

  TgcRdoContainer* rdoContainer = 0;

  // if the container is already in store gate, then we have to retrieve from SG
  if (rdoContainerHandle.isPresent()) {
    const TgcRdoContainer* rdoContainer_c = 0;
    ATH_CHECK( evtStore()->retrieve( rdoContainer_c, m_rdoContainerKey.key() ) );
    rdoContainer = const_cast<TgcRdoContainer*>(rdoContainer_c);
  } else {
    ATH_CHECK( rdoContainerHandle.record( std::make_unique<TgcRdoContainer> (m_maxhashtoUse) ) );
    ATH_MSG_DEBUG( "Created TGC container" );
    rdoContainer = rdoContainerHandle.ptr();
  }

  // this should never happen, but since we dereference the pointer, we should check
  if(!rdoContainer) {
    ATH_MSG_ERROR("TGC RDO Container is null, cannot decode TGC data");
    return StatusCode::FAILURE;
  }

  return convertIntoContainer(vecRobs, *rdoContainer);
}

StatusCode  Muon::TGC_RawDataProviderTool::convert(const ROBFragmentList& vecRobs,
						   const std::vector<IdentifierHash>&) 
{
  return convert(vecRobs);
}

StatusCode  Muon::TGC_RawDataProviderTool::convert()
{
  if(!m_cabling) {
    StatusCode sc = getCabling();
    if(sc.isFailure()) return sc;
  }

  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf;
  const std::vector<uint32_t>& robIds = m_hid2re.allRobIds();

  m_robDataProvider->getROBData(robIds, vecOfRobf);

  return convert(vecOfRobf); 
}

StatusCode  Muon::TGC_RawDataProviderTool::convert(const std::vector<IdentifierHash>& rdoIdhVect)
{
  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf = getROBData(rdoIdhVect);

  return convert(vecOfRobf, rdoIdhVect);
}

