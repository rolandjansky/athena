/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class TRT_DriftCircleOnTrackUniversalTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// AlgTool used for TRT_DriftCircleOnTrack object production
///////////////////////////////////////////////////////////////////


#include "TRT_DriftCircleOnTrackTool/TRT_DriftCircleOnTrackUniversalTool.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"

///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TRT_DriftCircleOnTrackUniversalTool::TRT_DriftCircleOnTrackUniversalTool
(const std::string& ty,const std::string& na,const IInterface* pa)
  : AthAlgTool(ty,na,pa),
    m_scalefactor(2.)
{
  declareInterface<IRIO_OnTrackCreator>(this);
  declareProperty("ScaleHitUncertainty"  ,m_scalefactor    );    
}

///////////////////////////////////////////////////////////////////
// Destructor  
///////////////////////////////////////////////////////////////////

InDet::TRT_DriftCircleOnTrackUniversalTool::~TRT_DriftCircleOnTrackUniversalTool(){}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_DriftCircleOnTrackUniversalTool::initialize()
{
  StatusCode sc = AlgTool::initialize(); 

  if(m_riontrackTube.retrieve().isFailure()){
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_riontrackTube << endmsg;
    return StatusCode::FAILURE;
  }

  if(m_riontrackDrift.retrieve().isFailure()){
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_riontrackDrift << endmsg;
    return StatusCode::FAILURE;
  }

  return sc;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_DriftCircleOnTrackUniversalTool::finalize()
{
   StatusCode sc = AlgTool::finalize(); return sc;
}

///////////////////////////////////////////////////////////////////
// Trk::TRT_DriftCircleOnTrack production
///////////////////////////////////////////////////////////////////

const Trk::RIO_OnTrack* InDet::TRT_DriftCircleOnTrackUniversalTool::correct
(const Trk::PrepRawData& rio,const Trk::TrackParameters& TP) const
{
  const InDet::TRT_DriftCircle* DC = dynamic_cast<const InDet::TRT_DriftCircle*>(&rio);
  if(!DC) return nullptr;

  const Trk::RIO_OnTrack *rot=nullptr;

  //now decide about tube or precision hit

  double error2       = m_scalefactor*DC->localCovariance()(Trk::driftRadius,Trk::driftRadius);
  double driftradius = DC->localPosition()[Trk::driftRadius];
  double distance2    = fabs(TP.parameters()[0])-driftradius; distance2 *= distance2;
  

  if(distance2 > error2){
    //tube hit
    rot=m_riontrackTube->correct(rio,TP);
  }else               {
    //precision hit
    rot=m_riontrackDrift->correct(rio,TP);
  }

  return rot;
}  


