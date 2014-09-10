/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BeamSpotGetterTool.h"

#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "AthenaKernel/errorcheck.h"

namespace D3PD {

BeamSpotGetterTool::BeamSpotGetterTool(const std::string& type,
				       const std::string& name,
				       const IInterface* parent) : 
  Base(type, name, parent),
  m_iBeamCondSvc(0)
{
  declareProperty("BeamCondSvcName", m_beamCondSvcName = "BeamCondSvc" );
}

BeamSpotGetterTool::~BeamSpotGetterTool(){

}

StatusCode BeamSpotGetterTool::initialize(){

  CHECK(Base::initialize());

  // Pick up the BeamConditionService
  if (service(m_beamCondSvcName, m_iBeamCondSvc).isFailure() || m_iBeamCondSvc == 0) {
    REPORT_MESSAGE (MSG::WARNING) << "Could not find BeamCondSvc: " <<  m_beamCondSvcName;
    REPORT_MESSAGE (MSG::WARNING) << "Will use nominal beamspot at (0,0,0)";
  }

  return StatusCode::SUCCESS;
}

const Trk::RecVertex* BeamSpotGetterTool::get (bool /*allowMissing*/){

  /// Pick up the beamspot parameters as a vertex object
  Trk::RecVertex *bvx = 0;
  if (m_iBeamCondSvc) {
    bvx = new Trk::RecVertex(m_iBeamCondSvc->beamVtx());
  }else{
    // no beam service => create dummy beam spot
    AmgSymMatrix(3) err;
    Amg::Vector3D pos(0,0,0);
    double ndf = 0;
    double chi2 = 0;
    bvx = new Trk::RecVertex(pos, err, ndf, chi2); // dummy beam spot
  }

  return bvx;
}

void BeamSpotGetterTool::releaseObject (const Trk::RecVertex* p){
  if(p) delete p;
}

} // namespace D3PD
