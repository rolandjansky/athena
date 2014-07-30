/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// OnlineSpacePointProviderTool
// ( see header-file for details )
// -------------------------------
////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include <algorithm>

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "DataModel/DataVector.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "ByteStreamData/RawEvent.h"

// EDM & Identifier
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h" 
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
// SpacePoint creation
//#include "IRegionSelector/IRegSelSvc.h"
#include "TrigInDetEvent/TrigSiSpacePoint.h"
#include "TrigInDetEvent/TrigSiSpacePointCollection.h"
#include "TrigOnlineSpacePointTool/PixelSpacePointTool.h"
#include "TrigOnlineSpacePointTool/SCT_SpacePointTool.h"
#include "TrigOnlineSpacePointTool/IPixelClusterCacheTool.h"
#include "TrigOnlineSpacePointTool/ISCT_ClusterCacheTool.h"
#include "TrigOnlineSpacePointTool/OnlineSpacePointProviderTool.h"
//#include "GaudiKernel/IssueSeverity.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "IRegionSelector/IRoiDescriptor.h"

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreClearedIncident.h"


typedef OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment         ROBF ; 

OnlineSpacePointProviderTool::OnlineSpacePointProviderTool(const std::string& t, 
							   const std::string& n,
							   const IInterface*  p ): 
  AlgTool(t,n,p), 
  m_isFullScan(false),
  m_pixelCacheTool("PixelClusterCacheTool"), 
  m_sctCacheTool("SCT_ClusterCacheTool") 
										   
{
  declareInterface< ISpacePointProvider >( this );
  
  declareProperty( "ROBDataProviderSvcName", m_ROBDataProviderSvcName = "ROBDataProviderSvc" );
  declareProperty( "RegionSelectorToolName", m_regionSelectorToolName = "RegSelSvc" );
  declareProperty( "SctSpToolName", m_sctSpToolName = "SCT_SpacePointTool" );
  declareProperty( "PixelSpToolName", m_pixelSpToolName = "PixelSpacePointTool" );
  declareProperty( "SctClusterContainerName", m_sctClusterContainerName = "SCT_OnlineClusters" );
  declareProperty( "PixelClusterContainerName", m_pixelClusterContainerName = "PixelOnlineClusters" );
  declareProperty( "SctClusterCacheTool", m_sctCacheTool,"SCT_ClusterCacheTool" );
  declareProperty( "PixelClusterCacheTool", m_pixelCacheTool,"PixelClusterCacheTool" );
  declareProperty( "SctClusterCacheName", m_sctClusterCacheName = "SCT_ClusterCache" );
  declareProperty( "PixelClusterCacheName", m_pixelClusterCacheName = "PixelClusterCache" );

  declareProperty( "SctSpContainerName", m_sctSpContainerName = "TrigSCT_SpacePoints" );
  declareProperty( "PixelSpContainerName", m_pixelSpContainerName = "TrigPixelSpacePoints" );
  declareProperty( "UseStoreGate", m_useStoreGate = false );
  declareProperty( "zMin", m_minZ = -168.0 );
  declareProperty( "zMax", m_maxZ =  168.0 );
  declareProperty( "UseSctClusterThreshold", m_useSctClustThreshold = false );
  declareProperty( "SctClusterThreshold",    m_sctClustThreshold    = 30 );
  declareProperty( "UsePixelClusterThreshold", m_usePixelClustThreshold = false );
  declareProperty( "PixelClusterThreshold",    m_pixClustThreshold    = 30 );
  declareProperty( "DoBS_Conversion", m_doBS = false);
}

StatusCode OnlineSpacePointProviderTool::finalize()
{
  MsgStream athenaLog(msgSvc(), name());
  athenaLog << MSG::INFO << "Total " <<nColl[0]<<" PIX collections, "<<nColl[1]<<" are filled"<< endreq;
  athenaLog << MSG::INFO << "Total " <<nColl[2]<<" SCT collections, "<<nColl[3]<<" are filled"<< endreq;
  StatusCode sc = AlgTool::finalize(); 
  return sc;
}
 

