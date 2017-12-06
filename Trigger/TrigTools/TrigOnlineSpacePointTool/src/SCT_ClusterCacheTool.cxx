/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "SCT_RawDataByteStreamCnv/ISCT_RodDecoder.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "TrigOnlineSpacePointTool/SCT_ClusterCacheTool.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "InDetIdentifier/SCT_ID.h"
#include "Identifier/IdentifierHash.h" 

#include "InDetReadoutGeometry/SiDetectorManager.h"
#include <string>
#include <sstream>

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "TrigOnlineSpacePointTool/FastSCT_Clusterization.h"
#include "eformat/SourceIdentifier.h" 
using eformat::helper::SourceIdentifier;
#include "ByteStreamData/ROBData.h" 
#include "SCT_Cabling/ISCT_CablingSvc.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"


#define MAXNUM_SCT_BS_ERRORS 14

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

SCT_ClusterCacheTool::SCT_ClusterCacheTool( const std::string& type, 
					    const std::string& name, 
					    const IInterface* parent )
  : AthAlgTool(type, name, parent), 
    m_offlineDecoder("SCT_RodDecoder",this), 
    m_byteStreamErrSvc("SCT_ByteStreamErrorsSvc",name),
    m_clusteringTool("InDet::SCT_ClusteringTool/InDetTrigSCT_ClusteringTool") 
{
  declareInterface< ISCT_ClusterCacheTool>( this );
  declareProperty( "ClusterContainerName", m_containerName = "SCT_ClusterCache");
  declareProperty( "SCT_ROD_Decoder", m_offlineDecoder,"SCT_RodDecoder");
  declareProperty( "UseOfflineDecoder", m_useOfflineDecoder = true);
  declareProperty( "RDO_ContainerName", m_rdoContainerName = "SCT_RDO_Cache");
  declareProperty( "UseOfflineClustering", m_useOfflineClustering = true);
  declareProperty( "SCT_ByteStreamErrorsSvc",m_byteStreamErrSvc );
  declareProperty( "SCT_ClusteringTool", m_clusteringTool,"InDet::SCT_ClusteringTool/InDetTrigSCT_ClusteringTool");
  declareProperty( "DoBS_Conversion", m_doBS = true);
}

