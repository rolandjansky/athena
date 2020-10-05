/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// NIMatEffUpdator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "TrkExTools/NIMatEffUpdator.h"

// Trk include
#include "TrkExInterfaces/IMaterialEffectsUpdator.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/MaterialProperties.h"

// constructor
Trk::NIMatEffUpdator::NIMatEffUpdator(const std::string &t, const std::string &n, const IInterface *p) :
  AthAlgTool(t, n, p),
  m_matUpdator("Trk::MaterialEffectsUpdator/MaterialEffectsUpdator") {
  declareInterface<ITimedMatEffUpdator>(this);
  // the tool parameters -----------------------------------------------------
  declareProperty("MaterialEffectsUpdator", m_matUpdator);
}

// destructor
Trk::NIMatEffUpdator::~NIMatEffUpdator() {
}

// Athena standard methods
// initialize
StatusCode
Trk::NIMatEffUpdator::initialize() {
  ATH_MSG_INFO("initialize()");

  // retrieve the material updator tool
  if (m_matUpdator.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not retrieve " << m_matUpdator);
    return StatusCode::FAILURE;
  } 
    ATH_MSG_DEBUG("Successfully retrieved " << m_matUpdator);
  

  return StatusCode::SUCCESS;
}

// finalize
StatusCode
Trk::NIMatEffUpdator::finalize() {
  ATH_MSG_INFO("finalize() successful");
  return StatusCode::SUCCESS;
}

const Trk::TrackParameters *
Trk::NIMatEffUpdator::update(const Trk::TrackParameters *parm,
                             const Trk::Layer &lay,
                             Trk::TimeLimit & /*timeLim*/,
                             Trk::PathLimit & /*pathLim*/,
                             Trk::GeometrySignature /*geoID*/,
                             Trk::PropDirection dir,
                             Trk::ParticleHypothesis particle) const {
  return m_matUpdator->update(parm, lay, dir, particle).release();
}

// update method
const Trk::TrackParameters *
Trk::NIMatEffUpdator::update(double /*time*/,
                             const Trk::TrackParameters &parm,
                             const Trk::MaterialProperties &matprop,
                             double pathCorrection,
                             Trk::PropDirection dir,
                             Trk::ParticleHypothesis particle,
                             Trk::MaterialUpdateMode) const {
  return m_matUpdator->update(parm, matprop, pathCorrection, dir, particle).release();
}

const Trk::TrackParameters *
Trk::NIMatEffUpdator::update(double /*time*/,
                             const Trk::TrackParameters *parm,
                             const Trk::MaterialEffectsOnTrack &meff,
                             Trk::ParticleHypothesis particle,
                             Trk::MaterialUpdateMode) const {
  return m_matUpdator->update(parm, meff, particle).release();
}
