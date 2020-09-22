/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStream/MdtRawDataProvider.h"

#include <algorithm>

Muon::MdtRawDataProvider::MdtRawDataProvider(const std::string& name,
                                      ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_regionSelector  ("RegSelSvc",name) 
{
  declareProperty ("RegionSelectionSvc", m_regionSelector, "Region Selector");
}

StatusCode Muon::MdtRawDataProvider::initialize() {

  ATH_MSG_INFO( "MdtRawDataProvider::initialize" );
  ATH_MSG_INFO( m_seededDecoding );

  ATH_CHECK( m_rawDataTool.retrieve() );

  ATH_CHECK( m_roiCollectionKey.initialize(m_seededDecoding) );// pass the seeded decoding flag - this marks the RoI collection flag as not used for the case when we decode the full detector

  if(m_seededDecoding) {
    // We only need the region selector in RoI seeded mode
    if (m_regionSelector.retrieve().isFailure()) {
      ATH_MSG_FATAL("Unable to retrieve RegionSelector Svc");
      return StatusCode::FAILURE;
    }  
  }//seededDecoding

  return StatusCode::SUCCESS;
}

StatusCode Muon::MdtRawDataProvider::execute() {
  ATH_MSG_VERBOSE( "MdtRawDataProvider::execute" );
  
  if(m_seededDecoding) {
    
    // read in the RoIs to process
    SG::ReadHandle<TrigRoiDescriptorCollection> muonRoI(m_roiCollectionKey);
    if(!muonRoI.isValid()){
      ATH_MSG_WARNING("Cannot retrieve muonRoI "<<m_roiCollectionKey.key());
      return StatusCode::SUCCESS;
    }

    // loop on RoIs
    std::vector<uint32_t> robs;
    for(auto roi : *muonRoI){
      ATH_MSG_DEBUG("Get ROBs for RoI " << *roi);
      // get list of ROBs from region selector
      m_regionSelector->DetROBIDListUint(MDT,*roi,robs);

      // decode the ROBs
      if(m_rawDataTool->convert(robs).isFailure()) {
        ATH_MSG_ERROR( "RoI seeded BS conversion into RDOs failed"  );
      }
      // clear vector of ROB IDs ready for next RoI
      robs.clear();
    }
  } else {

    // ask MdtRawDataProviderTool to decode the event and to fill the IDC
    if (m_rawDataTool->convert().isFailure())
      ATH_MSG_ERROR( "BS conversion into RDOs failed" );
  }

  return StatusCode::SUCCESS;
}
