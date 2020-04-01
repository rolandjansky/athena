/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelRawDataProvider.h"
#include <memory>

#include "InDetIdentifier/PixelID.h"
#include "EventContainers/IdentifiableContTemp.h"

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

// --------------------------------------------------------------------
// Constructor

PixelRawDataProvider::PixelRawDataProvider(const std::string& name,
				       ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator) {
  declareProperty("RDOCacheKey", m_rdoCacheKey);
  declareProperty("BSErrorsCacheKey", m_bsErrorsCacheKey);
}

// Destructor

PixelRawDataProvider::~PixelRawDataProvider() {
}

// --------------------------------------------------------------------
// Initialize

StatusCode PixelRawDataProvider::initialize() {
  ATH_MSG_INFO("PixelRawDataProvider::initialize");

  // Get ROBDataProviderSvc
  if (m_robDataProvider.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve service " << m_robDataProvider);
    return StatusCode::FAILURE;
  } else
    ATH_MSG_INFO("Retrieved service " << m_robDataProvider);
 
  // Get PixelRawDataProviderTool
  if (m_rawDataTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_rawDataTool);
    return StatusCode::FAILURE;
  } else
    ATH_MSG_INFO("Retrieved tool " << m_rawDataTool);
 
  ATH_CHECK(detStore()->retrieve(m_pixel_id, "PixelID"));

  ATH_CHECK( m_rdoContainerKey.initialize() );
  ATH_CHECK( m_rdoCacheKey.initialize(!m_rdoCacheKey.key().empty()) );

  if (m_roiSeeded) {
    ATH_CHECK( m_roiCollectionKey.initialize() );
    ATH_CHECK(m_regionSelector.retrieve());
  }

  ATH_CHECK(m_condCablingKey.initialize());

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------
// Execute

typedef EventContainers::IdentifiableContTemp<InDetRawDataCollection<PixelRDORawData>> DummyPixelRDO;

StatusCode PixelRawDataProvider::execute() {

#ifdef PIXEL_DEBUG
    ATH_MSG_DEBUG("Create Pixel RDO Container");
#endif

  // now create the container and register the collections
 

  // write into StoreGate
  bool ExternalCacheMode = !m_rdoCacheKey.key().empty();
  SG::WriteHandle<PixelRDO_Container> rdoContainer(m_rdoContainerKey);
  if(!ExternalCacheMode) rdoContainer = std::make_unique<PixelRDO_Container>(m_pixel_id->wafer_hash_max()); 
  else{
    SG::UpdateHandle<PixelRDO_Cache> updateh(m_rdoCacheKey);
    if( ! updateh.isValid() ) {
      ATH_MSG_FATAL("Failure to retrieve cache " << m_rdoCacheKey.key());
      return StatusCode::FAILURE;
    }    
    rdoContainer = std::make_unique<PixelRDO_Container>(updateh.ptr());
    ATH_MSG_DEBUG("Created container " << m_rdoContainerKey.key() << " using external cache " << m_rdoCacheKey.key());
  }
  ATH_CHECK(rdoContainer.isValid());

  // Print ROB map in m_robDataProvider
  //m_robDataProvider->print_robmap();

  // ask ROBDataProviderSvc for the vector of ROBFragment for all Pixel ROBIDs
  std::vector<uint32_t> listOfRobs; 

  if (!m_roiSeeded) {
    ATH_MSG_DEBUG("No RoI seed, fetching all ROBs");
    listOfRobs = SG::ReadCondHandle<PixelCablingCondData>(m_condCablingKey)->get_allRobs();  // need ROB id (not ROD)
  }
  else {//Enter RoI-seeded mode
     ATH_MSG_DEBUG("RoI seed, fetching regions infromation");
     SG::ReadHandle<TrigRoiDescriptorCollection> roiCollection(m_roiCollectionKey);
     ATH_CHECK(roiCollection.isValid());
     
     TrigRoiDescriptorCollection::const_iterator roi = roiCollection->begin();
     TrigRoiDescriptorCollection::const_iterator roiE = roiCollection->end();
     TrigRoiDescriptor superRoI;//add all RoIs to a super-RoI
     superRoI.setComposite(true);
     superRoI.manageConstituents(false);
     for (; roi!=roiE; ++roi) {
       superRoI.push_back(*roi);
     }
     m_regionSelector->DetROBIDListUint( PIXEL, 
					 superRoI,
					 listOfRobs);
  }
  std::vector<const ROBFragment*> listOfRobf;

  m_robDataProvider->getROBData( listOfRobs, listOfRobf);

#ifdef PIXEL_DEBUG
    ATH_MSG_DEBUG("Number of ROB fragments " << listOfRobf.size()
                  << " (out of=" << listOfRobs.size() << " expected)");
#endif

  std::unique_ptr<DummyPixelRDO> tempcont;
  if(ExternalCacheMode) tempcont = std::make_unique<DummyPixelRDO> (rdoContainer.ptr());

  IPixelRDO_Container *containerInterface = tempcont ? static_cast< IPixelRDO_Container* >(tempcont.get()) :
         static_cast< IPixelRDO_Container* >(rdoContainer.ptr());
  // ask PixelRawDataProviderTool to decode it and to fill the IDC
  // TODO: temporarily unused errors container
  IDCInDetBSErrContainer decodingErrors(0, std::numeric_limits<int>::min()); // we will use IDC Errors from elswhere
  if (m_rawDataTool->convert(listOfRobf,  containerInterface, decodingErrors).isFailure())
    ATH_MSG_ERROR("BS conversion into RDOs failed");

  if(tempcont) ATH_CHECK(tempcont->MergeToRealContainer(rdoContainer.ptr()));
#ifdef PIXEL_DEBUG
    ATH_MSG_DEBUG("Number of Collections in IDC " << rdoContainer->numberOfCollections());
#endif

  return StatusCode::SUCCESS;
}