StatusCode OnlineSpacePointProviderTool::initialize()
{
  
  StatusCode sc = AlgTool::initialize();
  MsgStream athenaLog(msgSvc(), name());
  athenaLog << MSG::DEBUG << "OnlineSpacePointProviderTool: in initialize" << endreq;

  athenaLog << MSG::INFO << "RegionSelectorToolName: " << m_regionSelectorToolName << endreq;
  athenaLog << MSG::INFO << "SctSpToolName: " <<  m_sctSpToolName << endreq;
  athenaLog << MSG::INFO << "PixelSpToolName: " << m_pixelSpToolName << endreq;
  if(m_useSctClustThreshold)
    athenaLog << MSG::INFO << "SctClusterThreshold= " << m_sctClustThreshold << endreq;
  if(m_usePixelClustThreshold)
    athenaLog << MSG::INFO << "PixelClusterThreshold= " << m_pixClustThreshold << endreq;
  if(m_useStoreGate)
    {
      athenaLog << MSG::INFO << "using proxy-based BS converters " << endreq;
      athenaLog << MSG::INFO << "SctClusterContainerName: " << m_sctClusterContainerName << endreq;
      athenaLog << MSG::INFO << "PixelClusterContainerName: " << m_pixelClusterContainerName << endreq;
    }
  else
    {
      athenaLog << MSG::INFO << "using own BS conversion/cluster caching tools: " << endreq;
      athenaLog << MSG::INFO << "SctCacheTool: " <<  m_sctCacheTool << endreq;
      athenaLog << MSG::INFO << "PixelCacheTool: " << m_pixelCacheTool << endreq;
      athenaLog << MSG::INFO << "SctClusterCacheName: " << m_sctClusterCacheName << endreq;
      athenaLog << MSG::INFO << "PixelClusterCacheName: " << m_pixelClusterCacheName << endreq;
    }

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

  // Get SCT & pixel Identifier helpers

  if (detStore->retrieve(m_pixelId, "PixelID").isFailure()) { 
     athenaLog << MSG::FATAL << "Could not get Pixel ID helper" << endreq;
     return StatusCode::FAILURE;
  }  

 if (detStore->retrieve(m_sctId, "SCT_ID").isFailure()) {
   athenaLog << MSG::FATAL << "Could not get SCT ID helper" << endreq;
   return StatusCode::FAILURE;  
 }
  
//   Get the region selector tool
//
  sc = serviceLocator()->service( m_regionSelectorToolName,m_regionSelector);
  if ( sc.isFailure() ) {
    athenaLog << MSG::FATAL 
	      << "Unable to retrieve RegionSelector Service  " << m_regionSelectorToolName << endreq;
    return sc;
  };

//   Get SPTools
//
  sc = toolSvc()->retrieveTool( m_pixelSpToolName, m_pixelSpTool, this );
  if ( sc.isFailure() ) {
    athenaLog << MSG::FATAL 
	      << "Unable to locate PixelSpacePointTool " << m_pixelSpToolName  << endreq;
    return sc;
  } 
  
  sc = toolSvc()->retrieveTool( m_sctSpToolName, m_sctSpTool, this );
  if ( sc.isFailure() ) {
    athenaLog << MSG::FATAL << "Unable to locate SCT_SpacePointTool " << m_sctSpToolName << endreq;
    return sc;
  } 
  if(!m_useStoreGate)
    {
      athenaLog << MSG::INFO<< "StoreGate will not be used " << endreq;
      sc = m_pixelCacheTool.retrieve();
      if ( sc.isFailure() ) {
	athenaLog << MSG::FATAL 
		  << "Unable to locate PixelClusterCacheTool " << m_pixelCacheTool  << endreq;
	return sc;
      }
      sc = m_sctCacheTool.retrieve();
      if ( sc.isFailure() ) {
	athenaLog << MSG::FATAL << "Unable to locate SCT_ClusterCacheTool " << m_sctCacheTool << endreq;
	return sc;
      } 
    }
  else athenaLog << MSG::INFO<< "using StoreGate ... " << endreq;

//  Get ROBDataProvider
  sc = service( m_ROBDataProviderSvcName, m_robDataProvider );
  if( sc.isFailure() ) {
    athenaLog << MSG::FATAL << " Can't get ROBDataProviderSvc " << endreq;
    return sc;
  }

  // Register incident handler
  ServiceHandle<IIncidentSvc> iincSvc( "IncidentSvc", name());
  sc = iincSvc.retrieve().isSuccess();
  if ( sc.isFailure() ) 
    {
      athenaLog << MSG::FATAL << "Unable to locate IncidentSvc " << endreq;
      return sc;
    }
  else
    {
      iincSvc->addListener( this, "StoreCleared" );
    }

  ITrigTimerSvc* timerSvc;
  StatusCode scTime = service( "TrigTimerSvc", timerSvc);
  if( scTime.isFailure() ) {
    athenaLog << MSG::INFO<< "Unable to locate Service TrigTimerSvc " << endreq;
    m_timers = false;
  } 
  else{
    m_timers = true;  
  }
  if ( m_timers ) {
    m_timer[0] = timerSvc->addItem("RegSel");
    m_timer[1] = timerSvc->addItem("RobProv");
    m_timer[2] = timerSvc->addItem("PixCont");
    m_timer[3] = timerSvc->addItem("SctCont");
    m_timer[4] = timerSvc->addItem("L2PixSP");
    m_timer[5] = timerSvc->addItem("L2SCTSP");
    m_timer[6] = timerSvc->addItem("L2PixClu");
    m_timer[6]->propName("L2PixClu.nPixCol");
    m_timer[7] = timerSvc->addItem("L2SCTClu");
    m_timer[7]->propName("L2SCTClu.nSctCol");
    m_timer[8] = timerSvc->addItem("L2Data");
  }
  for(int i=0;i<4;i++) nColl[i]=0;

  m_fsPixelDataReady=false;
  m_fsSCT_DataReady=false;
  return sc;
}


const std::vector<int>* OnlineSpacePointProviderTool::fillPixelDataErrors()
{
  return &m_pixelDataErrors;
}

const std::vector<int>* OnlineSpacePointProviderTool::fillSCT_DataErrors()
{
  return &m_sctDataErrors;
}

