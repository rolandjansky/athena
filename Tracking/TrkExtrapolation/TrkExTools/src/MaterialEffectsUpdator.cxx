/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialEffectsUpdator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk include
#include "TrkExTools/MaterialEffectsUpdator.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TrkDetDescrInterfaces/IMaterialMapper.h"
#include "TrkEventPrimitives/DefinedParameter.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkExInterfaces/IEnergyLossUpdator.h"
#include "TrkExInterfaces/IMultipleScatteringUpdator.h"
#include "TrkGeometry/AssociatedMaterial.h"
#include "TrkGeometry/CompoundLayer.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"
// Amg
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
// std
#include <algorithm>
#include <functional>
namespace {
const Trk::ParticleMasses s_particleMasses{};
}

// constructor
Trk::MaterialEffectsUpdator::MaterialEffectsUpdator(const std::string& t, const std::string& n, const IInterface* p)
  : AthAlgTool(t, n, p)
  ,
  //  TrkParametersManipulator(),
  m_doCompoundLayerCheck(false)
  , m_doEloss(true)
  , m_doMs(true)
  , m_forceMomentum(false)
  , m_xKalmanStraggling(false)
  , m_useMostProbableEloss(false)
  , m_msgOutputValidationDirection(true)
  , m_msgOutputCorrections(false)
  , m_validationMode(false)
  , m_validationIgnoreUnmeasured(true)
  , m_landauMode(false)
  , m_validationDirection(1)
  , m_momentumCut(50. * Gaudi::Units::MeV)
  , m_momentumMax(10. * Gaudi::Units::TeV)
  , m_forcedMomentum(2000. * Gaudi::Units::MeV)
  , m_eLossUpdator("Trk::EnergyLossUpdator/AtlasEnergyLossUpdator")
  , m_msUpdator("Trk::MultipleScatteringUpdator/AtlasMultipleScatteringUpdator")
  , m_materialMapper("Trk::MaterialMapper/AtlasMaterialMapper")
{
  declareInterface<IMaterialEffectsUpdator>(this);
  // configuration (to be changed to new genconf style)
  declareProperty("CheckForCompoundLayers", m_doCompoundLayerCheck);
  declareProperty("EnergyLoss", m_doEloss);
  declareProperty("EnergyLossUpdator", m_eLossUpdator);
  declareProperty("MultipleScattering", m_doMs);
  declareProperty("MultipleScatteringUpdator", m_msUpdator);
  // the momentum cut for particle interactions
  declareProperty("MinimalMomentum", m_momentumCut);
  declareProperty("MaximalMomentum", m_momentumMax);
  declareProperty("ForceMomentum", m_forceMomentum);
  declareProperty("ForcedMomentumValue", m_forcedMomentum);
  declareProperty("MostProbableEnergyLoss", m_useMostProbableEloss);
  declareProperty("ScreenOutputValidationDirection", m_msgOutputValidationDirection);
  declareProperty("ScreenOutputCorrections", m_msgOutputCorrections);
  // run vaidation mode true/false
  declareProperty("ValidationMode", m_validationMode);
  declareProperty("ValidationIgnoreUnmeasured", m_validationIgnoreUnmeasured);
  declareProperty("ValidationDirection", m_validationDirection);
  declareProperty("ValidationMaterialMapper", m_materialMapper);
  declareProperty("LandauMode", m_landauMode);
}

// destructor
Trk::MaterialEffectsUpdator::~MaterialEffectsUpdator() = default;

