/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "MuonRdoToPrepData/StgcRdoToStgcPrepData.h"

#include "Identifier/IdentifierHash.h"

StgcRdoToStgcPrepData::StgcRdoToStgcPrepData(const std::string& name, ISvcLocator* pSvcLocator) :
AthAlgorithm(name, pSvcLocator),
m_decoderTool ("Muon::STGC_RawDataToPrepDataTool/STGC_RawDataToPrepDataTool"),
m_prdContainer("sTGC_Measurements")
{
  declareProperty("OutputCollection",   m_prdContainer);
}

StatusCode StgcRdoToStgcPrepData::finalize() {
  ATH_MSG_DEBUG("in finalize()");
  return StatusCode::SUCCESS;
}

StatusCode StgcRdoToStgcPrepData::initialize(){
  ATH_MSG_DEBUG(" in initialize()");

  ATH_CHECK(m_prdContainer.initialize());

  return StatusCode::SUCCESS;
}

StatusCode StgcRdoToStgcPrepData::execute() {
  ATH_MSG_DEBUG( " *************** in StgcRdoToStgcPrepData::execute()");

  /// process CSC RDO
  std::vector<IdentifierHash> givenIDs;
  std::vector<IdentifierHash> decodedIDs;
  StatusCode status = StatusCode::SUCCESS;

  // givenIDs size is zero so this invokes all the RDOs conversion to PrepData
  status =   m_decoderTool->decode(givenIDs, decodedIDs);

  if (status.isFailure()) {
    ATH_MSG_ERROR("Unable to decode STGC RDO into STGC PrepRawData");
    return status;
  }

  return status;
}



