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

  ATH_MSG_INFO("DoSeededDecoding = " << m_seededDecoding);

  ATH_CHECK( m_roiCollectionKey.initialize(m_seededDecoding) ); // mark the RoI-collection flag as used or not used

  if (m_seededDecoding) {
    if (m_regsel_mm.retrieve().isFailure()) { // in RoI - seeded mode, retrieve the region selector
      ATH_MSG_FATAL("Unable to retrieve RegionSelector Tool");
      return StatusCode::FAILURE;
    }
  } else {
    m_regsel_mm.disable();
  }

  return StatusCode::SUCCESS;
}

StatusCode MM_RdoToMM_PrepData::execute() {

  ATH_MSG_DEBUG( " *************** in MM_RdoToMM_PrepData::execute() **************************************");
  

  if(m_seededDecoding) {
    // decode only in RoIs
    SG::ReadHandle<TrigRoiDescriptorCollection> muonRoI(m_roiCollectionKey);
    if(!muonRoI.isValid()){
      ATH_MSG_WARNING("Cannot retrieve muonRoI "<<m_roiCollectionKey.key());
      return StatusCode::SUCCESS;
    }
    

    std::vector<IdentifierHash> idlist;
    // loop on RoIs, collect up all HashIDs we need
    for(auto roi : *muonRoI) {
      std::vector<IdentifierHash> roi_idlist;
      m_regsel_mm->HashIDList(*roi,roi_idlist);
      idlist.insert(idlist.end(), roi_idlist.begin(), roi_idlist.end());
    }
    // Pass HashID list to decoding tool to decode all needed data
    std::vector<IdentifierHash> idlistWithData;
    ATH_CHECK(m_tool->decode( idlist, idlistWithData));
 
  } else {
    // decode full detector
    std::vector<IdentifierHash> myVector;
    std::vector<IdentifierHash> myVectorWithData;
    myVector.reserve(0); // empty vector 
    ATH_CHECK(  m_tool->decode(myVector, myVectorWithData) );
  }    

  if (m_print_inputRdo) m_tool->printInputRdo();
  if (m_print_prepData) m_tool->printPrepData();
  
  return StatusCode::SUCCESS;
}