StatusCode OnlineSpacePointProviderTool::fillCollections(bool getPixelSP, bool getSCT_SP,
							 std::vector<int>& PixelIDs,
							 std::vector<int>& SCT_IDs)
{
  StatusCode sc(StatusCode::SUCCESS);

  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

  int pix_bs_errors=0;
  bool bs_failure_pix=false;
  int sct_bs_errors=0;
  bool bs_failure_sct=false;
  m_pixelDataErrors.clear();
  m_sctDataErrors.clear();

  bool usePixelRS=PixelIDs.empty();
  bool useSctRS=SCT_IDs.empty();
  
  m_bstoclustersPixelTime = 0.0;//6
  m_bstoclustersSCTTime = 0.0;//7
  m_spformationPixelTime = 0.0;//4
  m_spformationSCTTime = 0.0;//5
  m_l2DataTime = 0.0;//8
  m_regSelTime = 0.0;//0
  
  if(m_timers) m_timer[8]->start();

  if(getPixelSP) 
    {
     std::vector<unsigned int> uIntListOfPixelRobs;
      
     if(m_timers) m_timer[0]->start();
		 m_regionSelector->DetROBIDListUint(PIXEL, uIntListOfPixelRobs);

     if(m_timers) m_timer[0]->pause();
     if(m_timers) m_timer[1]->start();
     if(m_doBS) {
       m_robDataProvider->addROBData( uIntListOfPixelRobs );
     }
     if(m_timers) m_timer[1]->pause();
     if(m_timers) m_timer[0]->resume();
     std::vector<IdentifierHash> listOfPixIds;
     listOfPixIds.clear();
     if (usePixelRS)
       {
				 m_regionSelector->DetHashIDList( PIXEL, listOfPixIds);
       }
     else
       std::copy(PixelIDs.begin(),PixelIDs.end(),std::back_inserter(listOfPixIds));
     
     PixelIDs.clear();
     if(m_timers) m_timer[0]->pause();
     if (outputLevel <= MSG::DEBUG) 
       {
	 athenaLog << MSG::DEBUG << "REGTEST / Pixel : Roi contains " 
		   << listOfPixIds.size() << " det. Elements" << endreq;
	 athenaLog << MSG::DEBUG << "Pixel : " 
		   << uIntListOfPixelRobs.size() << " ROBs will be requested" << endreq;
       }
     std::vector<const InDet::PixelClusterCollection*> pixClusterColl;
     unsigned int nPixClusters=0; 

     if(!m_useStoreGate)
	{

	  std::vector<const ROBF*> v;

	  if(m_timers) m_timer[1]->resume();
	  if(m_doBS) {
	    m_robDataProvider->getROBData(uIntListOfPixelRobs,v);
	  }
	  if(m_timers) 
	    {
	      m_timer[1]->pause();
	      m_timer[6]->start();
	    }
	  StatusCode scPixClu=m_pixelCacheTool->m_convertBStoClusters(v,listOfPixIds,m_pixelDataErrors, m_isFullScan);

	  if(scPixClu.isRecoverable()) pix_bs_errors++;
	  else 
	    if(scPixClu.isFailure()) bs_failure_pix=true;
          
	  if(m_timers) 
	    {
	      m_timer[6]->pause();
	    }
	}

     const InDet::PixelClusterContainer* pixClusterContainer;

     if(m_timers) m_timer[2]->start();
     if(m_useStoreGate)
       {
	 sc = m_StoreGate->retrieve(pixClusterContainer, m_pixelClusterContainerName);
	 if(m_timers) m_timer[2]->stop();
	 if (sc.isFailure())
	   {
	     athenaLog<<MSG::ERROR<<"Retrieval of pixel Cluster Container " 
		      << m_pixelClusterContainerName << " failed" << endreq;
	     if(m_timers) m_timer[8]->stop();
	     return sc;
	   }
       }
     else 
       {
	 sc = m_StoreGate->retrieve(pixClusterContainer, m_pixelClusterCacheName);
	 if(m_timers) m_timer[2]->stop();
	 if (sc.isFailure())
	   {
	     athenaLog<<MSG::ERROR<<"Retrieval of pixel Cluster Cache " 
		      << m_pixelClusterCacheName << " failed" << endreq;
	     if(m_timers) m_timer[8]->stop();
	     return sc;
	   }
       }
     if(m_useStoreGate && m_timers) 
       {
	 m_timer[6]->start();
	 m_timer[6]->pause();
       }
     for(unsigned int iPix=0; iPix<listOfPixIds.size(); iPix++) 
       {
	 if(m_timers) m_timer[6]->resume();
	 nColl[0]++;
	 InDet::PixelClusterContainer::const_iterator 
	   clusterCollection = pixClusterContainer->indexFind(listOfPixIds[iPix]); 	  
	 if(m_timers) m_timer[6]->pause();
	  
	 if (clusterCollection==pixClusterContainer->end()) continue;
	 if ((*clusterCollection)->size() == 0) continue;

	 nColl[1]++;

	 if ((m_usePixelClustThreshold  && (*clusterCollection)->size()>m_pixClustThreshold ) ) 
	    { 
	      athenaLog << MSG::DEBUG 
			<<  "number of Pixel clusters, " << (*clusterCollection)->size() 
			<< " exceeded "<<m_pixClustThreshold<<" for id=" << listOfPixIds[iPix] 
			<< ", skipping this module" << endreq;
	      continue;
	    }

	 PixelIDs.push_back(listOfPixIds[iPix]);

	 nPixClusters += (*clusterCollection)->size();
	 if (outputLevel <= MSG::DEBUG) 
	    {
	      Identifier id = (*clusterCollection)->identify();
	      athenaLog << MSG::DEBUG <<  m_pixelId->print_to_string(id) << " with "
			<< (*clusterCollection)->size() << " clusters" 	  <<endreq;
	    }
	 pixClusterColl.push_back( (*clusterCollection) );	  
	
	 if (outputLevel <= MSG::VERBOSE) {
	   InDet::PixelClusterCollection::const_iterator pPixClus = (*clusterCollection)->begin();
	   
	   for (int iPixClus=1; pPixClus != (*clusterCollection)->end(); pPixClus++, iPixClus++) {
	     const Amg::Vector2D pos = (*pPixClus)->localPosition();
	     athenaLog << MSG::VERBOSE <<  " cluster " << iPixClus << ": " 
		       << m_sctId->print_to_string((*pPixClus)->identify()) 
		       << " locT " << pos[0] << " locL " << pos[1] << endreq;
	     
	   }
	 }
       }
   
     if(m_timers) 
       {
	 m_timer[6]->propVal(listOfPixIds.size());
	 m_timer[6]->stop();
	 m_bstoclustersPixelTime=m_timer[6]->elapsed();
       }
     if(m_timers) m_timer[4]->start();
     sc = m_pixelSpTool->fillCollections(pixClusterColl);
     if(m_timers) 
       {
	 m_timer[4]->stop();
	 m_spformationPixelTime=m_timer[4]->elapsed();
       }
     if(sc.isFailure())
       {
	 athenaLog << MSG::WARNING << "Pixel SP tool failed"<<endreq;
	 return sc;
       }
     if (outputLevel <= MSG::DEBUG) athenaLog << MSG::DEBUG << "REGTEST / Pixel : Formed SP from " << 
				      nPixClusters << "  clusters" << endreq;
    }
  if(getSCT_SP) 
    {
      std::vector<unsigned int> uIntListOfSctRobs;

      if(m_timers) m_timer[0]->resume();
			m_regionSelector->DetROBIDListUint( SCT, uIntListOfSctRobs);
      if(m_timers) m_timer[0]->pause();
      if(m_timers) m_timer[1]->resume();
      if(m_doBS) {
	m_robDataProvider->addROBData( uIntListOfSctRobs );
      }
      if(m_timers) 
	{
	  if(m_useStoreGate) m_timer[1]->stop();
	  else m_timer[1]->pause();
	}
      if(m_timers) m_timer[0]->resume();
      std::vector<IdentifierHash> listOfSctIds;
      listOfSctIds.clear();
      if (useSctRS)
	{
		m_regionSelector->DetHashIDList( SCT, listOfSctIds);
	}
      else
	std::copy(SCT_IDs.begin(),SCT_IDs.end(),std::back_inserter(listOfSctIds));
      SCT_IDs.clear();

      if(m_timers) 
	{
	  m_timer[0]->stop();
	  m_regSelTime=m_timer[0]->elapsed();
	}
      if (outputLevel <= MSG::DEBUG) 
	{
	  athenaLog << MSG::DEBUG << "REGTEST / SCT : Roi contains " 
		    << listOfSctIds.size() << " det. Elements" << endreq;
	  athenaLog << MSG::DEBUG << "SCT : " 
		    << uIntListOfSctRobs.size() << " ROBs will be requested" << endreq;
	}
      std::vector<const InDet::SCT_ClusterCollection*> sctClusterColl;
      unsigned int nSctClusters=0;

      if(!m_useStoreGate)
	{
	  std::vector<const ROBF*> v;
	  if(m_timers) m_timer[1]->resume();
	  if(m_doBS) {
	    m_robDataProvider->getROBData(uIntListOfSctRobs,v);
	  }
	  if(m_timers) 
	    {
	      m_timer[1]->stop();
	      m_timer[7]->start();
	    }
	  StatusCode scSctClu=m_sctCacheTool->m_convertBStoClusters(v,listOfSctIds,m_sctDataErrors, m_isFullScan);
	  if(scSctClu.isRecoverable()) sct_bs_errors++;
	  else 
	    if(scSctClu.isFailure()) bs_failure_sct=true;
	  
	  if(m_timers) 
	    {
	      m_timer[7]->pause();
	    }
	}

      const InDet::SCT_ClusterContainer* sctClusterContainer; 
      if(m_timers) m_timer[3]->start();

      if(m_useStoreGate)
	{
	  sc = m_StoreGate->retrieve(sctClusterContainer, m_sctClusterContainerName);
	  if(m_timers) m_timer[3]->stop();
	  if(sc.isFailure())
	    {
	      athenaLog<<MSG::ERROR<<" Retrieval of SCT Cluster Container " 
		   << m_sctClusterContainerName << " failed" << endreq;
	      if(m_timers) m_timer[8]->stop();
	      return sc;
	    }
	}
      else
	{
	  sc = m_StoreGate->retrieve(sctClusterContainer, m_sctClusterCacheName);
	  if(m_timers) m_timer[3]->stop();
	  if (sc.isFailure()){
	    athenaLog<<MSG::ERROR<<" Retrieval of SCT Cluster Cache " << m_sctClusterCacheName 
		     << " failed" << endreq;
	    if(m_timers) m_timer[8]->stop();
	    return sc;
	  }
	}
      if(m_timers && m_useStoreGate) 
	{
	  m_timer[7]->start();
	  m_timer[7]->pause();
	}
      for(unsigned int iSct=0; iSct<listOfSctIds.size(); iSct++) 
	{
	  //      if(m_timers) m_timer[7]->resume();
	  nColl[2]++;
	  InDet::SCT_ClusterContainer::const_iterator clusterCollection = sctClusterContainer->indexFind(listOfSctIds[iSct]); 
	  // if(m_timers) m_timer[7]->pause();
      
	  if (clusterCollection==sctClusterContainer->end()) continue;
	  
	  if ((*clusterCollection)->size() == 0) continue;
	  nColl[3]++;
	  if ((m_useSctClustThreshold  && (*clusterCollection)->size()>m_sctClustThreshold ) ) 
	    { 
	      athenaLog << MSG::DEBUG 
			<<  "number of SCT clusters, " << (*clusterCollection)->size() 
			<< " exceeded "<<m_sctClustThreshold<<" for id=" << listOfSctIds[iSct] 
			<< ", skipping this module" << endreq;
	      continue;
	    }
	  
	  nSctClusters += (*clusterCollection)->size();
	 
	  sctClusterColl.push_back( (*clusterCollection) );	  
	  
	  if (outputLevel <= MSG::VERBOSE) 
	    {
	      Identifier id = (*clusterCollection)->identify();
	      athenaLog << MSG::VERBOSE <<  m_sctId->print_to_string(id) << " with "
			<< (*clusterCollection)->size() << " clusters" 	  <<endreq;
	      InDet::SCT_ClusterCollection::const_iterator pSctClus = (*clusterCollection)->begin();
	      for (int iSctClus=1; pSctClus != (*clusterCollection)->end(); pSctClus++, iSctClus++) 
		{
		  const Amg::Vector2D SCTpos = (*pSctClus)->localPosition();
		  athenaLog << MSG::VERBOSE <<  " cluster " << iSctClus << ": " << 
		    m_sctId->print_to_string((*pSctClus)->identify()) 
			    << " locT " << SCTpos[0] << " locL " << SCTpos[1] << endreq;
		}
	    }
	}
      if(m_timers) 
	{
	  m_timer[7]->propVal(listOfSctIds.size());
	  m_timer[7]->stop();
	  m_bstoclustersSCTTime=m_timer[7]->elapsed();
	}
      if(m_timers) m_timer[5]->start();
      sc = m_sctSpTool->fillCollections(sctClusterColl,SCT_IDs);

      if(sc.isFailure())
       {
	 athenaLog << MSG::WARNING << "SCT SP tool failed"<<endreq;
	 return sc;
       }
      if(m_timers) 
	{
	  m_timer[5]->stop();
	  m_spformationSCTTime = m_timer[5]->elapsed();
	}
      if (outputLevel <= MSG::DEBUG) athenaLog << MSG::DEBUG << 
				       "REGTEST / SCT : Formed SPs from " << nSctClusters 
					       <<"  clusters"<< endreq;    
    }
  if(m_timers) 
    {
      m_timer[8]->stop();
      m_l2DataTime = m_timer[8]->elapsed();
    }
  
  if((!bs_failure_pix) && (pix_bs_errors==0) && (!bs_failure_sct) && (sct_bs_errors==0)) return sc;
  if((bs_failure_pix) || (bs_failure_sct)) return StatusCode::FAILURE;
  if((sct_bs_errors!=0) || (pix_bs_errors!=0)) 
    {
      return StatusCode::RECOVERABLE;
    }
  return sc;
}

