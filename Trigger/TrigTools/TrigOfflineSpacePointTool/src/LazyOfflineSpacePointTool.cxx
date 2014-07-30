/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// SpacePointProviderTool
// ( see header-file for details )
// -------------------------------
// ATLAS Collaboration
//
// Author : Nikos Konstantinidis  
// Modified : J.Baines 10/5/04
// Modified : P. Conde Muino (04/02/05)
////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include <algorithm>

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "DataModel/DataVector.h"

// EDM & Identifier
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h" 
#include "TrkPrepRawData/PrepRawDataCLASS_DEF.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"

// Cluster Formation
#include "SiClusterizationTool/SCT_ClusteringTool.h"


#include "Identifier/Identifier.h"
#include "AtlasDetDescr/AtlasDetectorID.h"    

// SpacePoint creation
#include "IRegionSelector/IRegSelSvc.h"

#include "TrigOfflineSpacePointTool/LazyOfflineSpacePointTool.h"
#include "TrigInDetEvent/TrigSiSpacePoint.h"
#include "InDetTrigToolInterfaces/ITrigSCT_SpacePointTool.h"

#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "TrkSpacePoint/SpacePointCollection.h" 
#include "InDetPrepRawData/SiClusterContainer.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

using namespace InDet;

//-------------------------------------------------------------------------
LazyOfflineSpacePointTool::LazyOfflineSpacePointTool(const std::string& t, 
						     const std::string& n,
						     const IInterface*  p ): 
  AlgTool(t,n,p), 
  m_pixelSpTool("InDet::SiSpacePointMakerTool",this),
  m_sctSpTool("InDet::SCT_TrigSpacePointTool",this),
  m_SCT_clusteringTool("InDet::SCT_ClusteringTool",this), 
  m_pixelClusteringTool("InDet::MergedPixelsTool",this),
  m_gangedAmbiguitiesFinder("InDet::PixelGangedAmbiguitiesFinder",this),
  m_numberingTool("TrigL2LayerNumberTool")
{
//-------------------------------------------------------------------------
  declareInterface< ISpacePointProvider >( this );
  

  // RDO containers:
  declareProperty( "SCT_RDO_ContainerName", 
		   m_sctRdoContainerName = "SCT_RDOs" );
  declareProperty( "PixelRDO_ContainerName",
		   m_pixelRDOContainerName = "PixelRDOs" );

  // Region Selector tool:
  declareProperty( "RegionSelectorToolName", 
		   m_regionSelectorToolName = "RegSelSvc" );

  // Clustering:
  /*
  declareProperty( "SCT_ClusteringToolName",
		   m_SCT_clusteringToolName= "InDet::SCT_ClusteringTool");
  */

  declareProperty("ClustersName",   m_SCTclustersName = "SCT_TrgClusters");
  /*
  declareProperty( "Pixel_ClusteringToolName",
		   m_pixelClusteringToolName= "InDet::MergedPixelsTool");
  */
  // Space Point formation:
  /*
  declareProperty( "SCT_SpacePointToolName",
		   m_sctSpToolName = "InDet::SCT_TrigSpacePointTool" );
  declareProperty( "PixelSpacePointToolName", 
		   m_pixelSpToolName = "InDet::SiSpacePointMakerTool" );
  */
  // error strategy
  //
  // 0 : broad errors (cluster width/sqrt(12) )
  //     as in old clustering code (release 6 and 7)
  // 1 : narrow errors (strip pitch/sqrt(12.) )
  //     DEFAULT - should be more accurate, and still conservative
  declareProperty("ErrorStrategy",m_errorStrategy=1);
  //bad channels 0 or 1
  //  declareProperty("CTBBadChannels",m_CTBBadChannels=0);

  // position strategy
  //
  // 0 : arithmetic mean of pixel position
  // 1 : simple charge interpolation
  declareProperty("PositionStrategy",m_positionStrategy=0);

  declareProperty( "ROBDataProviderSvcName", 
		   m_ROBDataProviderSvcName = "ROBDataProviderSvc" );

  declareProperty( "SctSpContainerName", m_sctSpContainerName = "TrigSCT_SpacePoints" );
  declareProperty( "PixelSpContainerName", m_pixelSpContainerName = "TrigPixelSpacePoints" );
}

StatusCode LazyOfflineSpacePointTool::finalize()
{
  m_pixelSpContainer->cleanup();
  delete m_pixelSpContainer;
  m_sctSpContainer->cleanup();
  delete m_sctSpContainer;

  StatusCode sc = AlgTool::finalize(); 
  return sc;
}
 

