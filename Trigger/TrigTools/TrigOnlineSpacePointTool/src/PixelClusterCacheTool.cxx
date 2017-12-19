/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "InDetIdentifier/PixelID.h"
#include "Identifier/IdentifierHash.h" 

#include "InDetReadoutGeometry/SiDetectorManager.h"

#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/PixelClusterContainer.h"

#include <string>
#include <sstream>

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "TrigOnlineSpacePointTool/FastPixelClusterization.h"

#include "TrigOnlineSpacePointTool/PixelClusterCacheTool.h"
#include "eformat/SourceIdentifier.h" 
using eformat::helper::SourceIdentifier;
#include "ByteStreamData/ROBData.h" 

#define MAXNUM_PIX_BS_ERRORS 8

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

PixelClusterCacheTool::PixelClusterCacheTool( const std::string& type, 
					    const std::string& name, 
					    const IInterface* parent )
  : AthAlgTool(type, name, parent), 
    m_IdMapping("PixelCablingSvc",name), 
    m_offlineDecoder("PixelRodDecoder",this), 
    m_bsErrorSvc("PixelByteStreamErrorsSvc",name),
    m_clusteringTool("InDet::MergedPixelsTool/InDetTrigMergedPixelsTool") 
{

  declareInterface< IPixelClusterCacheTool>( this );
  declareProperty( "ClusterContainerName", m_containerName = "PixelClusterCache");
  declareProperty( "PixelROD_Decoder", m_offlineDecoder,"PixelRodDecoder");
  declareProperty( "RDO_ContainerName", m_rdoContainerName = "PixelRDO_Cache");
  declareProperty( "UseOfflineClustering", m_useOfflineClustering = true);
  declareProperty( "PixelClusteringTool", m_clusteringTool,"InDet::MergedPixelsTool/InDetTrigMergedPixelsTool");
  declareProperty( "DoBS_Conversion", m_doBS = true);
}

StatusCode PixelClusterCacheTool::initialize()  {

  ATH_MSG_INFO(name() << " in initialize");
  StatusCode sc = AthAlgTool::initialize(); 
  // get DetectorStore service


  const InDetDD::PixelDetectorManager * mgr;
  sc=detStore()->retrieve(mgr, "Pixel");
  if (sc.isFailure()) {
    ATH_MSG_ERROR(name() << "failed to get Pixel Manager");
    return StatusCode::FAILURE;
  } else { 
    ATH_MSG_DEBUG(name() << "Got Pixel Manager");
  }
 
  // Get SCT & pixel helpers
  if (detStore()->retrieve(m_pixel_id, "PixelID").isFailure()) {
    ATH_MSG_FATAL("Could not get Pixel ID helper");
    return StatusCode::FAILURE;
  } 

  p_indet_mgr = mgr;  

  m_clusterization.setPixelID(m_pixel_id);
  m_clusterization.initializeGeometry(mgr);
  m_cntx_pix = m_pixel_id->wafer_context();

  IToolSvc* toolSvc;
  sc=service("ToolSvc",toolSvc);
  if(sc.isFailure()) {
    ATH_MSG_ERROR(" Can't get ToolSvc ");
    return sc; 
  } 

  if(m_doBS) {

    sc = m_IdMapping.retrieve();
    if(sc.isFailure()) {
      ATH_MSG_ERROR("cannot retrieve PixelCablingService ");  
      return sc;
    }   

    if(StatusCode::SUCCESS !=m_offlineDecoder.retrieve()) {
      ATH_MSG_ERROR("initialize(): Can't get PixelRodDecoder ");
      return StatusCode::FAILURE; 
    }

    if(StatusCode::SUCCESS !=m_bsErrorSvc.retrieve()) {
      ATH_MSG_ERROR("initialize(): Can't get PixelByteStreamError service "<<m_bsErrorSvc);
      return StatusCode::FAILURE; 
    }
  }

  if(StatusCode::SUCCESS !=m_clusteringTool.retrieve()) {
    ATH_MSG_ERROR("initialize(): Can't get tool "<<m_clusteringTool);
    return StatusCode::FAILURE; 
  }
  else ATH_MSG_INFO("retrieved "<<m_clusteringTool); 

  ATH_MSG_INFO("PixelCluster_CacheName: " << m_containerName);
  m_clusterContainer = new InDet::PixelClusterContainer(m_pixel_id->wafer_hash_max());
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
    ATH_MSG_ERROR("Pixel clusters could not be symlinked in StoreGate !");
    return sc;
  }  
  else ATH_MSG_INFO("Pixel clusters " << m_containerName << " symlinked in StoreGate");

  ATH_MSG_INFO("PixelRDO_CacheName: " << m_rdoContainerName);

  if(m_doBS) {
    m_rdoContainer = new PixelRDO_Container(m_pixel_id->wafer_hash_max());
    m_rdoContainer->addRef();
  }

  return sc;
}