// Athena standard methods
// initialize
StatusCode
Trk::MaterialEffectsUpdator::initialize()
{

  ATH_MSG_INFO("Minimal momentum cut for material update : " << m_momentumCut << " MeV");

  // retrieve the EnergyLoss Updator and Material Effects updator
  if (m_doEloss) {
    if (m_eLossUpdator.retrieve().isFailure()) {
      ATH_MSG_FATAL("Failed to retrieve tool " << m_eLossUpdator
                                               << ". No multiple scattering effects will be taken into account.");
      m_doEloss = false;
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Retrieved tool " << m_eLossUpdator);

  } else {
    m_eLossUpdator.disable();
  }

  if (m_doMs) {
    if (m_msUpdator.retrieve().isFailure()) {
      ATH_MSG_FATAL("Failed to retrieve tool " << m_msUpdator
                                               << ". No energy loss effects will be taken into account.");
      m_doMs = false;
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Retrieved tool " << m_msUpdator);

  } else {
    m_msUpdator.disable();
  }

  // retrieve the material mapper for the validation mode
  if (m_validationMode) {
    if (m_materialMapper.retrieve().isFailure()) {
      ATH_MSG_FATAL("Failed to retrieve tool " << m_materialMapper << ". No material recording.");
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Retrieved tool " << m_materialMapper);

  } else {
    m_materialMapper.disable();
  }

  return StatusCode::SUCCESS;
}


Trk::TrackParameters*
Trk::MaterialEffectsUpdator::updateImpl(Cache& cache,
                                        const TrackParameters* parm,
                                        const Layer& lay,
                                        PropDirection dir,
                                        ParticleHypothesis particle,
                                        MaterialUpdateMode matupmode) const
{
  // no material properties - pass them back
  if (particle == Trk::geantino || particle == Trk::nonInteractingMuon || (!m_doMs && !m_doEloss) ||
      !lay.isOnLayer(parm->position())) {
    return parm->clone();
  }

  // get the quantities
  const Trk::MaterialProperties* mprop = lay.fullUpdateMaterialProperties(*parm);
  if (!mprop) {
    return parm->clone();
  }

  // get the real pathlength
  double pathCorrection = fabs(lay.surfaceRepresentation().pathCorrection(parm->position(), parm->momentum()));

  // set the output if restricted to the direction
  bool outputFlag = m_msgOutputValidationDirection ? dir == int(m_validationDirection) : true;

  // --------------------------------------------------------------------------------------------------
  if (msgLvl(MSG::VERBOSE) && outputFlag) {
    double layerR = lay.surfaceRepresentation().bounds().r();
    double layerZ = lay.surfaceRepresentation().center().z();
    double eta = parm->momentum().eta();
    double sX0 = mprop->thicknessInX0();
    double tX0 = pathCorrection * mprop->thicknessInX0();
    ATH_MSG_VERBOSE("  [M] full material update,  layer with [r,z] = [ " << layerR << ", " << layerZ << " ] - Index "
                                                                         << lay.layerIndex());
    ATH_MSG_VERBOSE("      thickness/X0 , path/X0  (eta: g.factor) = " << sX0 << " , " << tX0 << " (" << eta << ": "
                                                                       << pathCorrection << ")");
  }
  // --------------------------------------------------------------------------------------------------
  if (m_validationMode) {
    cache.validationLayer = &lay;
  }
  return (updateImpl(cache, parm, *mprop, pathCorrection, dir, particle, matupmode));
}

Trk::TrackParameters*
Trk::MaterialEffectsUpdator::updateImpl(Cache& cache,
                                        const TrackParameters* parm,
                                        const MaterialEffectsOnTrack& meff,
                                        ParticleHypothesis particle,
                                        MaterialUpdateMode matupmode) const
{
  // no material properties - pass them back
  // TODO, if the parm doesn't have a surface (i.e. its in
  // curvilinear) then should we fall through?
  if (particle == Trk::geantino || particle == Trk::nonInteractingMuon || (!m_doMs && !m_doEloss) ||
      parm->associatedSurface() != meff.associatedSurface()) {
    return parm->clone();
  }

  // get the kinematics
  double p = parm->momentum().mag();
  double updateMomentum = (m_forceMomentum) ? m_forcedMomentum : p;
  double m = s_particleMasses.mass[particle];
  double E = sqrt(p * p + m * m);
  double beta = p / E;

  double pathcorrection = 1.; // Trick the MultipleScatteringUpdator interface

  double energyLoss = 0;
  double energyLossSigma = 0;

  if (meff.energyLoss() != nullptr) {
    energyLoss = meff.energyLoss()->deltaE();
    energyLossSigma = meff.energyLoss()->sigmaDeltaE();
  }

  // update for mean energy loss
  double newP = p;
  double sigmaQoverP = 0;
  double sigmaQoverPSq = 0;
  // Landaus mpvs don't just add, if in Landau mode we need to do a different update
  if (m_landauMode && cache.accumulatedElossSigma != 0 && energyLossSigma != 0) {
    if (energyLoss > 0) {
      energyLoss += energyLossSigma * std::log(1 + cache.accumulatedElossSigma / (energyLossSigma)) +
                    cache.accumulatedElossSigma * std::log(1 + energyLossSigma / cache.accumulatedElossSigma);
    } else {
      energyLoss -= energyLossSigma * std::log(1 + cache.accumulatedElossSigma / energyLossSigma) +
                    cache.accumulatedElossSigma * std::log(1 + energyLossSigma / cache.accumulatedElossSigma);
    }
    cache.accumulatedElossSigma += energyLossSigma;
  } else if (m_landauMode) {
    cache.accumulatedElossSigma += energyLossSigma;
  }
  double qOverPnew = parm->parameters()[Trk::qOverP];

  if (p > m_momentumCut && p < m_momentumMax && m_doEloss) {
    double newP2 = (E + energyLoss) * (E + energyLoss) - m * m;
    if (newP2 < m_momentumCut * m_momentumCut) {
      return nullptr; // protect FPE
    }
    if (E + energyLoss < -m) {
      return nullptr; // protect against flip in correction
    }
    newP = sqrt(newP2);
    sigmaQoverP = energyLossSigma / (beta * p * p);
    sigmaQoverPSq = sigmaQoverP * sigmaQoverP;
    qOverPnew = parm->charge() / newP;
  }
  Trk::DefinedParameter qOverPmod(qOverPnew, Trk::qOverP);

  // check if Parameters are measured parameters
  // the updatedParameters - first a copy
  const Trk::TrackParameters* mpars = parm;
  AmgVector(5) updatedParameters(mpars->parameters());
  AmgSymMatrix(5)* updatedCovariance = nullptr;
  // initialize ErrorMatrix pointer
  if (m_validationMode && !m_validationIgnoreUnmeasured) {
    // the new CovarianceMatrix - a copy first
    updatedCovariance = mpars->covariance() ? new AmgSymMatrix(5)(*mpars->covariance()) : nullptr;
    double angularVariation = 0;
    double sigmaDeltaPhiSq = 0;
    double sigmaDeltaThetaSq = 0;

    if (m_doMs) {
      // If the meff has scattering angles use those, otherwise use MEffUpdator
      if (meff.scatteringAngles() == nullptr) {
        // Trick to keep using existing MultipleScatteringUpdator interface
        // Here we know the path length to be meff.thicknessX0, so we set pathcorrection = 1
        // and create a dummy materialProperties with the properties we are interested in
        MaterialProperties mprop(meff.thicknessInX0(), 1., 1., 0., 0., 0.);
        angularVariation = m_msUpdator->sigmaSquare(mprop, updateMomentum, pathcorrection, Trk::muon);
        // sigmaDeltaPhiSq = angularVariation/(parm->sinTheta()*parm->sinTheta());
        sigmaDeltaPhiSq =
          angularVariation / (std::sin(parm->parameters()[Trk::theta]) * std::sin(parm->parameters()[Trk::theta]));
        sigmaDeltaThetaSq = angularVariation;
      } else {
        // material update from mefots -> D.L.
        sigmaDeltaPhiSq = meff.scatteringAngles()->sigmaDeltaPhi();
        sigmaDeltaPhiSq *= sigmaDeltaPhiSq;
        sigmaDeltaThetaSq = meff.scatteringAngles()->sigmaDeltaTheta();
        sigmaDeltaThetaSq *= sigmaDeltaThetaSq;
        updatedParameters[Trk::phi] = parm->position().phi() + meff.scatteringAngles()->deltaPhi();
        updatedParameters[Trk::theta] = parm->position().theta() + meff.scatteringAngles()->deltaTheta();
      }
    }
    // update the covariance entries - angular variation in phi has dependency on theta direction
    if (updatedCovariance) {
      // sign of the noise adding -----------------------
      int sign = int(matupmode);
      // check for non-zero covariance matrix
      COVARIANCEUPDATEWITHCHECK((*updatedCovariance)(Trk::phi, Trk::phi), sign, sigmaDeltaPhiSq);
      COVARIANCEUPDATEWITHCHECK((*updatedCovariance)(Trk::theta, Trk::theta), sign, sigmaDeltaThetaSq);
      if (!m_xKalmanStraggling && !m_landauMode) {
        COVARIANCEUPDATEWITHCHECK((*updatedCovariance)(Trk::qOverP, Trk::qOverP), sign, sigmaQoverPSq);
      } else if (m_xKalmanStraggling) { /* to be filled in*/
      } else if (m_landauMode) {
        // subtract what we added up till now and add what we should add up till now
        // Landau's 68% limit is approx 1.6*sigmaParameter

        /* Get the TLS to a local here once and use it for calculation*/
        (*updatedCovariance)(Trk::qOverP, Trk::qOverP) -=
          sign * std::pow(1.6 * (cache.accumulatedElossSigma - p * p * sigmaQoverP) / (p * p), 2);
        (*updatedCovariance)(Trk::qOverP, Trk::qOverP) +=
          sign * std::pow(1.6 * cache.accumulatedElossSigma / (newP * newP), 2);
      }
      // the checks for the remove Noise mode -----------------------------------------------------
      if (matupmode == Trk::removeNoise && !checkCovariance(*updatedCovariance)) {
        // the covariance is invalid
        delete updatedCovariance;
        return nullptr;
      }

      // create the ErrorMatrix
      // updatedError = new Trk::ErrorMatrix(updatedCovariance);
      // -------------------------------------- screen output --------------------------------------
      if (m_msgOutputCorrections) {
        double sigmaAngle = sqrt(angularVariation);
        ATH_MSG_VERBOSE("    sigma(phi) / sigma(theta) = " << sigmaAngle / std::sin(parm->parameters()[Trk::theta]) << " / "
                                                           << sigmaAngle);
        ATH_MSG_VERBOSE("    deltaP / sigmaQoverP      = " << energyLoss << " / " << sigmaQoverP);
      }
      // -------------------------------------------------------------------------------------------
    }
    // ----------------------------------------- validation section ----------------------------------
    // validation if configured
    if (m_validationMode) {
      if (cache.validationLayer) {
        // all you have from MaterialProperties
        double pathInX0 = meff.thicknessInX0();

        Trk::AssociatedMaterial assMatHit(parm->position(),
                                          pathInX0,
                                          pathInX0,
                                          0,
                                          0,
                                          0,
                                          0,
                                          pathcorrection,
                                          cache.validationLayer->enclosingTrackingVolume(),
                                          cache.validationLayer);

        // record the Material hit ----------------------------------------------------------------
        m_materialMapper->recordMaterialHit(assMatHit, parm->position());

        // the steps
        cache.validationSteps++;
        cache.validationPhi += parm->position().phi();
        cache.validationEta += parm->position().eta();
        // reset the validation layer
        cache.validationLayer = nullptr;
      }
    }
    // ----------------------------------------- validation section ----------------------------------
  }
  updatedParameters[Trk::qOverP] = qOverPnew;
  return parm->associatedSurface().createTrackParameters(updatedParameters[Trk::loc1],
                                                         updatedParameters[Trk::loc2],
                                                         updatedParameters[Trk::phi],
                                                         updatedParameters[Trk::theta],
                                                         updatedParameters[Trk::qOverP],
                                                         updatedCovariance);
}

Trk::TrackParameters*
Trk::MaterialEffectsUpdator::preUpdateImpl(Cache& cache,
                                           const TrackParameters* parm,
                                           const Layer& lay,
                                           PropDirection dir,
                                           ParticleHypothesis particle,
                                           MaterialUpdateMode matupmode) const
{
  // no material properties - pass the parameters back
  if (particle == Trk::geantino || particle == Trk::nonInteractingMuon || (!m_doMs && !m_doEloss)) {
    return parm->clone();
  }

  // get the split factor
  double preFactor = lay.preUpdateMaterialFactor(*parm, dir);
  // return if the preFactor is less than one
  if (preFactor < 0.01) {
    return parm->clone();
  }

  // get the material properties
  const Trk::MaterialProperties* mprop = nullptr;

  // set the output if restricted to the validation direction
  bool outputFlag = m_msgOutputValidationDirection ? dir == int(m_validationDirection) : true;

  mprop = lay.fullUpdateMaterialProperties(*parm);
  double pathCorrection = fabs(lay.surfaceRepresentation().pathCorrection(parm->position(), parm->momentum()));
  pathCorrection *= preFactor;

  // exit if no mprop could be assigned
  if (!mprop) {
    return parm->clone();
  }
  // --------------------------------------------------------------------------------------------------
  if (outputFlag) {
    double layerR = lay.surfaceRepresentation().bounds().r();
    double layerZ = lay.surfaceRepresentation().center().z();
    double eta = parm->momentum().eta();
    double sX0 = mprop->thicknessInX0();
    double tX0 = pathCorrection * mprop->thicknessInX0();
    ATH_MSG_VERBOSE("  [M] pre material update at layer with [r,z] = [ " << layerR << ", " << layerZ << " ] - Index "
                                                                         << lay.layerIndex());
    ATH_MSG_VERBOSE("      thickness/X0 , path/X0  (eta: g.factor) = " << sX0 << " , " << tX0 << " (" << eta << ": "
                                                                       << pathCorrection << ")");
  }
  // --------------------------------------------------------------------------------------------------
  if (m_validationMode) {
    cache.validationLayer = &lay;
  }
  return (updateImpl(cache, parm, *mprop, pathCorrection, dir, particle, matupmode));
}

Trk::TrackParameters*
Trk::MaterialEffectsUpdator::postUpdateImpl(Cache& cache,
                                            const TrackParameters& parm,
                                            const Layer& lay,
                                            PropDirection dir,
                                            ParticleHypothesis particle,
                                            MaterialUpdateMode matupmode) const
{
  // no material properties - pass the parameters back
  if (particle == Trk::geantino || particle == Trk::nonInteractingMuon || (!m_doMs && !m_doEloss) ||
      !lay.isOnLayer(parm.position())) {
    return parm.clone();
  }

  // get the quantities
  const Trk::MaterialProperties* mprop = nullptr;
  double postFactor = lay.postUpdateMaterialFactor(parm, dir);

  // no material properties - pass them back
  if (postFactor < 0.01) {
    return parm.clone();
  }

  // set the output if restricted to the validation direction
  bool outputFlag = m_msgOutputValidationDirection ? dir == int(m_validationDirection) : true;

  mprop = lay.fullUpdateMaterialProperties(parm);
  double pathCorrection = fabs(lay.surfaceRepresentation().pathCorrection(parm.position(), parm.momentum()));
  pathCorrection *= postFactor;

  // exit if no material properties
  if (!mprop) {
    return parm.clone();
  }

  // --------------------------------------------------------------------------------------------------
  if (outputFlag) {
    double layerR = lay.surfaceRepresentation().bounds().r();
    double layerZ = lay.surfaceRepresentation().center().z();
    double eta = parm.momentum().eta();
    double sX0 = mprop->thicknessInX0();
    double tX0 = pathCorrection * mprop->thicknessInX0();
    ATH_MSG_VERBOSE("  [M] post material update,  layer with [r,z] = [ " << layerR << ", " << layerZ << " ] - Index "
                                                                         << lay.layerIndex());
    ATH_MSG_VERBOSE("      thickness/X0 , path/X0  (eta: g.factor) = " << sX0 << " , " << tX0 << " (" << eta << ": "
                                                                       << pathCorrection << ")");
  }
  // --------------------------------------------------------------------------------------------------
  if (m_validationMode) {
    cache.validationLayer = &lay;
  }
  return (updateImpl(cache, parm, *mprop, pathCorrection, dir, particle, matupmode));
}

// actual update method - manipulation
Trk::TrackParameters*
Trk::MaterialEffectsUpdator::updateImpl(Cache& cache,
                                        const TrackParameters* parm,
                                        const MaterialProperties& matprop,
                                        double pathcorrection,
                                        PropDirection dir,
                                        ParticleHypothesis particle,
                                        MaterialUpdateMode matupmode) const
{
  // no material properties - pass them back
  if (particle == Trk::geantino || particle == Trk::nonInteractingMuon || (!m_doMs && !m_doEloss)) {
    return parm->clone();
  }

  // get the kinematics
  double p = parm->momentum().mag();
  double updateMomentum = (m_forceMomentum) ? m_forcedMomentum : p;
  double m = s_particleMasses.mass[particle];
  double E = sqrt(p * p + m * m);
  double beta = p / E;

  // set the output if restricted to the validation direction
  bool outputFlag = m_msgOutputValidationDirection ? dir == int(m_validationDirection) : true;

  // no material update below/above a certain cut value
  if (p > m_momentumCut && p < m_momentumMax) {
    // get the delta of the Energy
    EnergyLoss* energyLoss =
      (m_doEloss)
        ? m_eLossUpdator->energyLoss(matprop, updateMomentum, pathcorrection, dir, particle, m_useMostProbableEloss)
        : nullptr;
    // update for mean energy loss
    double deltaE = energyLoss ? energyLoss->deltaE() : 0;
    double sigmaDeltaE = energyLoss ? energyLoss->sigmaDeltaE() : 0;
    delete energyLoss;

    if (m_landauMode && cache.accumulatedElossSigma != 0 && sigmaDeltaE != 0) {
      if (dir == Trk::oppositeMomentum) {
        deltaE += sigmaDeltaE * std::log(1 + cache.accumulatedElossSigma / sigmaDeltaE) +
                  cache.accumulatedElossSigma * std::log(1 + sigmaDeltaE / cache.accumulatedElossSigma);
      } else {
        deltaE -= sigmaDeltaE * std::log(1 + cache.accumulatedElossSigma / sigmaDeltaE) +
                  cache.accumulatedElossSigma * std::log(1 + sigmaDeltaE / cache.accumulatedElossSigma);
      }
      cache.accumulatedElossSigma += sigmaDeltaE;
    } else if (m_landauMode) {
      cache.accumulatedElossSigma += sigmaDeltaE;
    }

    double newP2 = (E + deltaE) * (E + deltaE) - m * m;
    if (E + deltaE < -m) {
      return nullptr; // protect against flip in correction
    }
    if (newP2 < m_momentumCut * m_momentumCut) {
      return nullptr; // protect against FPE
    }
    double deltaP = sqrt(newP2) - p;
    double sigmaQoverP = sigmaDeltaE / std::pow(beta * p, 2);

    Trk::DefinedParameter qOverPmod(parm->charge() / (p + deltaP), Trk::qOverP);

    // check if Parameters are measured parameters
    // const Trk::MeasuredTrackParameters* mpars = dynamic_cast<const Trk::MeasuredTrackParameters*>(parm);
    const Trk::TrackParameters* mpars = parm;
    AmgVector(5) updatedParameters(mpars->parameters());
    // initialize ErrorMatrix pointer
    // Trk::ErrorMatrix* updatedError      = 0;
    AmgSymMatrix(5)* updatedCovariance = nullptr;
    if (mpars || (m_validationMode && !m_validationIgnoreUnmeasured)) {
      // the new CovarianceMatrix - a copy first
      if (mpars) {
        updatedCovariance = mpars->covariance() ? new AmgSymMatrix(5)(*mpars->covariance()) : nullptr;
      }
      // only update if msUpdator exists
      double angularVariation =
        (m_doMs) ? m_msUpdator->sigmaSquare(matprop, updateMomentum, pathcorrection, particle) : 0.;
      // update the covariance entries - angular variation in phi has dependency on theta direction
      if (updatedCovariance) {
        // sign of the noise adding ----------------------------------------------------------------
        int sign = int(matupmode);

        double sigmaDeltaPhiSq =
          angularVariation / (std::sin(parm->parameters()[Trk::theta]) * std::sin(parm->parameters()[Trk::theta]));
        double sigmaDeltaThetaSq = angularVariation;
        // checks will only be done in the removeNoise mode
        COVARIANCEUPDATEWITHCHECK((*updatedCovariance)(Trk::phi, Trk::phi), sign, sigmaDeltaPhiSq);
        COVARIANCEUPDATEWITHCHECK((*updatedCovariance)(Trk::theta, Trk::theta), sign, sigmaDeltaThetaSq);
        if (!m_xKalmanStraggling && !m_landauMode) {
          COVARIANCEUPDATEWITHCHECK((*updatedCovariance)(Trk::qOverP, Trk::qOverP), sign, sigmaQoverP * sigmaQoverP);
        } else if (m_xKalmanStraggling) {
          double q = parm->charge();
          COVARIANCEUPDATEWITHCHECK(
            (*updatedCovariance)(Trk::qOverP, Trk::qOverP), sign, 0.2 * deltaP * deltaP * q * q * q * q);
        } else if (m_landauMode) {
          // subtract what we added up till now and add what we should add up till now
          // Landau's 68% limit is approx 1.6*sigmaParameter
          (*updatedCovariance)(Trk::qOverP, Trk::qOverP) -=
            sign * std::pow(1.6 * (cache.accumulatedElossSigma - p * p * sigmaQoverP) / (p * p), 2);
          (*updatedCovariance)(Trk::qOverP, Trk::qOverP) +=
            sign * std::pow(1.6 * cache.accumulatedElossSigma / ((p + deltaP) * (p + deltaP)), 2);
        }
        // the checks for the remove Noise mode -----------------------------------------------------
        if (matupmode == Trk::removeNoise && !checkCovariance(*updatedCovariance)) {
          // the covariance is invalid
          delete updatedCovariance;
          return nullptr;
        }

        // create the ErrorMatrix
        // updatedError = new Trk::ErrorMatrix(updatedCovariance);

        // -------------------------------------- screen output --------------------------------------
        if (outputFlag && m_msgOutputCorrections) {
          double sigmaAngle = sqrt(angularVariation);
          ATH_MSG_VERBOSE("    sigma(phi) / sigma(theta) = " << sigmaAngle / std::sin(parm->parameters()[Trk::theta])
                                                             << " / " << sigmaAngle);
          ATH_MSG_VERBOSE("    deltaP / sigmaQoverP      = " << deltaP << " / " << sigmaQoverP);
        }
        // -------------------------------------------------------------------------------------------
      }
      // ----------------------------------------- validation section ----------------------------------
      // validation if configured
      if (m_validationMode && dir == Trk::PropDirection(m_validationDirection) && updatedCovariance) {

        if (cache.validationLayer) {
          // all you have from MaterialProperties
          double pathInX0 = pathcorrection * matprop.thicknessInX0();
          double A = 0.;
          double Z = 0.;
          double rho = 0.;
          double l0 = 0.;
          // or better take the extended version for more information
          const Trk::MaterialProperties* extProperties = dynamic_cast<const Trk::MaterialProperties*>(&matprop);

          if (extProperties) {
            A = extProperties->averageA();
            Z = extProperties->averageZ();
            rho = extProperties->averageRho();
            l0 = extProperties->l0();
          }

          Trk::AssociatedMaterial assMatHit(parm->position(),
                                            pathInX0,
                                            matprop.x0(),
                                            l0,
                                            A,
                                            Z,
                                            rho,
                                            pathcorrection,
                                            cache.validationLayer->enclosingTrackingVolume(),
                                            cache.validationLayer);

          // record the Material hit ----------------------------------------------------------------
          m_materialMapper->recordMaterialHit(assMatHit, parm->position());
          // the steps
          cache.validationSteps++;
          cache.validationPhi += parm->position().phi();
          cache.validationEta += parm->position().eta();
          // reset the cache.tion layer
          cache.validationLayer = nullptr;
        }
      }
      // ----------------------------------------- validation section ----------------------------------
    }
    updatedParameters[Trk::qOverP] = parm->charge() / (p + deltaP);
    return parm->associatedSurface().createTrackParameters(updatedParameters[Trk::loc1],
                                                           updatedParameters[Trk::loc2],
                                                           updatedParameters[Trk::phi],
                                                           updatedParameters[Trk::theta],
                                                           updatedParameters[Trk::qOverP],
                                                           updatedCovariance);
  }
  //default if we have not returned just above
  return parm->clone();
}

// actual update method
Trk::TrackParameters*
Trk::MaterialEffectsUpdator::updateImpl(Cache& cache,
                                        const TrackParameters& parm,
                                        const MaterialProperties& matprop,
                                        double pathcorrection,
                                        PropDirection dir,
                                        ParticleHypothesis particle,
                                        MaterialUpdateMode matupmode) const
{
  // no material properties - pass them back
  if (particle == Trk::geantino || (!m_doMs && !m_doEloss)) {
    return parm.clone();
  }

  // get the kinematics
  double p = parm.momentum().mag();
  double updateMomentum = (m_forceMomentum) ? m_forcedMomentum : p;
  double m = s_particleMasses.mass[particle];
  double E = sqrt(p * p + m * m);
  double beta = p / E;

  // set the output if restricted to the validation direction
  bool outputFlag = m_msgOutputValidationDirection ? dir == int(m_validationDirection) : true;

  // no material update below or above a certain cut value
  if (p > m_momentumCut && p < m_momentumMax) {
    // the updatedParameters - first a copy
    AmgVector(5) updatedParameters(parm.parameters());

    // get the delta of the Energy
    EnergyLoss* energyLoss =
      (m_doEloss)
        ? m_eLossUpdator->energyLoss(matprop, updateMomentum, pathcorrection, dir, particle, m_useMostProbableEloss)
        : nullptr;
    // update for mean energy loss
    double deltaE = energyLoss ? energyLoss->deltaE() : 0;
    double sigmaDeltaE = energyLoss ? energyLoss->sigmaDeltaE() : 0;
    delete energyLoss;
    if (m_landauMode && cache.accumulatedElossSigma != 0 && sigmaDeltaE != 0) {
      if (dir == Trk::oppositeMomentum) {
        deltaE += sigmaDeltaE * std::log(1 + cache.accumulatedElossSigma / sigmaDeltaE) +
                  cache.accumulatedElossSigma * std::log(1 + sigmaDeltaE / cache.accumulatedElossSigma);
      } else {
        deltaE -= sigmaDeltaE * std::log(1 + cache.accumulatedElossSigma / sigmaDeltaE) +
                  cache.accumulatedElossSigma * std::log(1 + sigmaDeltaE / cache.accumulatedElossSigma);
      }

      cache.accumulatedElossSigma += sigmaDeltaE;
    } else if (m_landauMode) {
      cache.accumulatedElossSigma += sigmaDeltaE;
    }

    double newP2 = (E + deltaE) * (E + deltaE) - m * m;
    if (E + deltaE < -m) {
      return nullptr; // protect against flip in correction
    }
    if (newP2 < m_momentumCut * m_momentumCut) {
      return nullptr; // protect against FPE
    }
    double deltaP = sqrt(newP2) - p;
    double sigmaQoverP = sigmaDeltaE / std::pow(beta * p, 2);

    updatedParameters[Trk::qOverP] = parm.charge() / (p + deltaP);

    // check if Parameters are measured parameters
    AmgSymMatrix(5)* updatedCovariance = nullptr;
    if (parm.covariance() || (m_validationMode && !m_validationIgnoreUnmeasured)) {
      // the new CovarianceMatrix - a copy first
      updatedCovariance = new AmgSymMatrix(5)(*parm.covariance());
      // only update if msUpdator exists
      double angularVariation =
        (m_doMs) ? m_msUpdator->sigmaSquare(matprop, updateMomentum, pathcorrection, particle) : 0.;
      // update the covariance entries - angular variation in phi has dependency on theta direction
      // sign of the noise adding ----------------------------------------------------------------
      int sign = int(matupmode);
      // checks will only be done in the removeNoise mode
      double sigmaDeltaPhiSq =
        angularVariation / (std::sin(parm.parameters()[Trk::theta]) * std::sin(parm.parameters()[Trk::theta]));
      double sigmaDeltaThetaSq = angularVariation;
      // checks will only be done in the removeNoise mode
      COVARIANCEUPDATEWITHCHECK((*updatedCovariance)(Trk::phi, Trk::phi), sign, sigmaDeltaPhiSq);
      COVARIANCEUPDATEWITHCHECK((*updatedCovariance)(Trk::theta, Trk::theta), sign, sigmaDeltaThetaSq);
      if (!m_xKalmanStraggling && !m_landauMode) {
        COVARIANCEUPDATEWITHCHECK((*updatedCovariance)(Trk::qOverP, Trk::qOverP), sign, sigmaQoverP * sigmaQoverP);
      } else if (m_xKalmanStraggling) { /* to be filled in*/
      } else if (m_landauMode) {
        // subtract what we added up till now and add what we should add up till now
        // Landau's 68% limit is best modeled by 1.6*sigmaParameter
        (*updatedCovariance)(Trk::qOverP, Trk::qOverP) -=
          sign * std::pow(1.6 * (cache.accumulatedElossSigma - p * p * sigmaQoverP) / (p * p), 2);
        (*updatedCovariance)(Trk::qOverP, Trk::qOverP) +=
          sign * std::pow(1.6 * cache.accumulatedElossSigma / ((p + deltaP) * (p + deltaP)), 2);
      }

      // the checks for the remove Noise mode -----------------------------------------------------
      if (matupmode == Trk::removeNoise && !checkCovariance(*updatedCovariance)) {
        // the covariance is invalid
        delete updatedCovariance;
        return nullptr;
      }

      // create the ErrorMatrix         // -------------------------------------- screen output
      // --------------------------------------
      if (outputFlag && m_msgOutputCorrections) {
        double sigmaAngle = sqrt(angularVariation);
        ATH_MSG_VERBOSE("    sigma(phi) / sigma(theta) = " << sigmaAngle / std::sin(parm.parameters()[Trk::theta]) << " / "
                                                           << sigmaAngle);
        ATH_MSG_VERBOSE("    deltaP / sigmaQoverP      = " << deltaP << " / " << sigmaQoverP);
      }
      // ----------------------------------------- validation section ----------------------------------
      // validation if configured
      if (m_validationMode && dir == Trk::PropDirection(m_validationDirection)) {

        if (cache.validationLayer) {
          // all you have from MaterialProperties
          double pathInX0 = pathcorrection * matprop.thicknessInX0();
          double A = 0.;
          double Z = 0.;
          double rho = 0.;
          double l0 = 0.;
          // or better take the extended version for more information
          const Trk::MaterialProperties* extProperties = dynamic_cast<const Trk::MaterialProperties*>(&matprop);

          if (extProperties) {
            A = extProperties->averageA();
            Z = extProperties->averageZ();
            rho = extProperties->averageRho();
            l0 = extProperties->l0();
          }

          Trk::AssociatedMaterial assMatHit(parm.position(),
                                            pathInX0,
                                            matprop.x0(),
                                            l0,
                                            A,
                                            Z,
                                            rho,
                                            pathcorrection,
                                            cache.validationLayer->enclosingTrackingVolume(),
                                            cache.validationLayer);

          // record the Material hit ----------------------------------------------------------------
          m_materialMapper->recordMaterialHit(assMatHit, parm.position());

          // the steps
          cache.validationSteps++;
          cache.validationPhi += parm.position().phi();
          cache.validationEta += parm.position().eta();
          // reset the validation layer
          cache.validationLayer = nullptr;
        }
      }
      // ------------------------------------------validation section ----------------------------------
    }
    return parm.associatedSurface().createTrackParameters(updatedParameters[Trk::loc1],
                                                          updatedParameters[Trk::loc2],
                                                          updatedParameters[Trk::phi],
                                                          updatedParameters[Trk::theta],
                                                          updatedParameters[Trk::qOverP],
                                                          updatedCovariance);
  }
  return parm.clone();
}

void
Trk::MaterialEffectsUpdator::validationActionImpl(Cache& cache) const
{
  cache.validationEta = 0.;
  cache.validationPhi = 0.;
  cache.validationSteps = 0;
}

void
Trk::MaterialEffectsUpdator::modelActionImpl(Cache& cache, const Trk::TrackParameters* /*parm*/) const
{
  cache.accumulatedElossSigma = 0;
}

bool Trk::MaterialEffectsUpdator::checkCovariance(AmgSymMatrix(5) & updated) const
{
  if (updated(Trk::phi, Trk::phi) > 0. && updated(Trk::theta, Trk::theta) > 0. &&
      updated(Trk::qOverP, Trk::qOverP) > 0.) {
    return true;
  }

  // given an update
  ATH_MSG_DEBUG("  [-] update material with 'removeNoise' results in negative covariance entries. Skipping update.");
  return false;
}
