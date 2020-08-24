/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SCTRawDataProvider.h"

#include "SCT_RawDataByteStreamCnv/ISCTRawDataProviderTool.h"
#include "SCT_Cabling/ISCT_CablingTool.h"
#include "InDetIdentifier/SCT_ID.h"
#include "EventContainers/IdentifiableContTemp.h"
#include "EventContainers/IdentifiableContainerBase.h"

#include <memory>

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

// Constructor

SCTRawDataProvider::SCTRawDataProvider(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{
  declareProperty("RDOCacheKey", m_rdoContainerCacheKey="");
  declareProperty("BSErrCacheKey", m_bsErrContainerCacheKey="");
}

// Initialize

StatusCode SCTRawDataProvider::initialize()
{
  // Get ROBDataProviderSvc
  ATH_CHECK(m_robDataProvider.retrieve());

  // Get the SCT ID helper
  ATH_CHECK(detStore()->retrieve(m_sctID, "SCT_ID"));
  if (m_roiSeeded.value()) {
    // Don't need SCT cabling if running in RoI-seeded mode
    ATH_CHECK(m_roiCollectionKey.initialize());
    ATH_CHECK(m_regionSelector.retrieve());
    m_cabling.disable();
  }
  else {
    // Retrieve Cabling tool
    ATH_CHECK(m_cabling.retrieve());
    m_regionSelector.disable();
  }

  //Initialize
  ATH_CHECK(m_rdoContainerKey.initialize());
  ATH_CHECK(m_lvl1CollectionKey.initialize());
  ATH_CHECK(m_bcIDCollectionKey.initialize());
  ATH_CHECK(m_bsIDCErrContainerKey.initialize());
  ATH_CHECK(m_rdoContainerCacheKey.initialize(!m_rdoContainerCacheKey.key().empty()));
  ATH_CHECK(m_bsErrContainerCacheKey.initialize(!m_bsErrContainerCacheKey.key().empty()));

  ATH_CHECK( m_rawDataTool.retrieve() );

  return StatusCode::SUCCESS;
}

// Execute

StatusCode SCTRawDataProvider::execute(const EventContext& ctx) const
{
  SG::WriteHandle<SCT_RDO_Container> rdoContainer(m_rdoContainerKey, ctx);
  bool externalCacheRDO = !m_rdoContainerCacheKey.key().empty();
  if (not externalCacheRDO) {
    ATH_CHECK(rdoContainer.record (std::make_unique<SCT_RDO_Container>(m_sctID->wafer_hash_max(), EventContainers::Mode::OfflineFast)));
    ATH_MSG_DEBUG("Created container for " << m_sctID->wafer_hash_max());
  }
  else {
    SG::UpdateHandle<SCT_RDO_Cache> rdoCache(m_rdoContainerCacheKey, ctx);
    ATH_CHECK(rdoCache.isValid());
    ATH_CHECK(rdoContainer.record (std::make_unique<SCT_RDO_Container>(rdoCache.ptr())));
    ATH_MSG_DEBUG("Created container using cache for " << m_rdoContainerCacheKey.key());
  }

  SG::WriteHandle<IDCInDetBSErrContainer> bsIDCErrContainer(m_bsIDCErrContainerKey, ctx);
  if ( m_bsErrContainerCacheKey.key().empty() ) {
    ATH_CHECK(bsIDCErrContainer.record( std::make_unique<IDCInDetBSErrContainer>(m_sctID->wafer_hash_max(), std::numeric_limits<int>::min() )));
    ATH_MSG_DEBUG("Created IDCInDetBSErrContainer w/o using external cache");
  } else { // use cache
    SG::UpdateHandle<IDCInDetBSErrContainer_Cache> cacheHandle( m_bsErrContainerCacheKey, ctx );
    ATH_CHECK( cacheHandle.isValid() );
    ATH_CHECK(bsIDCErrContainer.record( std::make_unique<IDCInDetBSErrContainer>(cacheHandle.ptr())) );
    ATH_MSG_DEBUG("Created SCT IDCInDetBSErrContainer using external cache");
  }

  // Ask ROBDataProviderSvc for the vector of ROBFragment for all SCT ROBIDs
  std::vector<const ROBFragment*> vecROBFrags;
  std::vector<IdentifierHash> hashIDs;
  if (not m_roiSeeded.value()) {
    std::vector<uint32_t> rodList;
    m_cabling->getAllRods(rodList, ctx);
    m_robDataProvider->getROBData(rodList , vecROBFrags);
  }
  else {
    // Only load ROBs from RoI
    std::vector<uint32_t> listOfROBs;
    SG::ReadHandle<TrigRoiDescriptorCollection> roiCollection{m_roiCollectionKey, ctx};
    ATH_CHECK(roiCollection.isValid());
    TrigRoiDescriptor superRoI; // Add all RoIs to a super-RoI
    superRoI.setComposite(true);
    superRoI.manageConstituents(false);
    for (const TrigRoiDescriptor* roi : *roiCollection) {
      superRoI.push_back(roi);
    }
    m_regionSelector->ROBIDList(superRoI, listOfROBs);
    m_regionSelector->HashIDList(superRoI, hashIDs);
    m_robDataProvider->getROBData(listOfROBs, vecROBFrags);
  }


  ATH_MSG_DEBUG("Number of ROB fragments " << vecROBFrags.size());

  SG::WriteHandle<InDetTimeCollection> lvl1Collection{m_lvl1CollectionKey, ctx};
  lvl1Collection = std::make_unique<InDetTimeCollection>();
  ATH_CHECK(lvl1Collection.isValid());

  SG::WriteHandle<InDetTimeCollection> bcIDCollection{m_bcIDCollectionKey, ctx};
  bcIDCollection = std::make_unique<InDetTimeCollection>();
  ATH_CHECK(bcIDCollection.isValid());
  lvl1Collection->reserve(vecROBFrags.size());
  bcIDCollection->reserve(vecROBFrags.size());

  for (const ROBFragment* robFrag : vecROBFrags) {
    // Store LVL1ID and BCID information in InDetTimeCollection
    // to be stored in StoreGate at the end of the loop.
    // We want to store a pair<ROBID, LVL1ID> for each ROD, once per event.
    uint32_t robID{(robFrag)->rod_source_id()};

    unsigned int lvl1ID{(robFrag)->rod_lvl1_id()};
    lvl1Collection->emplace_back(robID, lvl1ID);

    unsigned int bcID{(robFrag)->rod_bc_id()};
    bcIDCollection->emplace_back(robID, bcID);

    ATH_MSG_DEBUG("Stored LVL1ID " << lvl1ID << " and BCID " << bcID << " in InDetTimeCollections");
  }

  if ( not hashIDs.empty() ) {
    int missingCount{};
    for ( IdentifierHash hash: hashIDs ) {
      if ( not rdoContainer->tryAddFromCache( hash ) ) missingCount++;
      bsIDCErrContainer->tryAddFromCache( hash );
    }
    ATH_MSG_DEBUG("Out of: " << hashIDs.size() << "Hash IDs missing: " << missingCount );
    if ( missingCount == 0 ) {
      return StatusCode::SUCCESS;
    }
  }

  // Ask SCTRawDataProviderTool to decode it and to fill the IDC
  if (m_rawDataTool->convert(vecROBFrags,
                             *(rdoContainer.ptr()),
			     *bsIDCErrContainer).isFailure()) {
    ATH_MSG_WARNING("BS conversion into RDOs failed");
  }

  return StatusCode::SUCCESS;
}
