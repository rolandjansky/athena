/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class TRT_DriftCircleOnTrackNoDriftTimeTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// AlgTool used for TRT_DriftCircleOnTrack object production
///////////////////////////////////////////////////////////////////
// Version 1.0 21/04/2004 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include "TRT_DriftCircleOnTrackTool/TRT_DriftCircleOnTrackNoDriftTimeTool.h"
#include "TrkEventPrimitives/LocalParameters.h"



///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TRT_DriftCircleOnTrackNoDriftTimeTool::TRT_DriftCircleOnTrackNoDriftTimeTool
(const std::string& ty,const std::string& na,const IInterface* pa)
  : AthAlgTool(ty,na,pa)
{
  declareInterface<IRIO_OnTrackCreator>(this);
}

///////////////////////////////////////////////////////////////////
// Destructor  
///////////////////////////////////////////////////////////////////

InDet::TRT_DriftCircleOnTrackNoDriftTimeTool::~TRT_DriftCircleOnTrackNoDriftTimeTool(){}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_DriftCircleOnTrackNoDriftTimeTool::initialize()
{
  StatusCode sc = AlgTool::initialize(); return sc;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_DriftCircleOnTrackNoDriftTimeTool::finalize()
{
   StatusCode sc = AlgTool::finalize(); return sc;
}

///////////////////////////////////////////////////////////////////
// Trk::TRT_DriftCircleOnTrack production
///////////////////////////////////////////////////////////////////
  
const InDet::TRT_DriftCircleOnTrack* InDet::TRT_DriftCircleOnTrackNoDriftTimeTool::correct
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
 

  // only create objects within the active area
  //
  double sl =  pE->strawLength()*.5;

  if     (predictedLocZ > sl) predictedLocZ = sl;
  else if(predictedLocZ <-sl) predictedLocZ =-sl;

  // TRT_DriftCircleOnTrack production
  //
  Trk::DefinedParameter  radius(0.,Trk::locX);
  Trk::LocalParameters  lp(radius);  
  Amg::MatrixX cov(1,1);
  cov(0,0)=1.33333; 
  return new InDet::TRT_DriftCircleOnTrack
    (DC,lp,cov,iH,predictedLocZ,dir,Trk::NODRIFTTIME);
}  


