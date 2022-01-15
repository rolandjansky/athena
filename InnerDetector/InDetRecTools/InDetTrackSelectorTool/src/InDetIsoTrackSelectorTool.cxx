/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTrackSelectorTool/InDetIsoTrackSelectorTool.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
// forward declares
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkTrack/Track.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "CLHEP/Units/SystemOfUnits.h"

using CLHEP::mm;

//_______________________________________________________________________________
InDet::InDetIsoTrackSelectorTool::InDetIsoTrackSelectorTool(const std::string & t, const std::string & n, const IInterface * p)
  : AthAlgTool(t,n,p), 
    m_robustCuts(true),
    m_applySinThetaCorrection(true),
    m_d0max(1.5*mm),
    m_z0stMax(1.5*mm),
    m_d0Significance(3),
    m_z0Significance(3),
    m_extrapolator("Trk::Extrapolator/InDetExtrapolator"),
    m_trackSelector("")
{
  declareInterface<Trk::IIsoTrackSelectorTool>(this);
  // properties via python binding
  declareProperty("RobustCuts",               m_robustCuts);
  declareProperty("SinThetaCorrection",       m_applySinThetaCorrection);
  declareProperty("maxD0",                    m_d0max);
  declareProperty("maxZ0",                    m_z0stMax);
  declareProperty("maxD0overSigmaD0",         m_d0Significance);
  declareProperty("maxZ0overSigmaZ0",         m_z0Significance);
  // tools
  declareProperty("Extrapolator",             m_extrapolator);
  declareProperty("TrackSelector",            m_trackSelector);
}

//_______________________________________________________________________________
InDet::InDetIsoTrackSelectorTool::~InDetIsoTrackSelectorTool()
{}

//_______________________________________________________________________________
StatusCode InDet::InDetIsoTrackSelectorTool::initialize()
{

  // get the extrapolator
  if ( m_extrapolator.retrieve().isFailure() ){
      ATH_MSG_ERROR("Could not retrieve Extrapolator '" << m_extrapolator << "' (essential). Abort.");
      return StatusCode::FAILURE;
  } else 
      ATH_MSG_DEBUG("Successfully retrieved " << m_extrapolator);
  
  // get the track selector if needed
  if ( !m_trackSelector.empty() && m_trackSelector.retrieve().isFailure() ){
      ATH_MSG_ERROR("Could not retrieve TrackSelector '" << m_trackSelector <<"' although configured. Abort.");
      return StatusCode::FAILURE;
  }
  // done
  return StatusCode::SUCCESS;
}


//_______________________________________________________________________________
bool InDet::InDetIsoTrackSelectorTool::decision(const Trk::AtaStraightLine& atl, const Trk::Track& track) const
{
    const Trk::Perigee* tPerigee = track.perigeeParameters();
    if (!tPerigee){
        ATH_MSG_DEBUG("No perigee on track, discard this one.");
        return false;
    }
    // call the workhorse interface
    bool passed = decision(atl,*tPerigee);
    // only check if needed
    passed = (!passed || m_trackSelector.empty()) ? passed : ( passed && m_trackSelector->decision(track) );
    // return what you have
    ATH_MSG_VERBOSE("Track " << ( passed ? "passed" : "did not pass") << " isolation track selector.");
    return passed;
}

   
//_______________________________________________________________________________
bool InDet::InDetIsoTrackSelectorTool::decision(const Trk::AtaStraightLine& atl, const Trk::TrackParticleBase& trackParticle) const
{
    
    // get the paramters base
    const Trk::TrackParameters* definintParameters = &(trackParticle.definingParameters());
    const Trk::TrackParameters* trackParameters=
      dynamic_cast<const Trk::Perigee*>(definintParameters);
    if (!trackParameters){
        ATH_MSG_DEBUG("No parameters to start from on track, discard this one.");
        return false;
    }
    // call the workhorse interface
    bool passed = decision(atl,*trackParameters);
    // only check if needed
    passed = (!passed || m_trackSelector.empty()) ? passed : ( passed && m_trackSelector->decision(trackParticle) );
    // return what you have
    ATH_MSG_VERBOSE("TrackParticle " << ( passed ? "passed" : "did not pass") << " isolation track selector.");
    return passed;    
}

