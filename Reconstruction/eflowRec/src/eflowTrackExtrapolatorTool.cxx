/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

 NAME:     eflowTrackExtrapolatorTool.cxx
 PACKAGE:  offline/Reconstruction/eflowRec

 AUTHORS:  T.Guttenberger
 CREATED:  17th September, 2014

 ********************************************************************/

#include "eflowRec/eflowTrackExtrapolatorTool.h"

#include "eflowRec/eflowTrackCaloPoints.h"
#include "eflowRec/cycle.h"
#include "eflowRec/eflowDepthCalculator.h"

#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkParameters/TrackParameters.h"  // typedef
#include "TrkSurfaces/Surface.h"

#include "CaloDetDescr/CaloDepthTool.h"

#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/ToolHandle.h"

#include <map>
#include <vector>
#include <utility>

using std::pair;

eflowTrackExtrapolatorTool::eflowTrackExtrapolatorTool(const std::string& type,
                                                       const std::string& name,
                                                       const IInterface* parent) :
    AthAlgTool(type, name, parent), m_theTrackExtrapolatorTool(""), m_tracksProcessed(0) {
  declareInterface<eflowTrackExtrapolatorBaseAlgTool>(this);
  declareProperty("eflowExtrapolator", m_theTrackExtrapolatorTool, "Extrapolator Tool Handle");
}

StatusCode eflowTrackExtrapolatorTool::initialize() {
  /* Tool service */
  IToolSvc* myToolSvc;
  if (service("ToolSvc", myToolSvc).isFailure()) {
    msg(MSG::WARNING) << " Tool Service Not Found" << endreq;
    return StatusCode::SUCCESS;
  }

  if (m_theTrackExtrapolatorTool.retrieve().isFailure()) {
    msg(MSG::WARNING) << "Cannot find Extrapolation tool "
    << m_theTrackExtrapolatorTool.typeAndName() << endreq;
    return StatusCode::SUCCESS;
  } else {
    msg(MSG::INFO) << "Successfully retrieved Extrapolation tool "
    << m_theTrackExtrapolatorTool.typeAndName() << endreq;
  }

  return StatusCode::SUCCESS;
}

eflowTrackCaloPoints* eflowTrackExtrapolatorTool::execute(const xAOD::TrackParticle* track) {
  m_tracksProcessed++;

  /* Construct track parameters object from which to start the extrapolation */
  /* TODO: This is quite a kludge - there should be a more elegant way to get this done! */
  const Amg::Vector3D pos(track->parameterX(xAOD::LastMeasurement),
                           track->parameterY(xAOD::LastMeasurement),
                           track->parameterZ(xAOD::LastMeasurement));
  const Amg::Vector3D mom(track->parameterPX(xAOD::LastMeasurement),
                           track->parameterPY(xAOD::LastMeasurement),
                           track->parameterPZ(xAOD::LastMeasurement));
  xAOD::ParametersCovMatrix_t covariance = track->trackParameterCovarianceMatrix(xAOD::LastMeasurement);

  Trk::TrackParameters* startParameters = new Trk::CurvilinearParameters(pos, mom, track->charge(), &covariance);

  /* Do the actual extrapolation */
  std::vector<const Trk::TrackStateOnSurface*>* dummy = 0; // Don't need a material collection...
  const std::vector<std::pair<const Trk::TrackParameters*, int> >* trackVector =
      m_theTrackExtrapolatorTool->extrapolate(*startParameters, Trk::alongMomentum, Trk::pion, dummy, 3); // 0 == don't pass a material collection, 3 == go to the end of the calo

  delete startParameters; // don't forget to delete the startParameters object when done

  /* Put extrapolations to Calo layers (detectorID between 3000 and 4000) into the map */
  std::map<eflowCalo::LAYER, const Trk::TrackParameters*> parametersMap;
  for (auto vectorPair : *trackVector) {
    int detectorID = vectorPair.second;
    if (detectorID < 3000 || detectorID > 4000) {
      continue;
    }
    parametersMap[translate(detectorID)] = vectorPair.first;
  }

  return new eflowTrackCaloPoints(parametersMap);
}

StatusCode eflowTrackExtrapolatorTool::finalize() {
  return StatusCode::SUCCESS;
}

eflowCalo::LAYER eflowTrackExtrapolatorTool::translate(int detectorID) {
  CaloCell_ID::CaloSample caloLayer = (CaloCell_ID::CaloSample)(detectorID % 1000);
  return eflowCalo::translateSampl(caloLayer);
}

