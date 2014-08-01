/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowTrackToCaloTrackExtrapolatorTool.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson
CREATED:  4th March, 2005

********************************************************************/

#include "eflowRec/eflowTrackToCaloTrackExtrapolatorTool.h"

#include "Particle/TrackParticle.h"

#include "TrkParameters/TrackParameters.h"  // typedef

#include "TrkSurfaces/Surface.h"

#include "CaloDetDescr/CaloDepthTool.h"

#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/ToolHandle.h"

eflowTrackToCaloTrackExtrapolatorTool::eflowTrackToCaloTrackExtrapolatorTool(const std::string& type,const std::string& name,const IInterface* parent) :
  AthAlgTool( type, name, parent),
  m_isTauMode(false),
  m_theTrackExtrapolatorTool("")
{ 
  declareInterface<eflowTrackToCaloTrackExtrapolatorTool>(this);
  declareProperty("IsTauMode",m_isTauMode, "Are we running in tau mode?");
  declareProperty("TrackExtrapolatorToolName",m_trackExtrapolatorToolName);
  declareProperty("TrackToCalo",m_theTrackExtrapolatorTool,"TrackToCalo Tool Handle");
 }

StatusCode eflowTrackToCaloTrackExtrapolatorTool::initialize(){

  m_subCalo = CaloCell_ID::EMB2;

  // tool service
  IToolSvc* myToolSvc;
  StatusCode sc = service("ToolSvc",myToolSvc);
  
  if ( sc.isFailure() ) {
    msg(MSG::WARNING) << " Tool Service Not Found" << endreq;
    return StatusCode::SUCCESS;
  }

  sc = m_theTrackExtrapolatorTool.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::WARNING) << "Cannot find TrackToCalo Extrapolation tool " <<  m_theTrackExtrapolatorTool.typeAndName() << endreq;
    return StatusCode::SUCCESS;
  }
  else msg(MSG::INFO) << "Successfully retrieved TrackToCalo Extrapolation tool " <<  m_theTrackExtrapolatorTool.typeAndName() << endreq;
  
  return StatusCode::SUCCESS;
}

StatusCode eflowTrackToCaloTrackExtrapolatorTool::setGeometry(CaloCell_ID::CaloSample subCalo) {

  m_subCalo = subCalo;

  return StatusCode::SUCCESS;
}

Amg::Vector3D eflowTrackToCaloTrackExtrapolatorTool::execute(const xAOD::TrackParticle* track) {

  /* Tau mode might still rely on this historic leftover */
  if (m_isTauMode && !isInBarrel(track)) m_subCalo = CaloCell_ID::EME2;

  const Trk::TrackParameters* extrapolatedParameters = extrapolate(track, m_subCalo);

  /* getPostions deletes the parameters, so no mem leak here */
  return getPosition(extrapolatedParameters);
}

std::vector<Amg::Vector3D > 
eflowTrackToCaloTrackExtrapolatorTool::execute(const xAOD::TrackParticle* track,
                                               const std::vector<CaloCell_ID::CaloSample>& samples) {

  /* Tau mode might still rely on this historic leftover */
  if (m_isTauMode &&!isInBarrel(track)){
    return std::vector<Amg::Vector3D >(samples.size(), getPosition(extrapolate(track, CaloCell_ID::EME2)));
  }

  return getPositions(extrapolate(track, samples));
}

const Trk::TrackParameters*
eflowTrackToCaloTrackExtrapolatorTool::extrapolate(const xAOD::TrackParticle* track,
                                                   CaloCell_ID::CaloSample targetLayer) {
  return m_theTrackExtrapolatorTool->extrapolate(*track, targetLayer, 0.0, Trk::alongMomentum, Trk::nonInteracting);
}

const Trk::TrackParameters*
eflowTrackToCaloTrackExtrapolatorTool::extrapolate(const Trk::TrackParameters* startingPoint,
                                                   CaloCell_ID::CaloSample targetLayer) {
  return m_theTrackExtrapolatorTool->extrapolate(*startingPoint, targetLayer, 0.0, Trk::nonInteracting, Trk::alongMomentum);
}

std::vector<const Trk::TrackParameters*>
eflowTrackToCaloTrackExtrapolatorTool::extrapolate(const xAOD::TrackParticle* track,
                                                   const std::vector<CaloCell_ID::CaloSample>& targetLayers) {
  int nTargets(targetLayers.size());

  std::vector<const Trk::TrackParameters*> extrapolatedParameters;
  extrapolatedParameters.reserve(nTargets);

  const Trk::TrackParameters* lastTrackPars = 0;
  for (int iSample = 0; iSample < nTargets; iSample++) {
    if (lastTrackPars) {
      extrapolatedParameters.push_back(extrapolate(lastTrackPars, targetLayers[iSample]));
    } else {
      extrapolatedParameters.push_back(extrapolate(track, targetLayers[iSample]));
    }
    if (extrapolatedParameters.back() != 0) {
      lastTrackPars = extrapolatedParameters.back();
    }
  }

  return extrapolatedParameters;
}

Amg::Vector3D
eflowTrackToCaloTrackExtrapolatorTool::getPosition(const Trk::TrackParameters* trackPars) {
  if (!trackPars) {
    return Amg::Vector3D(0.,0.,0.);
  }
  Amg::Vector3D localCoord = trackPars->position();
  /* Don't remove the deletion. Client code in this class relies on this. */
  delete trackPars;
  return localCoord;
}

std::vector<Amg::Vector3D >
eflowTrackToCaloTrackExtrapolatorTool::getPositions(std::vector<const Trk::TrackParameters*> extrapolatedParameters) {
  int nSamples(extrapolatedParameters.size());

  std::vector<Amg::Vector3D > extrapolatedPositions;
  extrapolatedPositions.reserve(nSamples);

  for (int iSample = 0; iSample < nSamples; ++iSample) {
    /* getPostions deletes the parameters, so no mem leak here */
    extrapolatedPositions.push_back(getPosition(extrapolatedParameters[iSample]));
  }

  return extrapolatedPositions;
}

bool eflowTrackToCaloTrackExtrapolatorTool::isInBarrel(const xAOD::TrackParticle* track) {
  double trketa = track->eta();
  ToolHandle<CaloDepthTool> m_calodepth = m_theTrackExtrapolatorTool->getCaloDepth();

  double distanceToBarrel = m_calodepth->deta(CaloCell_ID::PreSamplerB,trketa);
  /* Negative distance means we are inside the volume */
  if (distanceToBarrel < 0) return true;
  else {
    double distanceToEncdap = m_calodepth->deta(CaloCell_ID::PreSamplerE,trketa);
    /* This also covers the case that distanceToEncdap < 0 (as distanceToBarrel<0 was already handled) */
    return (distanceToBarrel < distanceToEncdap);
  }
}

StatusCode eflowTrackToCaloTrackExtrapolatorTool::finalize(){
  return StatusCode::SUCCESS;
}
