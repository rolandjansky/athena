/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <memory>
#include "SCTRawDataProvider.h"

#include "SCT_RawDataByteStreamCnv/ISCTRawDataProviderTool.h"
#include "SCT_Cabling/ISCT_CablingSvc.h"
#include "IRegionSelector/IRegSelSvc.h" 
#include "InDetIdentifier/SCT_ID.h"
#include "EventContainers/IdentifiableContTemp.h"
using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

/// --------------------------------------------------------------------
/// Constructor

SCTRawDataProvider::SCTRawDataProvider(const std::string& name,
                                       ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_regionSelector{"RegSelSvc", name},
  m_robDataProvider{"ROBDataProviderSvc", name},
  m_cabling{"SCT_CablingSvc", name},
  m_sct_id{nullptr},
  m_roiSeeded{false},
  m_roiCollectionKey{""},
  m_rdoContainerKey{""},
  m_lvl1CollectionKey{""},
  m_bcidCollectionKey{""},
  m_rdoContainerCacheKey{""}
{
  declareProperty("RoIs", m_roiCollectionKey = std::string{""}, "RoIs to read in");
  declareProperty("isRoI_Seeded", m_roiSeeded = false, "Use RoI");
  declareProperty("RDOKey", m_rdoContainerKey = std::string{"SCT_RDOs"});
  declareProperty("LVL1IDKey", m_lvl1CollectionKey = std::string{"SCT_LVL1ID"});
  declareProperty("BCIDKey", m_bcidCollectionKey = std::string{"SCT_BCID"});
  declareProperty("ByteStreamErrContainer", m_bsErrContainerKey = std::string{"SCT_ByteStreamErrs"});
  declareProperty("CablingSvc", m_cabling);
  declareProperty("RDOCacheKey", m_rdoContainerCacheKey);
}

/// --------------------------------------------------------------------
/// Initialize

StatusCode SCTRawDataProvider::initialize() {
  /** Get ROBDataProviderSvc */
  ATH_CHECK(m_robDataProvider.retrieve());
  /** Get the SCT ID helper **/
  ATH_CHECK(detStore()->retrieve(m_sct_id, "SCT_ID"));
  if (m_roiSeeded) {//Don't need SCT cabling if running in RoI-seeded mode
    ATH_CHECK(m_roiCollectionKey.initialize());
    ATH_CHECK(m_regionSelector.retrieve());
  } else {
    /** Retrieve Cabling service */ 
    ATH_CHECK(m_cabling.retrieve());
  }
  //Initialize 
  ATH_CHECK(m_rdoContainerKey.initialize());
  ATH_CHECK(m_lvl1CollectionKey.initialize());
  ATH_CHECK(m_bcidCollectionKey.initialize());
  ATH_CHECK(m_bsErrContainerKey.initialize());
  ATH_CHECK(m_rdoContainerCacheKey.initialize(!m_rdoContainerCacheKey.key().empty()));
  return StatusCode::SUCCESS;
}

typedef EventContainers::IdentifiableContTemp<InDetRawDataCollection<SCT_RDORawData>> DummySCTRDO;

/// --------------------------------------------------------------------
/// Execute
StatusCode SCTRawDataProvider::execute()
{

  m_rawDataTool->BeginNewEvent();

  SG::WriteHandle<SCT_RDO_Container> rdoContainer(m_rdoContainerKey);
  bool externalcacheRDO = !m_rdoContainerCacheKey.key().empty();
  if(!externalcacheRDO){
    ATH_CHECK(rdoContainer.record (std::make_unique<SCT_RDO_Container>(m_sct_id->wafer_hash_max())));
    ATH_MSG_DEBUG("Created container for " << m_sct_id->wafer_hash_max());
  }else{
    SG::UpdateHandle<SCT_RDO_Cache> update(m_rdoContainerCacheKey);
    ATH_CHECK(update.isValid());
    ATH_CHECK(rdoContainer.record (std::make_unique<SCT_RDO_Container>(update.ptr())));
    ATH_MSG_DEBUG("Created container using cache for " << m_rdoContainerCacheKey.key());
  }

  
  SG::WriteHandle<InDetBSErrContainer> bsErrContainer(m_bsErrContainerKey);
  ATH_CHECK(bsErrContainer.record(std::make_unique<InDetBSErrContainer>()));

  //// do we need this??  rdoIdc->cleanup();

  /** ask ROBDataProviderSvc for the vector of ROBFragment for all SCT ROBIDs */
  std::vector<const ROBFragment*> listOfRobf;
  if (!m_roiSeeded) {
    std::vector<uint32_t> rodList;
    m_cabling->getAllRods(rodList);
    m_robDataProvider->getROBData(rodList , listOfRobf);
  } else {//Only load ROBs from RoI
    std::vector<uint32_t> listOfRobs;
    SG::ReadHandle<TrigRoiDescriptorCollection> roiCollection{m_roiCollectionKey};
    ATH_CHECK(roiCollection.isValid());
    TrigRoiDescriptorCollection::const_iterator roi{roiCollection->begin()};
    TrigRoiDescriptorCollection::const_iterator roiE{roiCollection->end()};
    TrigRoiDescriptor superRoI;//add all RoIs to a super-RoI
    superRoI.setComposite(true);
    superRoI.manageConstituents(false);
    for (; roi!=roiE; ++roi) {
      superRoI.push_back(*roi);
    }
    m_regionSelector->DetROBIDListUint(SCT, superRoI, listOfRobs);
    m_robDataProvider->getROBData(listOfRobs, listOfRobf);
  }


  ATH_MSG_DEBUG("Number of ROB fragments " << listOfRobf.size());

  SG::WriteHandle<InDetTimeCollection> lvl1Collection{m_lvl1CollectionKey};
  lvl1Collection = std::make_unique<InDetTimeCollection>(listOfRobf.size()); 
  ATH_CHECK(lvl1Collection.isValid());

  SG::WriteHandle<InDetTimeCollection> bcidCollection{m_bcidCollectionKey};
  bcidCollection = std::make_unique<InDetTimeCollection>(listOfRobf.size()); 
  ATH_CHECK(bcidCollection.isValid());

  std::vector<const ROBFragment*>::const_iterator rob_it{listOfRobf.begin()};
  for (; rob_it!=listOfRobf.end(); ++rob_it) {
    
    uint32_t robid{(*rob_it)->rod_source_id()};
    /**
     * Store LVL1ID and BCID information in InDetTimeCollection 
     * to be stored in StoreGate at the end of the loop.
     * We want to store a pair<ROBID, LVL1ID> for each ROD, once per event.
     **/
    
    unsigned int lvl1id{(*rob_it)->rod_lvl1_id()};
    auto lvl1Pair{std::make_unique<std::pair<uint32_t, unsigned int>>(robid, lvl1id)};
    lvl1Collection->push_back(std::move(lvl1Pair));
    
    unsigned int bcid{(*rob_it)->rod_bc_id()};
    auto bcidPair{std::make_unique<std::pair<uint32_t, unsigned int>>(robid, bcid)};
    bcidCollection->push_back(std::move(bcidPair));
    
    ATH_MSG_DEBUG("Stored LVL1ID " << lvl1id << " and BCID " << bcid << " in InDetTimeCollections");
    
  }
  std::unique_ptr<DummySCTRDO> dummyrdo;
  if(externalcacheRDO) dummyrdo = std::make_unique<DummySCTRDO>(rdoContainer.ptr());
  ISCT_RDO_Container *rdoInterface = externalcacheRDO ? static_cast< ISCT_RDO_Container*> (dummyrdo.get()) 
                     : static_cast<ISCT_RDO_Container* >(rdoContainer.ptr());
  /** ask SCTRawDataProviderTool to decode it and to fill the IDC */
  if (m_rawDataTool->convert(listOfRobf, *rdoInterface, bsErrContainer.ptr()).isFailure()) {
    ATH_MSG_WARNING("BS conversion into RDOs failed");
  }
  if(dummyrdo) dummyrdo->MergeToRealContainer(rdoContainer.ptr());
  
  return StatusCode::SUCCESS;
}
