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

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
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
#include "IRegionSelector/IRegSelSvc.h"
#include "TrigInDetEvent/TrigSiSpacePoint.h"
#include "TrigInDetEvent/TrigSiSpacePointCollection.h"
#include "TrigOnlineSpacePointTool/PixelSpacePointTool.h"
#include "TrigOnlineSpacePointTool/SCT_SpacePointTool.h"
#include "TrigOnlineSpacePointTool/IPixelClusterCacheTool.h"
#include "TrigOnlineSpacePointTool/ISCT_ClusterCacheTool.h"
#include "TrigOnlineSpacePointTool/OnlineSpacePointProviderTool.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "IRegionSelector/IRoiDescriptor.h"

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ServiceHandle.h"


typedef OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment         ROBF ; 

OnlineSpacePointProviderTool::OnlineSpacePointProviderTool(const std::string& t, 
							   const std::string& n,
							   const IInterface*  p ): 
  AthAlgTool(t,n,p), 
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
  declareProperty( "UseSctClusterThreshold", m_useSctClustThreshold = false );
  declareProperty( "SctClusterThreshold",    m_sctClustThreshold    = 30 );
  declareProperty( "UsePixelClusterThreshold", m_usePixelClustThreshold = false );
  declareProperty( "PixelClusterThreshold",    m_pixClustThreshold    = 30 );
  declareProperty( "DoBS_Conversion", m_doBS = false);
}

StatusCode OnlineSpacePointProviderTool::finalize()
{
  ATH_MSG_INFO("Total " <<m_nColl[0]<<" PIX collections, "<<m_nColl[1]<<" are filled");
  ATH_MSG_INFO("Total " <<m_nColl[2]<<" SCT collections, "<<m_nColl[3]<<" are filled");
  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}
 

StatusCode OnlineSpacePointProviderTool::initialize()
{
  
  StatusCode sc = AthAlgTool::initialize();
  ATH_MSG_DEBUG("OnlineSpacePointProviderTool: in initialize");

  ATH_MSG_INFO("RegionSelectorToolName: " << m_regionSelectorToolName);
  ATH_MSG_INFO("SctSpToolName: " <<  m_sctSpToolName);
  ATH_MSG_INFO("PixelSpToolName: " << m_pixelSpToolName);
  if(m_useSctClustThreshold)
    ATH_MSG_INFO("SctClusterThreshold= " << m_sctClustThreshold);
  if(m_usePixelClustThreshold)
    ATH_MSG_INFO("PixelClusterThreshold= " << m_pixClustThreshold);
  if(m_useStoreGate)
    {
      ATH_MSG_INFO("using proxy-based BS converters ");
      ATH_MSG_INFO("SctClusterContainerName: " << m_sctClusterContainerName);
      ATH_MSG_INFO("PixelClusterContainerName: " << m_pixelClusterContainerName);
    }
  else
    {
      ATH_MSG_INFO("using own BS conversion/cluster caching tools: ");
      ATH_MSG_INFO("SctCacheTool: " <<  m_sctCacheTool);
      ATH_MSG_INFO("PixelCacheTool: " << m_pixelCacheTool);
      ATH_MSG_INFO("SctClusterCacheName: " << m_sctClusterCacheName);
      ATH_MSG_INFO("PixelClusterCacheName: " << m_pixelClusterCacheName);
    }


  // Get SCT & pixel Identifier helpers

  if (detStore()->retrieve(m_pixelId, "PixelID").isFailure()) { 
     ATH_MSG_FATAL("Could not get Pixel ID helper");
     return StatusCode::FAILURE;
  }  

 if (detStore()->retrieve(m_sctId, "SCT_ID").isFailure()) {
   ATH_MSG_FATAL("Could not get SCT ID helper");
   return StatusCode::FAILURE;  
 }
  
//   Get the region selector tool
//
  sc = serviceLocator()->service( m_regionSelectorToolName,m_regionSelector);
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL("Unable to retrieve RegionSelector Service  " << m_regionSelectorToolName);
    return sc;
  };

