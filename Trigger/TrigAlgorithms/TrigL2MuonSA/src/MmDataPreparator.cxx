/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MmDataPreparator.h"

TrigL2MuonSA::MmDataPreparator::MmDataPreparator(const std::string& type,
						 const std::string& name,
						 const IInterface*  parent):
  AthAlgTool(type,name,parent),
  m_regionSelector("RegSelTool/RegSelTool_MM",this)
{
  declareProperty("RegSel_MM", m_regionSelector);
}

StatusCode TrigL2MuonSA::MmDataPreparator::initialize()
{

  // Locate RegionSelector
  ATH_CHECK( m_regionSelector.retrieve() );
  ATH_MSG_DEBUG("Retrieved service RegionSelector");

  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_MSG_DEBUG("Retrieved " << m_idHelperSvc);

  ATH_CHECK(m_mmPrepContainerKey.initialize(!m_mmPrepContainerKey.empty()));

  return StatusCode::SUCCESS;
}

StatusCode TrigL2MuonSA::MmDataPreparator::prepareData(const TrigRoiDescriptor* p_roids,
						       TrigL2MuonSA::MmHits&  mmHits) const
{

  ATH_MSG_DEBUG("MmDataPreparator::prepareData() was called.");

  mmHits.clear();

  std::vector<const Muon::MMPrepDataCollection*> mmCols;
  std::vector<IdentifierHash> mmHashList;
  std::vector<IdentifierHash> mmHashList_cache;

  // Get MM container
  if(m_mmPrepContainerKey.empty()) {
    ATH_MSG_INFO("no mmPrepContainerKey");
    return StatusCode::SUCCESS;
  }
  auto mmPrepContainerHandle = SG::makeHandle(m_mmPrepContainerKey);
  if (!mmPrepContainerHandle.isValid()) {
    ATH_MSG_ERROR("Cannot retrieve MM PRD Container key: " << m_mmPrepContainerKey.key());
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("MM PRD Container retrieved with key: " << m_mmPrepContainerKey.key());
  }
  const Muon::MMPrepDataContainer* mmPrds = mmPrepContainerHandle.cptr();

  if (m_use_RoIBasedDataAccess) {
    // ATH_MSG_ERROR("RoI based data access is not available yet");

    ATH_MSG_DEBUG("Use RoI based data access");
    const IRoiDescriptor* iroi = (IRoiDescriptor*) p_roids;
    if (iroi) m_regionSelector->HashIDList(*iroi, mmHashList);
    else {
      TrigRoiDescriptor fullscan_roi( true );
      m_regionSelector->HashIDList(fullscan_roi, mmHashList);
    }
    ATH_MSG_DEBUG("mmHashList.size()=" << mmHashList.size());

    std::vector<uint32_t> mmRobList;
    m_regionSelector->ROBIDList(*iroi, mmRobList);

    if (!mmHashList.empty()) {

      // Get MM collections
      for(const IdentifierHash& id : mmHashList) {

	Muon::MMPrepDataContainer::const_iterator MMcoll = mmPrds->indexFind(id);

	if( MMcoll == mmPrds->end() ) {
	  continue;
	}

	if( (*MMcoll)->size() == 0) {
	  ATH_MSG_DEBUG("Empty MM list");
	  continue;
	}

	mmHashList_cache.push_back(id);
	mmCols.push_back(*MMcoll);
      }
    }

  }
  else {
    ATH_MSG_DEBUG("Use full data access");

    // Get MM collections
    for(const auto mmcoll : *mmPrds) {
      mmCols.push_back(mmcoll);
    }

  }


  for( const Muon::MMPrepDataCollection* mm : mmCols ){

    mmHits.reserve( mmHits.size() + mm->size() );
    for( const Muon::MMPrepData* prd : *mm ) {

      // save MM hit data in MmHitData
      TrigL2MuonSA::MmHitData lutDigit;

      lutDigit.eta = prd->globalPosition().eta();
      lutDigit.phi = prd->globalPosition().phi();
      lutDigit.r = prd->globalPosition().perp();
      lutDigit.z = prd->globalPosition().z();
      lutDigit.stationEta = m_idHelperSvc->mmIdHelper().stationEta(prd->identify());
      lutDigit.stationPhi = m_idHelperSvc->mmIdHelper().stationPhi(prd->identify());
      lutDigit.stationName = m_idHelperSvc->mmIdHelper().stationName(prd->identify());
      lutDigit.ResidualR = 999;
      lutDigit.ResidualPhi = 999;
      lutDigit.isOutlier = 0;
      lutDigit.layerNumber = m_idHelperSvc->mmIdHelper().gasGap(prd->identify())-1 + 4*(m_idHelperSvc->mmIdHelper().multilayer(prd->identify())-1);
      mmHits.push_back(lutDigit);

      ATH_MSG_DEBUG("MmDataPreparator: global eta/phi/r/z station eta/phi/name channel/layer " << lutDigit.eta << "/" << lutDigit.phi << "/" << lutDigit.r << "/" << lutDigit.z << " " << lutDigit.stationEta << "/" << lutDigit.stationPhi << "/" << lutDigit.stationName << " " << m_idHelperSvc->mmIdHelper().channel(prd->identify()) << "/" << lutDigit.layerNumber);

    }
  }

  return StatusCode::SUCCESS;

}
