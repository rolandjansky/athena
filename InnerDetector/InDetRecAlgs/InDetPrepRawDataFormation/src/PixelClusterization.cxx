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
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetRawData/PixelRDORawData.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"

#include "Identifier/Identifier.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetPrepRawData/PixelClusterCollection.h"

// TES includes
#include "StoreGate/StoreClearedIncident.h"

#include "SiClusterizationTool/IPixelClusteringTool.h"
#include "SiClusterizationTool/PixelGangedAmbiguitiesFinder.h"

// Gaudi includes
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/DataHandle.h"

namespace InDet{
  using namespace InDet;

  // Constructor with parameters:
  PixelClusterization::PixelClusterization(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_clusteringTool("InDet::MergedPixelsTool"),
  m_gangedAmbiguitiesFinder("InDet::PixelGangedAmbiguitiesFinder"),
  m_dataObjectName("PixelRDOs"),	// RDO container
  m_managerName("Pixel"),
  m_clustersName("PixelClusters"),
  m_page(0),
  m_idHelper(0),
  m_clusterContainer(0),
  m_manager(0){  
    // Get parameter values from jobOptions file
    declareProperty("DataObjectName",m_dataObjectName);
    declareProperty("DetectorManagerName",m_managerName);
    declareProperty("clusteringTool", m_clusteringTool);
    declareProperty("gangedAmbiguitiesFinder", m_gangedAmbiguitiesFinder);
    declareProperty("ClustersName",m_clustersName);
    declareProperty("PageNumber",m_page);
    
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
    if ( m_clusteringTool.retrieve().isFailure() ) {
      msg(MSG:: FATAL) << m_clusteringTool.propertyName() << ": Failed to retrieve tool " << m_clusteringTool.type() << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << m_clusteringTool.propertyName() << ": Retrieved tool " << m_clusteringTool.type() << endreq;
    }
    // get the InDet::PixelGangedAmbiguitiesFinder
    if ( m_gangedAmbiguitiesFinder.retrieve().isFailure() ) {
      msg(MSG:: FATAL) << m_gangedAmbiguitiesFinder.propertyName() << ": Failed to retrieve tool " << m_gangedAmbiguitiesFinder.type() << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << m_gangedAmbiguitiesFinder.propertyName() << ": Retrieved tool " << m_gangedAmbiguitiesFinder.type() << endreq;
    }
    
    StatusCode sc =detStore()->retrieve(m_manager, m_managerName);
    if (sc.isFailure()){
      msg(MSG:: FATAL) << "Cannot retrieve the Pixel detector manager " 
      << m_managerName << endreq;
      return StatusCode::FAILURE;
    } 

    ATH_MSG_DEBUG( "Pixel detector manager retrieved" );

    // Get the Pixel helper
    if (detStore()->retrieve(m_idHelper,"PixelID").isFailure()) 
      return (msg(MSG:: FATAL) << "Cannot retrieve ID helper!" << endreq), StatusCode::FAILURE;
    
    // declare the container:
    m_clusterContainer = new PixelClusterContainer(m_idHelper->wafer_hash_max());
    m_clusterContainer->addRef();
    
    // Register incident handler
    ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", name());
    if ( !incSvc.retrieve().isSuccess() ) {
      msg(MSG:: WARNING) << "Unable to retrieve the IncidentSvc" << endreq;
    } else {
      incSvc->addListener(this,"StoreCleared");
    }
    ATH_MSG_DEBUG( "Initialize done !" );
    return StatusCode::SUCCESS;
  }
  
  void PixelClusterization::handle(const Incident& incident){
    if (incident.type() == "StoreCleared") {
      if (const StoreClearedIncident* inc = dynamic_cast<const StoreClearedIncident*> (&incident)){
          if (inc->store() == &*evtStore()) m_clusterContainer->cleanup(); 
      }
    }
  }
  
  //----------------------------------------------------------------------------
  // Execute method:
  StatusCode PixelClusterization::execute() {
    m_clusterContainer->cleanup();    
    // register the IdentifiableContainer into StoreGate
    StatusCode sc = evtStore()->record(m_clusterContainer,m_clustersName);
    if (sc.isFailure()){
      msg(MSG:: FATAL) << "Container " << m_clustersName<< "could not be recorded in StoreGate !"<< endreq;
      return StatusCode::FAILURE;
    }else{
      ATH_MSG_DEBUG( "Container '" << m_clustersName<< "' recorded in StoreGate" );
    }
    // symlink the collection
    SiClusterContainer* symSiContainer(0);
    sc = evtStore()->symLink(m_clusterContainer,symSiContainer);
    if (sc.isFailure()) {
      msg(MSG:: FATAL) << "Pixel clusters could not be symlinked in StoreGate !"<< endreq;
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG( "Pixel clusters '" << m_clustersName << "' symlinked in StoreGate");
    }   
    ATH_MSG_DEBUG( "Creating the ganged ambiguities map");
    std::unique_ptr<PixelGangedClusterAmbiguities> AmbiguitiesMap(new PixelGangedClusterAmbiguities());
    const DataHandle<PixelRDO_Container> p_rdocontainer;
    sc=evtStore()->retrieve(p_rdocontainer,m_dataObjectName);
    if (sc.isFailure() ){
      msg(MSG:: FATAL) << "Could not find the data object "<< m_dataObjectName << " !" << endreq;
      //    return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG( "Data object " << m_dataObjectName << " found" );
      p_rdocontainer->clID(); 	// anything to dereference the DataHandle
      // will trigger the converter
      PixelRDO_Container::const_iterator rdoCollections      = p_rdocontainer->begin();
      PixelRDO_Container::const_iterator rdoCollectionsEnd   = p_rdocontainer->end();
      AtlasDetectorID detType;
      for(; rdoCollections!=rdoCollectionsEnd; ++rdoCollections){
        const COLLECTION* RDO_Collection(*rdoCollections);
        if (!RDO_Collection) continue;

        if (RDO_Collection->size() != 0){
          // Use one of the specific clustering AlgTools to make clusters
          PixelClusterCollection* clusterCollection = m_clusteringTool->clusterize(*RDO_Collection,*m_manager,*m_idHelper);
          if (clusterCollection){
            if (clusterCollection->size() != 0) {
              m_gangedAmbiguitiesFinder->execute(clusterCollection,*m_manager,*AmbiguitiesMap);
              // -me- new IDC does no longer register in Storegate if hash is used !
              sc= m_clusterContainer->addCollection( clusterCollection, clusterCollection->identifyHash() );
              if (sc.isFailure()){
                msg(MSG:: ERROR) << "Failed to add Clusters to container"<< endreq;
                return StatusCode::FAILURE;
              }
            } else {
              // -me- clean up memory
              delete (clusterCollection);
            }
          }
        }
      }
      sc = evtStore()->setConst(m_clusterContainer);
      if(sc.isFailure()){
        msg(MSG:: WARNING) << "Set Const failed!" << endreq;
        return sc;
      }
      
      if (msgLvl(MSG::DEBUG)) msg() << "Ambiguities map has " << AmbiguitiesMap->size() << " elements" << endreq;
      //sc = evtStore()->record(std::move(AmbiguitiesMap),"PixelClusterAmbiguitiesMap",false);
      const bool alreadyHaveMap(evtStore()->contains<PixelGangedClusterAmbiguities>("PixelClusterAmbiguitiesMap"));
      sc = (not alreadyHaveMap) ? evtStore()->record(std::move(AmbiguitiesMap),"PixelClusterAmbiguitiesMap",false) : StatusCode::SUCCESS;
      if (sc.isFailure()){
        msg(MSG:: FATAL)<< "PixelClusterAmbiguitiesMap could not be recorded in StoreGate !"<< endreq;
        return StatusCode::FAILURE;
      }else{
        ATH_MSG_DEBUG( "PixelClusterAmbiguitiesMap recorded in StoreGate");
      }
    }
    return StatusCode::SUCCESS;
  }
  //----------------------------------------------------------------------------
  // Finalize method:
  StatusCode PixelClusterization::finalize(){
    ATH_MSG_INFO( "PixelClusterization::finalize() " << PACKAGE_VERSION );
    m_clusterContainer->cleanup();    
    m_clusterContainer->release();
    return StatusCode::SUCCESS;
  }
  
}// namespace closure


