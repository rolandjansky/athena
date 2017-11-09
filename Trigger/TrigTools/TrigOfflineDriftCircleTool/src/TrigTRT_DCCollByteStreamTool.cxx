/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetIdentifier/TRT_ID.h"
#include "Identifier/IdentifierHash.h"

#include "InDetPrepRawData/TRT_DriftCircleCollection.h"
#include "TRT_DriftCircleTool/ITRT_DriftCircleTool.h"
#include "TRT_RawDataByteStreamCnv/ITRT_RodDecoder.h"
#include "TRT_Cabling/ITRT_CablingSvc.h"
#include "TrigOfflineDriftCircleTool/TrigTRT_DCCollByteStreamTool.h"
#include "IRegionSelector/IRoiDescriptor.h"


#include <map>
#include <math.h> 
#include <string> 
#include <sstream>

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

static const InterfaceID IID_ITrigTRT_DCCollByteStreamTool
  ("TrigTRT_DCCollByteStreamTool", 1, 0);

const InterfaceID& TrigTRT_DCCollByteStreamTool::interfaceID()
{
  return IID_ITrigTRT_DCCollByteStreamTool;
}


  // Contructor
TrigTRT_DCCollByteStreamTool::TrigTRT_DCCollByteStreamTool
  (const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type, name, parent),
    m_TRT_IdConvTool("TRT_CablingSvc",this->name()),
    m_decoder("TRT_RodDecoder"),
    m_driftCircleTool("InDet::TRT_DriftCircleTool/InDetTrigTRT_DriftCircleTool")
{
  declareInterface<TrigTRT_DCCollByteStreamTool >(this);
  declareProperty( "TRT_DriftCircleContainerName", m_trtDriftCircleContainerName = "Trig_OfflineDriftCircles" );
  declareProperty( "TRTBadChannels", m_trtBadChannels = false );
  declareProperty( "RDO_ContainerName", m_rdoContainerName = "TRT_RDO_Cache");
  declareProperty( "TRT_DriftCircleToolName",m_driftCircleTool);
  declareProperty( "TRTRodDecoder", m_decoder);
}


  // Destructor
TrigTRT_DCCollByteStreamTool::~TrigTRT_DCCollByteStreamTool() {}


StatusCode TrigTRT_DCCollByteStreamTool::initialize()
{
  StatusCode sc = AthAlgTool::initialize();
  if(sc.isFailure()){
    return StatusCode::FAILURE;
  }

  sc=AthAlgTool::detStore()->retrieve(m_trt_id, "TRT_ID");

  if(sc != StatusCode::SUCCESS) 
  {
    msg(MSG::ERROR) << "Failed to retrieve InDetDD::TRT ID helper"<< endmsg;
    return StatusCode::FAILURE;
  }
  m_cntx = m_trt_id->straw_layer_context();


  sc = m_driftCircleTool.retrieve();

  if(sc.isFailure()) {
    msg(MSG::ERROR) << "Could not get TRT_DriftCircleTool"<<endmsg;
    return sc;
  }
 
  sc = m_decoder.retrieve();

  if(sc.isFailure()) {
    msg(MSG::ERROR) << "Could not get TRT_RodDecoder tool"<<endmsg;
    return sc;
  }

  sc = m_TRT_IdConvTool.retrieve();


  if(sc.isFailure()) {
    msg(MSG::ERROR) << "Could not get TRT_CablingSvc"<<endmsg;
    return sc;
  }

  ITrigTimerSvc* timerSvc;
  StatusCode scTime = service( "TrigTimerSvc", timerSvc);
  if( scTime.isFailure() ) {
    msg(MSG::INFO) << "Unable to locate Service TrigTimerSvc " << endmsg;
    m_timers = false;
  } 
  else{
    m_timers = true;  
  }
  if ( m_timers ) {
    m_timer[0] = timerSvc->addItem("TRTDCTool.Total");
    m_timer[0]->propName("TRTDCC.nROBS");
    m_timer[1] = timerSvc->addItem("TRTDCTool.CreateDC");
    m_timer[1]->propName("TRTDCC.nDrCircles");
    m_timer[2] = timerSvc->addItem("TRTDCTool.CreateColl");
    m_timer[2]->propName("TRTDCC.nColl");
    //m_timer[1]->propName("GetPixCache.nPixReq");
  }

  if (msgLvl(MSG::INFO)) 
    msg() << " Create new TRT_DriftCircleContainer with trtDriftCircleContainerName: " << m_trtDriftCircleContainerName << endmsg;


  m_trtDriftCircleContainer = new InDet::TRT_DriftCircleContainer(m_trt_id->straw_layer_hash_max());
  m_trtDriftCircleContainer->addRef();

  m_rdoContainer = new TRT_RDO_Container(m_trt_id->straw_layer_hash_max());
  m_rdoContainer->addRef();

  return sc;
}
	
StatusCode TrigTRT_DCCollByteStreamTool::finalize()
{
//   if(m_storeGate->contains<InDet::TRT_DriftCircleContainer>(m_trtDriftCircleContainerName)) {
   m_trtDriftCircleContainer->cleanup();
   m_trtDriftCircleContainer->release();
   m_rdoContainer->cleanup();
   m_rdoContainer->release();
//   }
   StatusCode sc = AlgTool::finalize();
   if(sc.isFailure()){
     return StatusCode::FAILURE;
   } 
   return sc;
}


