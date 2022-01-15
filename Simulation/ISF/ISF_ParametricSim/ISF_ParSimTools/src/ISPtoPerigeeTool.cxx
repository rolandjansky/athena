/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISPtoPerigee.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "ISF_ParSimTools/ISPtoPerigeeTool.h"

#include "ISF_Event/ISFParticle.h"


//================ Constructor ====================================================
iParSim::ISPtoPerigeeTool::ISPtoPerigeeTool(const std::string& type,const std::string& name,const IInterface* parent):
base_class(type,name,parent),
m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"){
}
//================ Destructor =====================================================
iParSim::ISPtoPerigeeTool::~ISPtoPerigeeTool(){}

//================ Initialisation =================================================
StatusCode iParSim::ISPtoPerigeeTool::initialize(){

  if (m_extrapolator.retrieve().isFailure()){
      ATH_MSG_ERROR("Could not retrieve " << m_extrapolator << ". Exiting.");
      return StatusCode::FAILURE;
  }

  ATH_CHECK(m_beamSpotKey.initialize());

  ATH_MSG_VERBOSE( "initialize() successful." );
  return StatusCode::SUCCESS;
}

StatusCode iParSim::ISPtoPerigeeTool::finalize(){
  StatusCode sc(StatusCode::SUCCESS);
  return sc;
}

const Amg::Vector3D iParSim::ISPtoPerigeeTool::getPerigee() const{
  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
  return beamSpotHandle->beamPos();
}


const Trk::TrackParameters* iParSim::ISPtoPerigeeTool::extractTrkParameters(const ISF::ISFParticle& isp) const {
    const EventContext& ctx =  Gaudi::Hive::currentContext();
    if( isp.charge() == 0 ) {
      ATH_MSG_WARNING("     ISPtoPerigeeTool is not for neutral particles!");
    }
    //Get the momentum from the isp particle
    const Amg::Vector3D &momentum = isp.momentum();
    //Get the position from the isp particle
    const Amg::Vector3D &position = isp.position();
    //do the rest
    const Trk::CurvilinearParameters cParameters(position, momentum, isp.charge());
    SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx };
    Trk::PerigeeSurface persf( beamSpotHandle->beamPos() );
    const Trk::TrackParameters* tP = m_extrapolator->extrapolate(ctx,
                                                                 cParameters, 
                                                                 persf, 
                                                                 Trk::anyDirection, false);
    return tP;

}