StatusCode LazyOfflineSpacePointTool::initialize()
{
  
  StatusCode sc = AlgTool::initialize();
  MsgStream athenaLog(msgSvc(), name());
  m_outputLevel = msgSvc()->outputLevel( name() );

  if (m_outputLevel <= MSG::DEBUG) 
    athenaLog << MSG::DEBUG << ": in initialize" << endreq;

  //   Get StoreGate service
  //
  sc = service( "StoreGateSvc", m_StoreGate );
  if (sc.isFailure()) {
    athenaLog << MSG::FATAL 
	      << "Unable to retrieve StoreGate service" << endreq;
    return sc;
  }

  // Get DetectorStore service
  StoreGateSvc * detStore;
  sc = service("DetectorStore",detStore);
  if (sc.isFailure()) {
    athenaLog << MSG::FATAL << "DetectorStore service not found !" << endreq;
    return StatusCode::FAILURE;
  } 

  // Get the SCT Detector Description Manager
  sc=detStore->retrieve(m_SCT_manager, "SCT");

  if (sc.isFailure()) {
      athenaLog << MSG::ERROR
	     << "Unable to retrieve SCT manager from DetectorStore" << endreq;
      return StatusCode::FAILURE;
  }

  // Get the Pixel Detector Description Manager
  sc=detStore->retrieve(m_pixelManager, "Pixel");

  if (sc.isFailure()) {
    athenaLog << MSG::ERROR 
	   << "Unable to retrieve Pixel manager from DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }

  // Get SCT & pixel Identifier helpers

  if (detStore->retrieve(m_pixelId, "PixelID").isFailure()) {
     athenaLog << MSG::FATAL << "Could not get Pixel ID helper" << endreq;
     return StatusCode::FAILURE;
  }  
  if (detStore->retrieve(m_sctId, "SCT_ID").isFailure()) {
     athenaLog << MSG::FATAL << "Could not get SCT ID helper" << endreq; 
     return StatusCode::FAILURE; 
  }

  // Get the region selector tool  
  sc = serviceLocator()->service( m_regionSelectorToolName, m_regionSelector);  
  if ( sc.isFailure() ) { 
     athenaLog << MSG::FATAL << "Unable to retrieve RegionSelector Service " << m_regionSelectorToolName << endreq;
     return sc;
  }

  // get the SCT clustering tool

  sc = m_SCT_clusteringTool.retrieve();
  if (sc.isFailure()){
    athenaLog << MSG::FATAL << "Clustering tool '" << m_SCT_clusteringTool
	      << "' not found " << endreq;
    return StatusCode::RECOVERABLE;
  } 

  // Create the SCT ClusterContainer:

  //IdentifierHash maxKey = m_pixelId->wafer_hash_max();

  // declare the container, starting the keys after the maximum pixel key
  m_SCTclusterContainer = new InDet::SCT_ClusterContainer(m_sctId->wafer_hash_max());
  m_SCTclusterContainer->addRef();


  sc=m_numberingTool.retrieve();
  if (sc.isFailure()){
    athenaLog << MSG::FATAL << "Tool " << m_numberingTool
	      << " not found " << endreq;
    return StatusCode::FAILURE;
  } 

  // get the Pixel clustering tool
  sc=m_pixelClusteringTool.retrieve();
  if (sc.isFailure()){
    athenaLog << MSG::FATAL << "Clustering tool " << m_pixelClusteringTool
	<< " not found " << endreq;
    return StatusCode::RECOVERABLE;
  } 

  // Get the PixelGangedAmbiguities... tool:

  sc=m_gangedAmbiguitiesFinder.retrieve();
  if (sc.isFailure()){
    athenaLog << MSG::FATAL << "Tool " << m_gangedAmbiguitiesFinder
	      << " not found " << endreq;
    return StatusCode::SUCCESS;
  } 

  sc = m_pixelSpTool.retrieve();
  if ( sc.isFailure() ) {
    athenaLog << MSG::FATAL << "Unable to locate PixelSpacePointTool "
	   << m_pixelSpTool  << endreq;
    return sc;
  } 
  
  sc = m_sctSpTool.retrieve();
  if ( sc.isFailure() ) {
    athenaLog << MSG::FATAL << "Unable to locate SCT_SpacePointTool " 
	   << m_sctSpTool << endreq;
    return sc;
  } 

  // Get the lists of bad channels (for CTB only).
  // Should this be done for every single event?
  /*
  if (m_CTBBadChannels) {
    if (m_outputLevel <= MSG::DEBUG) 
      athenaLog << MSG::DEBUG 
		<< "Checking database for bad channels: retrieving tool "
		<< " SCT_ChannelStatusAlg" << endreq;
    std::string tool = "SCT_ChannelStatusAlg"; 
    if(StatusCode::SUCCESS != toolSvc()->retrieveTool(tool,m_ChannelStatus)){
      athenaLog << MSG::ERROR << " Can't get the SCT_ChannelStatusAlg " 
	     << endreq;
      return StatusCode::SUCCESS;
    }
    if (m_outputLevel <= MSG::DEBUG) 
      athenaLog << MSG::DEBUG << "Found the database interface" << endreq;
  }
  */
  
  //  Get ROBDataProvider
  sc = service( m_ROBDataProviderSvcName, m_robDataProvider );
  if( sc.isFailure() ) {
    athenaLog << MSG::FATAL << " Can't get ROBDataProviderSvc " <<
      endreq;
    return sc;
  }

  athenaLog << MSG::INFO << "PixelSP_ContainerName: " << m_pixelSpContainerName << endreq;
  m_pixelSpContainer = new TrigSiSpacePointContainer(m_pixelId->wafer_hash_max());
  m_pixelSpContainer->addRef();
  athenaLog << MSG::INFO << "SCT_SP_ContainerName: " << m_sctSpContainerName << endreq;
  m_sctSpContainer = new TrigSiSpacePointContainer(m_sctId->wafer_hash_max());
  m_sctSpContainer->addRef();

  ITrigTimerSvc* timerSvc;
  sc = service( "TrigTimerSvc", timerSvc);
  if( sc.isFailure() ) {
    athenaLog << MSG::INFO<< "Unable to locate Service TrigTimerSvc " << endreq;
    m_timers = false;
  } 
  else{
    m_timers = true;  
  }
  if ( m_timers ) {
    m_timer[0] = timerSvc->addItem("LazyPixelSP");
    m_timer[1] = timerSvc->addItem("LazySCT_SP");
    m_timer[2] = timerSvc->addItem("PixelClusters");
    m_timer[3] = timerSvc->addItem("TotalInPixel");
    m_timer[4] = timerSvc->addItem("SCTClusters");
    m_timer[5] = timerSvc->addItem("TotalInSCT");
    m_timer[6] = timerSvc->addItem("SCT_Clusterization");
  }
  m_pixelDataErrors.clear();
  m_sctDataErrors.clear();
  return sc;
}

const std::vector<int>* LazyOfflineSpacePointTool::fillPixelDataErrors()
{
  return &m_pixelDataErrors;
}

const std::vector<int>* LazyOfflineSpacePointTool::fillSCT_DataErrors()
{
  return &m_sctDataErrors;
}

//-------------------------------------------------------------------------
int LazyOfflineSpacePointTool::formPixelSpacePoints(const InDetRawDataCollection<PixelRDORawData>* 
						    RDO_collection, std::vector<int>& vPixIds) 
{
//-------------------------------------------------------------------------
  MsgStream athenaLog(msgSvc(), name());
  int nSP=0;

  if(!RDO_collection || RDO_collection->size()==0) {
    athenaLog << MSG::ERROR 
	   << " formPixel_spacePoints: Received invalid RDO_collection "
	   << endreq;
    return nSP;
  }

  InDet::PixelClusterCollection* clusterCollection =
    m_pixelClusteringTool->clusterize(*RDO_collection,*m_pixelManager, 
				      *m_pixelId);
  
  if (!clusterCollection || clusterCollection->size()==0){ 
    athenaLog << MSG::WARNING
	      << "Pixel Clustering algorithm didn't find clusters!" 
	      << endreq;
    return nSP;
  }

  // create and register the ganged pixel ambiguities map into SG
  /*
  InDet::PixelGangedClusterAmbiguities* AmbiguitiesMap =
    new InDet::PixelGangedClusterAmbiguities();     

  m_gangedAmbiguitiesFinder->execute(clusterCollection,
				     *m_pixelManager, *AmbiguitiesMap);
				     */

  // Space point formation algorithms:

  IdentifierHash idHash = clusterCollection->identifyHash(); 

  TrigSiSpacePointContainer::const_iterator spCollIt;
  spCollIt = m_pixelSpContainer->indexFind(idHash);
  if(spCollIt!=m_pixelSpContainer->end()) return nSP;

  Identifier elementID = clusterCollection->identify(); 
  SpacePointCollection* spacepointCollection = 
    new SpacePointCollection(idHash); 
  spacepointCollection->setIdentifier(elementID); 
      
  m_pixelSpTool->fillPixelSpacePointCollection(clusterCollection,
					       spacepointCollection);
      
  if(!spacepointCollection){
    if (m_outputLevel <= MSG::DEBUG) 
      athenaLog << MSG::DEBUG 
		<< " formPixelSpacePoints: no pixel space point found. "  
		<< endreq;
    return nSP;
  }

  // Copy space points into the space point vector:

  TrigSiSpacePointCollection* spacePointColl = new TrigSiSpacePointCollection(idHash);

  vPixIds.push_back((int)(idHash));

  DataVector<Trk::SpacePoint>::const_iterator nextSpacePoint;
  for( nextSpacePoint = spacepointCollection->begin() ; 
       nextSpacePoint!= spacepointCollection->end() ; ++nextSpacePoint) {
       
    const Trk::SpacePoint* sp = &(**nextSpacePoint);
    spacePointColl->push_back( this->makeTrigPixSP( sp ));
  }

  StatusCode sc=m_pixelSpContainer->addCollection(spacePointColl,spacePointColl->identifyHash());
	  
  if (sc.isFailure()) 
    {
      athenaLog << MSG::WARNING  << " Fail to store a TrigSiSpacePoint collection in Pixel with hash "
	  << idHash << endreq;
    } 
  else 
    {
      if(m_outputLevel<=MSG::DEBUG)
	athenaLog << MSG::DEBUG << " Pixel TrigSiSpacePoint collection is stored in IDC with hashId "
		  <<spacePointColl->identifyHash()<<endreq;	      
      nSP=spacepointCollection->size();
    } 
  return nSP;
}  


//-------------------------------------------------------------------------
void LazyOfflineSpacePointTool::
formSCT_spacePoints(const InDetRawDataCollection<SCT_RDORawData>* 
		    /*RDO_collection*/, 
		    spacePointVec& /*spVec*/){
//-------------------------------------------------------------------------

/*    Obsolete ??????????


  MsgStream athenaLog(msgSvc(), name());

  if(!RDO_collection || RDO_collection->size()==0) {
    athenaLog << MSG::ERROR 
	   << " formSCT_spacePoints: Received not valid RDO_collection "
	   << endreq;
    return;
  }
   
  InDet::SCT_ClusterCollection* clusterCollection = 
    m_SCT_clusteringTool->clusterize(*RDO_collection, *m_SCT_manager, *m_sctId);
  
  if (!clusterCollection || clusterCollection->size()==0)   {
    athenaLog << MSG::WARNING
	      << "SCT Clustering algorithm didn't find clusters!" 
	      << endreq;
    return;
  }

  // makes global position and error matrix
  InDet::SCT_ClusterCollection* clusterCollection2 = 
    new InDet::SCT_ClusterCollection(clusterCollection->identifyHash());

  m_globalPosAlg->execute(clusterCollection, clusterCollection2,*m_SCT_manager,
			  m_errorStrategy);

  if(!clusterCollection2 || clusterCollection2->size()==0){
    athenaLog << MSG::ERROR 
	      << "formSCT_spacePoints: GlobalPositionMaker algorithm failed!" 
	   << endreq;
    return;
  } 

  // Create SpacePointCollection
  IdentifierHash idHash = clusterCollection2->identifyHash();
  Identifier elementID = clusterCollection2->identify(); 
  SpacePointCollection* spacepointCollection = 
    new SpacePointCollection(idHash); 
  spacepointCollection->setIdentifier(elementID); 

  m_sctSpTool->addSCT_SpacePoints(clusterCollection2,  spacepointCollection);

  if(!spacepointCollection || spacepointCollection->size()==0){
    athenaLog << MSG::WARNING << "SCT SpacePoint tool didn't find SpacePoints" 
	   << endreq;
    return;
  }

  // copy each space point on the space point vector:
  DataVector<Trk::SpacePoint>::const_iterator nextSpacePoint;
  for( nextSpacePoint = spacepointCollection->begin() ; 
       nextSpacePoint!= spacepointCollection->end() ; ++nextSpacePoint) {
       
    const Trk::SpacePoint* sp = &(**nextSpacePoint);
    spVec.push_back( this->makeTrigSctSP(sp) );
  }


  delete clusterCollection;
  //delete clusterCollection2;
  */

}

StatusCode LazyOfflineSpacePointTool::fillCollections(bool getPixelSP, bool getSCT_SP,
									std::vector<int>& PixelIDs, std::vector<int>& SCT_IDs)
{
  StatusCode sc;
  MsgStream athenaLog(msgSvc(), name());
  PixelIDs.clear();SCT_IDs.clear();
  int nPixSp=0,nSCT_SP=0;
  if(m_timers ) 
    {
      m_timer[0]->start();
      m_timer[0]->pause();
      m_timer[2]->start();
      m_timer[2]->pause();
      m_timer[4]->start();
      m_timer[4]->pause();
    }
  
  if(getPixelSP) 
    {
      if(!m_StoreGate->contains<TrigSiSpacePointContainer>(m_pixelSpContainerName))
	{
	  m_pixelSpContainer->cleanup();
	  
	  sc=m_StoreGate->record(m_pixelSpContainer,m_pixelSpContainerName,false);
	  if(sc.isFailure())
	    {
	      athenaLog << MSG::WARNING << "Pixel TrigSP Container " << m_pixelSpContainerName 
		  <<" cannot be recorded in StoreGate !"<< endreq;
	      return sc;
	    }
	  else 
	    {
	      if(m_outputLevel <= MSG::DEBUG)
		athenaLog << MSG::DEBUG << "Pixel TrigSP Container " << m_pixelSpContainerName
			  << " is recorded in StoreGate" << endreq;
	    }
	}
      else 
	{    
	  if(m_outputLevel <= MSG::DEBUG)
	    athenaLog << MSG::DEBUG << "Pixel TrigSP Container " <<m_pixelSpContainerName 
		      << " is found in StoreGate" << endreq;
	}

      std::vector<unsigned int> uIntListOfPixelRobs;
      m_regionSelector->DetROBIDListUint(PIXEL, uIntListOfPixelRobs);
      m_robDataProvider->addROBData( uIntListOfPixelRobs );


      std::vector<IdentifierHash> listOfPixIds;
      listOfPixIds.clear();
      m_regionSelector->DetHashIDList(PIXEL, listOfPixIds);
    
      if (m_outputLevel <= MSG::DEBUG) 
	athenaLog << MSG::DEBUG << "REGTEST / Pixel : Roi contains " 
		  << listOfPixIds.size() << " det. Elements" << endreq;

    
      const PixelRDO_Container* p_pixelRDOContainer;
      StatusCode sc = m_StoreGate->retrieve(p_pixelRDOContainer,  
					    m_pixelRDOContainerName);
      if(sc.isFailure()) 
	{
	  athenaLog << MSG::WARNING << "Could not find the PixelRDO_Container " 
		    << m_pixelRDOContainerName << endreq;
	  return sc;
	} 
      else 
	{
	  if (m_outputLevel <= MSG::DEBUG) 
	    athenaLog << MSG::DEBUG << " Found the Pixel_RDO_Container " 
		      << m_pixelRDOContainerName << endreq;
	}
      
      if(m_timers) m_timer[3]->start();
      
      for(unsigned int i=0;i<listOfPixIds.size();i++) 
	{
	  if( m_timers ) m_timer[2]->resume();
	  PixelRDO_Container::const_iterator RDO_collection_iter=p_pixelRDOContainer->indexFind(listOfPixIds[i]);
	  if(RDO_collection_iter==p_pixelRDOContainer->end()) 
	    {
	      if ( m_timers ) m_timer[2]->pause();
	      continue;
	    }
	  const InDetRawDataCollection<PixelRDORawData>* RDO_Collection=&(**RDO_collection_iter);
	  if(!RDO_Collection) 
	    {
	      if ( m_timers ) m_timer[2]->pause();
	      continue;
	    }
	  if(RDO_Collection->size() == 0) 
	    {
	      if ( m_timers ) m_timer[2]->pause();
	      continue;
	    }
	  if(m_timers ) m_timer[2]->pause();
	
	  if(m_timers ) m_timer[0]->resume();
	  nPixSp+=formPixelSpacePoints(RDO_Collection,PixelIDs);
	  if(m_timers) m_timer[0]->pause();
	}
      if(m_timers ) m_timer[3]->stop();
    }
  if ( m_timers ) m_timer[0]->stop();
  if ( m_timers ) m_timer[2]->stop();

  if (m_outputLevel <= MSG::DEBUG) 
    athenaLog << MSG::INFO << " Number of pixel SP = " << nPixSp
	      << endreq;
  if ( m_timers ) 
    {
      m_timer[1]->start();
      m_timer[1]->pause();
    }
  if (getSCT_SP) 
    {
      if(!m_StoreGate->contains<TrigSiSpacePointContainer>(m_sctSpContainerName))
	{
	  m_sctSpContainer->cleanup();
	  
	  sc=m_StoreGate->record(m_sctSpContainer,m_sctSpContainerName,false);
	  if(sc.isFailure())
	    {
	      athenaLog << MSG::WARNING << "SCT TrigSP Container " << m_sctSpContainerName 
			<<" cannot be recorded in StoreGate !"<< endreq;
	      return sc;
	    }
	  else 
	    {
	      if(m_outputLevel <= MSG::DEBUG)
		 athenaLog << MSG::DEBUG << "SCT TrigSP Container " << m_sctSpContainerName
			   << " is recorded in StoreGate" << endreq;
	    }
	}
      else 
	{    
	  if(m_outputLevel <= MSG::DEBUG)
	    athenaLog << MSG::DEBUG << "SCT TrigSP Container " <<m_sctSpContainerName 
		<< " is found in StoreGate" << endreq;
	}

      std::vector<unsigned int> uIntListOfSctRobs;
      m_regionSelector->DetROBIDListUint(SCT, uIntListOfSctRobs);
      m_robDataProvider->addROBData( uIntListOfSctRobs );

      // register the IdentifiableContainer into StoreGate
      // ------------------------------------------------------
      if(!m_StoreGate->contains<InDet::SCT_ClusterContainer>(m_SCTclustersName))
	{
	  // The cluster container is cleant up only at the begining of the event.
	  m_SCTclusterContainer->cleanup();
	  sc=m_StoreGate->record(m_SCTclusterContainer,m_SCTclustersName,false);
	  if(sc.isFailure()) 
	    {
	      athenaLog << MSG::WARNING << " Container " << m_SCTclustersName
			<< " could not be recorded in StoreGate !" 
			<< endreq;
	      return sc;
	    } 
	  else 
	    {
	      if (m_outputLevel <= MSG::DEBUG)
		athenaLog << MSG::INFO << "Container " << m_SCTclustersName 
			  << " registered  in StoreGate" << endreq;  
	    }
	}
      else 
	{    
	  if (m_outputLevel <= MSG::DEBUG) 
	    athenaLog << MSG::INFO << "Container '" << m_SCTclustersName 
		      << "' existed already  in StoreGate. " 
		      << endreq;
	}
      const SCT_RDO_Container* p_sctRdoContainer;
      sc=m_StoreGate->retrieve(p_sctRdoContainer, 
			       m_sctRdoContainerName);
      if(sc.isFailure()) 
	{
	  athenaLog << MSG::FATAL << "Could not find the SCT_RDO_Container "
		    << m_sctRdoContainerName << endreq;
	  return sc;
	}
      else
	{
	  if (m_outputLevel <= MSG::DEBUG) 
	    athenaLog << MSG::DEBUG << " Found the SCT_RDO_Container "
		      << m_sctRdoContainerName << endreq;
	  
	  std::vector<IdentifierHash> listOfIds;
	  listOfIds.clear();
	 
	  m_regionSelector->DetHashIDList(SCT, listOfIds);
    
	  if (m_outputLevel <= MSG::DEBUG) 
	    athenaLog << MSG::DEBUG << "REGTEST / SCT : Roi contains " 
		      << listOfIds.size() << " det. Elements" << endreq;
    
	  //------------------------------------
	  // First Reconstruct all clusters:
	  //------------------------------------
      
	  std::vector<InDet::SCT_ClusterCollection* > vectClusterCol;
	  int nSCTclusters=0;

	  if ( m_timers ) 
	    {
	      m_timer[5]->start();
	      m_timer[6]->start();
	      m_timer[6]->pause();
	    }
	  for(unsigned int i=0; i<listOfIds.size(); i++) 
	    {
	      if ( m_timers ) m_timer[4]->resume();	

	      SCT_RDO_Container::const_iterator 
		RDO_collection_iter = p_sctRdoContainer->indexFind(listOfIds[i]); 
      
	      if (RDO_collection_iter == p_sctRdoContainer->end()) continue;
	      
	      const InDetRawDataCollection<SCT_RDORawData>* 
		RDO_collection = &(**RDO_collection_iter);
	
	      if (RDO_collection->size() == 0) continue;
	
	      if(!RDO_collection || RDO_collection->size()==0) 
		{
		  athenaLog << MSG::ERROR 
			    << "formSCT_spacePoints: Received not valid RDO_collection"
			    << endreq;
		  if ( m_timers ) m_timer[4]->pause();
		  continue;
		}
 
	      m_timer[6]->resume();
	      InDet::SCT_ClusterCollection* clusterCollection = 
		m_SCT_clusteringTool->clusterize(*RDO_collection, *m_SCT_manager, 
						 *m_sctId);
	      m_timer[6]->pause();
	      if (!clusterCollection || clusterCollection->size()==0)   
		{
		  athenaLog << MSG::WARNING
			    << "SCT Clustering algorithm didn't find clusters!" 
			    << endreq;
		  if ( m_timers ) m_timer[4]->pause();
		  continue;
		}

	      vectClusterCol.push_back(clusterCollection);

	      // Loop over the clusterCollections and form space points:
	
	      sc = m_SCTclusterContainer->addCollection(clusterCollection,
							clusterCollection->identifyHash());
	      if (sc.isFailure()) 
		{
		  athenaLog << MSG::ERROR 
			    << "Cluster collection " << clusterCollection->identify()
			    << " could not be recorded in StoreGate !  Hash id = "
			    << listOfIds[i]  << endreq;
		  return sc;
		} 
	      else 
		if (m_outputLevel <= MSG::DEBUG) 
		  {	    
		    athenaLog << MSG::INFO << "Cluster collection '" 
			      <<  clusterCollection->identify()
			      << "' recorded in StoreGate. Hash id = " << listOfIds[i]
			      << endreq;
		  }
	      nSCTclusters += clusterCollection->size();
	      if ( m_timers ) m_timer[4]->pause();
	    } // end of loop in hash id's
	  if ( m_timers ) 
	    {
	      m_timer[4]->stop();
	      m_timer[6]->stop();
	    }
	  if (m_outputLevel <= MSG::DEBUG)
	    athenaLog << MSG::INFO << " Found " << nSCTclusters << " SCT clusters "
		      << endreq;

	  // Loop over the cluster collections and execute the space point
	  // formation algorithm:
      
	  std::vector<InDet::SCT_ClusterCollection* >::iterator itClusterCol =
	    vectClusterCol.begin();
      
	  for(; itClusterCol != vectClusterCol.end(); itClusterCol++)
	    {
	      // Create SpacePointCollection
	      IdentifierHash idHash = (*itClusterCol)->identifyHash();

	      TrigSiSpacePointContainer::const_iterator spCollIt;
	      spCollIt = m_pixelSpContainer->indexFind(idHash);
	      if(spCollIt!=m_pixelSpContainer->end()) continue;

	      Identifier elementID = (*itClusterCol)->identify(); 
	      SpacePointCollection* spacepointCollection = new SpacePointCollection(idHash); 
	      spacepointCollection->setIdentifier(elementID); 
	      
	      if ( m_timers ) m_timer[1]->resume();
	      
	      m_sctSpTool->addSCT_SpacePoints((*itClusterCol), 
					      m_SCTclusterContainer,
					      spacepointCollection);
	      if ( m_timers ) m_timer[1]->pause();
	      if(!spacepointCollection || spacepointCollection->size()==0)
		{
		  if (m_outputLevel <= MSG::DEBUG) 
		    athenaLog << MSG::INFO 
			      << "SCT SpacePoint tool didn't find SpacePoints"
			      << endreq;
		  continue;
		}
	      nSCT_SP+=spacepointCollection->size();

	      TrigSiSpacePointCollection* spacePointColl = new TrigSiSpacePointCollection(idHash);

	      SCT_IDs.push_back((int)(idHash));

	      DataVector<Trk::SpacePoint>::const_iterator nextSpacePoint;
	      for( nextSpacePoint = spacepointCollection->begin() ; 
		   nextSpacePoint!= spacepointCollection->end(); ++nextSpacePoint) 
		{
		  const Trk::SpacePoint* sp = &(**nextSpacePoint);
		  spacePointColl->push_back( this->makeTrigSctSP(sp) );
		}
	      sc=m_sctSpContainer->addCollection(spacePointColl,spacePointColl->identifyHash());
	      if (sc.isFailure()) 
		{
		  athenaLog << MSG::WARNING  << " Fail to store a TrigSiSpacePoint collection in SCT with hash "
			    << idHash << endreq;
		  break;
		} 
	      else 
		{
		  if(m_outputLevel<=MSG::DEBUG)
		    athenaLog << MSG::DEBUG << " SCT TrigSiSpacePoint collection is stored in IDC with hashId "
			      <<spacePointColl->identifyHash()<<endreq;	      
		}
	    } 
	  if ( m_timers ) 
	    {
	      m_timer[5]->stop();
	    }
	}
    }
  if ( m_timers ) m_timer[1]->stop();

  if (m_outputLevel <= MSG::DEBUG) 
    athenaLog << MSG::INFO << " Number of SCT + Pixel SP = " << nPixSp+nSCT_SP
	      << endreq;
  
  return sc;
}

StatusCode LazyOfflineSpacePointTool::fillCollections(const IRoiDescriptor& roi, bool getPixelSP, bool getSCT_SP,
						      std::vector<int>& PixelIDs, std::vector<int>& SCT_IDs)
{
  StatusCode sc;
  MsgStream athenaLog(msgSvc(), name());
  PixelIDs.clear();SCT_IDs.clear();
  int nPixSp=0,nSCT_SP=0;
  if(m_timers ) 
    {
      m_timer[0]->start();
      m_timer[0]->pause();
      m_timer[2]->start();
      m_timer[2]->pause();
      m_timer[4]->start();
      m_timer[4]->pause();
    }
  
  if(getPixelSP) 
    {
      if(!m_StoreGate->contains<TrigSiSpacePointContainer>(m_pixelSpContainerName))
	{
	  m_pixelSpContainer->cleanup();
	  
	  sc=m_StoreGate->record(m_pixelSpContainer,m_pixelSpContainerName,false);
	  if(sc.isFailure())
	    {
	      athenaLog << MSG::WARNING << "Pixel TrigSP Container " << m_pixelSpContainerName 
		  <<" cannot be recorded in StoreGate !"<< endreq;
	      return sc;
	    }
	  else 
	    {
	      if(m_outputLevel <= MSG::DEBUG)
		athenaLog << MSG::DEBUG << "Pixel TrigSP Container " << m_pixelSpContainerName
			  << " is recorded in StoreGate" << endreq;
	    }
	}
      else 
	{    
	  if(m_outputLevel <= MSG::DEBUG)
	    athenaLog << MSG::DEBUG << "Pixel TrigSP Container " <<m_pixelSpContainerName 
		      << " is found in StoreGate" << endreq;
	}

      std::vector<unsigned int> uIntListOfPixelRobs;
      m_regionSelector->DetROBIDListUint(PIXEL, roi, uIntListOfPixelRobs);
      m_robDataProvider->addROBData( uIntListOfPixelRobs );


      std::vector<IdentifierHash> listOfPixIds;
      listOfPixIds.clear();
      m_regionSelector->DetHashIDList(PIXEL, roi, listOfPixIds);
    
      if (m_outputLevel <= MSG::DEBUG) 
	athenaLog << MSG::DEBUG << "REGTEST / Pixel : Roi contains " 
		  << listOfPixIds.size() << " det. Elements" << endreq;

    
      const PixelRDO_Container* p_pixelRDOContainer;
      StatusCode sc = m_StoreGate->retrieve(p_pixelRDOContainer,  
					    m_pixelRDOContainerName);
      if(sc.isFailure()) 
	{
	  athenaLog << MSG::WARNING << "Could not find the PixelRDO_Container " 
		    << m_pixelRDOContainerName << endreq;
	  return sc;
	} 
      else 
	{
	  if (m_outputLevel <= MSG::DEBUG) 
	    athenaLog << MSG::DEBUG << " Found the Pixel_RDO_Container " 
		      << m_pixelRDOContainerName << endreq;
	}
      
      if(m_timers) m_timer[3]->start();
      
      for(unsigned int i=0;i<listOfPixIds.size();i++) 
	{
	  if( m_timers ) m_timer[2]->resume();
	  PixelRDO_Container::const_iterator RDO_collection_iter=p_pixelRDOContainer->indexFind(listOfPixIds[i]);
	  if(RDO_collection_iter==p_pixelRDOContainer->end()) 
	    {
	      if ( m_timers ) m_timer[2]->pause();
	      continue;
	    }
	  const InDetRawDataCollection<PixelRDORawData>* RDO_Collection=&(**RDO_collection_iter);
	  if(!RDO_Collection) 
	    {
	      if ( m_timers ) m_timer[2]->pause();
	      continue;
	    }
	  if(RDO_Collection->size() == 0) 
	    {
	      if ( m_timers ) m_timer[2]->pause();
	      continue;
	    }
	  if(m_timers ) m_timer[2]->pause();
	
	  if(m_timers ) m_timer[0]->resume();
	  nPixSp+=formPixelSpacePoints(RDO_Collection,PixelIDs);
	  if(m_timers) m_timer[0]->pause();
	}
      if(m_timers ) m_timer[3]->stop();
    }
  if ( m_timers ) m_timer[0]->stop();
  if ( m_timers ) m_timer[2]->stop();

  if (m_outputLevel <= MSG::DEBUG) 
    athenaLog << MSG::INFO << " Number of pixel SP = " << nPixSp
	      << endreq;
  if ( m_timers ) 
    {
      m_timer[1]->start();
      m_timer[1]->pause();
    }
  if (getSCT_SP) 
    {
      if(!m_StoreGate->contains<TrigSiSpacePointContainer>(m_sctSpContainerName))
	{
	  m_sctSpContainer->cleanup();
	  
	  sc=m_StoreGate->record(m_sctSpContainer,m_sctSpContainerName,false);
	  if(sc.isFailure())
	    {
	      athenaLog << MSG::WARNING << "SCT TrigSP Container " << m_sctSpContainerName 
			<<" cannot be recorded in StoreGate !"<< endreq;
	      return sc;
	    }
	  else 
	    {
	      if(m_outputLevel <= MSG::DEBUG)
		 athenaLog << MSG::DEBUG << "SCT TrigSP Container " << m_sctSpContainerName
			   << " is recorded in StoreGate" << endreq;
	    }
	}
      else 
	{    
	  if(m_outputLevel <= MSG::DEBUG)
	    athenaLog << MSG::DEBUG << "SCT TrigSP Container " <<m_sctSpContainerName 
		<< " is found in StoreGate" << endreq;
	}

      std::vector<unsigned int> uIntListOfSctRobs;
      m_regionSelector->DetROBIDListUint(SCT, uIntListOfSctRobs);
      m_robDataProvider->addROBData( uIntListOfSctRobs );

      // register the IdentifiableContainer into StoreGate
      // ------------------------------------------------------
      if(!m_StoreGate->contains<InDet::SCT_ClusterContainer>(m_SCTclustersName))
	{
	  // The cluster container is cleant up only at the begining of the event.
	  m_SCTclusterContainer->cleanup();
	  sc=m_StoreGate->record(m_SCTclusterContainer,m_SCTclustersName,false);
	  if(sc.isFailure()) 
	    {
	      athenaLog << MSG::WARNING << " Container " << m_SCTclustersName
			<< " could not be recorded in StoreGate !" 
			<< endreq;
	      return sc;
	    } 
	  else 
	    {
	      if (m_outputLevel <= MSG::DEBUG)
		athenaLog << MSG::INFO << "Container " << m_SCTclustersName 
			  << " registered  in StoreGate" << endreq;  
	    }
	}
      else 
	{    
	  if (m_outputLevel <= MSG::DEBUG) 
	    athenaLog << MSG::INFO << "Container '" << m_SCTclustersName 
		      << "' existed already  in StoreGate. " 
		      << endreq;
	}
      const SCT_RDO_Container* p_sctRdoContainer;
      sc=m_StoreGate->retrieve(p_sctRdoContainer, 
			       m_sctRdoContainerName);
      if(sc.isFailure()) 
	{
	  athenaLog << MSG::FATAL << "Could not find the SCT_RDO_Container "
		    << m_sctRdoContainerName << endreq;
	  return sc;
	}
      else
	{
	  if (m_outputLevel <= MSG::DEBUG) 
	    athenaLog << MSG::DEBUG << " Found the SCT_RDO_Container "
		      << m_sctRdoContainerName << endreq;
	  
	  std::vector<IdentifierHash> listOfIds;
	  listOfIds.clear();
	 
	  m_regionSelector->DetHashIDList(SCT, listOfIds);
    
	  if (m_outputLevel <= MSG::DEBUG) 
	    athenaLog << MSG::DEBUG << "REGTEST / SCT : Roi contains " 
		      << listOfIds.size() << " det. Elements" << endreq;
    
	  //------------------------------------
	  // First Reconstruct all clusters:
	  //------------------------------------
      
	  std::vector<InDet::SCT_ClusterCollection* > vectClusterCol;
	  int nSCTclusters=0;

	  if ( m_timers ) 
	    {
	      m_timer[5]->start();
	      m_timer[6]->start();
	      m_timer[6]->pause();
	    }
	  for(unsigned int i=0; i<listOfIds.size(); i++) 
	    {
	      if ( m_timers ) m_timer[4]->resume();	

	      SCT_RDO_Container::const_iterator 
		RDO_collection_iter = p_sctRdoContainer->indexFind(listOfIds[i]); 
      
	      if (RDO_collection_iter == p_sctRdoContainer->end()) continue;
	      
	      const InDetRawDataCollection<SCT_RDORawData>* 
		RDO_collection = &(**RDO_collection_iter);
	
	      if (RDO_collection->size() == 0) continue;
	
	      if(!RDO_collection || RDO_collection->size()==0) 
		{
		  athenaLog << MSG::ERROR 
			    << "formSCT_spacePoints: Received not valid RDO_collection"
			    << endreq;
		  if ( m_timers ) m_timer[4]->pause();
		  continue;
		}
 
	      m_timer[6]->resume();
	      InDet::SCT_ClusterCollection* clusterCollection = 
		m_SCT_clusteringTool->clusterize(*RDO_collection, *m_SCT_manager, 
						 *m_sctId);
	      m_timer[6]->pause();
	      if (!clusterCollection || clusterCollection->size()==0)   
		{
		  athenaLog << MSG::WARNING
			    << "SCT Clustering algorithm didn't find clusters!" 
			    << endreq;
		  if ( m_timers ) m_timer[4]->pause();
		  continue;
		}

	      vectClusterCol.push_back(clusterCollection);

	      // Loop over the clusterCollections and form space points:
	
	      sc = m_SCTclusterContainer->addCollection(clusterCollection,
							clusterCollection->identifyHash());
	      if (sc.isFailure()) 
		{
		  athenaLog << MSG::ERROR 
			    << "Cluster collection " << clusterCollection->identify()
			    << " could not be recorded in StoreGate !  Hash id = "
			    << listOfIds[i]  << endreq;
		  return sc;
		} 
	      else 
		if (m_outputLevel <= MSG::DEBUG) 
		  {	    
		    athenaLog << MSG::INFO << "Cluster collection '" 
			      <<  clusterCollection->identify()
			      << "' recorded in StoreGate. Hash id = " << listOfIds[i]
			      << endreq;
		  }
	      nSCTclusters += clusterCollection->size();
	      if ( m_timers ) m_timer[4]->pause();
	    } // end of loop in hash id's
	  if ( m_timers ) 
	    {
	      m_timer[4]->stop();
	      m_timer[6]->stop();
	    }
	  if (m_outputLevel <= MSG::DEBUG)
	    athenaLog << MSG::INFO << " Found " << nSCTclusters << " SCT clusters "
		      << endreq;

	  // Loop over the cluster collections and execute the space point
	  // formation algorithm:
      
	  std::vector<InDet::SCT_ClusterCollection* >::iterator itClusterCol =
	    vectClusterCol.begin();
      
	  for(; itClusterCol != vectClusterCol.end(); itClusterCol++)
	    {
	      // Create SpacePointCollection
	      IdentifierHash idHash = (*itClusterCol)->identifyHash();

	      TrigSiSpacePointContainer::const_iterator spCollIt;
	      spCollIt = m_pixelSpContainer->indexFind(idHash);
	      if(spCollIt!=m_pixelSpContainer->end()) continue;

	      Identifier elementID = (*itClusterCol)->identify(); 
	      SpacePointCollection* spacepointCollection = new SpacePointCollection(idHash); 
	      spacepointCollection->setIdentifier(elementID); 
	      
	      if ( m_timers ) m_timer[1]->resume();
	      
	      m_sctSpTool->addSCT_SpacePoints((*itClusterCol), 
					      m_SCTclusterContainer,
					      spacepointCollection);
	      if ( m_timers ) m_timer[1]->pause();
	      if(!spacepointCollection || spacepointCollection->size()==0)
		{
		  if (m_outputLevel <= MSG::DEBUG) 
		    athenaLog << MSG::INFO 
			      << "SCT SpacePoint tool didn't find SpacePoints"
			      << endreq;
		  continue;
		}
	      nSCT_SP+=spacepointCollection->size();

	      TrigSiSpacePointCollection* spacePointColl = new TrigSiSpacePointCollection(idHash);

	      SCT_IDs.push_back((int)(idHash));

	      DataVector<Trk::SpacePoint>::const_iterator nextSpacePoint;
	      for( nextSpacePoint = spacepointCollection->begin() ; 
		   nextSpacePoint!= spacepointCollection->end(); ++nextSpacePoint) 
		{
		  const Trk::SpacePoint* sp = &(**nextSpacePoint);
		  spacePointColl->push_back( this->makeTrigSctSP(sp) );
		}
	      sc=m_sctSpContainer->addCollection(spacePointColl,spacePointColl->identifyHash());
	      if (sc.isFailure()) 
		{
		  athenaLog << MSG::WARNING  << " Fail to store a TrigSiSpacePoint collection in SCT with hash "
			    << idHash << endreq;
		  break;
		} 
	      else 
		{
		  if(m_outputLevel<=MSG::DEBUG)
		    athenaLog << MSG::DEBUG << " SCT TrigSiSpacePoint collection is stored in IDC with hashId "
			      <<spacePointColl->identifyHash()<<endreq;	      
		}
	    } 
	  if ( m_timers ) 
	    {
	      m_timer[5]->stop();
	    }
	}
    }
  if ( m_timers ) m_timer[1]->stop();

  if (m_outputLevel <= MSG::DEBUG) 
    athenaLog << MSG::INFO << " Number of SCT + Pixel SP = " << nPixSp+nSCT_SP
	      << endreq;
  
  return sc;
}


//-------------------------------------------------------------------------
spacePointVec LazyOfflineSpacePointTool::getSpacePoints(bool getPixelSP,bool getSctSP)
{
//-------------------------------------------------------------------------

  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

  spacePointVec spVec;
  std::vector<int> listOfPixIds;
  std::vector<int> listOfSctIds;

  StatusCode sc=fillCollections(getPixelSP,getSctSP,listOfPixIds,listOfSctIds);

  if(sc.isFailure())
    {
      athenaLog << MSG::WARNING << "SpacePoint formation failed " << endreq; 
      return spVec;
    }
  if(getPixelSP) 
    {
      const TrigSiSpacePointContainer* pCont;
      sc=m_StoreGate->retrieve(pCont,m_pixelSpContainerName);
      if(sc.isFailure())
	{
	  athenaLog << MSG::WARNING << "Trig SP Pixel container " <<m_pixelSpContainerName
		    <<" not found"<<endreq; 
	}
      else
	{
	  for(std::vector<int>::iterator idIt=listOfPixIds.begin();idIt!=listOfPixIds.end();++idIt)
	    {
	      TrigSiSpacePointContainer::const_iterator spCollIt=pCont->indexFind((*idIt));
	      if(spCollIt==pCont->end()) continue;
	      for(TrigSiSpacePointCollection::const_iterator spIt=(*spCollIt)->begin();
		  spIt!=(*spCollIt)->end();++spIt)
		{
		  spVec.push_back((*spIt));
		}
	    }      
	  if (outputLevel <= MSG::DEBUG) 
	    {    
	      for (unsigned int i=0; i<spVec.size(); i++) 
		{
		  athenaLog << MSG::DEBUG << "pix (r,phi,z): " << spVec[i]->r() << " / " 
			    << spVec[i]->phi() << " / "
			    << spVec[i]->z() << endreq; 
		}
	    }
	  if(outputLevel <= MSG::DEBUG) athenaLog << MSG::DEBUG << "REGTEST / Pixel : Formed " << spVec.size()
						  << "  SPs " << endreq;
	}
    }
  if(!getSctSP) 
    {
      return spVec;
    }
  spacePointVec spVecSCT;

  const TrigSiSpacePointContainer* pCont;
  sc=m_StoreGate->retrieve(pCont,m_sctSpContainerName);
  if(sc.isFailure())
    {
      athenaLog << MSG::WARNING << "Trig SCT Pixel container " <<m_sctSpContainerName
		<<" not found"<<endreq; 
    }
  else
    {
      for(std::vector<int>::iterator idIt=listOfSctIds.begin();idIt!=listOfSctIds.end();++idIt)
	{
	  TrigSiSpacePointContainer::const_iterator spCollIt=pCont->indexFind((*idIt));
	  if(spCollIt==pCont->end()) continue;
	  for(TrigSiSpacePointCollection::const_iterator spIt=(*spCollIt)->begin();
	      spIt!=(*spCollIt)->end();++spIt)
	    {
	      spVecSCT.push_back((*spIt));
	    }
	}      
      if (outputLevel <= MSG::DEBUG) 
	{    
	  for (unsigned int i=0; i<spVecSCT.size(); i++) {
	    athenaLog << MSG::DEBUG << "SCT (r,phi,z): " << spVecSCT[i]->r() << " / " 
		      << spVecSCT[i]->phi() << " / " 
		      << spVecSCT[i]->z() << endreq; 
	  }
	}    
      if (outputLevel <= MSG::DEBUG) athenaLog << MSG::DEBUG << 
				       "REGTEST / SCT : Formed " << spVecSCT.size() << 
				       "  SPs " << endreq;    
    }
  int nPixSP = spVec.size(); 
  spVec.resize(spVec.size()+spVecSCT.size());
  std::copy(spVecSCT.begin(), spVecSCT.end(), spVec.begin()+nPixSP);
  return spVec;
}


//-------------------------------------------------------------------------
TrigSiSpacePoint* LazyOfflineSpacePointTool::makeTrigPixSP( const Trk::SpacePoint* sp )  const
//-------------------------------------------------------------------------
{
  const Identifier elementId = m_pixelId->wafer_id( (sp->elementIdList()).first );
  bool isEndCap = m_pixelId->barrel_ec(elementId);

  double dr,df,dz;
  if ( isEndCap ){
    dz=0.01;
    dr=0.13;
  }else{
    dr=0.01;
    dz=0.13;
  }
  df=0.0001;
  
  long layerId = m_pixelId->layer_disk(elementId);
  if ( isEndCap ) { // endcap
    layerId += m_numberingTool->offsetEndcapPixels();
  } 

  return new TrigSiSpacePoint( dynamic_cast<const InDet::SiCluster* > ((sp->clusterList()).first), 
			       elementId, layerId,
			       (sp->globalPosition()).perp(), (sp->globalPosition()).phi(), (sp->globalPosition()).z(),
			       dr, df, dz );
}

//-------------------------------------------------------------------------

TrigSiSpacePoint* LazyOfflineSpacePointTool::makeTrigSctSP( const Trk::SpacePoint* sp )  const
//-------------------------------------------------------------------------
{
  const Identifier elementId = m_sctId->wafer_id( (sp->elementIdList()).first );
  bool isBarrel = m_sctId->is_barrel(elementId);

  double dr,df,dz;
  if ( isBarrel ){
    dr=0.01;
    dz=0.82;
  }else{
    dz=0.01;
    dr=0.82;
  }
  df=0.00005;
  
  long layerId = m_sctId->layer_disk(elementId);

  if (isBarrel){
    layerId += m_numberingTool->offsetBarrelSCT();
  }
  else{
    layerId += m_numberingTool->offsetEndcapSCT();
  }

  return new TrigSiSpacePoint( dynamic_cast<const InDet::SiCluster* > ((sp->clusterList()).first), 
			       dynamic_cast<const InDet::SiCluster* > ((sp->clusterList()).second),
			       elementId, layerId,
			       (sp->globalPosition()).perp(), (sp->globalPosition()).phi(), (sp->globalPosition()).z(),
			       dr, df, dz );
}



