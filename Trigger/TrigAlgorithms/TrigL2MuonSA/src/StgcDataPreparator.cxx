/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "StgcDataPreparator.h"
#include "RecMuonRoIUtils.h"

TrigL2MuonSA::StgcDataPreparator::StgcDataPreparator(const std::string& type,
						     const std::string& name,
						     const IInterface*  parent):
  AthAlgTool(type,name,parent),
  m_regionSelector("RegSelTool/RegSelTool_STGC",this)
{
  declareProperty("RegSel_STGC", m_regionSelector);
}

StatusCode TrigL2MuonSA::StgcDataPreparator::initialize()
{

  ATH_MSG_DEBUG("StgcDataPreparator::initialize() doDecoding/decodeBS " << m_doDecoding << "/" << m_decodeBS);

  // Locate RegionSelector
  ATH_CHECK( m_regionSelector.retrieve() );
  ATH_MSG_DEBUG("Retrieved service RegionSelector");

  // consistency check for decoding flag settings
  if(m_decodeBS && !m_doDecoding) {
    ATH_MSG_FATAL("Inconsistent setup, you tried to enable BS decoding but disable all decoding. Please fix the configuration");
    return StatusCode::FAILURE;
  }

  // disable the RDO->PRD decoding tool if we don't do the sTGC data decoding
  ATH_CHECK( m_stgcPrepDataProvider.retrieve(DisableTool{!m_doDecoding}) );
  ATH_MSG_DEBUG("Retrieved " << m_stgcPrepDataProvider);

  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_MSG_DEBUG("Retrieved " << m_idHelperSvc);

  // Retreive PRC raw data provider tool
  ATH_MSG_DEBUG("Decode BS set to " << m_decodeBS);
  // disable the BS->RDO decoding tool if we don't do the sTGC data decoding
  ATH_CHECK( m_rawDataProviderTool.retrieve(DisableTool{ !m_decodeBS || !m_doDecoding }) );
  ATH_MSG_DEBUG("Retrieved Tool " << m_rawDataProviderTool);

  // ATH_CHECK(m_readKey.initialize());
  ATH_CHECK(m_stgcPrepContainerKey.initialize(!m_stgcPrepContainerKey.empty()));

  return StatusCode::SUCCESS;
}

