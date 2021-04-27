/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class TRT_DriftCircleOnTrackRecalibrateTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// AlgTool used for TRT_DriftCircleOnTrack object production
///////////////////////////////////////////////////////////////////


#include "TRT_DriftCircleOnTrackTool/TRT_DriftCircleOnTrackRecalibrateTool.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TRT_DriftFunctionTool/ITRT_DriftFunctionTool.h"
#include "TRT_ReadoutGeometry/TRT_EndcapElement.h"
#include "TrkRIO_OnTrack/check_cast.h"


///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TRT_DriftCircleOnTrackRecalibrateTool::TRT_DriftCircleOnTrackRecalibrateTool
(const std::string& ty,const std::string& na,const IInterface* pa)
  : AthAlgTool(ty,na,pa),
    m_riontrackTube("InDet::TRT_DriftCircleOnTrackNoDriftTimeTool/TRT_DriftCircleOnTrackNoDriftTimeTool"),
    m_drifttool("TRT_DriftFunctionTool"),
    m_useToTCorrection(false),
    m_scalefactor(2.)
{
  declareInterface<IRIO_OnTrackCreator>(this);
  declareProperty("RIOonTrackToolTube",   m_riontrackTube  );
  declareProperty("DriftFunctionTool",    m_drifttool);
  declareProperty("ScaleHitUncertainty",  m_scalefactor    );
  declareProperty("useDriftTimeToTCorrection",m_useToTCorrection);
}

///////////////////////////////////////////////////////////////////
// Destructor  
///////////////////////////////////////////////////////////////////

InDet::TRT_DriftCircleOnTrackRecalibrateTool::~TRT_DriftCircleOnTrackRecalibrateTool(){}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_DriftCircleOnTrackRecalibrateTool::initialize()
{
  StatusCode sc = AlgTool::initialize();

  if(m_drifttool.retrieve().isFailure()){
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_drifttool << endmsg;
    return StatusCode::FAILURE;
  }

  if(m_riontrackTube.retrieve().isFailure()){
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_riontrackTube << endmsg;
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_trtErrorScalingKey.initialize(!m_trtErrorScalingKey.key().empty()));
  ATH_CHECK(m_lumiDataKey.initialize ( !m_lumiDataKey.key().empty() && !m_trtErrorScalingKey.key().empty()));
  return sc;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_DriftCircleOnTrackRecalibrateTool::finalize()
{
   StatusCode sc = AlgTool::finalize(); return sc;
}

///////////////////////////////////////////////////////////////////
// Trk::TRT_DriftCircleOnTrack production
///////////////////////////////////////////////////////////////////

const Trk::RIO_OnTrack* InDet::TRT_DriftCircleOnTrackRecalibrateTool::correct
(const Trk::PrepRawData& rio,const Trk::TrackParameters& TP) const
{
  const InDet::TRT_DriftCircle* DC = dynamic_cast<const InDet::TRT_DriftCircle*>(&rio);
  if(!DC) return nullptr;

  const Trk::RIO_OnTrack *rot=nullptr;

  //recalculate driftradius using the new calibration constants
  bool isOK=false;
  double t0=0.;
  double rawtime     = m_drifttool->rawTime(DC->driftTimeBin());
  
  if (m_useToTCorrection) {
        rawtime -= m_drifttool->driftTimeToTCorrection(DC->timeOverThreshold(), DC->identify());      
  } 
  
  double driftradius = m_drifttool->driftRadius(rawtime,DC->identify(),t0,isOK);
  double error       = m_drifttool->errorOfDriftRadius(rawtime-t0,DC->identify());

  double distance=fabs(fabs(TP.parameters()[0])-driftradius);


  msg(MSG::VERBOSE)<<"Old radius: "<< DC->localPosition()[Trk::driftRadius]<<" -> new radius: "<<driftradius<<endmsg;

  if(distance>error*m_scalefactor){
    
    //tube hit
    //
    rot=m_riontrackTube->correct(rio,TP);
  }
  else {
    // precision hit
    //
    const InDetDD::TRT_BaseElement* pE = DC->detectorElement(); if(!pE) return nullptr;
    IdentifierHash iH = pE->identifyHash();
    const Amg::VectorX& P = TP.parameters();
    double se = sin(P[3]); 
    Amg::Vector3D dir(cos(P[2])*se,sin(P[2])*se,cos(P[3]));
    
    double predictedLocZ=P[1];
    
    double sign = (TP.parameters()[0] < 0.) ? -1. : 1.;

    // only create objects within the active area
    //
    double sl = pE->strawLength()*.5;  
    if     (predictedLocZ > sl) predictedLocZ = sl;
    else if(predictedLocZ <-sl) predictedLocZ =-sl;


    double escale2=error*error/DC->localCovariance()(Trk::driftRadius,Trk::driftRadius);
    Amg::MatrixX cov(DC->localCovariance()*escale2);

    if (!m_trtErrorScalingKey.key().empty()) {
      const EventContext& ctx = Gaudi::Hive::currentContext();
      double mu=0.;
      if (!m_lumiDataKey.empty()) {
          SG::ReadCondHandle<LuminosityCondData> lumiData (m_lumiDataKey,ctx);
          mu = lumiData->lbAverageInteractionsPerCrossing();
      }

      bool endcap = false;
      if(dynamic_cast<const InDetDD::TRT_EndcapElement*>(pE)) endcap = true;
      // SG::ReadCondHandle<TRTRIO_OnTrackErrorScaling> error_scaling( m_trtErrorScalingKey );
      SG::ReadCondHandle<RIO_OnTrackErrorScaling> error_scaling( m_trtErrorScalingKey,ctx);
      cov = check_cast<TRTRIO_OnTrackErrorScaling>(*error_scaling)->getScaledCovariance( cov, endcap, mu);
    }

    Trk::DefinedParameter  radius(sign*driftradius,Trk::locX);
    Trk::LocalParameters lp(radius);  

    rot=new InDet::TRT_DriftCircleOnTrack(DC,lp,cov,iH,predictedLocZ,dir,Trk::DECIDED);
  }
  return rot;
}  


