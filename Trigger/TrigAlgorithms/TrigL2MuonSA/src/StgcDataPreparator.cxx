/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "StgcDataPreparator.h"

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

  // Locate RegionSelector
  ATH_CHECK( m_regionSelector.retrieve() );
  ATH_MSG_DEBUG("Retrieved service RegionSelector");

  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_MSG_DEBUG("Retrieved " << m_idHelperSvc);

  ATH_CHECK(m_stgcPrepContainerKey.initialize(!m_stgcPrepContainerKey.empty()));

  return StatusCode::SUCCESS;
}

StatusCode TrigL2MuonSA::StgcDataPreparator::prepareData(const TrigRoiDescriptor* p_roids,
							 TrigL2MuonSA::StgcHits&  stgcHits) const
{

  ATH_MSG_DEBUG("StgcDataPreparator::prepareData() was called.");

  stgcHits.clear();

  std::vector<const Muon::sTgcPrepDataCollection*> stgcCols;
  std::vector<IdentifierHash> stgcHashList;
  std::vector<IdentifierHash> stgcHashList_cache;

  // Get sTGC container
  if(m_stgcPrepContainerKey.empty()) {
    ATH_MSG_INFO("no stgcPrepContainerKey");
    return StatusCode::SUCCESS;
  }
  auto stgcPrepContainerHandle = SG::makeHandle(m_stgcPrepContainerKey);
  if (!stgcPrepContainerHandle.isValid()) {
    ATH_MSG_ERROR("Cannot retrieve sTgc PRD Container key: " << m_stgcPrepContainerKey.key());
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("sTgc PRD Container retrieved with key: " << m_stgcPrepContainerKey.key());
  }
  const Muon::sTgcPrepDataContainer* stgcPrds = stgcPrepContainerHandle.cptr();

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

    if (!stgcHashList.empty()) {

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

  }
  else {
    ATH_MSG_DEBUG("Use full data access");

    // Get sTgc collections
    for(const auto stgccoll : *stgcPrds) {
      stgcCols.push_back(stgccoll);
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
