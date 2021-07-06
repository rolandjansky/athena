/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "TRT_ReadoutGeometry/TRT_EndcapElement.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkRIO_OnTrack/check_cast.h"


///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TRT_DriftCircleOnTrackTool::TRT_DriftCircleOnTrackTool
(const std::string& ty,const std::string& na,const IInterface* pa)
  : AthAlgTool(ty,na,pa),
    m_useErrorCorrection(false)
{
  declareInterface<IRIO_OnTrackCreator>(this);
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

  // get the error scaling tool
  ATH_CHECK(m_trtErrorScalingKey.initialize(!m_trtErrorScalingKey.key().empty()));
  ATH_CHECK(m_lumiDataKey.initialize ( !m_lumiDataKey.key().empty() && !m_trtErrorScalingKey.key().empty()));
  if (m_lumiDataKey.key().empty()) {
     ATH_MSG_INFO("Luminosity conditions data key not set. No mu correction." );
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
  if(!DC) return nullptr;

  // Straw identification
  //
  const InDetDD::TRT_BaseElement* pE = DC->detectorElement(); if(!pE) return nullptr;

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
  if (m_trtErrorScalingKey.key().empty()) {
    cov = DC->localCovariance();
  } else             {

    const EventContext& ctx = Gaudi::Hive::currentContext();
    double mu=0.;
    if (!m_lumiDataKey.empty()) {
       SG::ReadCondHandle<LuminosityCondData> lumiData (m_lumiDataKey,ctx);
       mu = lumiData->lbAverageInteractionsPerCrossing();
    }

    bool endcap = false;
    if(dynamic_cast<const InDetDD::TRT_EndcapElement*>(pE)) endcap = true;
    //    SG::ReadCondHandle<TRTRIO_OnTrackErrorScaling> error_scaling( m_trtErrorScalingKey );
    SG::ReadCondHandle<RIO_OnTrackErrorScaling> error_scaling( m_trtErrorScalingKey );
    cov = check_cast<TRTRIO_OnTrackErrorScaling>(*error_scaling)->getScaledCovariance( DC->localCovariance(),endcap, mu );
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


