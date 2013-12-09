/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// Author: Ketevi A. Assamagan
/// BNL, April 03 2005

/// algorithm to decode RDO into PrepRawData

/// Author: Ketevi A. Assamagan
/// BNL, April 03 2005

/// algorithm to decode RDO into PrepRawData

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IToolSvc.h"

#include "StoreGate/DataHandle.h"
#include "Identifier/IdentifierHash.h"
#include "MuonRdoToPrepData/CscRdoToCscPrepData.h"

CscRdoToCscPrepData::CscRdoToCscPrepData(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator),
    m_muonRdoToPrepDataTool ("Muon::CscRdoToCscPrepDataTool/CscRdoToPrepDataTool"),
    m_print_inputRdo(false),
    m_print_prepData(false)
{
    declareProperty("CscRdoToCscPrepDataTool",     m_muonRdoToPrepDataTool );
    declareProperty("PrintInputRdo",      m_print_inputRdo, "If true, will dump information about the input RDOs");
    declareProperty("PrintPrepData",      m_print_prepData, "If true, will dump information about the resulting PRDs");
}

StatusCode CscRdoToCscPrepData::finalize() {
  ATH_MSG_DEBUG("in finalize()");
  return StatusCode::SUCCESS;
}

StatusCode CscRdoToCscPrepData::initialize(){
    ATH_MSG_DEBUG(" in initialize()");

    if ( m_muonRdoToPrepDataTool.retrieve().isFailure() ) {
        ATH_MSG_FATAL("Failed to retrieve " << m_muonRdoToPrepDataTool);
        return StatusCode::FAILURE;
    } else {
        ATH_MSG_INFO("Retrieved " << m_muonRdoToPrepDataTool);
    }
    return StatusCode::SUCCESS;
}

StatusCode CscRdoToCscPrepData::execute() {
    ATH_MSG_DEBUG( " *************** in CscRdoToCscPrepData::execute()");

    /// process CSC RDO
    std::vector<IdentifierHash> givenIDs;
    std::vector<IdentifierHash> decodedIDs;
    // givenIDs size is zero so this invokes all the RDOs conversion to PrepData
    StatusCode status =   m_muonRdoToPrepDataTool->decode(givenIDs, decodedIDs);
    if (status.isFailure()) {
        ATH_MSG_ERROR("Unable to decode CSC RDO into CSC PrepRawData");
        return status;
    }

    if (m_print_inputRdo) m_muonRdoToPrepDataTool->printInputRdo();//printRpcPrepRawData();
    if (m_print_prepData) m_muonRdoToPrepDataTool->printPrepData();//printRpcPrepRawData();

    return status;
}