StatusCode OnlineSpacePointProviderTool::fillCollections(const IRoiDescriptor& roi, bool getPixelSP, bool getSCT_SP,
							 std::vector<int>& PixelIDs,
							 std::vector<int>& SCT_IDs)
{
  StatusCode sc(StatusCode::SUCCESS);

  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

  int pix_bs_errors=0;
  bool bs_failure_pix=false;
  int sct_bs_errors=0;
  bool bs_failure_sct=false;
  m_pixelDataErrors.clear();
  m_sctDataErrors.clear();

  bool usePixelRS=PixelIDs.empty();
  bool useSctRS=SCT_IDs.empty();
  
  m_bstoclustersPixelTime = 0.0;//6
  m_bstoclustersSCTTime = 0.0;//7
  m_spformationPixelTime = 0.0;//4
  m_spformationSCTTime = 0.0;//5
  m_l2DataTime = 0.0;//8
  m_regSelTime = 0.0;//0
  
  if(m_timers) m_timer[8]->start();

  if(getPixelSP) 
    {
     std::vector<unsigned int> uIntListOfPixelRobs;
      
     if(m_timers) m_timer[0]->start();
		 m_regionSelector->DetROBIDListUint(PIXEL, roi, uIntListOfPixelRobs);

     if(m_timers) m_timer[0]->pause();
     if(m_timers) m_timer[1]->start();
     if(m_doBS) {
       m_robDataProvider->addROBData( uIntListOfPixelRobs );
     }
     if(m_timers) m_timer[1]->pause();
     if(m_timers) m_timer[0]->resume();
     std::vector<IdentifierHash> listOfPixIds;
     listOfPixIds.clear();
     if (usePixelRS)
       {
				 m_regionSelector->DetHashIDList(PIXEL, roi, listOfPixIds);
       }
     else
       std::copy(PixelIDs.begin(),PixelIDs.end(),std::back_inserter(listOfPixIds));
     
     PixelIDs.clear();
     if(m_timers) m_timer[0]->pause();
     if (outputLevel <= MSG::DEBUG) 
       {
	 athenaLog << MSG::DEBUG << "REGTEST / Pixel : Roi contains " 
		   << listOfPixIds.size() << " det. Elements" << endreq;
	 athenaLog << MSG::DEBUG << "Pixel : " 
		   << uIntListOfPixelRobs.size() << " ROBs will be requested" << endreq;
       }
     std::vector<const InDet::PixelClusterCollection*> pixClusterColl;
     unsigned int nPixClusters=0; 

     if(!m_useStoreGate)
	{

	  std::vector<const ROBF*> v;

	  if(m_timers) m_timer[1]->resume();
	  if(m_doBS) {
	    m_robDataProvider->getROBData(uIntListOfPixelRobs,v);
	  }
	  if(m_timers) 
	    {
	      m_timer[1]->pause();
	      m_timer[6]->start();
	    }
	  StatusCode scPixClu=m_pixelCacheTool->m_convertBStoClusters(v,listOfPixIds,m_pixelDataErrors, m_isFullScan);

	  if(scPixClu.isRecoverable()) pix_bs_errors++;
	  else 
	    if(scPixClu.isFailure()) bs_failure_pix=true;
          
	  if(m_timers) 
	    {
	      m_timer[6]->pause();
	    }
	}

     const InDet::PixelClusterContainer* pixClusterContainer;

     if(m_timers) m_timer[2]->start();
     if(m_useStoreGate)
       {
	 sc = m_StoreGate->retrieve(pixClusterContainer, m_pixelClusterContainerName);
	 if(m_timers) m_timer[2]->stop();
	 if (sc.isFailure())
	   {
	     athenaLog<<MSG::ERROR<<"Retrieval of pixel Cluster Container " 
		      << m_pixelClusterContainerName << " failed" << endreq;
	     if(m_timers) m_timer[8]->stop();
	     return sc;
	   }
       }
     else 
       {
	 sc = m_StoreGate->retrieve(pixClusterContainer, m_pixelClusterCacheName);
	 if(m_timers) m_timer[2]->stop();
	 if (sc.isFailure())
	   {
	     athenaLog<<MSG::ERROR<<"Retrieval of pixel Cluster Cache " 
		      << m_pixelClusterCacheName << " failed" << endreq;
	     if(m_timers) m_timer[8]->stop();
	     return sc;
	   }
       }
     if(m_useStoreGate && m_timers) 
       {
	 m_timer[6]->start();
	 m_timer[6]->pause();
       }
     for(unsigned int iPix=0; iPix<listOfPixIds.size(); iPix++) 
       {
	 if(m_timers) m_timer[6]->resume();
	 nColl[0]++;
	 InDet::PixelClusterContainer::const_iterator 
	   clusterCollection = pixClusterContainer->indexFind(listOfPixIds[iPix]); 	  
	 if(m_timers) m_timer[6]->pause();
	  
	 if (clusterCollection==pixClusterContainer->end()) continue;
	 if ((*clusterCollection)->size() == 0) continue;

	 nColl[1]++;

	 if ((m_usePixelClustThreshold  && (*clusterCollection)->size()>m_pixClustThreshold ) ) 
	    { 
	      athenaLog << MSG::DEBUG 
			<<  "number of Pixel clusters, " << (*clusterCollection)->size() 
			<< " exceeded "<<m_pixClustThreshold<<" for id=" << listOfPixIds[iPix] 
			<< ", skipping this module" << endreq;
	      continue;
	    }

	 PixelIDs.push_back(listOfPixIds[iPix]);

	 nPixClusters += (*clusterCollection)->size();
	 if (outputLevel <= MSG::DEBUG) 
	    {
	      Identifier id = (*clusterCollection)->identify();
	      athenaLog << MSG::DEBUG <<  m_pixelId->print_to_string(id) << " with "
			<< (*clusterCollection)->size() << " clusters" 	  <<endreq;
	    }
	 pixClusterColl.push_back( (*clusterCollection) );	  
	
	 if (outputLevel <= MSG::VERBOSE) {
	   InDet::PixelClusterCollection::const_iterator pPixClus = (*clusterCollection)->begin();
	   
	   for (int iPixClus=1; pPixClus != (*clusterCollection)->end(); pPixClus++, iPixClus++) {
	     const Amg::Vector2D pos = (*pPixClus)->localPosition();
	     athenaLog << MSG::VERBOSE <<  " cluster " << iPixClus << ": " 
		       << m_sctId->print_to_string((*pPixClus)->identify()) 
		       << " locT " << pos[0] << " locL " << pos[1] << endreq;
	     
	   }
	 }
       }
   
     if(m_timers) 
       {
	 m_timer[6]->propVal(listOfPixIds.size());
	 m_timer[6]->stop();
	 m_bstoclustersPixelTime=m_timer[6]->elapsed();
       }
     if(m_timers) m_timer[4]->start();
     sc = m_pixelSpTool->fillCollections(pixClusterColl);
     if(m_timers) 
       {
	 m_timer[4]->stop();
	 m_spformationPixelTime=m_timer[4]->elapsed();
       }
     if(sc.isFailure())
       {
	 athenaLog << MSG::WARNING << "Pixel SP tool failed"<<endreq;
	 return sc;
       }
     if (outputLevel <= MSG::DEBUG) athenaLog << MSG::DEBUG << "REGTEST / Pixel : Formed SP from " << 
				      nPixClusters << "  clusters" << endreq;
    }
  if(getSCT_SP) 
    {
      std::vector<unsigned int> uIntListOfSctRobs;

      if(m_timers) m_timer[0]->resume();
			m_regionSelector->DetROBIDListUint( SCT, roi, uIntListOfSctRobs);
      if(m_timers) m_timer[0]->pause();
      if(m_timers) m_timer[1]->resume();
      if(m_doBS) {
	m_robDataProvider->addROBData( uIntListOfSctRobs );
      }
      if(m_timers) 
	{
	  if(m_useStoreGate) m_timer[1]->stop();
	  else m_timer[1]->pause();
	}
      if(m_timers) m_timer[0]->resume();
      std::vector<IdentifierHash> listOfSctIds;
      listOfSctIds.clear();
      if (useSctRS)
	{
		m_regionSelector->DetHashIDList( SCT, roi, listOfSctIds);
	}
      else
	std::copy(SCT_IDs.begin(),SCT_IDs.end(),std::back_inserter(listOfSctIds));
      SCT_IDs.clear();

      if(m_timers) 
	{
	  m_timer[0]->stop();
	  m_regSelTime=m_timer[0]->elapsed();
	}
      if (outputLevel <= MSG::DEBUG) 
	{
	  athenaLog << MSG::DEBUG << "REGTEST / SCT : Roi contains " 
		    << listOfSctIds.size() << " det. Elements" << endreq;
	  athenaLog << MSG::DEBUG << "SCT : " 
		    << uIntListOfSctRobs.size() << " ROBs will be requested" << endreq;
	}
      std::vector<const InDet::SCT_ClusterCollection*> sctClusterColl;
      unsigned int nSctClusters=0;

      if(!m_useStoreGate)
	{
	  std::vector<const ROBF*> v;
	  if(m_timers) m_timer[1]->resume();
	  if(m_doBS) {
	    m_robDataProvider->getROBData(uIntListOfSctRobs,v);
	  }
	  if(m_timers) 
	    {
	      m_timer[1]->stop();
	      m_timer[7]->start();
	    }
	  StatusCode scSctClu=m_sctCacheTool->m_convertBStoClusters(v,listOfSctIds,m_sctDataErrors, m_isFullScan);
	  if(scSctClu.isRecoverable()) sct_bs_errors++;
	  else 
	    if(scSctClu.isFailure()) bs_failure_sct=true;
	  
	  if(m_timers) 
	    {
	      m_timer[7]->pause();
	    }
	}

      const InDet::SCT_ClusterContainer* sctClusterContainer; 
      if(m_timers) m_timer[3]->start();

      if(m_useStoreGate)
	{
	  sc = m_StoreGate->retrieve(sctClusterContainer, m_sctClusterContainerName);
	  if(m_timers) m_timer[3]->stop();
	  if(sc.isFailure())
	    {
	      athenaLog<<MSG::ERROR<<" Retrieval of SCT Cluster Container " 
		   << m_sctClusterContainerName << " failed" << endreq;
	      if(m_timers) m_timer[8]->stop();
	      return sc;
	    }
	}
      else
	{
	  sc = m_StoreGate->retrieve(sctClusterContainer, m_sctClusterCacheName);
	  if(m_timers) m_timer[3]->stop();
	  if (sc.isFailure()){
	    athenaLog<<MSG::ERROR<<" Retrieval of SCT Cluster Cache " << m_sctClusterCacheName 
		     << " failed" << endreq;
	    if(m_timers) m_timer[8]->stop();
	    return sc;
	  }
	}
      if(m_timers && m_useStoreGate) 
	{
	  m_timer[7]->start();
	  m_timer[7]->pause();
	}
      for(unsigned int iSct=0; iSct<listOfSctIds.size(); iSct++) 
	{
	  //      if(m_timers) m_timer[7]->resume();
	  nColl[2]++;
	  InDet::SCT_ClusterContainer::const_iterator clusterCollection = sctClusterContainer->indexFind(listOfSctIds[iSct]); 
	  // if(m_timers) m_timer[7]->pause();
      
	  if (clusterCollection==sctClusterContainer->end()) continue;
	  
	  if ((*clusterCollection)->size() == 0) continue;
	  nColl[3]++;
	  if ((m_useSctClustThreshold  && (*clusterCollection)->size()>m_sctClustThreshold ) ) 
	    { 
	      athenaLog << MSG::DEBUG 
			<<  "number of SCT clusters, " << (*clusterCollection)->size() 
			<< " exceeded "<<m_sctClustThreshold<<" for id=" << listOfSctIds[iSct] 
			<< ", skipping this module" << endreq;
	      continue;
	    }
	  
	  nSctClusters += (*clusterCollection)->size();
	 
	  sctClusterColl.push_back( (*clusterCollection) );	  
	  
	  if (outputLevel <= MSG::VERBOSE) 
	    {
	      Identifier id = (*clusterCollection)->identify();
	      athenaLog << MSG::VERBOSE <<  m_sctId->print_to_string(id) << " with "
			<< (*clusterCollection)->size() << " clusters" 	  <<endreq;
	      InDet::SCT_ClusterCollection::const_iterator pSctClus = (*clusterCollection)->begin();
	      for (int iSctClus=1; pSctClus != (*clusterCollection)->end(); pSctClus++, iSctClus++) 
		{
		  const Amg::Vector2D SCTpos = (*pSctClus)->localPosition();
		  athenaLog << MSG::VERBOSE <<  " cluster " << iSctClus << ": " << 
		    m_sctId->print_to_string((*pSctClus)->identify()) 
			    << " locT " << SCTpos[0] << " locL " << SCTpos[1] << endreq;
		}
	    }
	}
      if(m_timers) 
	{
	  m_timer[7]->propVal(listOfSctIds.size());
	  m_timer[7]->stop();
	  m_bstoclustersSCTTime=m_timer[7]->elapsed();
	}
      if(m_timers) m_timer[5]->start();
      sc = m_sctSpTool->fillCollections(sctClusterColl,SCT_IDs);

      if(sc.isFailure())
       {
	 athenaLog << MSG::WARNING << "SCT SP tool failed"<<endreq;
	 return sc;
       }
      if(m_timers) 
	{
	  m_timer[5]->stop();
	  m_spformationSCTTime = m_timer[5]->elapsed();
	}
      if (outputLevel <= MSG::DEBUG) athenaLog << MSG::DEBUG << 
				       "REGTEST / SCT : Formed SPs from " << nSctClusters 
					       <<"  clusters"<< endreq;    
    }
  if(m_timers) 
    {
      m_timer[8]->stop();
      m_l2DataTime = m_timer[8]->elapsed();
    }
  
  if((!bs_failure_pix) && (pix_bs_errors==0) && (!bs_failure_sct) && (sct_bs_errors==0)) return sc;
  if((bs_failure_pix) || (bs_failure_sct)) return StatusCode::FAILURE;
  if((sct_bs_errors!=0) || (pix_bs_errors!=0)) 
    {
      return StatusCode::RECOVERABLE;
    }
  return sc;
}



