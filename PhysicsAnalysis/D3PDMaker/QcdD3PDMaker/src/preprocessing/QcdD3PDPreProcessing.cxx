/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "QcdD3PDMaker/preprocessing/QcdD3PDPreProcessing.h"

#include "AthenaKernel/errorcheck.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "VxVertex/RecVertex.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxContainer.h"

#include <iostream>

namespace QcdD3PD {

QcdD3PDPreProcessing::QcdD3PDPreProcessing(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator),
  m_iBeamCondSvc (0)
{
  declareProperty("BeamCondSvcName", m_beamCondSvcName = "BeamCondSvc" );
}

QcdD3PDPreProcessing::~QcdD3PDPreProcessing()
{
}

StatusCode QcdD3PDPreProcessing::initialize(){

  // Pick up the BeamConditionService
  if (service(m_beamCondSvcName, m_iBeamCondSvc).isFailure() || m_iBeamCondSvc == 0) {
    REPORT_MESSAGE (MSG::WARNING) << "Could not find BeamCondSvc: " <<  m_beamCondSvcName;
    REPORT_MESSAGE (MSG::WARNING) << "Will use nominal beamspot at (0,0,0)";
  }
  return StatusCode::SUCCESS;
}

StatusCode QcdD3PDPreProcessing::execute(){

  ATH_CHECK( fillBeamSpot() );
  return StatusCode::SUCCESS;
}

StatusCode QcdD3PDPreProcessing::finalize(){

  return StatusCode::SUCCESS;
}



StatusCode QcdD3PDPreProcessing::fillBeamSpot(){

  /// Pick up the beamspot parameters as a vertex object
  Trk::RecVertex bvx;
  if (m_iBeamCondSvc) {
    bvx = Trk::RecVertex(m_iBeamCondSvc->beamVtx());
  }else{ 
    // no beam service => create dummy beam spot
    AmgSymMatrix(3) err;
    err.setZero();
    Amg::Vector3D pos;
    pos.setZero();
    double ndf = 0;
    double chi2 = 0;
    bvx = Trk::RecVertex(pos, err, ndf, chi2); // dummy beam spot
  }

  // Create a VxVertex object with beam spot information and store it in a container
  std::vector<Trk::VxTrackAtVertex*> tracksEmpty;
  Trk::VxCandidate *vx = new Trk::VxCandidate(bvx, tracksEmpty);
  vx->setVertexType(Trk::NoVtx);
  VxContainer *vxContainer = new VxContainer();
  vxContainer->push_back(vx);

  // register new object in StoreGate
  ATH_CHECK( evtStore()->record(vxContainer, "QcdD3PD::BeamSpotRecVertex") );

//   // Create a VxVertex object with beam spot information and store it in StoreGate
//   std::vector<Trk::VxTrackAtVertex*> tracksEmpty;
//   Trk::VxCandidate *vx = new Trk::VxCandidate(bvx, tracksEmpty);
//   vx->setVertexType(Trk::NoVtx);

//   // register new object in StoreGate
//   StatusCode sc = m_storeGate->record(vx, "QcdD3PD::BeamSpotRecVertex");
//   if(sc.isFailure()){
//     return StatusCode::FAILURE;
//   }

  return StatusCode::SUCCESS;
}

} // namespace QcdD3PD

