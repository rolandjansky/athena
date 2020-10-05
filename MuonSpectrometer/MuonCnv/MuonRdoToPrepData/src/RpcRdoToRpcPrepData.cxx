/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
    m_print_inputRdo(false),
    m_print_prepData(false),
    m_seededDecoding(false),
    m_roiCollectionKey("OutputRoIs"),
    m_rpcCollection("RPC_Measurements")
{
    declareProperty("PrintInputRdo",      m_print_inputRdo, "If true, will dump information about the input RDOs");
    declareProperty("PrintPrepData",      m_print_prepData, "If true, will dump information about the resulting PRDs");
    declareProperty("DoSeededDecoding",   m_seededDecoding, "If true decode only in RoIs");
    declareProperty("RoIs",               m_roiCollectionKey, "RoIs to read in");
    declareProperty("OutputCollection", m_rpcCollection);
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

  //Nullify key from scheduler if not needed  
  if(!m_seededDecoding){
    m_roiCollectionKey = "";
    m_rpcCollection = "";
  }
  if(m_seededDecoding){
    ATH_CHECK(m_roiCollectionKey.initialize());
    ATH_CHECK(m_rpcCollection.initialize());
    if (m_regsel_rpc.retrieve().isFailure()) {
      ATH_MSG_FATAL("Unable to retrieve m_regsel_rpc");
      return StatusCode::FAILURE;
    }
  }


  return StatusCode::SUCCESS;
}

StatusCode RpcRdoToRpcPrepData::execute() {

    StatusCode status= StatusCode::SUCCESS;

    
    ATH_MSG_DEBUG("**************** in RpcRdoToRpcPrepData::execute() ***********************************************");
    ATH_MSG_DEBUG("in execute()");
    
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

    if(m_seededDecoding){
      bool decoded=false;
      SG::ReadHandle<TrigRoiDescriptorCollection> muonRoI(m_roiCollectionKey);
      if(!muonRoI.isValid()){
	ATH_MSG_WARNING("Cannot retrieve muonRoI "<<m_roiCollectionKey.key());
	return StatusCode::SUCCESS;
      }
      else{
	std::vector<uint32_t> rpcrobs;
	for(auto roi : *muonRoI){
	  m_regsel_rpc->ROBIDList(*roi,rpcrobs);
	  if(rpcrobs.size()!=0){
	    decoded=true;
	    status=m_tool->decode(rpcrobs);
	    rpcrobs.clear();
	  }
	}
      }
      if(!decoded){
	//Need to store an empty prd container if we didn't decode anything
	//as the container is expected to exist downstream
	SG::WriteHandle<Muon::RpcPrepDataContainer> h_output (m_rpcCollection);
	ATH_CHECK(h_output.record(std::make_unique<Muon::RpcPrepDataContainer>(0)));
      }
    }
    else status = m_tool->decode(myVector, myVectorWithData);

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

