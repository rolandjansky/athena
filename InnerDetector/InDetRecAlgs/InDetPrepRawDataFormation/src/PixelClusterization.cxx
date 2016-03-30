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

// Gaudi includes
#include "StoreGate/DataHandle.h"
#include "CxxUtils/make_unique.h"


namespace InDet{
  using namespace InDet;
//    const SG::ReadHandle<PixelRDO_Container> p_rdoContainer(m_dataObjectName);
  // Constructor with parameters:
  PixelClusterization::PixelClusterization(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_clusteringTool("InDet::MergedPixelsTool", this), //made private
  m_gangedAmbiguitiesFinder("InDet::PixelGangedAmbiguitiesFinder"),
  m_rdoContainer(),
//  m_dataObjectName("PixelRDOs"),	// RDO container
  m_managerName("Pixel"),
//  m_clustersName("NOTTHISNOTTHIS"),
  m_idHelper(nullptr),
  m_clusterContainer(),
  m_manager(nullptr) {  
    // Get parameter values from jobOptions file
    declareProperty("DataObjectName", m_rdoContainer = SG::ReadHandle<PixelRDO_Container>("PixelRDOs"), "Pixel RDOs");
    declareProperty("DetectorManagerName",m_managerName);
    declareProperty("clusteringTool", m_clusteringTool);
    declareProperty("gangedAmbiguitiesFinder", m_gangedAmbiguitiesFinder);
//    declareProperty("ClustersName",m_clustersName);
    declareProperty("ClustersName", 
                  m_clusterContainer = SG::WriteHandle<PixelClusterContainer>("PixelClusters"),
                  "Pixel cluster container");
    declareProperty("AmbiguitiesMap", 
                  m_ambiguitiesMap = SG::WriteHandle<PixelGangedClusterAmbiguities>("PixelClusterAmbiguitiesMap"),
                  "Ambiguity Map container");
    
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
    ATH_MSG_INFO( "Container m_clusterContainer '" << m_clusterContainer.name() << "' set");
    ATH_MSG_DEBUG( "Pixel detector manager retrieved" );

    // Get the Pixel helper
    if (detStore()->retrieve(m_idHelper,"PixelID").isFailure()) {
      (msg(MSG:: FATAL) << "Cannot retrieve ID helper!" << endreq);
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "Initialize done !" );
    return StatusCode::SUCCESS;
  }
  
  //----------------------------------------------------------------------------
  // Execute method:
  StatusCode PixelClusterization::execute() {
  
//    ATH_MSG_INFO( "Container m_clusterContainer '" << m_clusterContainer.name() << "' set");
    m_clusterContainer = CxxUtils::make_unique<PixelClusterContainer>(m_idHelper->wafer_hash_max());
    // declare the container:
//    m_clusterContainer->addRef();
    if (! m_clusterContainer.isValid() ){
      msg(MSG:: FATAL) << "Container of type PixelClusterContainer could not be initialised !"<< endreq;
      return StatusCode::FAILURE;
    }else{
      ATH_MSG_DEBUG( "Container '" << m_clusterContainer->name() << "' initialised" );
    }
    SiClusterContainer* symSiContainer = nullptr;
    StatusCode sc = evtStore()->symLink(m_clusterContainer.cptr(), symSiContainer);
    if (sc.isFailure()) {
      ATH_MSG_FATAL("Pixel clusters could not be symlinked in StoreGate !");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG( "Pixel clusters '" << m_clusterContainer.name() << "' symlinked in StoreGate");
    }   
    ATH_MSG_DEBUG( "Creating the ganged ambiguities map");
    m_ambiguitiesMap = CxxUtils::make_unique<PixelGangedClusterAmbiguities>();


    if (!m_rdoContainer.isValid()){
      msg(MSG:: FATAL) << "Could not find the data object "<< m_rdoContainer.name() << " !" << endreq;
      //    return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG( "Data object " << m_rdoContainer.name() << " found" );
      m_rdoContainer->clID(); 	// anything to dereference the DataHandle
      // will trigger the converter
      PixelRDO_Container::const_iterator rdoCollections      = m_rdoContainer->begin();
      PixelRDO_Container::const_iterator rdoCollectionsEnd   = m_rdoContainer->end();

//Adam - detType  doesn't seem to be used ? needed?
//      AtlasDetectorID detType;


      for(; rdoCollections!=rdoCollectionsEnd; ++rdoCollections){
        const COLLECTION* RDO_Collection(*rdoCollections);
        if (!RDO_Collection || RDO_Collection->empty()) continue;

        // Use one of the specific clustering AlgTools to make clusters
        std::unique_ptr<PixelClusterCollection> clusterCollection (m_clusteringTool->clusterize(*RDO_Collection, *m_manager, *m_idHelper));
        if (clusterCollection && !clusterCollection->empty()){

           m_gangedAmbiguitiesFinder->execute(clusterCollection.get(),*m_manager,*m_ambiguitiesMap);
           // -me- new IDC does no longer register in Storegate if hash is used !
           //Using get because I'm unsure of move semantec status
           StatusCode sc = m_clusterContainer->addCollection( clusterCollection.get(), clusterCollection->identifyHash() );
           if (sc.isFailure()){
             msg(MSG:: ERROR) << "Failed to add Clusters to container"<< endreq;
             return StatusCode::FAILURE;
           }
//           ATH_MSG_DEBUG(" PixelClusterCollection to write "<< clusterCollection->size());
           clusterCollection.release();//Release ownership if sucessfully added to collection

         }else{
             ATH_MSG_DEBUG("No PixelClusterCollection to write");
         }
      }
      if (m_clusterContainer.setConst().isFailure()){
        ATH_MSG_FATAL("FAILED TO SET CONST");
        return StatusCode::FAILURE;
      }

      if (!m_ambiguitiesMap.isValid()){
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
    return StatusCode::SUCCESS;
  }
  
}// namespace closure


