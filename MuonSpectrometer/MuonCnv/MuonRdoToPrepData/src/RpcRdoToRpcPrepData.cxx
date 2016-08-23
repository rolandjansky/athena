/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// Author: Stefania Spagnolo 
/// Lecce, January  2006
///
/// Author: Davide Costanzo
/// BNL, April 06 2005

/// algorithm to decode RDO into PrepRawData

#include "MuonRdoToPrepData/RpcRdoToRpcPrepData.h"
#include "Identifier/IdentifierHash.h"

RpcRdoToRpcPrepData::RpcRdoToRpcPrepData(const std::string& name, ISvcLocator* pSvcLocator) 
    :
    AthAlgorithm(name, pSvcLocator),
    m_tool( "Muon::RpcRdoToPrepDataTool/RpcPrepDataProviderTool"), // 'this' as 2nd arg would make it private tool
    m_print_inputRdo(false),
    m_print_prepData(false)
{
    declareProperty("DecodingTool",       m_tool,       "rpc rdo to prep data conversion tool" );
    declareProperty("PrintInputRdo",      m_print_inputRdo, "If true, will dump information about the input RDOs");
    declareProperty("PrintPrepData",      m_print_prepData, "If true, will dump information about the resulting PRDs");
}  

StatusCode RpcRdoToRpcPrepData::finalize() {
  ATH_MSG_DEBUG("in finalize()");
  return StatusCode::SUCCESS;
}

StatusCode RpcRdoToRpcPrepData::initialize(){
    
  ATH_MSG_DEBUG(" in initialize()");
    
  // verify that our tool handle is pointing to an accessible tool
  if ( m_tool.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve " << m_tool << endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved " << m_tool << endmsg;
  }
    
    return StatusCode::SUCCESS;
}

StatusCode RpcRdoToRpcPrepData::execute() {

    StatusCode status;

    
    ATH_MSG_DEBUG("**************** in RpcRdoToRpcPrepData::execute() ***********************************************");
    ATH_MSG_DEBUG("in execute()");
    status = StatusCode::SUCCESS;
    
    std::vector<IdentifierHash> myVector;
    std::vector<IdentifierHash> myVectorWithData;
//     int bmlSideAStart = 0; //102
//     int bmlSideCStop = 600;
//     myVector.reserve(bmlSideCStop-bmlSideAStart); // Improve performance by reserving needed space in advance.
//     for (int bml=bmlSideAStart; bml<bmlSideCStop; ++bml)
//     {
//         IdentifierHash bmlHash((IdentifierHash)bml);
//         myVector.push_back(bmlHash);
//     }
    myVector.reserve(0); // empty vector 
    status = m_tool->decode(myVector, myVectorWithData);
    if (status.isFailure()) {
      msg(MSG::ERROR) << "Unable to decode RPC RDO into RPC PrepRawData" 
		      << endmsg;
        return status;
    }

    if (m_print_inputRdo) m_tool->printInputRdo();//printRpcPrepRawData();
    if (m_print_prepData) m_tool->printPrepData();//printRpcPrepRawData();

//     // try once more for debugging 
//     status = m_tool->decode(myVector, myVectorWithData);
//     if (status.isFailure()) {
//        msg(MSG::ERROR) << "Unable to decode RPC RDO into RPC PrepRawData" 
//             << endmsg;
//         return status;
//     }
//     if (m_print_prepData) m_tool->printPrepData();//printRpcPrepRawData();
    
    return status;

}