/*
  StatusCode sc;

  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );
  
  if(m_timers) m_timer[8]->start();

  PixelIDs.clear();SCT_IDs.clear();
  if(getPixelSP) 
    {
     std::vector<const InDet::PixelClusterCollection*> pixClusterColl;
     unsigned int nPixClusters=0; 
     const InDet::PixelClusterContainer* pixClusterContainer;

     if(m_timers) m_timer[2]->start();
     sc = m_StoreGate->retrieve(pixClusterContainer, m_pixelClusterContainerName);
     if(m_timers) m_timer[2]->stop();
     if (sc.isFailure())
       {
	 athenaLog<<MSG::ERROR<<"Retrieval of pixel Cluster Container " 
		  << m_pixelClusterContainerName << " failed" << endreq;
	 if(m_timers) m_timer[8]->stop();
	 return sc;
       }
     if(m_timers) 
       {
	 m_timer[6]->start();
	 m_timer[6]->pause();
       }
     for(unsigned int pixId=0; pixId < m_pixelId->wafer_hash_max();pixId++) 
       {
	 nColl[0]++;
	 if(m_timers) m_timer[6]->resume();
	 InDet::PixelClusterContainer::const_iterator clusterCollection = pixClusterContainer->indexFind(pixId);
	 if(m_timers) m_timer[6]->pause();
	 if(clusterCollection==pixClusterContainer->end()) continue;
	 if ((*clusterCollection)->size() == 0) continue;
	 nColl[1]++;
	 PixelIDs.push_back(pixId);
	 nPixClusters +=(*clusterCollection)->size();
	 if(outputLevel <= MSG::DEBUG) 
	   {
	     Identifier id = (*clusterCollection)->identify();
	     athenaLog << MSG::DEBUG <<  m_pixelId->print_to_string(id) << " with "
		       << (*clusterCollection)->size() << " clusters" 	  <<endreq;
	   }
	 pixClusterColl.push_back((*clusterCollection));	  
	
	 if (outputLevel <= MSG::DEBUG) {
	   InDet::PixelClusterCollection::const_iterator pPixClus = (*clusterCollection)->begin();
	   
	   for (int iPixClus=1; pPixClus != (*clusterCollection)->end(); pPixClus++, iPixClus++) {
	     const Trk::LocalPosition pos = (*pPixClus)->localPosition();
	     athenaLog << MSG::DEBUG <<  " cluster " << iPixClus << ": " 
		       << m_sctId->print_to_string((*pPixClus)->identify()) 
		       << " locT " << pos[0] << " locL " << pos[1] << endreq;
	     
	   }
	 }
       }
     if(m_timers) 
       {
	 m_timer[6]->propVal(PixelIDs.size());
	 m_timer[6]->stop();
       }
     if(m_timers) m_timer[4]->start();
     sc = m_pixelSpTool->fillCollections(pixClusterColl);
     if(m_timers) m_timer[4]->stop();
     if(sc.isFailure())
       {
	 athenaLog << MSG::WARNING << "Pixel SP tool failed"<<endreq;
	 return sc;
       }
     if (outputLevel <= MSG::DEBUG) athenaLog << MSG::DEBUG << "REGTEST / Pixel : Formed SP from " << 
				      nPixClusters << "  clusters" << endreq;
    }
  if(!getSCT_SP) 
    {
      if(m_timers) m_timer[8]->stop();
      return sc;
    }
  
  std::vector<const InDet::SCT_ClusterCollection*> sctClusterColl;
  unsigned int nSctClusters=0;
  const InDet::SCT_ClusterContainer* sctClusterContainer; 
  if(m_timers) m_timer[3]->start();
  sc = m_StoreGate->retrieve(sctClusterContainer, m_sctClusterContainerName);
  if(m_timers) m_timer[3]->stop();
  if(sc.isFailure())
    {
      athenaLog<<MSG::ERROR<<" Retrieval of SCT Cluster Container " 
	       << m_sctClusterContainerName << " failed" << endreq;
      if(m_timers) m_timer[8]->stop();
      return sc;
    }
  if(m_timers) 
    {
      m_timer[7]->start();
      m_timer[7]->pause();
    }
  for(unsigned int sctId=0;sctId<m_sctId->wafer_hash_max();sctId++) 
    {
      nColl[2]++;
      if(m_timers) m_timer[7]->resume();
      InDet::SCT_ClusterContainer::const_iterator clusterCollection = sctClusterContainer->indexFind(sctId);
      if(m_timers) m_timer[7]->pause();
      if(clusterCollection==sctClusterContainer->end()) continue;
      if ((*clusterCollection)->size() == 0) continue;
      nColl[3]++;
      SCT_IDs.push_back(sctId);
      nSctClusters += (*clusterCollection)->size();
      sctClusterColl.push_back(*clusterCollection);	  
      if(outputLevel <= MSG::DEBUG) 
	{
	  Identifier id = (*clusterCollection)->identify();
	  athenaLog << MSG::DEBUG <<  m_sctId->print_to_string(id) << " with "
		    << (*clusterCollection)->size() << " clusters" 	  <<endreq;
	  InDet::SCT_ClusterCollection::const_iterator pSctClus = (*clusterCollection)->begin();
	  for (int iSctClus=1; pSctClus != (*clusterCollection)->end(); pSctClus++, iSctClus++) 
	    {
	      const Trk::LocalPosition SCTpos = (*pSctClus)->localPosition();
	      athenaLog << MSG::DEBUG <<  " cluster " << iSctClus << ": " << 
		m_sctId->print_to_string((*pSctClus)->identify()) 
			<< " locT " << SCTpos[0] << " locL " << SCTpos[1] << endreq;
	    }
	}
    }
  if(m_timers) 
    {
      m_timer[7]->propVal(SCT_IDs.size());
      m_timer[7]->stop();
    }
  if(m_timers) m_timer[5]->start();
  sc = m_sctSpTool->fillCollections(sctClusterColl);
  if(m_timers) m_timer[5]->stop();
  if (outputLevel <= MSG::DEBUG) athenaLog << MSG::DEBUG << 
				   "REGTEST / SCT : Formed SPs from " << nSctClusters 
					   <<"  clusters"<< endreq;    
  if(m_timers) m_timer[8]->stop();
  return sc;
  
}   
*/


