/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DummyMaterialEffectsUpdator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk include
#include "TrkExTools/DummyMaterialEffectsUpdator.h"
#include "TrkExInterfaces/IEnergyLossUpdator.h"
#include "TrkExInterfaces/IMultipleScatteringUpdator.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkSurfaces/Surface.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/AssociatedMaterial.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkDetDescrInterfaces/IMaterialMapper.h"

// constructor
Trk::DummyMaterialEffectsUpdator::DummyMaterialEffectsUpdator(const std::string &t, const std::string &n,
                                                              const IInterface *p) :
  AthAlgTool(t, n, p),
  m_validationMode(false),
  m_validationDirectionSwitch(1),
  m_validationDirection(Trk::alongMomentum),
  m_materialMapper("Trk::MaterialMapper"){
  declareInterface<IMaterialEffectsUpdator>(this);
  declareProperty("ValidationMode", m_validationMode);
  declareProperty("ValidationDirection", m_validationDirectionSwitch);
  declareProperty("ValidationMaterialMapper", m_materialMapper);
}

// destructor
Trk::DummyMaterialEffectsUpdator::~DummyMaterialEffectsUpdator() = default;

// Athena standard methods
// initialize
StatusCode
Trk::DummyMaterialEffectsUpdator::initialize() {
  if (m_materialMapper.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_materialMapper);
    return StatusCode::FAILURE;
  }
    ATH_MSG_INFO("Retrieved tool " << m_materialMapper);


  // set the validation switch
  m_validationDirection = (m_validationDirectionSwitch == 1) ? Trk::alongMomentum : Trk::oppositeMomentum;

  ATH_MSG_INFO("initialize() successful");
  return StatusCode::SUCCESS;
}

// finalize
StatusCode
Trk::DummyMaterialEffectsUpdator::finalize() {
  ATH_MSG_INFO("finalize() successful");
  return StatusCode::SUCCESS;
}

Trk::TrackParameters *
Trk::DummyMaterialEffectsUpdator::update(const TrackParameters *parm,
                                         const Layer &lay,
                                         PropDirection,
                                         ParticleHypothesis,
                                         MaterialUpdateMode) const {
  if (m_validationMode && parm) {
    // get the numbers according
    const Trk::TrackingVolume *tvol = lay.enclosingTrackingVolume();
    const Trk::MaterialProperties *mprop = lay.fullUpdateMaterialProperties(*parm);

    const Trk::MaterialProperties *updateProperties
      = dynamic_cast<const Trk::MaterialProperties *>(mprop);

    if (tvol && updateProperties) {
      double correctionFactor = fabs(lay.surfaceRepresentation().pathCorrection(parm->position(), parm->momentum()));
      // material properties
      double pathInX0 = updateProperties->thicknessInX0();
      double x0 = updateProperties->x0();
      double l0 = updateProperties->l0();
      double A = updateProperties->averageA();
      double Z = updateProperties->averageZ();
      double rho = updateProperties->averageRho();
      // correct
      pathInX0 *= fabs(correctionFactor);
      // create the extended material step
      Trk::AssociatedMaterial assMatHit(parm->position(),
                                        pathInX0,
                                        x0, l0, A, Z, rho,
                                        correctionFactor,
                                        lay.enclosingTrackingVolume(),
                                        &lay);

      // record it
      m_materialMapper->recordMaterialHit(assMatHit, parm->momentum());
    }else {
      ATH_MSG_WARNING("update() ... dynamic cast to MaterialProperties failed!");
    }
  }
  return parm->clone();
}

