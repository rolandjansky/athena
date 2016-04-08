/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigHLTResultByteStream/HLTResultByteStreamTool.h"
#include "TrigHLTResultByteStream/HLTSrcIdMap.h"
#include "eformat/SourceIdentifier.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include <iostream>
#include <stdlib.h>

std::string HLT::HLTResultByteStreamTool::s_l2ResultName = "HLTResult_L2";
std::string HLT::HLTResultByteStreamTool::s_efResultName = "HLTResult_EF";
std::string HLT::HLTResultByteStreamTool::s_hltResultName = "HLTResult_HLT";
std::string HLT::HLTResultByteStreamTool::s_dataScoutingResultName = "DataScouting_";

eformat::SubDetector HLT::HLTResultByteStreamTool::s_idL2 = eformat::TDAQ_LVL2;
eformat::SubDetector HLT::HLTResultByteStreamTool::s_idEF = eformat::TDAQ_EVENT_FILTER;  // same as TDAQ_HLT in eformat5

HLT::HLTResultByteStreamTool::HLTResultByteStreamTool( const std::string& type,
                                                       const std::string& name,
                                                       const IInterface* parent )
  :  AthAlgTool(type,name,parent)
{
  declareInterface< HLT::HLTResultByteStreamTool  >( this );
  m_feaL2.idMap().setDetId(s_idL2);
  m_feaEF.idMap().setDetId(s_idEF);
}


HLT::HLTResultByteStreamTool::~HLTResultByteStreamTool()
{
}

eformat::SubDetector HLT::HLTResultByteStreamTool::byteStreamLocation(std::string objName)
{
  if (objName == s_l2ResultName) return s_idL2;
  if (objName == s_efResultName || objName == s_hltResultName) return s_idEF;
  if (objName.substr(0,s_dataScoutingResultName.length()) == s_dataScoutingResultName) return s_idEF;

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
  FullEventAssembler<HLTSrcIdMap>* m_fea = eventAssembler(objName);
  eformat::SubDetector subDet = byteStreamLocation(objName);

  if (!m_fea || subDet == eformat::OTHER) {
    ATH_MSG_ERROR("Cannot store object with name " << objName
                  << " in BS: we are expecting only HLTResult_(L2,EF,HLT)");
    return StatusCode::FAILURE;
  }

  m_fea->clear() ;

  uint32_t module_id(0);
  if (objName.substr(0,s_dataScoutingResultName.length()) == s_dataScoutingResultName) {
    module_id = atoi( (objName.substr(s_dataScoutingResultName.length(),2)).c_str() ) ;
  }
  eformat::helper::SourceIdentifier helpID(subDet, module_id);
  uint32_t rodIdHLTResult = helpID.code();

  std::vector<uint32_t>* rod = m_fea->getRodData( rodIdHLTResult );
  if (!rod) return StatusCode::FAILURE;

  result->serialize( *rod );
  m_fea->fill(re, msg());

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

  //  (*log) << MSG::DEBUG << "Got ROB fragments: "<< robFrags.size() << endreq;

  for (IROBDataProviderSvc::VROBFRAG::const_iterator rob = robFrags.begin();
       rob != robFrags.end(); ++rob) {

    uint32_t nData  = (*rob)->rod_ndata();

    //    size_t sourceID = (*rob)->rod_source_id();
    //    (*log) << MSG::DEBUG << "Reading fragment of size " << nData << " from source "
    //           << sourceID << endreq;

    OFFLINE_FRAGMENTS_NAMESPACE::PointerType rodData = 0;
    (*rob)->rod_data(rodData);

    for (size_t i = 0; i < nData; i++) hltContent.push_back(rodData[i]);
  }

  result->deserialize(hltContent);
  ATH_MSG_DEBUG("Deserialized HLT Result " << objName << " (" << hltContent.size()
                << " words) from location " << subDet);
  
  return StatusCode::SUCCESS;
}
