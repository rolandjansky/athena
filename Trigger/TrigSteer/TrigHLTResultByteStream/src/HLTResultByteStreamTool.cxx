/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "HLTResultByteStreamTool.h"
#include "HLTSrcIdMap.h"
#include "eformat/SourceIdentifier.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include <iostream>
#include <stdlib.h>

HLT::HLTResultByteStreamTool::HLTResultByteStreamTool( const std::string& type,
                                                       const std::string& name,
                                                       const IInterface* parent )
  : AthAlgTool(type,name,parent)
{
  declareInterface<HLT::HLTResultByteStreamTool>( this );
}


/**
    Conversion from RDO to eformat::ROBFragment.
    this is called from the createRep method.
*/
StatusCode HLT::HLTResultByteStreamTool::convert( HLTResult* result, RawEventWrite* re,
                                                  std::string objName)
{
  // find the ROB ID for the given HLTResult name
  const auto itr = m_robIDMap.find(objName);
  if ( itr==m_robIDMap.end() ) {
    ATH_MSG_ERROR("No ROB ID configured for " << objName);
    return StatusCode::FAILURE;
  }
  eformat::helper::SourceIdentifier rob(itr->second);

  // configure the EventAssembler and serialize into bytestream
  m_fea.clear();
  m_fea.idMap().setDetId(rob.subdetector_id());

  std::vector<uint32_t>* rod = m_fea.getRodData( rob.code() );
  if (!rod) return StatusCode::FAILURE;

  result->serialize( *rod );
  m_fea.fill(re, msg());

  ATH_MSG_DEBUG("Serialized HLT Result " << objName << " (" << rod->size()
                << " words) to location " << rob.human());
 
  return StatusCode::SUCCESS;
}


/**
    Conversion from eformat::ROBFragment to RDO.
    this is called from the createObj method.
*/
StatusCode HLT::HLTResultByteStreamTool::convert(IROBDataProviderSvc& dataProvider,
                                                 HLT::HLTResult*& result, std::string objName)
{
  // find the ROB ID for the given HLTResult name
  const auto itr = m_robIDMap.find(objName);
  if ( itr==m_robIDMap.end() ) {
    ATH_MSG_ERROR("No ROB ID configured for " << objName);
    return StatusCode::FAILURE;
  }
  eformat::helper::SourceIdentifier rob(itr->second);

  // request the ROB and deserialize into HLTResult
  IROBDataProviderSvc::VROBFRAG robFrags;
  dataProvider.getROBData({rob.code()}, robFrags);

  // unsigned int vector where to store HLT payload
  std::vector<uint32_t> hltContent;
  for (const IROBDataProviderSvc::ROBF* rob : robFrags ) {

    OFFLINE_FRAGMENTS_NAMESPACE::PointerType rodData = nullptr;
    rob->rod_data(rodData);

    const uint32_t nData = rob->rod_ndata();
    hltContent.reserve(nData);
    for (size_t i = 0; i < nData; i++) hltContent.push_back(rodData[i]);
  }

  result->deserialize(hltContent);
  ATH_MSG_DEBUG("Deserialized HLT Result " << objName << " (" << hltContent.size() << " words)");
  
  return StatusCode::SUCCESS;
}
