/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "MuonRdoToPrepData/StgcRdoToStgcPrepData.h"

#include "Identifier/IdentifierHash.h"

StgcRdoToStgcPrepData::StgcRdoToStgcPrepData(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_prdContainer("STGC_Measurements")
{
  declareProperty("OutputCollection",   m_prdContainer);
}

StatusCode StgcRdoToStgcPrepData::initialize(){
  ATH_MSG_DEBUG(" in initialize()");
  ATH_CHECK(m_prdContainer.initialize());
  ATH_CHECK( m_decoderTool.retrieve() );
  ATH_MSG_INFO("Retrieved" << m_decoderTool);

  ATH_MSG_INFO("DoSeededDecoding = " << m_seededDecoding);

  ATH_CHECK( m_roiCollectionKey.initialize(m_seededDecoding) ); // mark the RoI-collection flag as used or not used

  if (m_seededDecoding) {
    if (m_regsel_stgc.retrieve().isFailure()) { // in RoI - seeded mode, retrieve the region selector
      ATH_MSG_FATAL("Unable to retrieve RegionSelector Tool");
      return StatusCode::FAILURE;
    }
  } else {
    m_regsel_stgc.disable();
  }

  return StatusCode::SUCCESS;
}

StatusCode StgcRdoToStgcPrepData::execute() {
  ATH_MSG_DEBUG( " *************** in StgcRdoToStgcPrepData::execute()");

  /// process STGC RDO
  std::vector<IdentifierHash> idlist;
  std::vector<IdentifierHash> decodedIDs;
  StatusCode status = StatusCode::SUCCESS;

  
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
      m_regsel_stgc->HashIDList(*roi,roi_idlist);
      idlist.insert(idlist.end(), roi_idlist.begin(), roi_idlist.end());
    }
    // Pass HashID list to decoding tool to decode all needed data
    std::vector<IdentifierHash> idlistWithData;
    ATH_CHECK(m_decoderTool->decode( idlist, idlistWithData));
 
  } else {
    // idlist size is zero so this invokes all the RDOs conversion to PrepData
    status =   m_decoderTool->decode(idlist, decodedIDs);  

    if (status.isFailure()) {
      ATH_MSG_ERROR("Unable to decode STGC RDO into STGC PrepRawData");
      return status;
    }
  }

  return status;
}