//   Get SPTools
//
  sc = toolSvc()->retrieveTool( m_pixelSpToolName, m_pixelSpTool, this );
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL("Unable to locate PixelSpacePointTool " << m_pixelSpToolName );
    return sc;
  } 
  
  sc = toolSvc()->retrieveTool( m_sctSpToolName, m_sctSpTool, this );
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL("Unable to locate SCT_SpacePointTool " << m_sctSpToolName);
    return sc;
  } 
  if(!m_useStoreGate)
    {
      ATH_MSG_INFO("StoreGate will not be used ");
      sc = m_pixelCacheTool.retrieve();
      if ( sc.isFailure() ) {
	ATH_MSG_FATAL("Unable to locate PixelClusterCacheTool " << m_pixelCacheTool );
	return sc;
      }
      sc = m_sctCacheTool.retrieve();
      if ( sc.isFailure() ) {
	ATH_MSG_FATAL("Unable to locate SCT_ClusterCacheTool " << m_sctCacheTool);
	return sc;
      } 
    }
  else ATH_MSG_INFO("using StoreGate ... ");

//  Get ROBDataProvider
  sc = service( m_ROBDataProviderSvcName, m_robDataProvider );
  if( sc.isFailure() ) {
    ATH_MSG_FATAL(" Can't get ROBDataProviderSvc ");
    return sc;
  }

  // Register incident handler
  ServiceHandle<IIncidentSvc> iincSvc( "IncidentSvc", name());
  sc = iincSvc.retrieve().isSuccess();
  if ( sc.isFailure() ) 
    {
      ATH_MSG_FATAL("Unable to locate IncidentSvc ");
      return sc;
    }
  else
    {
      iincSvc->addListener( this, "StoreCleared" );
    }

  ITrigTimerSvc* timerSvc;
  StatusCode scTime = service( "TrigTimerSvc", timerSvc);
  if( scTime.isFailure() ) {
    ATH_MSG_INFO("Unable to locate Service TrigTimerSvc ");
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
  for(int i=0;i<4;i++) m_nColl[i]=0;

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
     ATH_MSG_DEBUG("REGTEST / Pixel : Roi contains " << listOfPixIds.size() << " det. Elements");
     ATH_MSG_DEBUG("Pixel : " << uIntListOfPixelRobs.size() << " ROBs will be requested");
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
	  StatusCode scPixClu=m_pixelCacheTool->convertBStoClusters(v,listOfPixIds,m_pixelDataErrors, m_isFullScan);

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
	 sc = evtStore()->retrieve(pixClusterContainer, m_pixelClusterContainerName);
	 if(m_timers) m_timer[2]->stop();
	 if (sc.isFailure())
	   {
	     ATH_MSG_ERROR("Retrieval of pixel Cluster Container " 
		      << m_pixelClusterContainerName << " failed");
	     if(m_timers) m_timer[8]->stop();
	     return sc;
	   }
       }
     else 
       {
	 sc = evtStore()->retrieve(pixClusterContainer, m_pixelClusterCacheName);
	 if(m_timers) m_timer[2]->stop();
	 if (sc.isFailure())
	   {
	     ATH_MSG_ERROR("Retrieval of pixel Cluster Cache " 
		      << m_pixelClusterCacheName << " failed");
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
	 m_nColl[0]++;
	 InDet::PixelClusterContainer::const_iterator 
	   clusterCollection = pixClusterContainer->indexFind(listOfPixIds[iPix]); 	  
	 if(m_timers) m_timer[6]->pause();
	  
	 if (clusterCollection==pixClusterContainer->end()) continue;
	 if ((*clusterCollection)->size() == 0) continue;

	 m_nColl[1]++;

	 if ((m_usePixelClustThreshold  && (*clusterCollection)->size()>m_pixClustThreshold ) ) 
	    { 
	      ATH_MSG_DEBUG("number of Pixel clusters, " << (*clusterCollection)->size() 
			<< " exceeded "<<m_pixClustThreshold<<" for id=" << listOfPixIds[iPix] 
			<< ", skipping this module");
	      continue;
	    }

	 PixelIDs.push_back(listOfPixIds[iPix]);

	 nPixClusters += (*clusterCollection)->size();
	 if (msgLvl(MSG::DEBUG)) 
	    {
	      Identifier id = (*clusterCollection)->identify();
	      ATH_MSG_DEBUG(m_pixelId->print_to_string(id) << " with "
			<< (*clusterCollection)->size() << " clusters");
	    }
	 pixClusterColl.push_back( (*clusterCollection) );	  
	
	 if (msgLvl(MSG::VERBOSE)) {
	   InDet::PixelClusterCollection::const_iterator pPixClus = (*clusterCollection)->begin();
	   
	   for (int iPixClus=1; pPixClus != (*clusterCollection)->end(); pPixClus++, iPixClus++) {
	     const Amg::Vector2D pos = (*pPixClus)->localPosition();
	     ATH_MSG_VERBOSE(" cluster " << iPixClus << ": " 
		       << m_sctId->print_to_string((*pPixClus)->identify()) 
		       << " locT " << pos[0] << " locL " << pos[1]);
	     
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
	 ATH_MSG_WARNING("Pixel SP tool failed");
	 return sc;
       }
     ATH_MSG_DEBUG("REGTEST / Pixel : Formed SP from " << 
				      nPixClusters << "  clusters");
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
	  ATH_MSG_DEBUG("REGTEST / SCT : Roi contains " 
		    << listOfSctIds.size() << " det. Elements");
	  ATH_MSG_DEBUG("SCT : " << uIntListOfSctRobs.size() << " ROBs will be requested");

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
	  StatusCode scSctClu=m_sctCacheTool->convertBStoClusters(v,listOfSctIds,m_sctDataErrors, m_isFullScan);
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
	  sc = evtStore()->retrieve(sctClusterContainer, m_sctClusterContainerName);
	  if(m_timers) m_timer[3]->stop();
	  if(sc.isFailure())
	    {
	      ATH_MSG_ERROR(" Retrieval of SCT Cluster Container " 
		   << m_sctClusterContainerName << " failed");
	      if(m_timers) m_timer[8]->stop();
	      return sc;
	    }
	}
      else
	{
	  sc = evtStore()->retrieve(sctClusterContainer, m_sctClusterCacheName);
	  if(m_timers) m_timer[3]->stop();
	  if (sc.isFailure()){
	    ATH_MSG_ERROR(" Retrieval of SCT Cluster Cache " << m_sctClusterCacheName 
		     << " failed");
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
	  m_nColl[2]++;
	  InDet::SCT_ClusterContainer::const_iterator clusterCollection = sctClusterContainer->indexFind(listOfSctIds[iSct]); 
	  // if(m_timers) m_timer[7]->pause();
      
	  if (clusterCollection==sctClusterContainer->end()) continue;
	  
	  if ((*clusterCollection)->size() == 0) continue;
	  m_nColl[3]++;
	  if ((m_useSctClustThreshold  && (*clusterCollection)->size()>m_sctClustThreshold ) ) 
	    { 
	      ATH_MSG_DEBUG("number of SCT clusters, " << (*clusterCollection)->size() 
			<< " exceeded "<<m_sctClustThreshold<<" for id=" << listOfSctIds[iSct] 
			<< ", skipping this module");
	      continue;
	    }
	  
	  nSctClusters += (*clusterCollection)->size();
	 
	  sctClusterColl.push_back( (*clusterCollection) );	  
	  
	  if (msgLvl(MSG::VERBOSE)) 
	    {
	      Identifier id = (*clusterCollection)->identify();
	      ATH_MSG_VERBOSE(m_sctId->print_to_string(id) << " with "
			<< (*clusterCollection)->size() << " clusters");
	      InDet::SCT_ClusterCollection::const_iterator pSctClus = (*clusterCollection)->begin();
	      for (int iSctClus=1; pSctClus != (*clusterCollection)->end(); pSctClus++, iSctClus++) 
		{
		  const Amg::Vector2D SCTpos = (*pSctClus)->localPosition();
		  ATH_MSG_VERBOSE(" cluster " << iSctClus << ": " << 
		    m_sctId->print_to_string((*pSctClus)->identify()) 
			    << " locT " << SCTpos[0] << " locL " << SCTpos[1]);
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
	 ATH_MSG_WARNING("SCT SP tool failed");
	 return sc;
       }
      if(m_timers) 
	{
	  m_timer[5]->stop();
	  m_spformationSCTTime = m_timer[5]->elapsed();
	}
      ATH_MSG_DEBUG("REGTEST / SCT : Formed SPs from " << nSctClusters << "  clusters");    
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
     ATH_MSG_DEBUG("REGTEST / Pixel : Roi contains " 
         << listOfPixIds.size() << " det. Elements");
     ATH_MSG_DEBUG("Pixel : " 
         << uIntListOfPixelRobs.size() << " ROBs will be requested");
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
	  StatusCode scPixClu=m_pixelCacheTool->convertBStoClusters(v,listOfPixIds,m_pixelDataErrors, m_isFullScan);

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
	 sc = evtStore()->retrieve(pixClusterContainer, m_pixelClusterContainerName);
	 if(m_timers) m_timer[2]->stop();
	 if (sc.isFailure())
	   {
	     ATH_MSG_ERROR("Retrieval of pixel Cluster Container " 
		      << m_pixelClusterContainerName << " failed");
	     if(m_timers) m_timer[8]->stop();
	     return sc;
	   }
       }
     else 
       {
	 sc = evtStore()->retrieve(pixClusterContainer, m_pixelClusterCacheName);
	 if(m_timers) m_timer[2]->stop();
	 if (sc.isFailure())
	   {
	     ATH_MSG_ERROR("Retrieval of pixel Cluster Cache " 
		      << m_pixelClusterCacheName << " failed");
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
	 m_nColl[0]++;
	 InDet::PixelClusterContainer::const_iterator 
	   clusterCollection = pixClusterContainer->indexFind(listOfPixIds[iPix]); 	  
	 if(m_timers) m_timer[6]->pause();
	  
	 if (clusterCollection==pixClusterContainer->end()) continue;
	 if ((*clusterCollection)->size() == 0) continue;

	 m_nColl[1]++;

	 if ((m_usePixelClustThreshold  && (*clusterCollection)->size()>m_pixClustThreshold ) ) 
	    { 
	      ATH_MSG_DEBUG("number of Pixel clusters, " << (*clusterCollection)->size() 
			<< " exceeded "<<m_pixClustThreshold<<" for id=" << listOfPixIds[iPix] 
			<< ", skipping this module");
	      continue;
	    }

	 PixelIDs.push_back(listOfPixIds[iPix]);

	 nPixClusters += (*clusterCollection)->size();
	 if (msgLvl(MSG::DEBUG)) 
	    {
	      Identifier id = (*clusterCollection)->identify();
	      ATH_MSG_DEBUG(m_pixelId->print_to_string(id) << " with "
			<< (*clusterCollection)->size() << " clusters");
	    }
	 pixClusterColl.push_back( (*clusterCollection) );	  
	
	 if (msgLvl(MSG::VERBOSE)) {
	   InDet::PixelClusterCollection::const_iterator pPixClus = (*clusterCollection)->begin();
	   
	   for (int iPixClus=1; pPixClus != (*clusterCollection)->end(); pPixClus++, iPixClus++) {
	     const Amg::Vector2D pos = (*pPixClus)->localPosition();
	     ATH_MSG_VERBOSE(" cluster " << iPixClus << ": " 
		       << m_sctId->print_to_string((*pPixClus)->identify()) 
		       << " locT " << pos[0] << " locL " << pos[1]);
	     
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
	 ATH_MSG_WARNING("Pixel SP tool failed");
	 return sc;
       }
     ATH_MSG_DEBUG("REGTEST / Pixel : Formed SP from " << 
				      nPixClusters << "  clusters");
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
      ATH_MSG_DEBUG("REGTEST / SCT : Roi contains " 
        << listOfSctIds.size() << " det. Elements");
      ATH_MSG_DEBUG("SCT : " 
        << uIntListOfSctRobs.size() << " ROBs will be requested");
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
	  StatusCode scSctClu=m_sctCacheTool->convertBStoClusters(v,listOfSctIds,m_sctDataErrors, m_isFullScan);
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
	  sc = evtStore()->retrieve(sctClusterContainer, m_sctClusterContainerName);
	  if(m_timers) m_timer[3]->stop();
	  if(sc.isFailure())
	    {
	      ATH_MSG_ERROR(" Retrieval of SCT Cluster Container " 
		   << m_sctClusterContainerName << " failed");
	      if(m_timers) m_timer[8]->stop();
	      return sc;
	    }
	}
      else
	{
	  sc = evtStore()->retrieve(sctClusterContainer, m_sctClusterCacheName);
	  if(m_timers) m_timer[3]->stop();
	  if (sc.isFailure()){
	    ATH_MSG_ERROR(" Retrieval of SCT Cluster Cache " << m_sctClusterCacheName 
		     << " failed");
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
	  m_nColl[2]++;
	  InDet::SCT_ClusterContainer::const_iterator clusterCollection = sctClusterContainer->indexFind(listOfSctIds[iSct]); 
	  // if(m_timers) m_timer[7]->pause();
      
	  if (clusterCollection==sctClusterContainer->end()) continue;
	  
	  if ((*clusterCollection)->size() == 0) continue;
	  m_nColl[3]++;
	  if ((m_useSctClustThreshold  && (*clusterCollection)->size()>m_sctClustThreshold ) ) 
	    { 
	      ATH_MSG_DEBUG("number of SCT clusters, " << (*clusterCollection)->size() 
			<< " exceeded "<<m_sctClustThreshold<<" for id=" << listOfSctIds[iSct] 
			<< ", skipping this module");
	      continue;
	    }
	  
	  nSctClusters += (*clusterCollection)->size();
	 
	  sctClusterColl.push_back( (*clusterCollection) );	  
	  
	  if (msgLvl(MSG::VERBOSE)) 
	    {
	      Identifier id = (*clusterCollection)->identify();
	      ATH_MSG_VERBOSE(m_sctId->print_to_string(id) << " with "
			<< (*clusterCollection)->size() << " clusters");
	      InDet::SCT_ClusterCollection::const_iterator pSctClus = (*clusterCollection)->begin();
	      for (int iSctClus=1; pSctClus != (*clusterCollection)->end(); pSctClus++, iSctClus++) 
		{
		  const Amg::Vector2D SCTpos = (*pSctClus)->localPosition();
		  ATH_MSG_VERBOSE(" cluster " << iSctClus << ": " << 
		    m_sctId->print_to_string((*pSctClus)->identify()) 
			    << " locT " << SCTpos[0] << " locL " << SCTpos[1]);
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
	 ATH_MSG_WARNING("SCT SP tool failed");
	 return sc;
       }
      if(m_timers) 
	{
	  m_timer[5]->stop();
	  m_spformationSCTTime = m_timer[5]->elapsed();
	}
      ATH_MSG_DEBUG("REGTEST / SCT : Formed SPs from " << nSctClusters 
					       <<"  clusters");    
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


spacePointVec OnlineSpacePointProviderTool::getSpacePoints(bool getPixelSP, bool getSctSP)
{

  spacePointVec spVec;
  std::vector<int> listOfPixIds;
  std::vector<int> listOfSctIds;

  StatusCode sc=fillCollections(getPixelSP,getSctSP,listOfPixIds,listOfSctIds);

  if(!(sc.isSuccess()||sc.isRecoverable()))
    {
      ATH_MSG_WARNING("SpacePoint formation failed "); 
      return spVec;
    }
  if(getPixelSP) 
    {
      const TrigSiSpacePointContainer* pCont;
      sc=evtStore()->retrieve(pCont,m_pixelSpContainerName);
      if(sc.isFailure())
	{
	  ATH_MSG_WARNING("Trig SP Pixel container " <<m_pixelSpContainerName
		    <<" not found"); 
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
	  if (msgLvl(MSG::VERBOSE)) 
	    {    
	      for (unsigned int i=0; i<spVec.size(); i++) 
		{
		  ATH_MSG_VERBOSE("pix (r,phi,z): " << spVec[i]->r() << " / " 
			    << spVec[i]->phi() << " / "
			    << spVec[i]->z()); 
		}
	    }
	  ATH_MSG_DEBUG("REGTEST / Pixel : Formed " << spVec.size() << "  SPs ");
	}
    }
  if(!getSctSP) 
    {
      return spVec;
    }
  spacePointVec spVecSCT;

  const TrigSiSpacePointContainer* pCont;
  sc=evtStore()->retrieve(pCont,m_sctSpContainerName);
  if(sc.isFailure())
    {
      ATH_MSG_WARNING("Trig SCT Pixel container " <<m_sctSpContainerName
		<<" not found"); 
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
      if (msgLvl(MSG::VERBOSE)) 
	{    
	  for (unsigned int i=0; i<spVecSCT.size(); i++) {
	    ATH_MSG_VERBOSE("SCT (r,phi,z): " << spVecSCT[i]->r() << " / " 
		      << spVecSCT[i]->phi() << " / " 
		      << spVecSCT[i]->z()); 
	  }
	}    
      ATH_MSG_DEBUG("REGTEST / SCT : Formed " << spVecSCT.size() << "  SPs ");    
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



