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

#include "StoreGate/StoreClearedIncident.h"

#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "StoreGate/DataHandle.h"
#include "CxxUtils/make_unique.h"

namespace InDet{
  using namespace InDet;
  static const std::string moduleFailureReason("SCT_Clusterization: Exceeds max RDOs");

// Constructor with parameters:
  SCT_Clusterization::SCT_Clusterization(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
    m_clusteringTool("InDet::SCT_ClusteringTool", this),       //Changed to private  // public - does this need to be changed for athenaMT
//    m_dataObjectName("SCT_RDOs"),                    // RDO container
    m_managerName("SCT"),
//    m_clustersName("NOTTHISNOTHIS"),
//    m_page(0),                     
    m_idHelper(nullptr),
//    m_maxKey(0),
    m_rdoContainer(),
    m_clusterContainer(),
    m_manager(nullptr),
    m_maxRDOs(384), //(77),
    m_pSummarySvc("SCT_ConditionsSummarySvc", name),
    m_flaggedConditionSvc("SCT_FlaggedConditionSvc",name),
    m_checkBadModules(true),
    m_flaggedModules(),
    m_maxTotalOccupancyPercent(10)
  {  
  // Get parameter values from jobOptions file    
    declareProperty("DataObjectName", m_rdoContainer = SG::ReadHandle<SCT_RDO_Container> ("SCT_RDOs"), "SCT RDOs" );
    declareProperty("DetectorManagerName",m_managerName);
    declareProperty("clusteringTool",m_clusteringTool);    //inconsistent nomenclature!
//    declareProperty("ClustersName",m_clustersName);
//    declareProperty("PageNumber",m_page);
    declareProperty("conditionsService" , m_pSummarySvc);
    declareProperty("maxRDOs", m_maxRDOs);
    declareProperty("checkBadModules",m_checkBadModules);
    declareProperty("FlaggedConditionService", m_flaggedConditionSvc);
    declareProperty("maxTotalOccupancyInPercent",m_maxTotalOccupancyPercent);
    declareProperty("ClustersName", 
                  m_clusterContainer = SG::WriteHandle<SCT_ClusterContainer>("SCT_Clusters"),
                  "SCT cluster container");    
    
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

    // Get the flagged conditions service
    ATH_CHECK(m_flaggedConditionSvc.retrieve());

    // Get the clustering tool
    ATH_CHECK (m_clusteringTool.retrieve());

    // Get the SCT manager
    ATH_CHECK (detStore()->retrieve(m_manager,"SCT"));

    // Get the SCT ID helper
    ATH_CHECK (detStore()->retrieve(m_idHelper,"SCT_ID"));

    ATH_MSG_INFO( "Container m_clusterContainer '" << m_clusterContainer.name() << "' set");

    return StatusCode::SUCCESS;
  }

  
// Execute method:
  StatusCode SCT_Clusterization::execute(){
  // Register the IdentifiableContainer into StoreGate
//      ATH_MSG_INFO( "Container m_clusterContainer '" << m_clusterContainer.name() << "' set");
   m_clusterContainer = CxxUtils::make_unique<SCT_ClusterContainer>(m_idHelper->wafer_hash_max());   
   // declare the container:
//   m_clusterContainer->addRef();
   
    SiClusterContainer* symSiContainer = nullptr;
    StatusCode sc = evtStore()->symLink(m_clusterContainer.cptr(), symSiContainer);
    if (sc.isFailure()) {
      ATH_MSG_FATAL("Pixel clusters could not be symlinked in StoreGate !");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG( "Pixel clusters '" << m_clusterContainer.name() << "' symlinked in StoreGate");
    }   
   if (! m_clusterContainer.isValid() ){
      msg(MSG:: FATAL) << "Container of type SCT_ClusterContainer could not be initialised !"<< endreq;
      return StatusCode::FAILURE;
   }else{
    ATH_MSG_DEBUG( "Container '" << m_clusterContainer.name() << "' initialised" );
   }
   

  // First, we have to retrieve and access the container, not because we want to 
  // use it, but in order to generate the proxies for the collections, if they 
  // are being provided by a container converter.
//    const SG::ReadHandle<SCT_RDO_Container> p_rdoContainer(m_dataObjectName);
    if (!m_rdoContainer.isValid()){
      msg(MSG:: FATAL) << "Could not find the data object "<< m_rdoContainer.name() << " !" << endreq;
     return StatusCode::FAILURE;
    }
  // Anything to dereference the DataHandle will trigger the converter
    m_rdoContainer->clID();   
    SCT_RDO_Container::const_iterator rdoCollections    = m_rdoContainer->begin();
    SCT_RDO_Container::const_iterator rdoCollectionsEnd = m_rdoContainer->end();
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
      rdoCollections    = m_rdoContainer->begin();
      if (totalHits >  maxAllowableHits) {
          ATH_MSG_INFO("This event has too many hits in the SCT");
          dontDoClusterization=true;
      }
    }
    
    
    //detType doesn't seem to do anything, does it need to be here?
//    AtlasDetectorID detType;
    if (not dontDoClusterization){
        for(; rdoCollections != rdoCollectionsEnd; ++rdoCollections){
          const InDetRawDataCollection<SCT_RDORawData>* rd(*rdoCollections);
    #ifndef NDEBUG
          ATH_MSG_DEBUG("RDO collection size=" << rd->size() << ", Hash=" << rd->identifyHash());
    #endif
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
//              ATH_MSG_DEBUG("SCT_ClusterCollection" << clusterCollection->size() <<  "\n");
              if (not clusterCollection->empty()) {
                //Using get because I'm unsure of move semantec status
                if (m_clusterContainer->addCollection(clusterCollection.get(), clusterCollection->identifyHash()).isFailure()){
                  msg(MSG:: FATAL) << "Clusters could not be added to container !"<< endreq;
//                  delete clusterCollection;   // Graceful exit?
                  return StatusCode::FAILURE;
                } else {
    #ifndef NDEBUG
                  ATH_MSG_DEBUG("Clusters with key '" << clusterCollection->identifyHash() << "' added to Container\n");
    #endif
                 clusterCollection.release();//Release ownership if sucessfully added to collection
                } 
              } else { 
    #ifndef NDEBUG
                ATH_MSG_DEBUG("Don't write empty collections\n");
    #endif    
//                delete clusterCollection;
              }
            } else { 
                ATH_MSG_DEBUG("Clustering algorithm found no clusters\n");
            }
          }
        }
    }
  // Set container to const
    if (m_clusterContainer.setConst().isFailure()){
      ATH_MSG_FATAL("FAILED TO SET CONST");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

// Finalize method:
  StatusCode SCT_Clusterization::finalize() 
  {
    msg(MSG::INFO) << "SCT_Clusterization::finalize() " << PACKAGE_VERSION << endreq;
    if (m_maxRDOs) {
      msg(MSG::INFO) 
        << "Number of noisy modules killed by maximum RDO limit of " 
        << m_maxRDOs << " = " << m_flaggedModules.size() << endreq;
      msg(MSG::INFO) << "Printing info on up to 10 modules:" << endreq;
      std::set<IdentifierHash>::const_iterator itr(m_flaggedModules.begin());
      std::set<IdentifierHash>::const_iterator end(m_flaggedModules.end());
      for (int num(0); (itr != end) && (num < 10) ; ++itr, ++num) {
        msg(MSG::INFO) << "Noisy: " << m_idHelper->print_to_string(m_idHelper->wafer_id(*itr)) << endreq;
      }
    }

    return StatusCode::SUCCESS;
  }

}