StatusCode TrigL2MuonSA::StgcDataPreparator::prepareData(const TrigRoiDescriptor* p_roids,
							 TrigL2MuonSA::StgcHits&  stgcHits)
{

  ATH_MSG_DEBUG("StgcDataPreparator::prepareData() was called.");

  stgcHits.clear();

  std::vector<const Muon::sTgcPrepDataCollection*> stgcCols;
  std::vector<IdentifierHash> stgcHashList;
  std::vector<IdentifierHash> stgcHashList_cache;

  if (m_use_RoIBasedDataAccess) {
    // ATH_MSG_ERROR("RoI based data access is not available yet");

    ATH_MSG_DEBUG("Use RoI based data access");
    const IRoiDescriptor* iroi = (IRoiDescriptor*) p_roids;
    if (iroi) m_regionSelector->HashIDList(*iroi, stgcHashList);
    else {
      TrigRoiDescriptor fullscan_roi( true );
      m_regionSelector->HashIDList(fullscan_roi, stgcHashList);
    }
    ATH_MSG_DEBUG("stgcHashList.size()=" << stgcHashList.size());

    std::vector<uint32_t> stgcRobList;
    m_regionSelector->ROBIDList(*iroi, stgcRobList);
     if(m_doDecoding) {
       if(m_decodeBS) {
         if ( m_rawDataProviderTool->convert(stgcRobList).isFailure()) {
             ATH_MSG_WARNING("Conversion of BS for decoding of sTGC failed");
         }
       }
       if ( m_stgcPrepDataProvider->decode(stgcRobList).isFailure() ) {
         ATH_MSG_WARNING("Problems when preparing sTGC PrepData ");
       }
     }//do decoding

  }
  else {
    ATH_MSG_DEBUG("Use full data access");

    TrigRoiDescriptor fullscan_roi( true );
    m_regionSelector->HashIDList(fullscan_roi, stgcHashList);
    ATH_MSG_DEBUG("stgcHashList.size()=" << stgcHashList.size());

    std::vector<uint32_t> stgcRobList;
    m_regionSelector->ROBIDList(fullscan_roi, stgcRobList);
    if(m_doDecoding) {
      if(m_decodeBS) {
	if ( m_rawDataProviderTool->convert(stgcRobList).isFailure()) {
	  ATH_MSG_WARNING("Conversion of BS for decoding of sTgcs failed");
	}
	ATH_MSG_ERROR("Conversion of BS for decoding of sTgcs is not available yet");
      }
      if ( m_stgcPrepDataProvider->decode(stgcRobList).isFailure() ) {
	ATH_MSG_WARNING("Problems when preparing sTgc PrepData ");
      }
    }//do decoding

  }

  if (!stgcHashList.empty()) {

    // Get sTGC container
    const Muon::sTgcPrepDataContainer* stgcPrds;
    auto stgcPrepContainerHandle = SG::makeHandle(m_stgcPrepContainerKey);
    stgcPrds = stgcPrepContainerHandle.cptr();
    if (!stgcPrepContainerHandle.isValid()) {
      ATH_MSG_ERROR("Cannot retrieve sTgc PRD Container key: " << m_stgcPrepContainerKey.key());
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("sTgc PRD Container retrieved with key: " << m_stgcPrepContainerKey.key());
    }

    // Get sTGC collections
    for(const IdentifierHash& id : stgcHashList) {

      Muon::sTgcPrepDataContainer::const_iterator STGCcoll = stgcPrds->indexFind(id);

      if( STGCcoll == stgcPrds->end() ) {
	continue;
      }

      if( (*STGCcoll)->size() == 0) {
	ATH_MSG_DEBUG("Empty STGC list");
	continue;
      }

      stgcHashList_cache.push_back(id);
      stgcCols.push_back(*STGCcoll);
    }
  }

  for( const Muon::sTgcPrepDataCollection* stgc : stgcCols ){

    stgcHits.reserve( stgcHits.size() + stgc->size() );
    for( const Muon::sTgcPrepData* prepData : *stgc ) {

      // save sTGC hit data in StgcHitData
      TrigL2MuonSA::StgcHitData lutDigit;

      lutDigit.eta = prepData->globalPosition().eta();
      lutDigit.phi = prepData->globalPosition().phi();
      lutDigit.r = prepData->globalPosition().perp();
      lutDigit.z = prepData->globalPosition().z();
      lutDigit.stationEta = m_idHelperSvc->stgcIdHelper().stationEta(prepData->identify());
      lutDigit.stationPhi = m_idHelperSvc->stgcIdHelper().stationPhi(prepData->identify());
      lutDigit.stationName = m_idHelperSvc->stgcIdHelper().stationName(prepData->identify());
      lutDigit.channelType = m_idHelperSvc->stgcIdHelper().channelType(prepData->identify()); // strip=1, wire=2, pad=0
      lutDigit.ResidualR = 999;
      lutDigit.ResidualPhi = 999;
      lutDigit.isOutlier = 0;
      lutDigit.layerNumber = m_idHelperSvc->stgcIdHelper().gasGap(prepData->identify())-1 + 4*(m_idHelperSvc->stgcIdHelper().multilayer(prepData->identify())-1);
      stgcHits.push_back(lutDigit);

      ATH_MSG_DEBUG("StgcDataPreparator: global eta/phi/r/z station eta/phi/name channel_type layer " << lutDigit.eta << "/" << lutDigit.phi << "/" << lutDigit.r << "/" << lutDigit.z << " " << lutDigit.stationEta << "/" << lutDigit.stationPhi << "/" << lutDigit.stationName << " " << lutDigit.channelType << " " << (m_idHelperSvc->stgcIdHelper().gasGap(prepData->identify())-1 + 4*(m_idHelperSvc->stgcIdHelper().multilayer(prepData->identify())-1)) );

    }
  }

  return StatusCode::SUCCESS;
}
