/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// Author: Davide Costanzo
/// BNL, April 04 2005

/// algorithm to decode RDO into PrepRawData


#include "MuonRdoToPrepData/MdtRdoToMdtPrepData.h"
#include "Identifier/IdentifierHash.h"

MdtRdoToMdtPrepData::MdtRdoToMdtPrepData(const std::string& name,
                                         ISvcLocator* pSvcLocator) :
Algorithm(name, pSvcLocator),
m_tool( "Muon::MdtRdoToPrepDataTool/MdtPrepDataProviderTool"), // 'this' as 2nd arg would make it private tool
m_print_inputRdo(false),
m_print_prepData(false),
m_log(msgSvc(), name),
m_debug(false),
m_verbose(false)
{
    declareProperty("DecodingTool",       m_tool,       "mdt rdo to prep data conversion tool" );
    declareProperty("PrintInputRdo",      m_print_inputRdo, "If true, will dump information about the input RDOs");
    declareProperty("PrintPrepData",      m_print_prepData, "If true, will dump information about the resulting PRDs");
}

StatusCode MdtRdoToMdtPrepData::finalize()
{
    if (m_debug) m_log << MSG::DEBUG << "in finalize()" << endreq;
    return StatusCode::SUCCESS;

}

StatusCode MdtRdoToMdtPrepData::initialize()
{  
    // Set cached output variables
    m_debug = m_log.level() <= MSG::DEBUG;
    m_verbose = m_log.level() <= MSG::VERBOSE;
    
    if (m_debug) m_log << MSG::DEBUG << " in initialize()" << endreq;
    
    // verify that our tool handle is pointing to an accessible tool
    if ( m_tool.retrieve().isFailure() ) {
      m_log << MSG::FATAL << "Failed to retrieve " << m_tool << endreq;
      return StatusCode::FAILURE;
    } else {
      m_log << MSG::INFO << "Retrieved " << m_tool << endreq;
    }
    
    return StatusCode::SUCCESS;
}

StatusCode MdtRdoToMdtPrepData::execute()
{
    StatusCode status;

    if( m_debug ){
      m_log << MSG::DEBUG << "**************** in MdtRdoToMdtPrepData::execute() ***********************************************" << endreq;
      m_log << MSG::DEBUG << "in execute()" << endreq;
    }
    status = StatusCode::SUCCESS;
    
    std::vector<IdentifierHash> myVector;
    std::vector<IdentifierHash> myVectorWithData;
    myVector.reserve(0); // empty vector 
    status = m_tool->decode(myVector, myVectorWithData);
    if (status.isFailure()) {
        m_log << MSG::ERROR << "Unable to decode MDT RDO into MDT PrepRawData" 
              << endreq;
        return status;
    }

    if (m_print_inputRdo) m_tool->printInputRdo();
    if (m_print_prepData) m_tool->printPrepData();

    return status;    
}