spacePointVec OnlineSpacePointProviderTool::getSpacePoints(bool getPixelSP, bool getSctSP)
{
  MsgStream athenaLog(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

  spacePointVec spVec;
  std::vector<int> listOfPixIds;
  std::vector<int> listOfSctIds;

  StatusCode sc=fillCollections(getPixelSP,getSctSP,listOfPixIds,listOfSctIds);

  if(!(sc.isSuccess()||sc.isRecoverable()))
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
	  if (outputLevel <= MSG::VERBOSE) 
	    {    
	      for (unsigned int i=0; i<spVec.size(); i++) 
		{
		  athenaLog << MSG::VERBOSE << "pix (r,phi,z): " << spVec[i]->r() << " / " 
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
      if (outputLevel <= MSG::VERBOSE) 
	{    
	  for (unsigned int i=0; i<spVecSCT.size(); i++) {
	    athenaLog << MSG::VERBOSE << "SCT (r,phi,z): " << spVecSCT[i]->r() << " / " 
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


void OnlineSpacePointProviderTool::handle(const Incident& incident)
{
  if ( incident.type() == "StoreCleared" )
  {
    m_fsPixelDataReady=false;
    m_fsSCT_DataReady=false;
    //    std::cout<<"Got StoreCleared incident"<<std::endl;
  }
}

//-------------------------------------------------------------------------



