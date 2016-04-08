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
AthAlgorithm(name, pSvcLocator),
m_tool( "Muon::MdtRdoToPrepDataTool/MdtPrepDataProviderTool"), // 'this' as 2nd arg would make it private tool
m_print_inputRdo(false),
m_print_prepData(false)
{
    declareProperty("DecodingTool",       m_tool,       "mdt rdo to prep data conversion tool" );
    declareProperty("PrintInputRdo",      m_print_inputRdo, "If true, will dump information about the input RDOs");
    declareProperty("PrintPrepData",      m_print_prepData, "If true, will dump information about the resulting PRDs");
}

StatusCode MdtRdoToMdtPrepData::finalize()
{
    ATH_MSG_DEBUG( "in finalize()"  );
    return StatusCode::SUCCESS;
}

StatusCode MdtRdoToMdtPrepData::initialize()
{  
    ATH_MSG_DEBUG( " in initialize()"  );
    
    // verify that our tool handle is pointing to an accessible tool
    ATH_CHECK( m_tool.retrieve() );
    ATH_MSG_INFO( "Retrieved " << m_tool  );
    
    return StatusCode::SUCCESS;
}

StatusCode MdtRdoToMdtPrepData::execute()
{
    ATH_MSG_DEBUG( "**************** in MdtRdoToMdtPrepData::execute() ***********************************************"  );
    ATH_MSG_DEBUG( "in execute()"  );
    
    std::vector<IdentifierHash> myVector;
    std::vector<IdentifierHash> myVectorWithData;
    myVector.reserve(0); // empty vector 
    ATH_CHECK(  m_tool->decode(myVector, myVectorWithData) );

    if (m_print_inputRdo) m_tool->printInputRdo();
    if (m_print_prepData) m_tool->printPrepData();

    return StatusCode::SUCCESS;
}