//_______________________________________________________________________________
bool InDet::InDetIsoTrackSelectorTool::decision(const Trk::AtaStraightLine& atl, const Trk::TrackParameters& trackPars) const
{
  // get the surface
  bool passed = false;
  const Trk::StraightLineSurface& alSurface = atl.associatedSurface();
  // no surface: bail out
  // get the track to the BeamLine Parameters ( given by AtaStrainghtLine)
  const Trk::TrackParameters* trackAtBL = m_extrapolator->extrapolate(
    Gaudi::Hive::currentContext(),
    trackPars,
    alSurface,
    Trk::anyDirection,
    false);
  // no parameterisation : bail out
  if (!trackAtBL) return false;
  // d0,z0 wrt BL for reference and track
  double d0track_wrtBL = trackAtBL->parameters()[Trk::d0]; 
  double sinTheta = m_applySinThetaCorrection ? 1. : sin(trackAtBL->parameters()[Trk::theta]);
  double z0track_wrtBL = trackAtBL->parameters()[Trk::z0]*sinTheta;
  double sinThetaRef   = m_applySinThetaCorrection ? 1. : sin(atl.parameters()[Trk::theta]); 
  double z0ref_wrtBL   = atl.parameters()[Trk::z0] * sinThetaRef;
  if (m_robustCuts){
        // check d0 cut with respect to BL
        passed = (d0track_wrtBL*d0track_wrtBL < m_d0max*m_d0max);
        ATH_MSG_VERBOSE("TrackParameters " << ( passed  ? "passed" : "did not pass" ) << " d0 cut wrt BL :  " 
                                 <<  d0track_wrtBL << " (cut is : | " << m_d0max << " | ).");
        // check z0 cut with respect to reference
        passed = ( (z0track_wrtBL-z0ref_wrtBL)*(z0track_wrtBL-z0ref_wrtBL) < m_z0stMax*m_z0stMax );
        ATH_MSG_VERBOSE("TrackParameters " << ( passed  ? "passed" : "did not pass" ) << " z0 " << ( m_applySinThetaCorrection ? "*sin(theta)" : "") 
                                 << " cut wrt reference :" 
                                 <<  (z0track_wrtBL-z0ref_wrtBL) << " (cut is : | " << m_z0stMax << " | ).");
  } else {
        // cast to measured parameters
        if (!trackAtBL->covariance()){
            ATH_MSG_VERBOSE("Can not apply signficance cut on Parameters w/o Error. Ignore Track.");
            return false;
        }
        // get the error on the track
        double covTrackD0    = (*trackAtBL->covariance())(Trk::d0,Trk::d0);
        double covTrackZ0    = (*trackAtBL->covariance())(Trk::z0,Trk::z0);
        // check d0 signficiance cut with respect to BL
        passed = (d0track_wrtBL*d0track_wrtBL)/(covTrackD0) < m_d0Significance*m_d0Significance;
        ATH_MSG_VERBOSE("TrackParameters " << ( passed  ? "passed" : "did not pass" ) << " d0 signficance cut wrt BL :  " 
                                 <<  (d0track_wrtBL*d0track_wrtBL)/(covTrackD0) << " (cut is : | " << m_d0Significance << " | ).");
        double deltaZ = z0ref_wrtBL - z0track_wrtBL;
        if (m_applySinThetaCorrection){
	    double covTrackTheta = (*trackAtBL->covariance())(Trk::theta,Trk::theta);
	    double covTrackZ0Theta = (*trackAtBL->covariance())(Trk::z0,Trk::theta);
            // check z0 signficance cut with respect to reference -- apply theta projection into longitudinal track frame
            double cosTheta = cos(trackAtBL->parameters()[Trk::theta]);
            // derivatives + apply jacobian transormation
            double dZIPdTheta = deltaZ*cosTheta;
            double dZIPdz0 = sinTheta;
            double DTheta2 = dZIPdTheta*dZIPdTheta*covTrackTheta;
            double DZ02 = dZIPdz0*dZIPdz0*covTrackZ0;
            double DThetaZ0 = 2.*dZIPdTheta*dZIPdz0*covTrackZ0Theta;
            // check for it
            double newZ0Err = DTheta2 + DZ02 + DThetaZ0;
            passed = (deltaZ*deltaZ)/(newZ0Err) < m_z0Significance*m_z0Significance;
            ATH_MSG_VERBOSE("TrackParameters " << ( passed  ? "passed" : "did not pass" ) << " z0*sin(theta) signficance cut wrt BL :  " 
                                      <<  (deltaZ*deltaZ)/(newZ0Err) << " (cut is : | " << m_z0Significance << " | ).");            
            
        } else {
            // check z0 signficiance cut with respect to BL
            passed = (deltaZ*deltaZ)/(covTrackZ0) < m_z0Significance*m_z0Significance;
            ATH_MSG_VERBOSE("TrackParameters " << ( passed  ? "passed" : "did not pass" ) << " z0 signficance cut wrt BL :  " 
                                     <<  (deltaZ*deltaZ)/(covTrackZ0) << " (cut is : | " << m_z0Significance << " | ).");
            
        }
  }
  // memory cleanup
  delete trackAtBL;
  return passed;
}

