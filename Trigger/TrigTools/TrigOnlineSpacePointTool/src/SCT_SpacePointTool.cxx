/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigSiSpacePointCollection.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "TrigOnlineSpacePointTool/SCT_SpacePointTool.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "Identifier/IdentifierHash.h" 
#include "StoreGate/StoreGate.h"
#include <string>
#include "TrigTimeAlgs/TrigTimerSvc.h"

static const InterfaceID IID_ISCT_SpacePointTool
            ("SCT_SpacePointTool", 136, 0);

const InterfaceID& SCT_SpacePointTool::interfaceID() { 
  return IID_ISCT_SpacePointTool; 
}

SCT_SpacePointTool::SCT_SpacePointTool( const std::string& type, 
                                        const std::string& name, 
                                        const IInterface* parent )
  : AlgTool(type, name, parent),
    m_xVertex(0.0),
    m_yVertex(0.0),
    m_zVertex(0.0),
    m_xCenter(0.0),
    m_yCenter(0.0),
    m_iBeamCondSvc(0),
    m_useBeamSpot(true),
    m_useOfflineAlgorithm(true),
    m_numberingTool("TrigL2LayerNumberTool")
{

  declareInterface< SCT_SpacePointTool>( this );
  declareProperty("AllowUnassociatedPhiPoints",m_unassociatedPhi=false);
  declareProperty( "SCT_SP_ContainerName", m_spacepointContainerName = "TrigSCT_SpacePoints" );
  declareProperty("UseBeamSpot", m_useBeamSpot);
  declareProperty("UseOfflineAlgorithm", m_useOfflineAlgorithm);
  declareProperty("VertexX", m_xVertex);
  declareProperty("VertexY", m_yVertex);
  declareProperty("VertexZ", m_zVertex);

}

StatusCode SCT_SpacePointTool::initialize()  {
  MsgStream log(msgSvc(), name());

  log << MSG::DEBUG << name() << " in initialize" << endreq;
  // Get StoreGate service
  StatusCode sc=service("StoreGateSvc", m_StoreGate);
  if (sc.isFailure()) {
    log << MSG::ERROR << name() <<" failed to get StoreGate" << endreq;
    return sc;
  }
  
  // get DetectorStore service
  StoreGateSvc* detStore; 
  sc=service("DetectorStore",detStore);
  if (sc.isFailure()) { 
    log << MSG::ERROR << name() <<" failed to get detStore" << endreq;
    return sc;
  }

  
  sc=detStore->retrieve(m_mgr, "SCT");
  if (sc.isFailure()) {
    log << MSG::ERROR << name() << "failed to get SCT Manager" << endreq;
    return StatusCode::FAILURE;
  }

  // Get SCT  helpers                                                                                                                         
  if (detStore->retrieve(m_id_sct, "SCT_ID").isFailure()) { 
     log << MSG::FATAL << "Could not get SCT ID helper" << endreq; 
     return StatusCode::FAILURE; 
  }  

  sc=m_numberingTool.retrieve();
  if(sc.isFailure()) {
    log << MSG::ERROR<<"Could not retrieve "<<m_numberingTool<<endreq;
    return sc;
  }
  
  m_builder = new SCT_GCBuilder(m_mgr,m_id_sct,m_useOfflineAlgorithm,
				m_numberingTool->offsetBarrelSCT(),
				m_numberingTool->offsetEndcapSCT());

  m_cntx_sct = m_id_sct->wafer_context();

  sc = AlgTool::initialize(); 

  if (m_unassociatedPhi) {
    log << MSG::INFO << name() << "  Forming spacepoints from stereo pairs and unassociated phi clusters" << endreq;
  } else {
    log << MSG::INFO << name() << "  Forming spacepoints from stereo pairs only" << endreq;
  }

  log << MSG::INFO << "SCT_SP_ContainerName: " << m_spacepointContainerName << endreq;
  m_spacepointContainer = new TrigSiSpacePointContainer(m_id_sct->wafer_hash_max());
  m_spacepointContainer->addRef();

  if (m_useBeamSpot)
    {
      StatusCode scBS = service("BeamCondSvc", m_iBeamCondSvc);
      if (scBS.isFailure() || m_iBeamCondSvc == 0) 
	{
	  m_iBeamCondSvc = 0;
	  log << MSG::WARNING << "Could not retrieve Beam Conditions Service. " << endreq;
	  log << MSG::WARNING << "Using origin at ( "<< m_xVertex <<" , "<< m_yVertex << " , " 
	      << m_yVertex<< " ) "<< endreq;
	  m_xCenter = m_xVertex;
	  m_yCenter = m_yVertex;
	}
    }
  else {
    m_xCenter = m_xVertex;
    m_yCenter = m_yVertex;
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
     m_timer[0] = timerSvc->addItem("FormSCTSP");
     m_timer[0]->propName("FormSCTSP.nSCTSP");
     m_timer[1] = timerSvc->addItem("SCTLookUp");
     m_timer[1]->propName("SCTLookUp.nSctReq");
     m_timer[2] = timerSvc->addItem("SctRec");
     m_timer[2]->propName("SctRec.nSctRec");
  }
  return sc;
}