StatusCode SCT_ClusterCacheTool::initialize()  {

  ATH_MSG_INFO(name() << " in initialize");
  StatusCode sc = AthAlgTool::initialize(); 

  sc=detStore()->retrieve(p_indet_mgr, "SCT");
  if (sc.isFailure()) {
    ATH_MSG_ERROR( name() << "failed to get SCT Manager");
    return sc;
  } else { 
    ATH_MSG_DEBUG( name() << "Got SCT Manager");
  }
 
  // Get SCT & pixel helpers

  if (detStore()->retrieve(m_sct_id, "SCT_ID").isFailure()) {
     ATH_MSG_FATAL("Could not get SCT ID helper");
     return StatusCode::FAILURE;
  } 

  m_cntx_sct = m_sct_id->wafer_context();

  m_clusterization.setSctID(m_sct_id);
  m_clusterization.initializeGeometry(p_indet_mgr);

  IToolSvc* toolSvc;
  sc=service("ToolSvc",toolSvc);
  if(sc.isFailure()) 
    {
      ATH_MSG_ERROR( name() << "failed to get ToolSvc");
      return sc; 
    }  

  if(m_doBS) {
 
    sc=service("SCT_CablingSvc",m_cablingSvc);
    if(sc.isFailure()) 
      {
	ATH_MSG_ERROR( name() << "failed to get CablingSvc");
	return sc; 
      }  
  }
  ATH_MSG_INFO("SCT_Cluster_CacheName: " << m_containerName);
  m_clusterContainer = new InDet::SCT_ClusterContainer(m_sct_id->wafer_hash_max());
  m_clusterContainer->addRef();

  sc = evtStore()->record(m_clusterContainer, m_containerName);

  if (sc.isFailure()) { 
    ATH_MSG_ERROR(" Container " << m_containerName << " could not be recorded in StoreGate !");
    return sc;
  }  
  else { 
    ATH_MSG_INFO("Container " << m_containerName << " registered  in StoreGate");   
  } 

  // symlink the container 
  const InDet::SiClusterContainer* symSiContainer(0); 
  sc = evtStore()->symLink(m_clusterContainer,symSiContainer); 
  if (sc.isFailure()) { 
    ATH_MSG_ERROR( "SCT clusters could not be symlinked in StoreGate !");
    return sc;
  }  
  else ATH_MSG_INFO("SCT clusters " << m_containerName << " symlinked in StoreGate");


  ATH_MSG_INFO("SCT_RDO_CacheName: " << m_rdoContainerName);

  if(m_doBS) {

    m_rdoContainer = new SCT_RDO_Container(m_sct_id->wafer_hash_max());
    m_rdoContainer->addRef();

    if(StatusCode::SUCCESS !=toolSvc->retrieveTool("FastSCT_RodDecoder",m_decoder)) {
      ATH_MSG_ERROR( "initialize(): Can't get FastSCT_RodDecoder ");
      return StatusCode::FAILURE; 
    }
    
    if(StatusCode::SUCCESS !=m_offlineDecoder.retrieve()) {
      ATH_MSG_ERROR( "initialize(): Can't get "<<m_offlineDecoder);
      return StatusCode::FAILURE; 
    } 

    if (m_byteStreamErrSvc.retrieve().isFailure()) 
      {
	ATH_MSG_ERROR("initialize(): Can't get "<<m_byteStreamErrSvc);
	return StatusCode::FAILURE; 
      }
  }
  if(m_useOfflineClustering)
    {
      if (m_clusteringTool.retrieve().isFailure()) 
	{
	  ATH_MSG_ERROR("initialize(): Can't get "<<m_clusteringTool);
	  return StatusCode::FAILURE; 
	}
      else 
	ATH_MSG_INFO("retrieved "<<m_clusteringTool);
    }

  ITrigTimerSvc* timerSvc;
  StatusCode scTime = service( "TrigTimerSvc", timerSvc);
  if( scTime.isFailure() ) {
    ATH_MSG_WARNING("Unable to locate Service TrigTimerSvc ");
    m_timers = false;
  } 
  else{
    m_timers = true;  
  }
  if ( m_timers ) {
    m_timer[0] = timerSvc->addItem("RDOmap");
    m_timer[1] = timerSvc->addItem("fillColl");
    m_timer[2] = timerSvc->addItem("Clust");
    m_timer[3] = timerSvc->addItem("FastRodDec");
    m_timer[4] = timerSvc->addItem("SCT_CLTot");
  }

  return sc;
}

StatusCode SCT_ClusterCacheTool::finalize() 
{
  m_clusterContainer->cleanup();
  m_clusterContainer->release();
  if(m_doBS) {
    m_rdoContainer->cleanup();
    m_rdoContainer->release();
  }
  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}

