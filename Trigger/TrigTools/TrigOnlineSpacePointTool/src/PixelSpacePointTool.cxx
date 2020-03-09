/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigSiSpacePointCollection.h"
#include "PixelSpacePointTool.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "InDetIdentifier/PixelID.h"
#include "Identifier/IdentifierHash.h" 
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include <string>

static const InterfaceID IID_IPixelSpacePointTool
            ("PixelSpacePointTool", 135, 0);

const InterfaceID& PixelSpacePointTool::interfaceID() { 
  return IID_IPixelSpacePointTool; 
}

PixelSpacePointTool::PixelSpacePointTool( const std::string& type, 
                                          const std::string& name, 
                                          const IInterface* parent )
  : AthAlgTool(type, name, parent),
    m_xVertex(0.0),
    m_yVertex(0.0),
    m_zVertex(0.0),
    m_xCenter(0.0),
    m_yCenter(0.0),
    m_useBeamSpot(true),
    m_numberingTool("TrigL2LayerNumberTool")
{

  declareInterface< PixelSpacePointTool>( this );
  declareProperty( "PixelSP_ContainerName", m_spacepointContainerName = "TrigPixelSpacePoints" );
  declareProperty("UseBeamSpot", m_useBeamSpot);
  declareProperty("VertexX", m_xVertex);
  declareProperty("VertexY", m_yVertex);
  declareProperty("VertexZ", m_zVertex);
}

StatusCode PixelSpacePointTool::initialize()  {
  ATH_MSG_DEBUG(name() << " in initialize");

  // Get pixel helper

  if (detStore()->retrieve(m_id_pixel, "PixelID").isFailure()) {
     ATH_MSG_FATAL("Could not get Pixel ID helper"); 
     return StatusCode::FAILURE;
  } 

  m_cntx_pixel = m_id_pixel->wafer_context();

  StatusCode sc = AthAlgTool::initialize(); 

  sc=m_numberingTool.retrieve();
  if(sc.isFailure()) {
    ATH_MSG_ERROR("Could not retrieve "<<m_numberingTool);
    return sc;
  }

  ATH_CHECK(m_pixelDetEleCollKey.initialize());

  m_builder = new PixelGCBuilder(m_id_pixel, m_numberingTool->offsetEndcapPixels());

  ATH_MSG_INFO("PixelSP_ContainerName: " << m_spacepointContainerName);
  m_spacepointContainer = new TrigSiSpacePointContainer(m_id_pixel->wafer_hash_max());
  m_spacepointContainer->addRef();

  if (m_useBeamSpot)
  {
    if(m_beamSpotKey.initialize().isFailure()) return StatusCode::FAILURE;
  }
  else {
    m_xCenter = m_xVertex;
    m_yCenter = m_yVertex;
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
    m_timer[0] = timerSvc->addItem("FormPixSP");
    m_timer[0]->propName("FormPixSP.nPixSP");
    m_timer[1] = timerSvc->addItem("PixLookUp");
    m_timer[1]->propName("PixLookUp.nPixReq");
    m_timer[2] = timerSvc->addItem("PixRec");
    m_timer[2]->propName("PixRec.nPixRec");
  }

  return sc;
}

StatusCode PixelSpacePointTool::finalize() 
{
  m_spacepointContainer->cleanup();
  //delete m_spacepointContainer;
  m_spacepointContainer->release();
  delete m_builder;

  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}

StatusCode PixelSpacePointTool::fillCollections(ClusterCollectionData& clusterCollData) 
{ 

  // Get PixelDetectorElementCollection
  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> pixelDetEle(m_pixelDetEleCollKey);
  const InDetDD::SiDetectorElementCollection* elements(pixelDetEle.retrieve());
  if (elements==nullptr) {
    ATH_MSG_FATAL(m_pixelDetEleCollKey.fullKey() << " could not be retrieved");
    return StatusCode::FAILURE;
  }

  StatusCode sc;
  int nSP=0;

  if(!evtStore()->contains<TrigSiSpacePointContainer>(m_spacepointContainerName))
    {
      m_spacepointContainer->cleanup();

      sc=evtStore()->record(m_spacepointContainer,m_spacepointContainerName,false);
      if(sc.isFailure())
	{
	  ATH_MSG_WARNING("Pixel TrigSP Container " << m_spacepointContainerName 
	      <<" cannot be recorded in StoreGate !");
	  return sc;
	}
      else 
	{
	    ATH_MSG_DEBUG("Pixel TrigSP Container " << m_spacepointContainerName
		<< " is recorded in StoreGate");
	}
      if (m_useBeamSpot)
	{
          SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
	  const Amg::Vector3D &vertex = beamSpotHandle->beamPos();
          ATH_MSG_DEBUG("Beam spot position " << vertex); 
	  m_xCenter=vertex.x() - beamSpotHandle->beamTilt(0)*vertex.z();
	  m_yCenter=vertex.y() - beamSpotHandle->beamTilt(1)*vertex.z();
	}
      else {
	m_xCenter = m_xVertex;
	m_yCenter = m_yVertex;
      }
    }
  else 
    {    
	ATH_MSG_DEBUG("Pixel TrigSP Container " <<m_spacepointContainerName 
	    << " is found in StoreGate");
    }

  int nReq=0,nRec=0;
  if ( m_timers ) 
    {
      m_timer[1]->start();
      m_timer[1]->pause();
      m_timer[2]->start();
      m_timer[2]->pause();
      m_timer[0]->start();
      m_timer[0]->pause();
    }

  ATH_MSG_DEBUG("Center position:  " << m_xCenter <<"  "<<m_yCenter);

  ClusterCollectionData::iterator iter_coll = clusterCollData.begin();

  // Loop over cluster collections 
  for(;iter_coll!=clusterCollData.end();iter_coll++) 
    {
      // Get cluster collection identifier 
     
      Identifier elementId = (*iter_coll)->identify();
      IdentifierHash hashId=(*iter_coll)->identifyHash();
//      m_id_pixel->get_hash(elementId, hashId, &m_cntx_pixel);
      nReq++;

      if( m_timers ) m_timer[1]->resume();

      TrigSiSpacePointContainer::const_iterator spCollIt;
      spCollIt = m_spacepointContainer->indexFind(hashId);
      if ( m_timers ) m_timer[1]->pause();
      if(spCollIt!=m_spacepointContainer->end())
	  continue;

      SiSpacePointData newSpacePointData;

      if ( m_timers ) m_timer[0]->resume();
      m_builder->formSpacePoints(*(*iter_coll), elements, newSpacePointData);
      if ( m_timers ) m_timer[0]->pause();

      if(!newSpacePointData.empty()) 
	{
	  if ( m_timers ) m_timer[2]->resume();
	  nRec++;
	  TrigSiSpacePointCollection* spacePointColl = new TrigSiSpacePointCollection(hashId);
	  spacePointColl->setIdentifier(elementId); 

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

	  sc=m_spacepointContainer->addCollection(spacePointColl,spacePointColl->identifyHash());
	  
	  if (sc.isFailure()) 
	    {
	      ATH_MSG_WARNING(" Fail to store a TrigSiSpacePoint collection in Pixel with hash " << hashId);
	    } 
	  else 
	    {
		ATH_MSG_DEBUG(" Pixel TrigSiSpacePoint collection is stored in IDC with hashId " <<spacePointColl->identifyHash());	      
	    }
          if ( m_timers ) m_timer[2]->pause();    
	}
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

