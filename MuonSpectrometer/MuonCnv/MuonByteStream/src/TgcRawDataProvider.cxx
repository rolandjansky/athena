/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStream/TgcRawDataProvider.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "MuonRDO/TgcRdoIdHash.h"
#include "eformat/SourceIdentifier.h"
#include <algorithm>

using eformat::helper::SourceIdentifier;

// --------------------------------------------------------------------
// Constructor

Muon::TgcRawDataProvider::TgcRawDataProvider(const std::string& name,
        ISvcLocator* pSvcLocator) :
        AthAlgorithm(name, pSvcLocator),
        m_rawDataTool     ("Muon::TGC_RawDataProviderToolMT/TgcRawDataProviderTool", this),
        m_regionSelector  ("RegSelSvc",name)
{
    declareProperty ("ProviderTool", m_rawDataTool);
    declareProperty ("RegionSelectionSvc", m_regionSelector, "Region Selector");
}

// Destructor

Muon::TgcRawDataProvider::~TgcRawDataProvider()
{}

// --------------------------------------------------------------------
// Initialize

StatusCode Muon::TgcRawDataProvider::initialize()
{
    ATH_MSG_INFO( "TgcRawDataProvider::initialize"  );
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

StatusCode Muon::TgcRawDataProvider::finalize()
{
    return StatusCode::SUCCESS;
}
// --------------------------------------------------------------------
// Execute

StatusCode Muon::TgcRawDataProvider::execute()
{
    ATH_MSG_VERBOSE( "TgcRawDataProvider::execute"  );

  if(m_seededDecoding) {
    
    // read in the RoIs to process
    SG::ReadHandle<TrigRoiDescriptorCollection> muonRoI(m_roiCollectionKey);
    if(!muonRoI.isValid()){
      ATH_MSG_WARNING("Cannot retrieve muonRoI "<<m_roiCollectionKey.key());
      return StatusCode::SUCCESS;
    }

    // loop on RoIs
    std::vector<IdentifierHash>  tgc_hash_ids;
    for(auto roi : *muonRoI){
      ATH_MSG_DEBUG("Get ROBs for RoI " << *roi);
      // get list of hash IDs from region selection
      m_regionSelector->DetHashIDList(TGC, *roi, tgc_hash_ids);

      // decode the ROBs
      if(m_rawDataTool->convert(tgc_hash_ids).isFailure()) {
        ATH_MSG_ERROR( "RoI seeded BS conversion into RDOs failed"  );
      }
      // clear vector of hash IDs ready for next RoI
      tgc_hash_ids.clear();
    }
  } else {
    // ask TgcRawDataProviderTool to decode full detector and to fill the IDC
    if (m_rawDataTool->convert().isFailure())
      ATH_MSG_ERROR( "BS conversion into RDOs failed"  );
  }

    return StatusCode::SUCCESS;
}
