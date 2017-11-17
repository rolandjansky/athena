#include "MuonRdoToPrepData/MM_RdoToMM_PrepData.h"

MM_RdoToMM_PrepData::MM_RdoToMM_PrepData(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator)
{
}

StatusCode MM_RdoToMM_PrepData::finalize() {
  ATH_MSG_DEBUG("in finalize()");
  return StatusCode::SUCCESS;
}

StatusCode MM_RdoToMM_PrepData::initialize(){
    ATH_MSG_DEBUG(" in initialize()");
    return StatusCode::SUCCESS;
}

StatusCode MM_RdoToMM_PrepData::execute() {
    ATH_MSG_DEBUG( " *************** in MM_RdoToMM_PrepData::execute()");
    return StatusCode::SUCCESS;
}