StatusCode PixelClusterCacheTool::finalize() 
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

StatusCode PixelClusterCacheTool::convertBStoClusters(std::vector<const ROBF*>& robFrags,
                                                      const std::vector<IdentifierHash>& listOfPixIds,
							std::vector<int>& errorVect, bool isFullScan)
{
  if(!evtStore()->contains<InDet::PixelClusterContainer>(m_containerName))
    {
      m_clusterContainer->cleanup();
      StatusCode sc=evtStore()->record(m_clusterContainer,m_containerName,false);
      if(sc.isFailure())
	{
	  ATH_MSG_WARNING("Pixel Cluster Container " << m_containerName 
	      <<" cannot be recorded in StoreGate !");
	  return StatusCode::FAILURE;
	}
      else 
	{
	    ATH_MSG_DEBUG("Pixel Cluster Container " << m_containerName
		<< " is recorded in StoreGate");
	}
      if(m_doBS) {
	m_rdoContainer->cleanup();
	sc=evtStore()->record(m_rdoContainer,m_rdoContainerName,false);
	if(sc.isFailure())
	  {
	    ATH_MSG_WARNING("PixelRDO Container " << m_rdoContainerName 
		<<" cannot be recorded in StoreGate !");
	    return StatusCode::FAILURE;
	  }
	else 
	  {
	      ATH_MSG_DEBUG("PixelRDO Container " << m_rdoContainerName
		  << " is recorded in StoreGate");
	  }
      }
    }
  else 
    {    
	ATH_MSG_DEBUG("Pixel Cluster Container " <<m_containerName 
	    << " is found in StoreGate");
    }

  if(!m_doBS) {

    //retrieve m_rdoContainer here 

    if(!evtStore()->contains<PixelRDO_Container>(m_rdoContainerName)) {
      ATH_MSG_ERROR("Pixel RDO Container " << m_rdoContainerName
	  <<" is not found in StoreGate !");
      return StatusCode::FAILURE;
    }
    const PixelRDO_Container* pCont = NULL; 
    StatusCode sc = evtStore()->retrieve(pCont, m_rdoContainerName);
    if (sc.isFailure()) {
      ATH_MSG_ERROR("retrieval of Pixel RDO Container " << m_rdoContainerName
	  <<" failed");
      return sc;
    }

    m_rdoContainer = const_cast<PixelRDO_Container*>(pCont);

    if(m_rdoContainer==NULL) {
      ATH_MSG_ERROR("Pixel RDO Container const_cast failed ");
      return StatusCode::FAILURE;
    }
  }

  std::vector<IdentifierHash> reducedList;
  reducedList.clear();

  for(std::vector<IdentifierHash>::const_iterator it=listOfPixIds.begin();it!=listOfPixIds.end();++it) {
    if(m_clusterContainer->indexFind((*it))==m_clusterContainer->end())
      reducedList.push_back(*it);
  }

  // std::copy(reducedList.begin(),reducedList.end(),std::ostream_iterator<IdentifierHash>(std::cout, "\n"));
  bool bs_failure=false;
  int n_rob_count=0;
  int n_recov_errors=0;

  if(m_doBS) {
    if(robFrags.size() == 0) {
	ATH_MSG_DEBUG(" There are NO ROB data ");
      return StatusCode::SUCCESS;
    }    
      ATH_MSG_DEBUG(" There are SOME ROB data " <<robFrags.size());
    
    std::vector<bool> idList(m_pixel_id->wafer_hash_max(), false);
    
    IROBDataProviderSvc::VROBFRAG::const_iterator rob_it = robFrags.begin(); 
    IROBDataProviderSvc::VROBFRAG::const_iterator rob_it_end = robFrags.end(); 
    
    m_clusterization.initialize();
    
    for(; rob_it != rob_it_end; ++rob_it) 
      { 
	uint32_t rodid = (*rob_it)->rod_source_id() ; 
	eformat::helper::SourceIdentifier sid_rob(rodid) ;
	uint32_t detid = sid_rob.subdetector_id() ;
  ATH_MSG_DEBUG("  ROB source ID SID " <<std::hex<<rodid <<std::dec);
	
	const ROBFragment* robFrag = (*rob_it); 
	  ATH_MSG_DEBUG(std::hex<<"Det ID 0x"<<detid<<" Rod version 0x"<<
	    robFrag->rod_version()<<", Type="<<robFrag->rod_detev_type()<<std::dec);
	
	m_bsErrorSvc->resetCounts();
	
	StatusCode scRod(StatusCode::SUCCESS);
	if(!isFullScan) {
	  scRod=m_offlineDecoder->fillCollection(robFrag,m_rdoContainer,&reducedList);
	}
	else {
	  scRod=m_offlineDecoder->fillCollection(robFrag,m_rdoContainer,NULL);
	}
	
	if(scRod.isRecoverable())
	  {
	    n_recov_errors++;
	    for(int idx=0;idx<MAXNUM_PIX_BS_ERRORS;idx++)
	      {
		int n_errors = m_bsErrorSvc->getNumberOfErrors(idx);
		for(int ierror = 0; ierror < n_errors; ierror++)
		  errorVect.push_back(idx);
	      }
	  }
	else if(scRod.isFailure())
	  {
	    bs_failure=true;
	    break;
	  }
	++n_rob_count;
      }
  } 
  std::vector<InDet::PixelClusterCollection*> offlineCollVector;
  offlineCollVector.clear();

  int npixels=0;
  for (std::vector<IdentifierHash>::iterator it=reducedList.begin(); it != reducedList.end(); ++it)
    {
      PixelRDO_Container::const_iterator collIt=m_rdoContainer->indexFind((*it));
      if(collIt==m_rdoContainer->end())
	continue;
      if((*collIt)->size()!=0)
	{		  
	  if(!m_useOfflineClustering)
	    {
	      Identifier coll_id=(*collIt)->identify();
	      IdentifierHash coll_hash_id=(*collIt)->identifyHash();
	      for(PixelRDO_Collection::const_iterator rdoIt=(*collIt)->begin();rdoIt!=(*collIt)->end();
		  ++rdoIt)
		{
		  Identifier pixId=(*rdoIt)->identify();
		      
		  npixels++;
		  m_clusterization.addHit(coll_id,coll_hash_id,
					  m_pixel_id->phi_index(pixId),
					  m_pixel_id->eta_index(pixId),(*rdoIt));
		}
	    }
	  else 
	    {
	      const InDetRawDataCollection<PixelRDORawData>* pRdoColl = (*collIt);
	      InDet::PixelClusterCollection* pColl=m_clusteringTool->clusterize(*pRdoColl,*p_indet_mgr,*m_pixel_id);
	      if(pColl!=NULL)
		offlineCollVector.push_back(pColl);
	    }
	}
    }
  if(!m_useOfflineClustering) {
    if(npixels!=0) m_clusterization.finishHits();
  }
  std::vector<InDet::PixelClusterCollection*>& vColl=m_clusterization.getClusterCollections();

  if(m_useOfflineClustering) vColl=offlineCollVector;

    ATH_MSG_DEBUG("No of collections found: " <<  vColl.size());
  
  // Loop over cluster collections
  std::vector<InDet::PixelClusterCollection*>::iterator iter_coll = vColl.begin();
  for(;iter_coll!=vColl.end();iter_coll++) 
    {
      IdentifierHash hashId=(*iter_coll)->identifyHash();
      StatusCode sc=m_clusterContainer->addCollection((*iter_coll),hashId);
      if(sc.isFailure())
	{
	    ATH_MSG_INFO("IDC record failed HashId="<<hashId);
	}
    }
  if(bs_failure)
    {
      ATH_MSG_DEBUG(" FAILURE in PixelRodDecoder");
      return StatusCode::FAILURE;
    }
  else 
    {
      if(n_recov_errors!=0)
	{
    ATH_MSG_DEBUG(" number of ROBs used  "<<n_rob_count<< "Total number of recoverable errors "<<n_recov_errors);
	  return StatusCode::RECOVERABLE;
	}
      else return StatusCode::SUCCESS;
    }
}