StatusCode SCT_ClusterCacheTool::convertBStoClusters(std::vector<const ROBF*>& robFrags,
                                                     const std::vector<IdentifierHash>& listOfSCTIds,
                                                     std::vector<int>& errorVect, bool isFullScan)
{
  if(m_timers) 
    {
      m_timer[4]->start();
    }

  if(!evtStore()->contains<InDet::SCT_ClusterContainer>(m_containerName))
    {
      m_clusterContainer->cleanup();
      StatusCode sc=evtStore()->record(m_clusterContainer,m_containerName,false);
      if(sc.isFailure())
	{
	  ATH_MSG_WARNING("SCT Cluster Container " << m_containerName 
	      <<" cannot be recorded in StoreGate !");
	  return StatusCode::FAILURE;
	}
      else 
	{
	    ATH_MSG_DEBUG( "SCT Cluster Container " << m_containerName
		<< " is recorded in StoreGate");
	}
      if(m_doBS) {
	m_rdoContainer->cleanup();
	sc=evtStore()->record(m_rdoContainer,m_rdoContainerName,false);
	if(sc.isFailure())
	  {
	    ATH_MSG_WARNING("SCT RDO Container " << m_rdoContainerName 
		<<" cannot be recorded in StoreGate !");
	    return StatusCode::FAILURE;
	  }
	else 
	  {
	      ATH_MSG_DEBUG( "SCT RDO Container " << m_rdoContainerName
		  << " is recorded in StoreGate");
	}
      }
    }
  else 
    {    
	ATH_MSG_DEBUG( "SCT Cluster Container " <<m_containerName 
	    << " is found in StoreGate");
    }

  if(!m_doBS) {
    //retrieve m_rdoContainer here 

    if(!evtStore()->contains<SCT_RDO_Container>(m_rdoContainerName)) {
      ATH_MSG_ERROR( "SCT RDO Container " << m_rdoContainerName
	  <<" is not found in StoreGate !");
      return StatusCode::FAILURE;
    }
    const SCT_RDO_Container* pCont = NULL; 
    StatusCode sc = evtStore()->retrieve(pCont, m_rdoContainerName);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "retrieval of SCT RDO Container " << m_rdoContainerName
	  <<" failed");
      return sc;
    }
    m_rdoContainer = const_cast<SCT_RDO_Container*>(pCont);

    if(m_rdoContainer==NULL) {
      ATH_MSG_ERROR( "SCT RDO Container const_cast failed ");
      return StatusCode::FAILURE;
    }

  }
  
  if(m_timers) 
    {
      m_timer[0]->start();m_timer[0]->pause();
      m_timer[1]->start();m_timer[1]->pause(); 
      m_timer[2]->start();m_timer[2]->pause();
      m_timer[3]->start();m_timer[3]->pause();
    }

  int n_rob_count =0;
  int n_recov_errors =0;
  bool bs_failure=false;

  std::vector<IdentifierHash> reducedList;
  reducedList.clear();
  for(std::vector<IdentifierHash>::const_iterator it=listOfSCTIds.begin();it!=listOfSCTIds.end();++it)
    {
      if(m_clusterContainer->indexFind((*it))==m_clusterContainer->end())
	reducedList.push_back(*it);
    }
  
  if(m_doBS) {

    if(robFrags.size() == 0) 
      {
        ATH_MSG_DEBUG(" There are NO ROB data " );
	if(m_timers) 
	  {
	    m_timer[0]->stop();
	    m_timer[1]->stop();
	    m_timer[2]->stop();
	    m_timer[3]->stop();
	    m_timer[4]->stop();
	  }
	return StatusCode::SUCCESS;
      } 
      
    ATH_MSG_DEBUG(" There are SOME ROB data " ) ;
      
      std::vector<IdentifierHash>::const_iterator sctIdBeginIter=listOfSCTIds.begin();
      std::vector<IdentifierHash>::const_iterator sctIdEndIter=listOfSCTIds.end();
      std::vector<bool> idList(m_sct_id->wafer_hash_max(), false);

      if(!m_useOfflineDecoder)
	{
	  for (std::vector<IdentifierHash>::const_iterator sctIdIter=sctIdBeginIter;
	       sctIdIter!=sctIdEndIter; sctIdIter++) 
	    {
	      if (m_clusterContainer->indexFind(*sctIdIter)==m_clusterContainer->end())
		idList[*sctIdIter]=true;
	    }
	}
      IROBDataProviderSvc::VROBFRAG::const_iterator rob_it = robFrags.begin(); 
      IROBDataProviderSvc::VROBFRAG::const_iterator rob_it_end = robFrags.end();
      m_clusterization.initialize();

      for(; rob_it!=rob_it_end; ++rob_it) 
	{ 
	  uint32_t rodid = (*rob_it)->rod_source_id() ; 
	  eformat::helper::SourceIdentifier sid_rob(rodid) ;
	  uint32_t detid = sid_rob.subdetector_id() ;
	    ATH_MSG_DEBUG("  ROB source ID SID " <<std::hex<<rodid <<std::dec) ;
	  const ROBFragment * robFrag = (*rob_it);
	    ATH_MSG_DEBUG(std::hex<<"Det ID 0x"<<detid<<" Rod version 0x"<<
	      robFrag->rod_version()<<", Type="<<robFrag->rod_detev_type()<<std::dec) ;
	  ++n_rob_count;
	  if(!m_useOfflineDecoder)
	    {
	      if(m_timers) m_timer[3]->resume();	    
	      bool processedSomething = m_decoder->fillCollections((*rob_it), rodid, m_cablingSvc, idList, &m_clusterization); 
	      if (processedSomething) m_clusterization.finishHits();        
	      if(m_timers) m_timer[3]->pause();
	    }
	  else
	    {
	      if(m_timers) m_timer[1]->resume();
	      m_byteStreamErrSvc->resetCounts();

	      StatusCode scRod = StatusCode::SUCCESS;
	      if(!isFullScan) {
		scRod=m_offlineDecoder->fillCollection(*robFrag,*m_rdoContainer,nullptr,&reducedList);
	      }
	      else {
		scRod=m_offlineDecoder->fillCollection(*robFrag,*m_rdoContainer,nullptr,NULL);
	      }
	      if(scRod.isRecoverable())
		{
		  n_recov_errors++;
		  for(int idx=0;idx<MAXNUM_SCT_BS_ERRORS;idx++)
		    {
		      int n_error = m_byteStreamErrSvc->getNumberOfErrors(idx);
		      for(int ierror = 0;ierror<n_error;ierror++)
			errorVect.push_back(idx);
		    }
		    
		}
              else if(scRod.isFailure())
                {
                  bs_failure=true;break;
                }
	      if(m_timers) m_timer[1]->pause();	      
	    }	
	}
  }
  std::vector<const InDet::SCT_ClusterCollection*> offlineCollVector;
  offlineCollVector.clear();
  if(m_useOfflineDecoder)
    {
      if(m_timers) m_timer[2]->resume();
      
      int nstrips=0;
      for (std::vector<IdentifierHash>::iterator it=reducedList.begin(); it != reducedList.end(); ++it)        
	{
	  SCT_RDO_Container::const_iterator collIt=m_rdoContainer->indexFind((*it));
	  if(collIt==m_rdoContainer->end())
	    continue;
	  if((*collIt)->size()!=0)
	    {
	      if(!m_useOfflineClustering)
		{
		  for(SCT_RDO_Collection::const_iterator rdoIt=(*collIt)->begin();rdoIt!=(*collIt)->end();
		      ++rdoIt)
		    {
		      Identifier stripId=(*rdoIt)->identify();
		      int strip=m_sct_id->strip(stripId);
		      nstrips++;
		      m_clusterization.addHit((*collIt)->identify(),(*collIt)->identifyHash(),
					      strip);
		      int groupSize=(*rdoIt)->getGroupSize();
		      for(int ig=1;ig<groupSize;ig++)
			{
			  strip++;
			  m_clusterization.addHit((*collIt)->identify(),(*collIt)->identifyHash(),
						  strip);
			  nstrips++;
			}
		    }
		}
	      else
		{
		  const InDetRawDataCollection<SCT_RDORawData>* pRdoColl = (*collIt);
		  const InDet::SCT_ClusterCollection* pColl = m_clusteringTool->clusterize(*pRdoColl,
											   *p_indet_mgr,
											   *m_sct_id);
		  if(pColl!=NULL) 
		    {
		      if(pColl->size()==0) delete pColl;
		      else offlineCollVector.push_back(pColl);
		    }
		}
	    }
	}
      if(nstrips!=0) m_clusterization.finishHits();
      if(m_timers) m_timer[2]->pause();
    }
  std::vector<const InDet::SCT_ClusterCollection*>& vColl=m_clusterization.getClusterCollections();
  if(m_useOfflineClustering)
    vColl=offlineCollVector;
    ATH_MSG_DEBUG( "No of collections found: " <<  vColl.size()) ;
      
  // Loop over cluster collections
  std::vector<const InDet::SCT_ClusterCollection*>::iterator iter_coll = vColl.begin();
  for(;iter_coll!=vColl.end();iter_coll++) 
    {
      IdentifierHash hashId=(*iter_coll)->identifyHash();
      StatusCode sc=m_clusterContainer->addCollection((*iter_coll),hashId);
      if(sc.isFailure())
	{
	    ATH_MSG_INFO("IDC record failed" );
	}
    }
  if(m_timers) 
    {
      m_timer[0]->stop();
      m_timer[1]->stop();
      m_timer[2]->stop();
      m_timer[3]->stop();
      m_timer[4]->stop();
    }
  if(bs_failure)
    {
      ATH_MSG_DEBUG(" FAILURE in SCT_RodDecoder");
      return StatusCode::FAILURE;
    }
  else 
    {
      ATH_MSG_DEBUG(" number of ROBs used  "<<n_rob_count<< "Total number of recoverable errors "<<n_recov_errors);
      if(n_recov_errors!=0)
	{
	  return StatusCode::RECOVERABLE;
	}
      else return StatusCode::SUCCESS;
    }
}
