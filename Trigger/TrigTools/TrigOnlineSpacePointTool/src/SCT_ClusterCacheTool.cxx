/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IssueSeverity.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"
#include "SCT_RawDataByteStreamCnv/ISCT_RodDecoder.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "TrigOnlineSpacePointTool/SCT_ClusterCacheTool.h"
#include "GaudiKernel/MsgStream.h"
#include "InDetIdentifier/SCT_ID.h"
#include "Identifier/IdentifierHash.h" 
#include "StoreGate/StoreGate.h"

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

SCT_ClusterCacheTool::SCT_ClusterCacheTool( const std::string& type, 
					    const std::string& name, 
					    const IInterface* parent )
  : AlgTool(type, name, parent), 
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
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << name() << " in initialize" << endreq;
  StatusCode sc = AlgTool::initialize(); 
  // get DetectorStore service
  StoreGateSvc* detStore; 
  sc=service("DetectorStore",detStore);
  if (sc.isFailure()) { 
    log << MSG::ERROR << name() <<" failed to get detStore" << endreq;
    return sc;
  }

  sc = service( "StoreGateSvc", m_StoreGate );
  if (sc.isFailure()) {
    log << MSG::FATAL << "Unable to retrieve StoreGate service" << endreq;
    return sc;
  }

  
  sc=detStore->retrieve(p_indet_mgr, "SCT");
  if (sc.isFailure()) {
    log << MSG::ERROR << name() << "failed to get SCT Manager" << endreq;
    return sc;
  } else { 
    log << MSG::DEBUG << name() << "Got SCT Manager" << endreq;
  }
 
  // Get SCT & pixel helpers

  if (detStore->retrieve(m_sct_id, "SCT_ID").isFailure()) {
     log << MSG::FATAL << "Could not get SCT ID helper" << endreq;
     return StatusCode::FAILURE;
  } 

  m_cntx_sct = m_sct_id->wafer_context();

  m_clusterization.setSctID(m_sct_id);
  m_clusterization.initializeGeometry(p_indet_mgr);

  IToolSvc* toolSvc;
  sc=service("ToolSvc",toolSvc);
  if(sc.isFailure()) 
    {
      log << MSG::ERROR << name() << "failed to get ToolSvc" << endreq;
      return sc; 
    }  

  if(m_doBS) {
 
    sc=service("SCT_CablingSvc",m_cablingSvc);
    if(sc.isFailure()) 
      {
	log << MSG::ERROR << name() << "failed to get CablingSvc" << endreq;
	return sc; 
      }  
  }
  log << MSG::INFO << "SCT_Cluster_CacheName: " << m_containerName << endreq;
  m_clusterContainer = new InDet::SCT_ClusterContainer(m_sct_id->wafer_hash_max());
  m_clusterContainer->addRef();

  sc = m_StoreGate->record(m_clusterContainer, m_containerName);

  if (sc.isFailure()) { 
    log << MSG::ERROR  << " Container " << m_containerName << " could not be recorded in StoreGate !" << endreq;
    return sc;
  }  
  else { 
    log << MSG::INFO << "Container " << m_containerName << " registered  in StoreGate" << endreq;   
  } 

  // symlink the container 
  const InDet::SiClusterContainer* symSiContainer(0); 
  sc = m_StoreGate->symLink(m_clusterContainer,symSiContainer); 
  if (sc.isFailure()) { 
    log << MSG::ERROR << "SCT clusters could not be symlinked in StoreGate !"<< endreq;
    return sc;
  }  
  else log << MSG::INFO  << "SCT clusters " << m_containerName << " symlinked in StoreGate" << endreq;


  log << MSG::INFO << "SCT_RDO_CacheName: " << m_rdoContainerName << endreq;

  if(m_doBS) {

    m_rdoContainer = new SCT_RDO_Container(m_sct_id->wafer_hash_max());
    m_rdoContainer->addRef();

    if(StatusCode::SUCCESS !=toolSvc->retrieveTool("FastSCT_RodDecoder",m_decoder)) {
      log << MSG::ERROR << "initialize(): Can't get FastSCT_RodDecoder " << endreq;
      return StatusCode::FAILURE; 
    }
    
    if(StatusCode::SUCCESS !=m_offlineDecoder.retrieve()) {
      log << MSG::ERROR << "initialize(): Can't get "<<m_offlineDecoder<< endreq;
      return StatusCode::FAILURE; 
    } 

    if (m_byteStreamErrSvc.retrieve().isFailure()) 
      {
	log<<MSG::ERROR <<"initialize(): Can't get "<<m_byteStreamErrSvc<<endreq;
	return StatusCode::FAILURE; 
      }
  }
  if(m_useOfflineClustering)
    {
      if (m_clusteringTool.retrieve().isFailure()) 
	{
	  log<<MSG::ERROR <<"initialize(): Can't get "<<m_clusteringTool<<endreq;
	  return StatusCode::FAILURE; 
	}
      else 
	log<<MSG::INFO <<"retrieved "<<m_clusteringTool<<endreq;
    }

  ITrigTimerSvc* timerSvc;
  StatusCode scTime = service( "TrigTimerSvc", timerSvc);
  if( scTime.isFailure() ) {
    log << MSG::INFO<< "Unable to locate Service TrigTimerSvc " << endreq;
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
  StatusCode sc = AlgTool::finalize(); 
  return sc;
}