StatusCode SCT_SpacePointTool::finalize() {

  m_spacepointContainer->cleanup();
  //delete m_spacepointContainer;
  m_spacepointContainer->release();
  delete m_builder;
  StatusCode sc = AlgTool::finalize(); 
  return sc;
}

StatusCode SCT_SpacePointTool::fillCollections(ClusterCollectionVector& clusterCollData,std::vector<int>& listOfIds) 
{ 

  StatusCode sc;

  MsgStream log(msgSvc(), name());
  int outputLevel = msgSvc()->outputLevel( name() );
  std::map <Identifier,  PhiUVPair> phi_uv_table;
  std::map <Identifier,  PhiUVPair>::iterator iter_phiUV;

  int nSP=0;listOfIds.clear();

  if(!m_StoreGate->contains<TrigSiSpacePointContainer>(m_spacepointContainerName))
    {
      m_spacepointContainer->cleanup();
      sc=m_StoreGate->record(m_spacepointContainer,m_spacepointContainerName,false);
      if(sc.isFailure())
	{
	  log << MSG::WARNING << "SCT TrigSP Container " << m_spacepointContainerName 
	      <<" cannot be recorded in StoreGate !"<< endreq;
	  return sc;
	}
      else 
	{
	  if(outputLevel <= MSG::DEBUG)
	    log << MSG::DEBUG << "SCT TrigSP Container " << m_spacepointContainerName
		<< " is recorded in StoreGate" << endreq;
	}
      if (m_useBeamSpot && m_iBeamCondSvc)
	{
	  Amg::Vector3D vertex = m_iBeamCondSvc->beamPos();
	  if(outputLevel <= MSG::DEBUG)
	    log << MSG::DEBUG << "Beam spot position " << vertex << endreq;
	  m_xCenter=vertex.x() - m_iBeamCondSvc->beamTilt(0)*vertex.z();
	  m_yCenter=vertex.y() - m_iBeamCondSvc->beamTilt(1)*vertex.z();
	}
      else {
	m_xCenter = m_xVertex;
	m_yCenter = m_yVertex;
      }
    }
  else 
    {    
      if(outputLevel <= MSG::DEBUG)
	log << MSG::DEBUG << "SCT TrigSP Container " <<m_spacepointContainerName 
	    << " is found in StoreGate" << endreq;
    }

  if(outputLevel <= MSG::DEBUG)
    log << MSG::DEBUG << "Center position:  " << m_xCenter <<"  "<<m_yCenter<< endreq;

  // Loop over cluster collections 
  ClusterCollectionVector::iterator iter_coll = clusterCollData.begin();
  //if ( m_timers ) m_timer[1]->start();

  for (; iter_coll != clusterCollData.end(); iter_coll++) {

    Identifier wafer_id = (*iter_coll)->identify();
    Identifier module_id =  m_id_sct->wafer_id(m_id_sct->barrel_ec(wafer_id),
					       m_id_sct->layer_disk(wafer_id),
					       m_id_sct->phi_module(wafer_id),
					       m_id_sct->eta_module(wafer_id),
					       0);
    bool phi_wafer_found = false;

    // Find out if it is a phi wafer   
    const InDetDD::SiDetectorElement* element=m_mgr->getDetectorElement(wafer_id);
    if(!element->isStereo()) phi_wafer_found = true;

    /*
    if (m_id_sct->is_barrel(wafer_id)) {
      if (m_id_sct->side(wafer_id) == 1) {
	phi_wafer_found = true;   
	if (module_id == wafer_id) {
	  log << MSG::ERROR << " Failure of SCT_ID helper: module-id has side=1 !!" << endreq; 
	  log << MSG::ERROR << " module-id: " <<  m_id_sct->print_to_string(module_id) << endreq;
	  log << MSG::ERROR << " Stereo association failed " << endreq;
	}
      } 
    } else {
      if (m_id_sct->side(wafer_id) == 0) {
	phi_wafer_found = true;
      }	else if (module_id == wafer_id) {
	log << MSG::ERROR << " Failure of SCT_ID helper: module-id has side=1 !!" << endreq; 
	log << MSG::ERROR << " module-id: " <<  m_id_sct->print_to_string(module_id) << endreq;
	log << MSG::ERROR << " Stereo association failed " << endreq;
      }
    }
    */

    // Fill in phi_uv_table
    if (phi_wafer_found) 
      {
	iter_phiUV = phi_uv_table.find(module_id);
	if (iter_phiUV == phi_uv_table.end()) 
	  {
	    PhiUVPair wafer_pair((*iter_coll), 0);
	    phi_uv_table.insert(std::make_pair(module_id, wafer_pair)); 
	  } 
	else 
	  {
	    ((*iter_phiUV).second).addPhi(*iter_coll);
	  }
      } 
    else 
      {
	//log << MSG::DEBUG << " got stereo wafer " <<  m_id_sct->print_to_string(wafer_id) << endreq;
	//log << MSG::DEBUG << " module-id: " <<  m_id_sct->print_to_string(module_id) << endreq;
	iter_phiUV = phi_uv_table.find(module_id);
	if (iter_phiUV == phi_uv_table.end()) 
	  {
	    PhiUVPair wafer_pair(0, (*iter_coll));
	    phi_uv_table.insert(std::make_pair(module_id, wafer_pair));  
	  } 
	else 
	  {
	    ((*iter_phiUV).second).addUV(*iter_coll);
	  }        
      }
  }

  int nRec=0,nReq=0;
  if ( m_timers ) 
    {
      m_timer[0]->start();
      m_timer[0]->pause();
      m_timer[1]->start();
      m_timer[1]->pause();
      m_timer[2]->start();
      m_timer[2]->pause();
    }


  
  // Convert clusters in the table to  space points
  for (iter_phiUV = phi_uv_table.begin(); iter_phiUV != phi_uv_table.end(); iter_phiUV++) 
    {

    if ( ((*iter_phiUV).second).phiWafer() == 0) continue;
  
    // Select cached space points 

    const Identifier& phiWaferId = (((*iter_phiUV).second).phiWafer())->identify(); 
    IdentifierHash hashId;
    m_id_sct->get_hash(phiWaferId, hashId, &m_cntx_sct);

    nReq++;

    if( m_timers ) m_timer[1]->resume();
      
    TrigSiSpacePointContainer::const_iterator spCollIt = m_spacepointContainer->indexFind(hashId);
    
    if ( m_timers ) m_timer[1]->pause();
    
    if(spCollIt!=m_spacepointContainer->end()) 
      {	
	if(((*iter_phiUV).second).uvWafer() != 0) {
	  listOfIds.push_back(hashId);
	}
	continue;
      }

    SiSpacePointData newSpacePointData; 
    // Space point formation
    //    log << MSG::DEBUG  << " Form Space-points for a SCT Phi collection id " << 
    //  m_id_sct->print_to_string(phiWaferId) << endreq;
    if ( ((*iter_phiUV).second).uvWafer() == 0 ) 
      {
	if (m_unassociatedPhi) 
	  m_builder->formSpacePoints( *(((*iter_phiUV).second).phiWafer()), newSpacePointData ); 
      } 
    else 
      {
	if ( m_timers ) m_timer[0]->resume();
	/*
	  const Identifier& uvWaferId = (((*iter_phiUV).second).uvWafer())->identify(); 
	  log << MSG::DEBUG  << " and SCT uv collection id " << 
	  m_id_sct->print_to_string(uvWaferId) << endreq;
	*/
	m_builder->formSpacePoints( *(((*iter_phiUV).second).phiWafer()), 
				    *(((*iter_phiUV).second).uvWafer()),
				    m_unassociatedPhi, newSpacePointData);  
	if ( m_timers ) m_timer[0]->pause();
      } 

    if(!newSpacePointData.empty())
      {
	if ( m_timers ) m_timer[2]->resume();
	nRec++;

	TrigSiSpacePointCollection* spacePointColl = new TrigSiSpacePointCollection(hashId);
	spacePointColl->setIdentifier(phiWaferId); 
	listOfIds.push_back(hashId);

	nSP+=newSpacePointData.size();

	SiSpacePointData::iterator iter_sp;
	for( iter_sp = newSpacePointData.begin(); 
	     iter_sp != newSpacePointData.end(); iter_sp++) 
	  {

	    TrigSiSpacePoint* pSP = *iter_sp;

	    //shift spacepoint here 

	    double xs = pSP->x() - m_xCenter;
	    double ys = pSP->y() - m_yCenter;
	    double Phi = atan2(ys,xs);
	    double r = sqrt(xs*xs+ys*ys);
	    pSP->phi(Phi);
	    pSP->r(r);
	    pSP->x(xs);
	    pSP->y(ys);

	    spacePointColl->push_back(pSP);
	  }  	      
	sc=m_spacepointContainer->addCollection(spacePointColl,
						spacePointColl->identifyHash());
	if (sc.isFailure()) 
	  {
	    log << MSG::WARNING  << " Fail to store a TrigSiSpacePoint collection in SCT with hash "
		<< hashId << endreq;
	  } 
	if ( m_timers ) m_timer[2]->pause();
      }
    if(outputLevel<=MSG::DEBUG)
      log << MSG::DEBUG  << newSpacePointData.size() << " SpacePoints formed for hashId="<<hashId << endreq; 
    }

  if ( m_timers ) 
    {
      m_timer[0]->propVal(nSP);
      m_timer[0]->stop();
      m_timer[1]->propVal(nReq);
      m_timer[1]->stop();
      m_timer[2]->propVal(nRec);
      m_timer[2]->stop();
    }
  return sc;
}


