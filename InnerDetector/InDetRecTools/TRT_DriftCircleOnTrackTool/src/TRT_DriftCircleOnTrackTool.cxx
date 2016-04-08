/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class TRT_DriftCircleOnTrackTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// AlgTool used for TRT_DriftCircleOnTrack object production
///////////////////////////////////////////////////////////////////
// Version 1.0 21/04/2004 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include "TRT_DriftCircleOnTrackTool/TRT_DriftCircleOnTrackTool.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "TrkToolInterfaces/IRIO_OnTrackErrorScalingTool.h"
#include "TrkEventPrimitives/LocalParameters.h"


///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TRT_DriftCircleOnTrackTool::TRT_DriftCircleOnTrackTool
(const std::string& ty,const std::string& na,const IInterface* pa)
  : AthAlgTool(ty,na,pa),
    m_errorScalingTool("Trk::RIO_OnTrackErrorScalingTool/RIO_OnTrackErrorScalingTool"),
    m_scaleTrtCov(false),
    m_useErrorCorrection(false)
{
  declareInterface<IRIO_OnTrackCreator>(this);
  declareProperty("ErrorScalingTool",m_errorScalingTool);
  declareProperty("UseErrorCorrection",m_useErrorCorrection);
}

///////////////////////////////////////////////////////////////////
// Destructor  
///////////////////////////////////////////////////////////////////

InDet::TRT_DriftCircleOnTrackTool::~TRT_DriftCircleOnTrackTool(){}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_DriftCircleOnTrackTool::initialize()
{
  StatusCode sc = AlgTool::initialize(); 

  // get error scaling tool
  //
  if ( m_errorScalingTool.retrieve().isFailure() ) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_errorScalingTool << endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_errorScalingTool << endreq;
    m_scaleTrtCov   = m_errorScalingTool->needToScaleTrt();
    if (m_scaleTrtCov) msg(MSG::DEBUG) << "Detected need for scaling TRT errors." << endreq;
  }

  return sc;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_DriftCircleOnTrackTool::finalize()
{
   StatusCode sc = AlgTool::finalize(); return sc;
}

///////////////////////////////////////////////////////////////////
// Trk::TRT_DriftCircleOnTrack production
///////////////////////////////////////////////////////////////////

const InDet::TRT_DriftCircleOnTrack* InDet::TRT_DriftCircleOnTrackTool::correct
(const Trk::PrepRawData& rio,const Trk::TrackParameters& TP) const
{
  const InDet::TRT_DriftCircle* DC = dynamic_cast<const InDet::TRT_DriftCircle*>(&rio);
  if(!DC) return 0;

  // Straw identification
  //
  const InDetDD::TRT_BaseElement* pE = DC->detectorElement(); if(!pE) return 0;

  // Identifier     iD = DC->identify();
  // IdentifierHash iH = trt_id->straw_layer_hash(trt_id->layer_id(iD));
  IdentifierHash iH = pE->identifyHash();
  

  // Global direction of the track parameters
  //
  const Amg::VectorX& P = TP.parameters();
  double se = sin(P[3]); 
  Amg::Vector3D dir(cos(P[2])*se,sin(P[2])*se,cos(P[3]));

  double predictedLocZ=P[1];

  double sign = (TP.parameters()[0] < 0.) ? -1. : 1.;


  //only create objects within the active area
  if(predictedLocZ>pE->strawLength()/2.){
    predictedLocZ=pE->strawLength()/2.;
  }else if(predictedLocZ<-pE->strawLength()/2.){
    predictedLocZ=-pE->strawLength()/2.;
  }



  // TRT_DriftCircleOnTrack production
  //
  Amg::MatrixX cov; 
  if(!m_scaleTrtCov) {
    cov = DC->localCovariance();
  } else             {
    bool endcap = false;
    if(dynamic_cast<const InDetDD::TRT_EndcapElement*>(pE)) endcap = true;
    Amg::MatrixX* newCov = m_errorScalingTool->createScaledTrtCovariance
      (DC->localCovariance(),endcap);
    if( !newCov ) {
      ATH_MSG_WARNING("Failed to create scaled error for SCT");
      return 0;
    }
    cov = *newCov;
    delete newCov;
  }

  if(!m_useErrorCorrection || DC->localPosition().x() > .30) {

    Trk::DefinedParameter  radius(sign*DC->localPosition().x(),Trk::locX);
    Trk::LocalParameters lp(radius);  
    return new InDet::TRT_DriftCircleOnTrack(DC,lp,cov,iH,predictedLocZ,dir,Trk::DECIDED);
  }

  cov(0,0) =  DC->localPosition().x()*DC->localPosition().x() +.09; 

  Trk::DefinedParameter radius(0.,Trk::locX);
  Trk::LocalParameters  lp(radius);  

  return new InDet::TRT_DriftCircleOnTrack(DC,lp,cov,iH,predictedLocZ,dir,Trk::DECIDED);
}  


