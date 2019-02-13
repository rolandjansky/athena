/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigHLTResultByteStream/HLTResultByteStreamTool.h"
#include "TrigHLTResultByteStream/HLTSrcIdMap.h"
#include "eformat/SourceIdentifier.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include <iostream>
#include <stdlib.h>

const std::string HLT::HLTResultByteStreamTool::s_l2ResultName = "HLTResult_L2";
const std::string HLT::HLTResultByteStreamTool::s_efResultName = "HLTResult_EF";
const std::string HLT::HLTResultByteStreamTool::s_hltResultName = "HLTResult_HLT";
const std::string HLT::HLTResultByteStreamTool::s_dataScoutingResultName = "DataScouting_";

HLT::HLTResultByteStreamTool::HLTResultByteStreamTool( const std::string& type,
                                                       const std::string& name,
                                                       const IInterface* parent )
  :  AthAlgTool(type,name,parent)
{
  declareInterface< HLT::HLTResultByteStreamTool  >( this );
  m_feaL2.idMap().setDetId(eformat::TDAQ_LVL2);
  m_feaEF.idMap().setDetId(eformat::TDAQ_EVENT_FILTER);
}


HLT::HLTResultByteStreamTool::~HLTResultByteStreamTool()
{
}

eformat::SubDetector HLT::HLTResultByteStreamTool::byteStreamLocation(std::string objName)
{
  if (objName == s_l2ResultName) return eformat::TDAQ_LVL2;
  if (objName == s_efResultName || objName == s_hltResultName) return eformat::TDAQ_EVENT_FILTER;
  if (objName.substr(0,s_dataScoutingResultName.length()) == s_dataScoutingResultName) return eformat::TDAQ_EVENT_FILTER;

  return eformat::OTHER;
}


FullEventAssembler<HLT::HLTSrcIdMap>*
HLT::HLTResultByteStreamTool::eventAssembler(std::string objName)
{
  if (objName == s_l2ResultName) return &m_feaL2;
  if (objName == s_efResultName || objName == s_hltResultName) return &m_feaEF;
  if (objName.substr(0,s_dataScoutingResultName.length()) == s_dataScoutingResultName) return &m_feaEF;

  return 0;
}

/**
    Conversion from RDO to eformat::ROBFragment.
    this is called from the createRep method.
*/
StatusCode HLT::HLTResultByteStreamTool::convert( HLTResult* result, RawEventWrite* re,
                                                  std::string objName)
{
  FullEventAssembler<HLTSrcIdMap>* fea = eventAssembler(objName);
  eformat::SubDetector subDet = byteStreamLocation(objName);

  if (!fea || subDet == eformat::OTHER) {
    ATH_MSG_ERROR("Cannot store object with name " << objName
                  << " in BS: we are expecting only HLTResult_(L2,EF,HLT)");
    return StatusCode::FAILURE;
  }

  fea->clear() ;

  uint32_t module_id(0);
  if (objName.substr(0,s_dataScoutingResultName.length()) == s_dataScoutingResultName) {
    module_id = atoi( (objName.substr(s_dataScoutingResultName.length(),2)).c_str() ) ;
  }
  eformat::helper::SourceIdentifier helpID(subDet, module_id);
  uint32_t rodIdHLTResult = helpID.code();

  std::vector<uint32_t>* rod = fea->getRodData( rodIdHLTResult );
  if (!rod) return StatusCode::FAILURE;

  result->serialize( *rod );
  fea->fill(re, msg());

  ATH_MSG_DEBUG(std::dec << "Serialized HLT Result " << objName << " (" << rod->size()
                << " words) to location " << subDet);
 
  return StatusCode::SUCCESS;
}


/**
    Conversion from eformat::ROBFragment to RDO.
    this is called from the createObj method.
*/
StatusCode HLT::HLTResultByteStreamTool::convert(IROBDataProviderSvc& dataProvider,
                                                 HLT::HLTResult*& result, std::string objName)
{
  eformat::SubDetector subDet = byteStreamLocation(objName);

  if (subDet == eformat::OTHER) {
    ATH_MSG_ERROR("Cannot store object with name " << objName
                  << " in BS: we are expecting only HLTResult_(L2,EF,HLT)");
    return StatusCode::FAILURE;
  }

  // unsigned int vector where to store HLT payload
  std::vector<uint32_t> hltContent;

  uint32_t module_id(0);
  if (objName.substr(0,s_dataScoutingResultName.length()) == s_dataScoutingResultName) {
    module_id = atoi( (objName.substr(s_dataScoutingResultName.length(),2)).c_str() ) ;
  }
  eformat::helper::SourceIdentifier helpID(byteStreamLocation(objName), module_id);
  uint32_t robId = helpID.code();

  std::vector<uint32_t> vID;
  vID.push_back(robId);

  IROBDataProviderSvc::VROBFRAG robFrags;
  dataProvider.getROBData(vID, robFrags);

  //  (*log) << MSG::DEBUG << "Got ROB fragments: "<< robFrags.size() << endmsg;

  for (IROBDataProviderSvc::VROBFRAG::const_iterator rob = robFrags.begin();
       rob != robFrags.end(); ++rob) {

    uint32_t nData  = (*rob)->rod_ndata();

    //    size_t sourceID = (*rob)->rod_source_id();
    //    (*log) << MSG::DEBUG << "Reading fragment of size " << nData << " from source "
    //           << sourceID << endmsg;

    OFFLINE_FRAGMENTS_NAMESPACE::PointerType rodData = 0;
    (*rob)->rod_data(rodData);

    for (size_t i = 0; i < nData; i++) hltContent.push_back(rodData[i]);
  }

  result->deserialize(hltContent);
  ATH_MSG_DEBUG("Deserialized HLT Result " << objName << " (" << hltContent.size()
                << " words) from location " << subDet);
  
  return StatusCode::SUCCESS;
}
