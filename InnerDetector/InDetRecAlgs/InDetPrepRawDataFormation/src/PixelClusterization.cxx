/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file PixelClusterization.h
 * Implementation for PixelClusterization.
 * (modified s.roe, 18.12.2008)
 **/


// Silicon trackers includes
#include "InDetRawData/PixelRDO_Container.h"
#include "InDetPrepRawDataFormation/PixelClusterization.h"
#include "InDetRawData/PixelRDORawData.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"

#include "Identifier/Identifier.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetPrepRawData/PixelClusterCollection.h"


#include "SiClusterizationTool/IPixelClusteringTool.h"
#include "SiClusterizationTool/PixelGangedAmbiguitiesFinder.h"

#include "StoreGate/WriteHandle.h"


namespace InDet{
  using namespace InDet;
  // Constructor with parameters:
  PixelClusterization::PixelClusterization(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_clusteringTool("InDet::MergedPixelsTool", this), //made private
  m_gangedAmbiguitiesFinder("InDet::PixelGangedAmbiguitiesFinder", this), //made private
  m_rdoContainerKey(""),
  m_roiCollectionKey(""),
  m_regionSelector("RegSelSvc", name),
  m_managerName("Pixel"),
  m_roiSeeded(false),
  m_idHelper(nullptr),
  m_clusterContainerKey(""),
  m_clusterContainerLinkKey(""),
  m_ambiguitiesMapKey(""),
  m_clusterContainercacheKey(""),
  m_manager(nullptr) {  
    // Get parameter values from jobOptions file
    declareProperty("DataObjectName", m_rdoContainerKey = std::string("PixelRDOs"));
    declareProperty("DetectorManagerName",m_managerName);
    declareProperty("clusteringTool", m_clusteringTool);
    declareProperty("gangedAmbiguitiesFinder", m_gangedAmbiguitiesFinder);
    declareProperty("ClustersName", 
                  m_clusterContainerKey = std::string("PixelClusters"),
                  "Pixel cluster container");
    declareProperty("ClustersLinkName_", 
                  m_clusterContainerLinkKey = std::string("PixelClusters"),
                  "Pixel cluster container link name (don't set this)");
    declareProperty("AmbiguitiesMap", 
                  m_ambiguitiesMapKey = std::string("PixelClusterAmbiguitiesMap"),
                  "Ambiguity Map container");
    declareProperty("RoIs", m_roiCollectionKey = std::string(""), "RoIs to read in");
    declareProperty("isRoI_Seeded", m_roiSeeded = false, "Use RoI");
    declareProperty("ClusterContainerCacheKey", m_clusterContainercacheKey, "Optional External Pixel cluster Cache");
    // error strategy <-- this is now in the MergedPixelTool
    //
    // 0 : broad errors (cluster width/sqrt(12) )
    //     as in old clustering code (release 6 and 7)
    // 1 : narrow errors (pixel pitch/sqrt(12.) )
    //     DEFAULT - should be more accurate, 
    //     and still conservative
    // 2 : parameterization as a function of eta & cluster size
    //     same as in atrecon (fortran code)
    //       declareProperty("ErrorStrategy",m_errorStrategy); <-- this is now in the MergedPixelTool
    // position strategy
    //
    // 0 : arithmetic mean of pixel position
    // 1 : simple charge interpolation
    //       declareProperty("PositionStrategy",m_positionStrategy); <-- this is now in the MergedPixelTool
  }
  
  //-----------------------------------------------------------------------------
  // Initialize method:
  StatusCode PixelClusterization::initialize(){
    // get the InDet::MergedPixelsTool
    ATH_CHECK(m_clusteringTool.retrieve());
    // get the InDet::PixelGangedAmbiguitiesFinder
    ATH_CHECK(m_gangedAmbiguitiesFinder.retrieve());
    ATH_CHECK(detStore()->retrieve(m_manager, m_managerName));
    // Get the Pixel helper
    ATH_CHECK(detStore()->retrieve(m_idHelper,"PixelID"));
    ATH_CHECK( m_rdoContainerKey.initialize() );
    if (m_roiSeeded) {
      ATH_CHECK( m_roiCollectionKey.initialize() );
      ATH_CHECK(m_regionSelector.retrieve());
    }

    m_clusterContainerLinkKey = m_clusterContainerKey.key();

    ATH_CHECK( m_clusterContainerKey.initialize() );
    ATH_CHECK( m_clusterContainerLinkKey.initialize() );
    ATH_CHECK( m_ambiguitiesMapKey.initialize() );
    ATH_CHECK( m_clusterContainercacheKey.initialize(!m_clusterContainercacheKey.key().empty()) );

    ATH_MSG_DEBUG( "Initialize done !" );
    return StatusCode::SUCCESS;
  }
  
  //----------------------------------------------------------------------------
  // Execute method:
  StatusCode PixelClusterization::execute() {
  

    SG::WriteHandle<PixelClusterContainer> clusterContainer(m_clusterContainerKey);
    if(m_clusterContainercacheKey.key().empty()){
      ATH_CHECK( clusterContainer.record (std::make_unique<PixelClusterContainer>(m_idHelper->wafer_hash_max())) );
    }else{
      SG::UpdateHandle<PixelClusterContainerCache> clusterContainercache(m_clusterContainercacheKey);
      ATH_CHECK(clusterContainercache.isValid());
      ATH_CHECK( clusterContainer.record (std::make_unique<PixelClusterContainer>(clusterContainercache.ptr() )));
    }

    ATH_CHECK(clusterContainer.isValid());
    ATH_MSG_DEBUG( "Container '" << clusterContainer->name() << "' initialised" );

    ATH_CHECK( clusterContainer.symLink (m_clusterContainerLinkKey) );
    ATH_MSG_DEBUG( "Pixel clusters '" << clusterContainer.name() << "' symlinked in StoreGate");

    ATH_MSG_DEBUG( "Creating the ganged ambiguities map");
    SG::WriteHandle<PixelGangedClusterAmbiguities> ambiguitiesMap(m_ambiguitiesMapKey);
    ambiguitiesMap = std::make_unique<PixelGangedClusterAmbiguities>();

    SG::ReadHandle<PixelRDO_Container> rdoContainer(m_rdoContainerKey);

    ATH_CHECK(rdoContainer.isValid());

    ATH_MSG_DEBUG( "Data object " << rdoContainer.name() << " found" );
    if (!m_roiSeeded) {//Full-scan mode
      PixelRDO_Container::const_iterator rdoCollections      = rdoContainer->begin();
      PixelRDO_Container::const_iterator rdoCollectionsEnd   = rdoContainer->end();

      for(; rdoCollections!=rdoCollectionsEnd; ++rdoCollections){
        const COLLECTION* RDO_Collection(*rdoCollections);
        if (!RDO_Collection || RDO_Collection->empty()) continue;
        if( clusterContainer->tryFetch( rdoCollections.hashId() )) continue;


        // Use one of the specific clustering AlgTools to make clusters
        std::unique_ptr<PixelClusterCollection> clusterCollection (m_clusteringTool->clusterize(*RDO_Collection, *m_manager, *m_idHelper));
        if (clusterCollection && !clusterCollection->empty()){

          m_gangedAmbiguitiesFinder->execute(clusterCollection.get(),*m_manager,*ambiguitiesMap);
          
          ATH_CHECK(clusterContainer->addOrDelete( std::move(clusterCollection), clusterCollection->identifyHash() ));
        }else{
          ATH_MSG_DEBUG("No PixelClusterCollection to write");
        }
      }
    }
    else {//enter RoI-seeded mode
      SG::ReadHandle<TrigRoiDescriptorCollection> roiCollection(m_roiCollectionKey);
      ATH_CHECK(roiCollection.isValid());

      TrigRoiDescriptorCollection::const_iterator roi = roiCollection->begin();
      TrigRoiDescriptorCollection::const_iterator roiE = roiCollection->end();

      std::vector<IdentifierHash> listOfPixIds;
      for (; roi!=roiE; ++roi) {
        
        listOfPixIds.clear();//Prevents needless memory reallocations
        m_regionSelector->DetHashIDList( PIXEL, **roi, listOfPixIds);
        ATH_MSG_VERBOSE(**roi);
        ATH_MSG_VERBOSE( "REGTEST: Pixel : Roi contains " 
		     << listOfPixIds.size() << " det. Elements" );
        for (unsigned int i=0; i < listOfPixIds.size(); i++) {

          if( clusterContainer->tryFetch( listOfPixIds[i] )) continue;

          const InDetRawDataCollection<PixelRDORawData>* RDO_Collection (rdoContainer->indexFindPtr(listOfPixIds[i]));

          if (!RDO_Collection) continue;

          // Use one of the specific clustering AlgTools to make clusters
          std::unique_ptr<PixelClusterCollection> clusterCollection (m_clusteringTool->clusterize(*RDO_Collection, *m_manager, *m_idHelper));
          if (clusterCollection && !clusterCollection->empty()){
            ATH_MSG_VERBOSE( "REGTEST: Pixel : clusterCollection contains " 
                << clusterCollection->size() << " clusters" );
            m_gangedAmbiguitiesFinder->execute(clusterCollection.get(),*m_manager,*ambiguitiesMap);
            ATH_CHECK(clusterContainer->addOrDelete( std::move(clusterCollection), clusterCollection->identifyHash() ));

          }else{
            ATH_MSG_DEBUG("No PixelClusterCollection to write");
          }
        }
      }
    }
    ATH_CHECK(clusterContainer.setConst());

    ATH_CHECK(ambiguitiesMap.isValid());
    ATH_MSG_DEBUG( "PixelClusterAmbiguitiesMap recorded in StoreGate");
    return StatusCode::SUCCESS;
  }
  //----------------------------------------------------------------------------
  // Finalize method:
  StatusCode PixelClusterization::finalize(){
    ATH_MSG_INFO( "PixelClusterization::finalize() " << PACKAGE_VERSION );
    return StatusCode::SUCCESS;
  }
  
}// namespace closure


