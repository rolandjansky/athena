/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStream/CscRawDataProvider.h"

#include "CSCcabling/CSCcablingSvc.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

#include <algorithm>

Muon::CscRawDataProvider::CscRawDataProvider(const std::string& name,
                                      ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_regionSelector  ("RegSelSvc",name)
{
  declareProperty ("RegionSelectionSvc", m_regionSelector, "Region Selector");
}

// --------------------------------------------------------------------
// Initialize
StatusCode Muon::CscRawDataProvider::initialize() {
  ATH_MSG_VERBOSE(" in initialize()");
  ATH_MSG_VERBOSE( m_seededDecoding );

  ATH_CHECK( m_roiCollectionKey.initialize(m_seededDecoding) );// pass the seeded decoding flag - this marks the RoI collection flag as not used for the case when we decode the full detector

  // Get CscRawDataProviderTool
  ATH_CHECK( m_rawDataTool.retrieve() );

  // We only need the region selector in RoI seeded mode
  if(m_seededDecoding) ATH_CHECK( m_regionSelector.retrieve() );

  ATH_CHECK( m_ALineKey.initialize(m_seededDecoding) ); // !!! REMOVEME: when MuonDetectorManager in cond store

  return StatusCode::SUCCESS;
}

StatusCode Muon::CscRawDataProvider::execute() {
  ATH_MSG_VERBOSE( "CscRawDataProvider::execute" );

  if(m_seededDecoding) {

    SG::ReadCondHandle<ALineMapContainer> readALineHandle(m_ALineKey);// !!! REMOVEME: when MuonDetectorManager in cond store
    if(!readALineHandle.isValid()){// !!! REMOVEME: when MuonDetectorManager in cond store
      ATH_MSG_WARNING("Cannot retrieve ALine Handle "<<m_ALineKey.key());// !!! REMOVEME: when MuonDetectorManager in cond store
      return StatusCode::SUCCESS;// !!! REMOVEME: when MuonDetectorManager in cond store
    }// !!! REMOVEME: when MuonDetectorManager in cond store
  
    // read in the RoIs to process
    SG::ReadHandle<TrigRoiDescriptorCollection> muonRoI(m_roiCollectionKey);
    if(!muonRoI.isValid()){
      ATH_MSG_WARNING("Cannot retrieve muonRoI "<<m_roiCollectionKey.key());
      return StatusCode::SUCCESS;
    }

    // loop on RoIs
    std::vector<IdentifierHash>  csc_hash_ids;
    for(auto roi : *muonRoI){
      ATH_MSG_DEBUG("Get has IDs for RoI " << *roi);
      // get list of hash IDs from region selection
      m_regionSelector->DetHashIDList(CSC, *roi, csc_hash_ids);

      // decode the ROBs
      if(m_rawDataTool->convert(csc_hash_ids).isFailure()) {
        ATH_MSG_ERROR( "RoI seeded BS conversion into RDOs failed"  );
      }
      // clear vector of hash IDs ready for next RoI
      csc_hash_ids.clear();
    }
  } else {
   // ask CscRawDataProviderTool to decode entire event and to fill the IDC
   if (m_rawDataTool->convert().isFailure())
     ATH_MSG_ERROR ( "BS conversion into RDOs failed" );
 }

  return StatusCode::SUCCESS;
}