StatusCode TrigTRT_DCCollByteStreamTool::convert(VROBDATA& vRobData,
		 const std::vector<IdentifierHash> listIds)
{
  

  StatusCode scData(StatusCode::SUCCESS);
  IdentifierHash IdHash;
  
  bool bs_failure=false;
  int n_recov_errors=0;

  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << " Making TRT ClusterCollections for " <<listIds.size()<<" DetElements" 
	<< endmsg;
    msg() << " Number of ROBS = " << vRobData.size() << endmsg;
  }
  int nrob =0;
  //int nColl =0;
  //int nDC =0;
  if(m_timers) 
    {
      m_timer[0]->start();
    }

  if(!evtStore()->contains<InDet::TRT_DriftCircleContainer>(m_trtDriftCircleContainerName))
    {
      m_trtDriftCircleContainer->cleanup();
      StatusCode sc=evtStore()->record(m_trtDriftCircleContainer,m_trtDriftCircleContainerName,false);
      if(sc.isFailure())
	{
	  msg(MSG::WARNING) << "TRT DriftCircleContainer " << m_trtDriftCircleContainerName <<
	    " cannot be recorded into StoreGate! "<<endmsg;
	  return sc;
	} 
      else 
	{ 
	  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) <<
	    "TRT DriftCircleContainer " << m_trtDriftCircleContainerName <<
            "  recorded into StoreGate! "<<endmsg;
	}
      m_rdoContainer->cleanup();
      sc=evtStore()->record(m_rdoContainer,m_rdoContainerName,false);
      if(sc.isFailure())
	{
	  msg(MSG::WARNING) << "TRT_RDO Container " << m_rdoContainerName 
	      <<" cannot be recorded in StoreGate !"<< endmsg;
	  return StatusCode::FAILURE;
	}
      else 
	{
	  if(msgLvl(MSG::DEBUG))
	    msg(MSG::DEBUG) << "TRT_RDO Container " << m_rdoContainerName
		<< " is recorded in StoreGate" << endmsg;
	}
    } 
  else 
    {
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) <<
				 "TRT DriftCircleContainer " << m_trtDriftCircleContainerName <<
				 "  is found in StoreGate "<<endmsg;
    }
  
  std::vector<IdentifierHash> reducedList;
  reducedList.clear();

  if(vRobData.size() == 0) 
    { 
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "0.0 Collections made for this ROB " <<endmsg; 
      return scData;
    }
 
  for(std::vector<IdentifierHash>::const_iterator it=listIds.begin();it!=listIds.end();++it)
    {
      if(m_trtDriftCircleContainer->indexFind((*it))==m_trtDriftCircleContainer->end())
	reducedList.push_back(*it);
    }
      
  VROBDATA::const_iterator rob_it = vRobData.begin(); 
  VROBDATA::const_iterator rob_it_end = vRobData.end();  
  
  for(; rob_it!=rob_it_end; ++rob_it)
    {
      const ROBFragment* robFrag = rob_it->getROBFragment() ;

      scData=m_decoder->fillCollection(robFrag, m_rdoContainer, &reducedList);

      if(scData.isRecoverable())
	{ 
	  n_recov_errors++;
	}
      else if(scData.isFailure())
	{
	  bs_failure=true;
	  break;
	}
      ++nrob; 
    } //end of loop over ROBs  
  
  
  int RecDriftCircles=0;
    
  for (std::vector<IdentifierHash>::iterator it=reducedList.begin(); it != reducedList.end(); ++it)
    {
      TRT_RDO_Container::const_iterator rdo_it=m_rdoContainer->indexFind((*it));
      if(rdo_it==m_rdoContainer->end())
	continue;
      if(msgLvl(MSG::VERBOSE)) {
        msg() << " Collection " << (*it)
	      << " has " << (*rdo_it)->size() << " RDOs " << endmsg;
        msg() << " adding DC collection size= " << (*rdo_it)->size() << " hashId=" <<
	           (*rdo_it)->identifyHash()<<endmsg;
      }
      
      if((*rdo_it)->size() !=0 ) { 
	  const InDet::TRT_DriftCircleCollection* p_rio = 
          m_driftCircleTool->convert(0,(*rdo_it),m_trtBadChannels);
	  
	  RecDriftCircles+=p_rio->size();
	  
	  if(p_rio->size()!=0) {
	    StatusCode sc=m_trtDriftCircleContainer->addCollection(p_rio,p_rio->identifyHash());
	    if(sc.isFailure()) {
            if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << 
		" failed register TRT_DriftCircle collection (hashId)"<<
		  p_rio->identifyHash()<<endmsg;
	    delete p_rio;

	    } else { 
             if(msgLvl(MSG::DEBUG)) {
                msg(MSG::DEBUG) << " Store a TRT_DriftCircle collection  with Key "
		      << p_rio->identifyHash()<<endmsg;
		  
	        InDet::TRT_DriftCircleContainer::const_iterator
		    dcCollIt=m_trtDriftCircleContainer->indexFind(p_rio->identifyHash());
	        if(dcCollIt==m_trtDriftCircleContainer->end())
		  {
		    msg() << "not found for hashId="<<p_rio->identifyHash()<<endmsg;
		    continue;
		  }
	         msg() << "Found, size="<<(*dcCollIt)->size()<<endmsg;
	     }
	    }
	  } else {
	      delete (p_rio); 
	  }
      }
    }

  if (msgLvl(MSG::DEBUG)) 
    msg(MSG::DEBUG) << "  Reconstructed  RecDriftCircles "<<RecDriftCircles
       <<" in the ROI "<<endmsg;
    

  if(msgLvl(MSG::DEBUG))
    {
      if(bs_failure)
	{
	  msg() <<" FAILURE in TRT_RodDecoder"<<endmsg;
	}
      else
	  msg() <<"Total number of recoverable errors "<<n_recov_errors<<endmsg;
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