StatusCode SCT_ClusterCacheTool::m_convertBStoClusters(std::vector<const ROBF*>& robFrags,
                                                 std::vector<IdentifierHash> listOfSCTIds,
						       std::vector<int>& errorVect, bool isFullScan)
{
  if(m_timers) 
    {
      m_timer[4]->start();
    }

  MsgStream log(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );

  if(!m_StoreGate->contains<InDet::SCT_ClusterContainer>(m_containerName))
    {
      m_clusterContainer->cleanup();
      StatusCode sc=m_StoreGate->record(m_clusterContainer,m_containerName,false);
      if(sc.isFailure())
	{
	  log << MSG::WARNING << "SCT Cluster Container " << m_containerName 
	      <<" cannot be recorded in StoreGate !"<< endreq;
	  return StatusCode::FAILURE;
	}
      else 
	{
	  if(outputLevel <= MSG::DEBUG)
	    log << MSG::DEBUG << "SCT Cluster Container " << m_containerName
		<< " is recorded in StoreGate" << endreq;
	}
      if(m_doBS) {
	m_rdoContainer->cleanup();
	sc=m_StoreGate->record(m_rdoContainer,m_rdoContainerName,false);
	if(sc.isFailure())
	  {
	    log << MSG::WARNING << "SCT RDO Container " << m_rdoContainerName 
		<<" cannot be recorded in StoreGate !"<< endreq;
	    return StatusCode::FAILURE;
	  }
	else 
	  {
	    if(outputLevel <= MSG::DEBUG)
	      log << MSG::DEBUG << "SCT RDO Container " << m_rdoContainerName
		  << " is recorded in StoreGate" << endreq;
	}
      }
    }
  else 
    {    
      if(outputLevel <= MSG::DEBUG)
	log << MSG::DEBUG << "SCT Cluster Container " <<m_containerName 
	    << " is found in StoreGate" << endreq;
    }

  if(!m_doBS) {
    //retrieve m_rdoContainer here 

    if(!m_StoreGate->contains<SCT_RDO_Container>(m_rdoContainerName)) {
      log << MSG::ERROR << "SCT RDO Container " << m_rdoContainerName
	  <<" is not found in StoreGate !"<< endreq;
      return StatusCode::FAILURE;
    }
    const SCT_RDO_Container* pCont = NULL; 
    StatusCode sc = m_StoreGate->retrieve(pCont, m_rdoContainerName);
    if (sc.isFailure()) {
      log << MSG::ERROR << "retrieval of SCT RDO Container " << m_rdoContainerName
	  <<" failed"<< endreq;
      return sc;
    }
    m_rdoContainer = const_cast<SCT_RDO_Container*>(pCont);

    if(m_rdoContainer==NULL) {
      log << MSG::ERROR << "SCT RDO Container const_cast failed " << endreq;
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
	if(outputLevel <= MSG::DEBUG)
	  log<<MSG::DEBUG<<" There are NO ROB data " <<endreq;
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
      
      if(outputLevel <= MSG::DEBUG)
	log << MSG::DEBUG<<" There are SOME ROB data " <<endreq ;
      
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
	  if(outputLevel <= MSG::DEBUG)
	    log <<MSG::DEBUG<<"  ROB source ID SID " <<std::hex<<rodid <<std::dec<<endreq ;
	  const ROBFragment * robFrag = (*rob_it);
	  if(outputLevel <= MSG::DEBUG)
	    log<<MSG::DEBUG<<std::hex<<"Det ID 0x"<<detid<<" Rod version 0x"<<
	      robFrag->rod_version()<<", Type="<<robFrag->rod_detev_type()<<std::dec<<endreq ;
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
		scRod=m_offlineDecoder->fillCollection(robFrag,m_rdoContainer,&reducedList);
	      }
	      else {
		scRod=m_offlineDecoder->fillCollection(robFrag,m_rdoContainer,NULL);
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
  if(outputLevel <= MSG::DEBUG)
    log<<MSG::DEBUG<< "No of collections found: " <<  vColl.size() << endreq ;
      
  // Loop over cluster collections
  std::vector<const InDet::SCT_ClusterCollection*>::iterator iter_coll = vColl.begin();
  for(;iter_coll!=vColl.end();iter_coll++) 
    {
      IdentifierHash hashId=(*iter_coll)->identifyHash();
      StatusCode sc=m_clusterContainer->addCollection((*iter_coll),hashId);
      if(sc.isFailure())
	{
	  if(outputLevel <= MSG::INFO)
	    log<<MSG::INFO<< "IDC record failed"  << endreq;
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
  if(outputLevel <= MSG::DEBUG)
    {
      if(bs_failure)
	{
	  log << MSG::DEBUG <<" FAILURE in SCT_RodDecoder"<<endreq;
	}
      else
	log << MSG::DEBUG <<" number of ROBs used  "<<n_rob_count<< "Total number of recoverable errors "<<n_recov_errors<<endreq;
    }
  if(bs_failure)
    {
      return StatusCode::FAILURE;
    }
  else 
    {
      if(n_recov_errors!=0)
	{
	  return StatusCode::RECOVERABLE;
	}
      else return StatusCode::SUCCESS;
    }
}