Trk::TrackParameters *
Trk::DummyMaterialEffectsUpdator::preUpdate(const TrackParameters *parm,
                                            const Layer &lay,
                                            PropDirection dir,
                                            ParticleHypothesis,
                                            MaterialUpdateMode) const {
  if (m_validationMode && dir == m_validationDirection && parm) {
    // get the numbers according
    const Trk::TrackingVolume *tvol = lay.enclosingTrackingVolume();
    // the preFactor
    double preFactor = lay.preUpdateMaterialFactor(*parm, dir);
    // return if the preFactor is too small
    if (preFactor < 0.1) {
      return parm->clone();
    }

    double correctionFactor = fabs(lay.surfaceRepresentation().pathCorrection(parm->position(), parm->momentum()));
    /** surfaceValidation is never used
       bool  surfaceValidation = false;
     **/
    // get the material properties
    const Trk::MaterialProperties *updateProperties = lay.fullUpdateMaterialProperties(*parm);

    correctionFactor *= preFactor;

    if (tvol && updateProperties) {
      // material properties
      double pathInX0 = updateProperties->thicknessInX0();
      double x0 = updateProperties->x0();
      double l0 = updateProperties->l0();
      double A = updateProperties->averageA();
      double Z = updateProperties->averageZ();
      double rho = updateProperties->averageRho();
      // correct
      pathInX0 *= fabs(correctionFactor) * preFactor;
      // create the extended material step
      Trk::AssociatedMaterial assMatHit(parm->position(),
                                        pathInX0,
                                        x0, l0, A, Z, rho,
                                        correctionFactor,
                                        lay.enclosingTrackingVolume(),
                                        &lay);
      // record it
      m_materialMapper->recordMaterialHit(assMatHit, parm->momentum());
      // and if it was a reference material : the SurfaceValidation
      /** sroe: surfaceValidation must be false at this point, so the following line is redundant **/
      // if (surfaceValidation)  m_materialMapper->recordSurfaceHit(parm->localPosition(),assMatHit);
    } else {
      ATH_MSG_WARNING("preUpdate() ... dynamic cast to MaterialProperties failed!");
    }
  }
  return parm->clone();
}

Trk::TrackParameters *
Trk::DummyMaterialEffectsUpdator::postUpdate(const TrackParameters &parm,
                                             const Layer &lay,
                                             PropDirection dir,
                                             ParticleHypothesis,
                                             MaterialUpdateMode) const {
  if (m_validationMode && dir == m_validationDirection) {
    const Trk::TrackingVolume *tvol = lay.enclosingTrackingVolume();

    // get the quantities
    double postFactor = lay.postUpdateMaterialFactor(parm, dir);

    if (postFactor < 0.1) {
      return nullptr;
    }

    // the correction Factor to the layer
    double correctionFactor = fabs(lay.surfaceRepresentation().pathCorrection(parm.position(), parm.momentum()));
    // get the material properties
    const Trk::MaterialProperties *updateProperties = lay.fullUpdateMaterialProperties(parm);
    correctionFactor *= postFactor;

    if (tvol && updateProperties) {
      // material properties
      double pathInX0 = updateProperties->thicknessInX0();
      double x0 = updateProperties->x0();
      double l0 = updateProperties->l0();
      double A = updateProperties->averageA();
      double Z = updateProperties->averageZ();
      double rho = updateProperties->averageRho();
      // correct
      pathInX0 *= fabs(correctionFactor);
      // create the extended material step
      Trk::AssociatedMaterial assMatHit(parm.position(),
                                        pathInX0,
                                        x0, l0, A, Z, rho,
                                        correctionFactor,
                                        lay.enclosingTrackingVolume(),
                                        &lay);
      // record it
      m_materialMapper->recordMaterialHit(assMatHit, parm.momentum());
    }else {
      ATH_MSG_WARNING("postUpdate() ... dynamic cast to MaterialProperties failed!");
    }
  }
  return(parm.clone());
}

// actual update method
Trk::TrackParameters *
Trk::DummyMaterialEffectsUpdator::update(const TrackParameters &parm,
                                         const MaterialProperties &,
                                         double,
                                         PropDirection,
                                         ParticleHypothesis,
                                         MaterialUpdateMode) const {
  return(parm.clone());
}


