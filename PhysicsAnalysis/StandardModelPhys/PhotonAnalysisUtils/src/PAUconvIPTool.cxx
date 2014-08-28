/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Tool to compute impact parameters for converted photons.
// Author: F.Polci<francesco.polci@cern.ch>  
// 8 Oct 2009

#include "PhotonAnalysisUtils/PAUconvIPTool.h"
#include "PhotonAnalysisUtils/PAUnamespace.h"

#include "egammaEvent/egamma.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"

#include <algorithm>
#include <math.h>
#include <functional>

#include "InDetBeamSpotService/IBeamCondSvc.h" // get beam spot position


using HepGeom::Point3D;


// constructor
PAUconvIPTool::PAUconvIPTool(const std::string& t, const std::string& n, const IInterface* p) :    
  AthAlgTool(t,n,p), 
  m_calodepth("CaloDepthTool")
{
  declareInterface<IPAUconvIPTool>(this);
  declareProperty("CaloDepthTool",m_calodepth);
  declareProperty("beamCondSvcName",m_beamCondSvcName="BeamCondSvc");  //for getting beam position
  m_beamCondSvc = 0;
}

// destructor
PAUconvIPTool::~PAUconvIPTool()
{}

// Athena standard methods

// initialize
StatusCode PAUconvIPTool::initialize()
{
  MsgStream log(msgSvc(), name());
  log <<  MSG::INFO  << name() <<"in initialize()..." << endreq;
  std::cout <<"Initializing PAUconvIPTool ..." << std::endl ;

  if (m_calodepth.retrieve().isFailure()) {
    log << MSG::ERROR << "Cannot get CaloDepthTool" << endreq;
    return StatusCode::FAILURE;
  }

  
  StatusCode sc = service(m_beamCondSvcName,m_beamCondSvc);               /*  getting beam spot */
  if (sc.isFailure() || m_beamCondSvc == 0) {
    log << MSG::INFO << "Could not find BeamCondSvc. Will use (0,0,0) if no vertex is given and extrapolation is needed." << endreq;
    m_beamCondSvc = 0;
    return sc;
  }
  
  
  return StatusCode::SUCCESS;
}

// finalize
StatusCode PAUconvIPTool::finalize()
{
  MsgStream log(msgSvc(), name());
  log <<  MSG::VERBOSE  << "... in finalize() ..." << endreq ;
  return StatusCode::SUCCESS;
}


const std::vector<float> PAUconvIPTool::convIP(const egamma* g) const {
  
  std::vector<float> result_for_output;
  
  if(!g->conversion()){
    result_for_output.push_back(PAU::UNKNOWN);
    result_for_output.push_back(PAU::UNKNOWN);
    return result_for_output;
  }

  float raggio = 0.;
  float emme = -9999.;
  float qu = -9999.;
  float emmeBis = -9999.;
  float quBis = -9999.;
  float Xbary = -9999.;
  float Ybary = -9999.;
  //float Rconv    = PAU::UNKNOWN ;
  //float Zconv    = PAU::UNKNOWN ;
  float Xconv = PAU::UNKNOWN;
  float Yconv = PAU::UNKNOWN;
  float beamSpotX = 0.;
  float beamSpotY = 0.;
  //float beamSpotZ = 0.;
  float distance = -9999.;
  float distanceBary = -9999.;
  
  
  ///RECOVER THE BEAMSPOT
  if (m_beamCondSvc){
    Amg::Vector3D bpos = m_beamCondSvc->beamPos();
    beamSpotX = bpos.x();
    beamSpotY = bpos.y();
    //beamSpotZ = bpos.z();
  }
  else std::cout << "m_beamCondSvc is false" << std::endl;
  
  
  //POSITION OF RECONSTRUCTED CLUSTER BARYCENTER
  raggio = m_calodepth->radius(CaloCell_ID::EMB2,g->cluster()->etaBE(2),g->cluster()->phiBE(2));
  
  
  Xbary = raggio*cos(g->cluster()->phi());
  Ybary = raggio*sin(g->cluster()->phi());
  
  /*NOTUSED float phi[2]*/ ;
  //Rconv = g->Rconv();
  //Zconv = g->convVx()->recVertex().position()[2] ;
  Xconv = g->conversion()->recVertex().position()[0] ;
  Yconv = g->conversion()->recVertex().position()[1] ;

  //CALCULATE THE DISTANCE OF EVENTVERTEX FROM BARY-VTX LINE 
  emme = (Yconv-Ybary)/(Xconv-Xbary);
  qu = -Xbary*emme+Ybary;
  distance = fabs(-emme*beamSpotX+beamSpotY-qu)/sqrt(1+emme*emme);
  result_for_output.push_back(distance);

  //CALCULATE THE DISTANCE OF BARYCENTER FROM EVENTVTX-VTX LINE 
  emmeBis = (Yconv-beamSpotY)/(Xconv-beamSpotX); 
  quBis = -beamSpotX*emmeBis+beamSpotY;
  distanceBary =  fabs(-emmeBis*Xbary+Ybary-quBis)/sqrt(1+emmeBis*emmeBis);
  result_for_output.push_back(distanceBary);
  

  return result_for_output;

}
