/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "InDetIdentifier/TRT_ID.h"
#include "Identifier/IdentifierHash.h" 
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetIdentifier/TRT_ID.h"
#include "TrigTRT_TrackExtensionTool/TrigTRT_TrackExtensionGeometry.h"
#include "TrigTRT_TrackExtensionTool/TrigTRT_DetElementRoadTool.h"
#include "TrigTRT_TrackExtensionTool/TrigMagneticFieldTool.h"
#include "TrkDistributedKalmanFilter/TrkTrackState.h"
#include "TrkDistributedKalmanFilter/TrkPlanarSurface.h"
#include "TrigTRT_TrackExtensionTool/TrigTRT_ProbabilisticDataAssociation.h"

static const InterfaceID IID_TrigTRT_DetElementRoadTool("TrigTRT_DetElementRoadTool", 1, 0);

const InterfaceID& TrigTRT_DetElementRoadTool::interfaceID() { 
  return IID_TrigTRT_DetElementRoadTool; 
}

TrigTRT_DetElementRoadTool::TrigTRT_DetElementRoadTool( const std::string& type, 
					    const std::string& name, 
					    const IInterface* parent )
  : AthAlgTool(type, name, parent), m_trigFieldTool("TrigMagneticFieldTool") {

  declareInterface< TrigTRT_DetElementRoadTool>( this );
  declareProperty( "TrigFieldTool", m_trigFieldTool, "TrigMagneticFieldTool");
  declareProperty( "bkgProbBarrel", m_noiseProbabilityBarrel=0.01);
  declareProperty( "bkgProbEndcap", m_noiseProbabilityEndcap=0.01);
  declareProperty( "gateWidthBarrel", m_gateBarrel=10.0);
  declareProperty( "gateWidthEndcap", m_gateEndcap=20.0);
  declareProperty( "hitEfficiency", m_hitEfficiency=0.96);
  declareProperty( "hitResolution", m_hitResolution=0.2);
  declareProperty( "associationThreshold", m_associationThreshold=0.2);
  declareProperty( "makeWireHits", m_makeWireHits=true);
  m_trtGeo=NULL;
}

StatusCode TrigTRT_DetElementRoadTool::initialize()  {
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << name() << " in initialize" << endreq;

  StatusCode sc=detStore()->retrieve(m_trtMgr,"TRT");
  if (sc.isFailure()) 
    {
      log << MSG::ERROR << name() << "failed to get TRT Manager" << endreq;
      return StatusCode::FAILURE;
    } 
  else 
    { 
      log << MSG::DEBUG << name() << "Got TRT Manager" << endreq;
    }

  sc = AthAlgTool::initialize(); 

  if (detStore()->retrieve(m_trtHelper, "TRT_ID").isFailure()) {
     log << MSG::FATAL << "Could not get TRT ID helper" << endreq;
     return StatusCode::FAILURE;  
  }
  sc = m_trigFieldTool.retrieve();
  if(sc.isFailure())  {
      log << MSG::FATAL << "Unable to locate TrigMagneticField Tool" <<m_trigFieldTool<<endreq;
      return sc;
    } 

  TrigTRT_BasePDAF *pB,*pE;
  pB=new TrigTRT_BarrelPDAF(m_hitEfficiency,m_noiseProbabilityBarrel,m_hitResolution,m_gateBarrel,m_associationThreshold,
			    m_makeWireHits);
  pE=new TrigTRT_EndcapPDAF(m_hitEfficiency,m_noiseProbabilityEndcap,m_hitResolution,m_gateEndcap,m_associationThreshold,
			    m_makeWireHits);
  m_trtGeo=new TrigTRT_TrackExtensionGeometry(m_trtMgr,m_trigFieldTool,pB,pE,m_trtHelper);
  //m_trtGeo->m_dump("geo.txt");
  return sc;
}

StatusCode TrigTRT_DetElementRoadTool::finalize() 
{
  if(m_trtGeo!=NULL)
    {
      delete m_trtGeo;
      m_trtGeo=NULL;
    }
  StatusCode sc = AthAlgTool::finalize(); 
  return sc;
}

TrigTRT_DetElementRoadTool::~TrigTRT_DetElementRoadTool()
{

}

TrigTRT_DetElementRoad* TrigTRT_DetElementRoadTool::m_buildTRT_Road(Trk::TrkTrackState* pTS)
{
  TrigTRT_DetElementRoad* pR=NULL;
  double lP[3],gP[3],P[6];

  Trk::TrkPlanarSurface* pS=pTS->m_getSurface();

  if(pS!=NULL)
    {
      lP[0]=pTS->m_getTrackState(0);
      lP[1]=pTS->m_getTrackState(1);
      lP[2]=0.0;
      pS->m_transformPointToGlobal(lP,gP);
      P[0]=gP[0];
      P[1]=gP[1];
      P[2]=gP[2];
      P[3]=pTS->m_getTrackState(2);
      P[4]=pTS->m_getTrackState(3);
      P[5]=pTS->m_getTrackState(4);
      //printf("Initial track params:\n");
      //pTS->m_report();
      pR=m_trtGeo->m_buildTRT_Road(P);
    }
  return pR;
}
