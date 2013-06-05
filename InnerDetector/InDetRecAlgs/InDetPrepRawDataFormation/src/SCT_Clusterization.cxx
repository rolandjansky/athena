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


namespace InDet{
  using namespace InDet;
  static const std::string moduleFailureReason("SCT_Clusterization: Exceeds max RDOs");

// Constructor with parameters:
  SCT_Clusterization::SCT_Clusterization(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
    m_clusteringTool("InDet::SCT_ClusteringTool"),         // public
    m_dataObjectName("SCT_RDOs"),                    // RDO container
    m_managerName("SCT"),
    m_clustersName("SCT_Clusters"),
    m_page(0),                     
    m_idHelper(0),
    m_maxKey(0),
    m_clusterContainer(0),
    m_maxRDOs(384), //(77),
    m_pSummarySvc("SCT_ConditionsSummarySvc", name),
    m_flaggedConditionSvc("SCT_FlaggedConditionSvc",name),
    m_checkBadModules(true),
    m_maxTotalOccupancyPercent(10)
  {  
  // Get parameter values from jobOptions file    
    declareProperty("DataObjectName",m_dataObjectName);
    declareProperty("DetectorManagerName",m_managerName);
    declareProperty("clusteringTool",m_clusteringTool);    //inconsistent nomenclature!
    declareProperty("ClustersName",m_clustersName);
    declareProperty("PageNumber",m_page);
    declareProperty("conditionsService" , m_pSummarySvc);
    declareProperty("maxRDOs", m_maxRDOs);
    declareProperty("checkBadModules",m_checkBadModules);
    declareProperty("FlaggedConditionService", m_flaggedConditionSvc);
    declareProperty("maxTotalOccupancyInPercent",m_maxTotalOccupancyPercent);
  }


// Initialize method:
  StatusCode SCT_Clusterization::initialize(){
    msg(MSG::INFO) << "SCT_Clusterization::initialize()!" << endreq;
    static const StatusCode fail(StatusCode::FAILURE);

    // Get the conditions summary service (continue anyway, just check the pointer 
    // later and declare everything to be 'good' if it is NULL)
    if (m_checkBadModules){
      msg(MSG::INFO) << "Clusterization has been asked to look at bad module info" << endreq;
      if (m_pSummarySvc.retrieve().isFailure()) 
        msg(MSG:: ERROR) << "Conditions summary service not found !" << endreq; 
    }

    // Get the flagged conditions service
    if (m_flaggedConditionSvc.retrieve().isFailure()) 
      return msg(MSG:: ERROR) << "Flagged conditions service not found !" << endreq, fail; 

    // Get the clustering tool
    if (m_clusteringTool.retrieve().isFailure()) 
      return (msg(MSG:: FATAL) << m_clusteringTool.propertyName() << ": Failed to retrieve tool " << m_clusteringTool.type() << endreq), fail;

    // Get the SCT manager
    if (detStore()->retrieve(m_manager,"SCT").isFailure()) 
      return (msg(MSG:: FATAL) << "Cannot retrieve detector manager!" << endreq), fail;

    // Get the SCT ID helper
    if (detStore()->retrieve(m_idHelper,"SCT_ID").isFailure()) 
      return (msg(MSG:: FATAL) << "Cannot retrieve ID helper!" << endreq), fail;

    // Instantiate a cluster container to record later
    m_clusterContainer = new SCT_ClusterContainer(m_idHelper->wafer_hash_max()); 
    m_clusterContainer->addRef();

    // Register incident handler
    ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", name());
    if ( !incSvc.retrieve().isSuccess() ) {
      msg(MSG:: WARNING) << "Unable to retrieve the IncidentSvc" << endreq;
    } else {
      incSvc->addListener(this,"StoreCleared");
    }

    return StatusCode::SUCCESS;
  }

  void SCT_Clusterization::handle(const Incident& incident){
    if (incident.type() == "StoreCleared") {
      if (const StoreClearedIncident* inc = dynamic_cast<const StoreClearedIncident*> (&incident)){
        if (inc->store() == &*evtStore()) m_clusterContainer->cleanup(); 
      }
    }
  }
  
// Execute method:
  StatusCode SCT_Clusterization::execute(){
#ifndef NDEBUG
    ATH_MSG_DEBUG("SCT_Clusterization::execute()");
#endif
    static const StatusCode fail(StatusCode::FAILURE);
  // Register the IdentifiableContainer into StoreGate
    m_clusterContainer->cleanup(); 
    if (evtStore()->record(m_clusterContainer,m_clustersName).isFailure()) 
      return (msg(MSG:: FATAL) << "Container could not be recorded in StoreGate !" << endreq), fail;
  // Symlink the collection
    SiClusterContainer* symSiContainer(0);
    if (evtStore()->symLink(m_clusterContainer,symSiContainer).isFailure()) 
      return (msg(MSG:: FATAL) << "SCT clusters could not be symlinked in StoreGate !" << endreq), fail;
  // First, we have to retrieve and access the container, not because we want to 
  // use it, but in order to generate the proxies for the collections, if they 
  // are being provided by a container converter.
    const DataHandle<SCT_RDO_Container> p_rdocontainer;
    if (evtStore()->retrieve(p_rdocontainer,m_dataObjectName).isFailure()) 
      return (msg(MSG:: FATAL) << "Could not find the data object "<< m_dataObjectName << " !" << endreq), fail;
  // Anything to dereference the DataHandle will trigger the converter
    p_rdocontainer->clID();   
    SCT_RDO_Container::const_iterator rdoCollections    = p_rdocontainer->begin();
    SCT_RDO_Container::const_iterator rdoCollectionsEnd = p_rdocontainer->end();
    bool dontDoClusterization(false);
    //new code to remove large numbers of hits (what is large?)
    if (m_maxTotalOccupancyPercent != 100){
      const int totalNumberOfChannels(6279168);
      const int maxAllowableHits(totalNumberOfChannels*m_maxTotalOccupancyPercent/100);//integer arithmetic, should be ok
      int totalHits(0);
      for(; rdoCollections != rdoCollectionsEnd; ++rdoCollections){
          const InDetRawDataCollection<SCT_RDORawData>* rd(*rdoCollections);
          totalHits+=rd->size();
      }//iterator is now at the end
      //reset the iterator
      rdoCollections    = p_rdocontainer->begin();
      if (totalHits >  maxAllowableHits) {
          ATH_MSG_INFO("This event has too many hits in the SCT");
          dontDoClusterization=true;
      }
    }
    
    AtlasDetectorID detType;
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
            SCT_ClusterCollection* clusterCollection = m_clusteringTool->clusterize(*rd,*m_manager,*m_idHelper);
            if (clusterCollection) {          
              if (not clusterCollection->empty()) {
                if (m_clusterContainer->addCollection(clusterCollection, clusterCollection->identifyHash()).isFailure()){
                  msg(MSG:: FATAL) << "Clusters could not be added to container !"<< endreq;
                  delete clusterCollection;   // Graceful exit?
                  return StatusCode::FAILURE;
                } else {
    #ifndef NDEBUG
                  ATH_MSG_DEBUG("Clusters with key '" << clusterCollection->identifyHash() << "' added to Container\n");
    #endif
                } 
              } else { 
    #ifndef NDEBUG
                ATH_MSG_DEBUG("Don't write empty collections\n");
    #endif    
                delete clusterCollection;
              }
            } else { 
                ATH_MSG_DEBUG("Clustering algorithm found no clusters\n");
            }
          }
        }
    }
  // Set container to const
    if (evtStore()->setConst(m_clusterContainer).isFailure()) 
      return (msg(MSG:: WARNING) << "Set Const failed!" << endreq), fail;
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
      msg(MSG::INFO) << "Printing info on upto 10 modules:" << endreq;
      std::set<IdentifierHash>::const_iterator itr(m_flaggedModules.begin());
      std::set<IdentifierHash>::const_iterator end(m_flaggedModules.end());
      for (int num(0); (itr != end) && (num < 10) ; ++itr, ++num) {
        msg(MSG::INFO) << "Noisy: " << m_idHelper->print_to_string(m_idHelper->wafer_id(*itr)) << endreq;
      }
    }
    m_clusterContainer->cleanup();   
  //delete m_clusterContainer;
    m_clusterContainer->release();
    return StatusCode::SUCCESS;
  }

}
