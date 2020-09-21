/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRdoToPrepData/MM_RdoToMM_PrepData.h"
#include "Identifier/IdentifierHash.h"

MM_RdoToMM_PrepData::MM_RdoToMM_PrepData(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_print_inputRdo(false),
  m_print_prepData(false)
{
  declareProperty("PrintInputRdo",      m_print_inputRdo, "If true, will dump information about the input RDOs");
  declareProperty("PrintPrepData",      m_print_prepData, "If true, will dump information about the resulting PRDs");
}

StatusCode MM_RdoToMM_PrepData::initialize(){
  ATH_MSG_DEBUG(" in initialize()");
  
  ATH_CHECK( m_tool.retrieve() );
  ATH_MSG_INFO("Retrieved" << m_tool);

  return StatusCode::SUCCESS;
}

StatusCode MM_RdoToMM_PrepData::execute() {

  ATH_MSG_DEBUG( " *************** in MM_RdoToMM_PrepData::execute() **************************************");
  
  std::vector<IdentifierHash> myVector;
  std::vector<IdentifierHash> myVectorWithData;
  myVector.reserve(0); // empty vector 
  ATH_CHECK(  m_tool->decode(myVector, myVectorWithData) );
  
  if (m_print_inputRdo) m_tool->printInputRdo();
  if (m_print_prepData) m_tool->printPrepData();
  
  return StatusCode::SUCCESS;
}



