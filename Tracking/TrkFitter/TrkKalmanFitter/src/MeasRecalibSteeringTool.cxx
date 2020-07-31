/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MeasRecalibSteeringTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkKalmanFitter/MeasRecalibSteeringTool.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkEventUtils/RoT_Extractor.h"
#include "TrkEventUtils/MeasurementTypeID.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"

using namespace Trk;
using namespace Trk::TrackState;

//================ Constructor =================================================

Trk::MeasRecalibSteeringTool::MeasRecalibSteeringTool(const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_rotCreator("Trk::RIO_OnTrackCreator/InDetRefitRotCreator"),
  m_broadPixelClusterCreator("InDet::PixelClusterOnTrackTool/InDetBroadPixelClusterOnTrackTool"),
  m_broadSctClusterCreator("InDet::SCT_ClusterOnTrackTool/InDetBroadSCT_ClusterOnTrackTool"),
  m_trtDriftCircleCreator("InDet::TRT_DriftCircleOnTrackTool/TRT_DriftCircleOnTrackTool"),
  m_trtTubeHitCreator("InDet::TRT_DriftCircleOnTrackNoDriftTimeTool/TRT_StrawTubeOnTrackTool"),
  m_haveInDetTools(true),
  //m_haveMuonTools(false),
  m_idHelper(nullptr),
  m_mbHelper(nullptr)
{
  declareInterface<IMeasurementRecalibrator>(this);

  //  template for property decalration
  declareProperty("CommonRotCreator",             m_rotCreator);
  declareProperty("BroadPixelClusterOnTrackTool", m_broadPixelClusterCreator);
  declareProperty("BroadSCT_ClusterOnTrackTool",  m_broadSctClusterCreator);
  declareProperty("TRT_DriftCircleOnTrackTool",   m_trtDriftCircleCreator);
  declareProperty("TRT_StrawTubeOnTrackTool",     m_trtTubeHitCreator);
}

//================ Destructor =================================================

Trk::MeasRecalibSteeringTool::~MeasRecalibSteeringTool()
{}


//================ Initialisation =================================================

StatusCode Trk::MeasRecalibSteeringTool::initialize()
{
  
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;

  if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
    ATH_MSG_ERROR ("Could not get AtlasDetectorID helper" );
    return StatusCode::FAILURE;
  } // need this helper for detector I.D. in internal DAF and outlier logics, to maintain perf
  m_mbHelper = new MeasurementTypeID(m_idHelper);

  if (m_rotCreator.retrieve().isFailure()) {
    ATH_MSG_ERROR ("can not retrieve ROT creator of type " << m_rotCreator.typeAndName());
    return StatusCode::FAILURE;
  } ATH_MSG_INFO ("retrieved tool " << m_rotCreator.typeAndName());

  // FIXME replace this zoo of tools with something smart, like flags through RC interface

  if (m_haveInDetTools) {
    if (m_broadPixelClusterCreator.retrieve().isFailure()) {
      ATH_MSG_ERROR ("can not retrieve ROT creator for broad Pixel clusters " <<
                     m_broadPixelClusterCreator.typeAndName() );
      return StatusCode::FAILURE;
    } ATH_MSG_INFO ("retrieved " << m_broadPixelClusterCreator.typeAndName());
    if (m_broadSctClusterCreator.retrieve().isFailure()) {
      ATH_MSG_ERROR ("can not retrieve ROT creator for broad SCT clusters " <<
                     m_broadSctClusterCreator.typeAndName() );
      return StatusCode::FAILURE;
    } ATH_MSG_INFO ("retrieved " << m_broadSctClusterCreator.typeAndName());
    if (m_trtDriftCircleCreator.retrieve().isFailure()) {
      ATH_MSG_ERROR ("can not retrieve ROT creator for full drift-time hits of type " <<
                     m_trtDriftCircleCreator.typeAndName() );
      return StatusCode::FAILURE;
    } ATH_MSG_INFO ("retrieved tool " << m_trtDriftCircleCreator.typeAndName());
    if (m_trtTubeHitCreator.retrieve().isFailure()) {
      ATH_MSG_ERROR ("can not retrieve ROT creator for straw tube hits of type " <<
                     m_trtTubeHitCreator.typeAndName() );
      return StatusCode::FAILURE;
    } ATH_MSG_INFO ("retrieved tool " << m_trtTubeHitCreator.typeAndName());
  }

  // if (m_haveMuonTools) ...


  ATH_MSG_INFO ("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode Trk::MeasRecalibSteeringTool::finalize()
{
  delete m_mbHelper;
  StatusCode sc = AthAlgTool::finalize();
  return sc;
}

const Trk::RIO_OnTrack*
Trk::MeasRecalibSteeringTool::makePreciseMeasurement
(const MeasurementBase& meas,
 const TrackParameters& trkPar,
 const MeasurementType  inputMtype) const
{
  // prepare guaranteed input
  const Trk::RIO_OnTrack* rot = nullptr; 
  Trk::RoT_Extractor::extract( rot, &meas); // std and competing ROTs
  if (rot==nullptr) {
    ATH_MSG_WARNING ("Misconfigured fitters: no recalibration input!" );
    return nullptr;
  }
  if (rot->prepRawData() == nullptr) {
    ATH_MSG_WARNING ("Misconfigured fitters: re-calibration called " <<
                     "on data without PrepRawData objects available." );
    return nullptr;
  }
  MeasurementType mType = inputMtype;
  if (inputMtype == unidentified) mType = m_mbHelper->defineType(&meas);

  if (mType == TRT) {


    if ( !( fabs(meas.localParameters()[Trk::locX])>0.5 
            && fabs(meas.localParameters()[Trk::locX])<1.7 )  ) {

      /* ==> currently the std tool does not do wire hits. <==
      double radius = rot->prepRawData()->localPosition().get(Trk::driftRadius);
      double error  = rot->prepRawData()->localErrorMatrix().error(Trk::driftRadius);
      if (radius <= .30
          && fabs(fabs(trkPar.parameters()[Trk::driftRadius])-radius) < 2.0*error) {

        const Trk::RIO_OnTrack* newrot = 
          m_trtDriftCircleCreator->correct(*(rot->prepRawData()), trkPar);
        if (m_log.level() == MSG::INFO) m_log
          << MSG::INFO << " TRT wire hit: "<<rot->localParameters()[Trk::locX]
          << ", predict " << trkPar.parameters()[Trk::locX]
          << " --> " << (newrot->localParameters())[Trk::locX]
          << " +/- " << newrot->localErrorMatrix().covariance().fast(1,1) << endmsg;
        return newrot;
        } else { */
        const Trk::RIO_OnTrack* newrot = m_rotCreator->correct(*(rot->prepRawData()), trkPar);
        ATH_MSG_VERBOSE (" TRT universal hit: "<<rot->localParameters()[Trk::locX] <<
                         ", predict " << trkPar.parameters()[Trk::locX] <<
                         " --> " << (newrot->localParameters())[Trk::locX] <<
                         " +/- " << sqrt(newrot->localCovariance()(0,0)) );
        return newrot;
        //      }
    }
    return nullptr;
  } 
    return m_rotCreator->correct(*(rot->prepRawData()), trkPar);
  
}

Trk::TrackState::CalibrationType
Trk::MeasRecalibSteeringTool::calibrationStatus (const MeasurementBase& meas,
                                                 const MeasurementType inputMtype ) const {

  MeasurementType mType = inputMtype;
  if (inputMtype == unidentified) mType = m_mbHelper->defineType(&meas);

  CalibrationType cType = CalibrationNotKnown;
  if (mType==Pixel) {
    cType = Nominal;
    if ( (meas.localCovariance()(Trk::locX,Trk::locX) < 0.01*0.01)
         || (meas.localCovariance()(Trk::locY,Trk::locY) < 0.08*0.08) ) cType = HighPrecision;
  } else if (mType == SCT) {
    cType = Nominal;
    if (meas.localCovariance()(Trk::locX,Trk::locX) < 0.018*0.018) cType = HighPrecision;
  } else if (mType==TRT) {
    if (fabs(meas.localParameters()[Trk::locR])<0.001 &&
        meas.localCovariance()(Trk::locR,Trk::locR)>0.5*0.5) 
      cType = TubeHit;
    else cType = Nominal;
  } else if (mType==MDT) {
    if (fabs(meas.localParameters()[Trk::locR])<0.001 &&
        meas.localCovariance()(Trk::locR,Trk::locR)>1.0) 
      cType = TubeHit;
    else cType = HighPrecision;
  } else cType = Nominal;
  return cType;
}

const RIO_OnTrack*
Trk::MeasRecalibSteeringTool::makeBroadMeasurement(const MeasurementBase& meas,
                                                   const TrackParameters& trkPar,
                                                   const MeasurementType inputMtype )
  const {

  // prepare guaranteed input
  const Trk::RIO_OnTrack* rot = nullptr; 
  Trk::RoT_Extractor::extract( rot, &meas); // std and competing ROTs
  if (rot==nullptr) {
    ATH_MSG_WARNING ("Misconfigured fitters: no recalibration input!" );
    return nullptr;
  }
  if (rot->prepRawData() == nullptr) {
    ATH_MSG_WARNING ("Misconfigured fitters: re-calibration called " <<
                     "on data without PrepRawData objects available." );
    return nullptr;
  }
  MeasurementType mType = inputMtype;
  if (inputMtype == unidentified) mType = m_mbHelper->defineType(&meas);

  if (mType == Pixel) {
    ATH_MSG_VERBOSE ("calling broad-cluster creator for Pixel hit" );
    return m_broadPixelClusterCreator->correct(*rot->prepRawData(), trkPar);
  }
  if (mType == SCT) {
    ATH_MSG_VERBOSE ("calling broad-cluster creator for SCT hit" );
    return m_broadSctClusterCreator->correct(*rot->prepRawData(), trkPar);
  }
  if (mType == TRT
      && fabs(trkPar.parameters()[Trk::locR]) < 2.20 ) {
    ATH_MSG_VERBOSE ("calling tube-hit creator for TRT hit" );
    return m_trtTubeHitCreator->correct(*rot->prepRawData(), trkPar);
  }

  return nullptr;
}

//================================================================================

