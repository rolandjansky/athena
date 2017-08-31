/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**   @file SCT_Clusterization.cxx
 *   Implementation file for the SCT_Clusterization class.
 *   @author Paul Bell, Tommaso Lari, Shaun Roe, Carl Gwilliam
 *   @date 08 July 2008
 */

#include "InDetPrepRawDataFormation/SCT_Clusterization.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "InDetRawData/SCT_RDORawData.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "AtlasDetDescr/AtlasDetectorID.h"    
#include "InDetIdentifier/SCT_ID.h"
#include "SCT_ConditionsServices/ISCT_FlaggedConditionSvc.h"
#include "SiClusterizationTool/ISCT_ClusteringTool.h"

#include "StoreGate/WriteHandle.h"


namespace InDet{
  using namespace InDet;
  static const std::string moduleFailureReason("SCT_Clusterization: Exceeds max RDOs");

// Constructor with parameters:
  SCT_Clusterization::SCT_Clusterization(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
    m_clusteringTool("InDet::SCT_ClusteringTool", this),       //Changed to private  // public - does this need to be changed for athenaMT
    m_idHelper(nullptr),
    m_roiCollectionKey(""),
    m_regionSelector("RegSelSvc", name),
    m_rdoContainerKey("SCT_RDOs"),
    m_roiSeeded(false),
    m_clusterContainerKey("SCT_Clusters"),
    m_clusterContainerLinkKey("SCT_Clusters"),
    m_manager(nullptr),
    m_maxRDOs(384), //(77),
    m_pSummarySvc("SCT_ConditionsSummarySvc", name),
    m_flaggedConditionSvc("SCT_FlaggedConditionSvc",name),
    m_checkBadModules(true),
    m_flaggedModules(),
    m_maxTotalOccupancyPercent(10),
    m_clusterContainerCacheKey("")
  {  
  // Get parameter values from jobOptions file    
    declareProperty("DataObjectName", m_rdoContainerKey, "SCT RDOs" );
    declareProperty("DetectorManagerName",m_managerName);
    declareProperty("clusteringTool",m_clusteringTool);    //inconsistent nomenclature!
    declareProperty("conditionsService" , m_pSummarySvc);
    declareProperty("RoIs", m_roiCollectionKey, "RoIs to read in");
    declareProperty("isRoI_Seeded", m_roiSeeded, "Use RoI");
    declareProperty("maxRDOs", m_maxRDOs);
    declareProperty("checkBadModules",m_checkBadModules);
    declareProperty("FlaggedConditionService", m_flaggedConditionSvc);
    declareProperty("maxTotalOccupancyInPercent",m_maxTotalOccupancyPercent);
    declareProperty("ClustersName", m_clusterContainerKey, "SCT cluster container");    
    declareProperty("ClustersLinkName_", m_clusterContainerLinkKey, "SCT cluster container link name (don't set this)");
    declareProperty("ClusterContainerCacheKey", m_clusterContainerCacheKey);
  }


// Initialize method:
  StatusCode SCT_Clusterization::initialize(){
    ATH_MSG_INFO( "SCT_Clusterization::initialize()!");

    // Get the conditions summary service (continue anyway, just check the pointer 
    // later and declare everything to be 'good' if it is NULL)
    if (m_checkBadModules){
      ATH_MSG_INFO( "Clusterization has been asked to look at bad module info" );
      ATH_CHECK(m_pSummarySvc.retrieve());
    }

    m_clusterContainerLinkKey = m_clusterContainerKey.key();

    ATH_CHECK(m_rdoContainerKey.initialize());
    ATH_CHECK(m_clusterContainerKey.initialize());
    ATH_CHECK(m_clusterContainerLinkKey.initialize());
    ATH_CHECK(m_clusterContainerCacheKey.initialize(!m_clusterContainerCacheKey.key().empty()));
    // Get the flagged conditions service
    ATH_CHECK(m_flaggedConditionSvc.retrieve());

    // Get the clustering tool
    ATH_CHECK (m_clusteringTool.retrieve());

    // Get the SCT manager
    ATH_CHECK (detStore()->retrieve(m_manager,"SCT"));

    // Get the SCT ID helper
    ATH_CHECK (detStore()->retrieve(m_idHelper,"SCT_ID"));

    if (m_roiSeeded) {
      ATH_CHECK( m_roiCollectionKey.initialize() );
      ATH_CHECK(m_regionSelector.retrieve());
    }

    return StatusCode::SUCCESS;
  }

  
// Execute method:
  StatusCode SCT_Clusterization::execute(){
  // Register the IdentifiableContainer into StoreGate
   SG::WriteHandle<SCT_ClusterContainer> clusterContainer(m_clusterContainerKey);
   if(m_clusterContainerCacheKey.key().empty()){
    ATH_CHECK( clusterContainer.record (std::make_unique<SCT_ClusterContainer>(m_idHelper->wafer_hash_max())) );
   }else{
    SG::UpdateHandle<SCT_ClusterContainerCache> clusterContainercache(m_clusterContainerCacheKey);
    ATH_CHECK( clusterContainer.record (std::make_unique<SCT_ClusterContainer>(clusterContainercache.ptr()) ));
   }
   ATH_MSG_DEBUG( "Container '" << clusterContainer.name() << "' initialised" );

   ATH_CHECK( clusterContainer.symLink (m_clusterContainerLinkKey) );
   ATH_CHECK(clusterContainer.isValid());
   ATH_MSG_DEBUG( "SCT clusters '" << clusterContainer.name() << "' symlinked in StoreGate");

   

  // First, we have to retrieve and access the container, not because we want to 
  // use it, but in order to generate the proxies for the collections, if they 
  // are being provided by a container converter.
    SG::ReadHandle<SCT_RDO_Container> rdoContainer(m_rdoContainerKey);
    ATH_CHECK(rdoContainer.isValid());


  // Anything to dereference the DataHandle will trigger the converter
    SCT_RDO_Container::const_iterator rdoCollections    = rdoContainer->begin();
    SCT_RDO_Container::const_iterator rdoCollectionsEnd = rdoContainer->end();
    bool dontDoClusterization(false);
    //new code to remove large numbers of hits (what is large?)
    if (m_maxTotalOccupancyPercent != 100){
      constexpr int totalNumberOfChannels(6279168);
      const int maxAllowableHits(totalNumberOfChannels*m_maxTotalOccupancyPercent/100);//integer arithmetic, should be ok
      int totalHits(0);
      for(; rdoCollections != rdoCollectionsEnd; ++rdoCollections){
          const InDetRawDataCollection<SCT_RDORawData>* rd(*rdoCollections);
          totalHits+=rd->size();
      }//iterator is now at the end
      //reset the iterator
      rdoCollections    = rdoContainer->begin();
      if (totalHits >  maxAllowableHits) {
          ATH_MSG_INFO("This event has too many hits in the SCT");
          dontDoClusterization=true;
      }
    }
    
    if (not dontDoClusterization){
     if (!m_roiSeeded) {//Full-scan mode
    
    
        for(; rdoCollections != rdoCollectionsEnd; ++rdoCollections){
          const InDetRawDataCollection<SCT_RDORawData>* rd(*rdoCollections);
    #ifndef NDEBUG
          ATH_MSG_DEBUG("RDO collection size=" << rd->size() << ", Hash=" << rd->identifyHash());
    #endif
          if( clusterContainer->tryFetch( rdoCollections.hashId() )){ 
    #ifndef NDEBUG
            ATH_MSG_DEBUG("Item already in cache , Hash=" << rd->identifyHash());
    #endif
            continue;
          }

          bool goodModule = (m_checkBadModules and m_pSummarySvc) ? m_pSummarySvc->isGood(rd->identifyHash()) : true;
          // Check the RDO is not empty and that the wafer is good according to the conditions
          if ((not rd->empty()) and goodModule){
            // If more than a certain number of RDOs set module to bad
            if (m_maxRDOs and (rd->size() > m_maxRDOs)) {
              m_flaggedConditionSvc->flagAsBad(rd->identifyHash(), moduleFailureReason);
              m_flaggedModules.insert(rd->identifyHash());
              continue;
            }
            // Use one of the specific clustering AlgTools to make clusters    
            std::unique_ptr<SCT_ClusterCollection> clusterCollection ( m_clusteringTool->clusterize(*rd,*m_manager,*m_idHelper));
            if (clusterCollection) { 
              if (not clusterCollection->empty()) {
                //Using get because I'm unsure of move semantec status
                ATH_CHECK(clusterContainer->addOrDelete(std::move(clusterCollection), clusterCollection->identifyHash()));

    #ifndef NDEBUG
                 ATH_MSG_DEBUG("Clusters with key '" << clusterCollection->identifyHash() << "' added to Container\n");
    #endif                
              } else { 
    #ifndef NDEBUG
                ATH_MSG_DEBUG("Don't write empty collections\n");
    #endif    
              }
            } else { 
                ATH_MSG_DEBUG("Clustering algorithm found no clusters\n");
            }
          }
        }
       }else{//enter RoI-seeded mode
         SG::ReadHandle<TrigRoiDescriptorCollection> roiCollection(m_roiCollectionKey);
         ATH_CHECK(roiCollection.isValid());

         TrigRoiDescriptorCollection::const_iterator roi = roiCollection->begin();
         TrigRoiDescriptorCollection::const_iterator roiE = roiCollection->end();
         std::vector<IdentifierHash> listOfSCTIds;
         for (; roi!=roiE; ++roi) {
          listOfSCTIds.clear(); //Prevents needless memory reallocations
          m_regionSelector->DetHashIDList( SCT, **roi, listOfSCTIds);
#ifndef NDEBUG
          ATH_MSG_VERBOSE(**roi);     
          ATH_MSG_VERBOSE( "REGTEST: SCT : Roi contains " 
		     << listOfSCTIds.size() << " det. Elements" );
#endif
          for (size_t i=0; i < listOfSCTIds.size(); i++) {

            if( clusterContainer->tryFetch( listOfSCTIds[i] )){
              #ifndef NDEBUG
              ATH_MSG_DEBUG("Item already in cache , Hash=" << listOfSCTIds[i]);
              #endif
              continue;
            }
            
            const InDetRawDataCollection<SCT_RDORawData>* RDO_Collection (rdoContainer->indexFindPtr(listOfSCTIds[i]));

            if (!RDO_Collection) continue;

          // Use one of the specific clustering AlgTools to make clusters
            std::unique_ptr<SCT_ClusterCollection> clusterCollection (m_clusteringTool->clusterize(*RDO_Collection, *m_manager, *m_idHelper));
            if (clusterCollection && !clusterCollection->empty()){
#ifndef NDEBUG 
              ATH_MSG_VERBOSE( "REGTEST: SCT : clusterCollection contains " 
                << clusterCollection->size() << " clusters" );
#endif
              ATH_CHECK(clusterContainer->addOrDelete( std::move(clusterCollection), clusterCollection->identifyHash() ));
          }else{
              ATH_MSG_DEBUG("No SCTClusterCollection to write");
          }
        }
      }
       
     }
    }
    
    // Set container to const
    ATH_CHECK(clusterContainer.setConst());
    
    return StatusCode::SUCCESS;
  }

// Finalize method:
  StatusCode SCT_Clusterization::finalize() 
  {
    msg(MSG::INFO) << "SCT_Clusterization::finalize() " << PACKAGE_VERSION << endmsg;
    if (m_maxRDOs) {
      msg(MSG::INFO) 
        << "Number of noisy modules killed by maximum RDO limit of " 
        << m_maxRDOs << " = " << m_flaggedModules.size() << endmsg;
      msg(MSG::INFO) << "Printing info on up to 10 modules:" << endmsg;
      std::set<IdentifierHash>::const_iterator itr(m_flaggedModules.begin());
      std::set<IdentifierHash>::const_iterator end(m_flaggedModules.end());
      for (int num(0); (itr != end) && (num < 10) ; ++itr, ++num) {
        msg(MSG::INFO) << "Noisy: " << m_idHelper->print_to_string(m_idHelper->wafer_id(*itr)) << endmsg;
      }
    }

    return StatusCode::SUCCESS;
  }

}
