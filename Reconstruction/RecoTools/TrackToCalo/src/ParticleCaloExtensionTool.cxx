/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "ParticleCaloExtensionTool.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkCaloExtension/CaloExtension.h"
#include "TrkCaloExtension/CaloExtensionCollection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "xAODEgamma/Electron.h"
#include "xAODMuon/Muon.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "xAODTruth/TruthVertex.h"
namespace Trk {
ParticleCaloExtensionTool::ParticleCaloExtensionTool(const std::string& t,
                                                     const std::string& n,
                                                     const IInterface* p)
  : AthAlgTool(t, n, p)
  , m_detID(nullptr)
  , m_particleStrategy(muon)
{
  declareInterface<IParticleCaloExtensionTool>(this);
}

ParticleCaloExtensionTool::~ParticleCaloExtensionTool() {}

StatusCode
ParticleCaloExtensionTool::initialize()
{
  /* Retrieve track extrapolator from ToolService */
  ATH_CHECK(m_extrapolator.retrieve());
  // Retrieve the calo surface builder
  /* Retrieve the Atlas detector ID */
  ATH_CHECK(detStore()->retrieve(m_detID, "AtlasID"));
  /* convert string to proper particle type enum*/
  if (m_particleTypeName == "nonInteracting") {
    m_particleStrategy = nonInteracting;
  } else if (m_particleTypeName == "muon") {
    m_particleStrategy = muon;
  } else if (m_particleTypeName == "pion") {
    m_particleStrategy = pion;
  } else if (m_particleTypeName == "electron") {
    m_particleStrategy = electron;
  } else {
    ATH_MSG_WARNING("Unsupported particle type, using strategy based on type "
                    << m_particleTypeName);
  }
  if (!m_calosurf.empty()) {
    ATH_CHECK(m_calosurf.retrieve());
  } else {
    m_calosurf.disable();
  }
  ATH_MSG_INFO(" Using strategy based on particle type "
               << m_particleTypeName << " enum value " << m_particleStrategy);
  return StatusCode::SUCCESS;
}

StatusCode
ParticleCaloExtensionTool::finalize()
{
  return StatusCode::SUCCESS;
}

std::unique_ptr<Trk::CaloExtension>
ParticleCaloExtensionTool::caloExtension(const EventContext& ctx,
                                         const xAOD::IParticle& particle) const
{
  if (particle.type() == xAOD::Type::TrackParticle) {
    const xAOD::TrackParticle* trackParticle =
      static_cast<const xAOD::TrackParticle*>(&particle);
    return caloExtension(ctx, *trackParticle);
  }
  if (particle.type() == xAOD::Type::TruthParticle) {
    const xAOD::TruthParticle* truthParticle =
      static_cast<const xAOD::TruthParticle*>(&particle);
    return caloExtension(ctx, *truthParticle);
  } else if (particle.type() == xAOD::Type::Electron) {
    const xAOD::Electron* el = static_cast<const xAOD::Electron*>(&particle);
    if (el->trackParticle()) {
      return caloExtension(ctx, *(el->trackParticle()));
    }
  } else if (particle.type() == xAOD::Type::Muon) {
    const xAOD::Muon* muon = static_cast<const xAOD::Muon*>(&particle);
    if (muon->primaryTrackParticle()) {
      return caloExtension(ctx, *(muon->primaryTrackParticle()));
    }
  } else if (particle.type() == xAOD::Type::NeutralParticle) {
    const xAOD::NeutralParticle* neutralParticle =
      static_cast<const xAOD::NeutralParticle*>(&particle);
    return caloExtension(ctx, *neutralParticle);
  }
  ATH_MSG_WARNING("Unsupported IParticle type");
  return nullptr;
}

const Trk::CaloExtension*
ParticleCaloExtensionTool::caloExtension(
  const EventContext& ctx,
  const xAOD::IParticle& particle,
  IParticleCaloExtensionTool::Cache& cache) const
{
  /*if not there , default ctor for unique_ptr (nullptr)*/
  std::unique_ptr<Trk::CaloExtension>& extension = cache[particle.index()];
  if (extension == nullptr) {
    extension = caloExtension(ctx, particle);
  }
  return extension.get();
}

const Trk::CaloExtension*
ParticleCaloExtensionTool::caloExtension(
  const xAOD::IParticle& particle,
  const CaloExtensionCollection& cache) const
{
  size_t index = particle.index();
  if (index < cache.size()) {
    return cache[index];
  }
  ATH_MSG_WARNING("cache size smaller than particle index");
  return nullptr;
}

StatusCode
ParticleCaloExtensionTool::caloExtensionCollection(
  const EventContext& ctx,
  const xAOD::IParticleContainer& particles,
  const std::vector<bool>& mask,
  CaloExtensionCollection& caloextensions) const
{
  const size_t numparticles = particles.size();

  if (mask.size() != numparticles) {
    ATH_MSG_ERROR("mask does not have the same size as in input collection");
    return StatusCode::FAILURE;
  }
  caloextensions.reserve(numparticles);
  /* Either create a proper CaloExtension or otherwise a dummy one
   * i.e one with no intersections
   */
  for (size_t i = 0; i < numparticles; ++i) {
    if (mask[i]) {
      std::unique_ptr<Trk::CaloExtension> extension =
        caloExtension(ctx, *(particles[i]));
      caloextensions.push_back(std::move(extension));
    } else {
      caloextensions.push_back(nullptr);
    }
  }
  return StatusCode::SUCCESS;
}

std::unique_ptr<Trk::CaloExtension>
ParticleCaloExtensionTool::caloExtension(
  const EventContext& ctx,
  const xAOD::TruthParticle& particle) const
{
  ParticleHypothesis particleType = muon;
  if (abs(particle.pdgId()) == 11) {
    particleType = muon;
  } else if (abs(particle.pdgId()) == 13) {
    particleType = muon;
  }
  // get start parameters
  const xAOD::TruthVertex* pvtx = particle.prodVtx();
  if (pvtx == nullptr) {
    return nullptr;
  }
  double charge = particle.charge();
  Amg::Vector3D pos(pvtx->x(), pvtx->y(), pvtx->z());
  Amg::Vector3D mom(particle.px(), particle.py(), particle.pz());
  // Aproximate neutral particles as charged with infinite momentum
  if (particle.isNeutral()) {
    charge = 1.;
    mom.normalize();
    mom *= 1e10;
  }
  Trk::CurvilinearParameters startPars(pos, mom, charge);
  // get extension
  return caloExtension(ctx, startPars, alongMomentum, particleType);
}

std::unique_ptr<Trk::CaloExtension>
ParticleCaloExtensionTool::caloExtension(
  const EventContext& ctx,
  const xAOD::NeutralParticle& particle) const
{
  // create start parameters
  const Trk::NeutralPerigee& perigee = particle.perigeeParameters();
  double charge = 1.;
  const Amg::Vector3D& pos(perigee.position());
  Amg::Vector3D mom(perigee.momentum());
  // Aproximate neutral particles as charged with infinite momentum
  mom.normalize();
  mom *= 1e10;
  Trk::CurvilinearParameters startPars(pos, mom, charge);
  // get extension
  return caloExtension(ctx, startPars, alongMomentum, muon);
}

std::unique_ptr<Trk::CaloExtension>
ParticleCaloExtensionTool::caloExtension(
  const EventContext& ctx,
  const xAOD::TrackParticle& particle) const
{
  /*
   * In principle we will extrapolate either from the perigee or
   * from the last measurement of the trackParticle.
   *
   * For electrons the extrapolation will be done as a muon since this
   * is the closest to non-interacting in the calorimeter
   * while still providing intersections.
   *
   * Also for electrons track Particles (GSF)
   * we retain the perigee,first,last.
   * even in AOD.
   */

  // Start with what the user opted as strategy
  ParticleHypothesis particleType = m_particleStrategy;
  // special treatment when we want an electron strategy
  if (m_particleStrategy == electron) {
    particleType = muon;
    if (m_startFromPerigee) {
      return caloExtension(
        ctx, particle.perigeeParameters(), alongMomentum, particleType);
    } else {
      unsigned int index(0);
      if (particle.indexOfParameterAtPosition(index, xAOD::LastMeasurement)) {
        const Trk::CurvilinearParameters& lastParams =
          particle.curvilinearParameters(index);
        const Amg::Vector3D& position = lastParams.position();
        // Muon Entry is around z 6783 and r  4255
        if (position.perp() > 4200. || std::abs(position.z()) > 6700.) {
          ATH_MSG_WARNING("Can extrapolate to calo along momentum if already "
                          "past it. Probematic parameters : "
                          << lastParams);
          return nullptr;
        }
        return caloExtension(ctx, lastParams, alongMomentum, particleType);
      }
    }
  }
  /*
   * The following are tuned mainly for
   * the strategy we want to follow for muons.
   * But should also work well as a generic
   * strategy. So is the default/fallback.
   */

  // Check if the fitter has used electron hypo
  // and force using the muon strategy
  if (particle.particleHypothesis() == xAOD::electron) {
    particleType = muon;
  }
  if (m_startFromPerigee || !particle.track()) {
    bool idExit = true;
    // Muon Entry is around z 6783 and r  4255
    if (std::abs(particle.perigeeParameters().position().z()) > 6700.)
      idExit = false;
    if (particle.perigeeParameters().position().perp() > 4200.)
      idExit = false;
    PropDirection propDir = idExit ? alongMomentum : oppositeMomentum;
    return caloExtension(
      ctx, particle.perigeeParameters(), propDir, particleType);
  }

  // Extrapolation from last track parameters
  const Track& track = *particle.track();
  // Look-up the parameters closest to the calorimeter in
  // ID and muon system
  ATH_MSG_DEBUG("trying to add calo layers");
  const TrackParameters* idExitParamers = nullptr;
  const TrackParameters* muonEntryParamers = nullptr;
  DataVector<const TrackStateOnSurface>::const_iterator itTSoS =
    track.trackStateOnSurfaces()->begin();
  for (; itTSoS != track.trackStateOnSurfaces()->end(); ++itTSoS) {
    // select state with track parameters on a measurement
    if (!(**itTSoS).trackParameters() ||
        !(**itTSoS).type(TrackStateOnSurface::Measurement) ||
        (**itTSoS).type(TrackStateOnSurface::Outlier)) {
      continue;
    }

    const Identifier& id = (**itTSoS)
                             .trackParameters()
                             ->associatedSurface()
                             .associatedDetectorElementIdentifier();
    if (m_detID->is_indet(id))
      idExitParamers = (**itTSoS).trackParameters();
    if (m_detID->is_muon(id) && !muonEntryParamers)
      muonEntryParamers = (**itTSoS).trackParameters();
  }
  // require at least one of them
  if (!idExitParamers && !muonEntryParamers) {
    idExitParamers = track.perigeeParameters();
  }
  // pick start parameters, start in ID if possible
  const TrackParameters* startPars =
    idExitParamers ? idExitParamers : muonEntryParamers;

  if (!startPars) {
    ATH_MSG_WARNING("Failed to find  start parameters");
    return nullptr;
  }
  PropDirection propDir = idExitParamers ? alongMomentum : oppositeMomentum;

  return caloExtension(ctx, *startPars, propDir, particleType);
}

std::unique_ptr<Trk::CaloExtension>
ParticleCaloExtensionTool::caloExtension(const EventContext& ctx,
                                         const TrackParameters& startPars,
                                         PropDirection propDir,
                                         ParticleHypothesis particleType) const
{
  ATH_MSG_DEBUG("looking up calo states: r "
                << startPars.position().perp() << " z "
                << startPars.position().z() << " momentum "
                << startPars.momentum().mag());

  // pointers to hold results and go
  std::vector<const TrackStateOnSurface*>* material = nullptr;

  /* The last argument to the extrapolate  overload
   * corresponds  to a GeometrySignature value from
   * TrkDetDescrUtils/GeometrySignature.h
   * The extrapolation stop at the indicated subdetector exit
   */
  const std::vector<std::pair<const Trk::TrackParameters*, int>>*
    caloParameters = m_extrapolator->extrapolate(
      ctx, startPars, propDir, particleType, material, m_extrapolDetectorID);

  if (material) {
    ATH_MSG_DEBUG("Got material " << material->size());
    for (auto& m : *material) {
      ATH_MSG_DEBUG(
        " layer "
        << " param " << m->trackParameters() << " pos: r "
        << (m->trackParameters() ? m->trackParameters()->position().perp()
                                 : -999)
        << " z "
        << (m->trackParameters() ? m->trackParameters()->position().z() : -999)
        << " pt "
        << (m->trackParameters() ? m->trackParameters()->momentum().perp()
                                 : -999)
        << " mat "
        << (m->materialEffectsOnTrack()
              ? m->materialEffectsOnTrack()->thicknessInX0()
              : -999));
      delete m;
    }
    delete material;
  }
  if (!caloParameters) {
    return nullptr;
  }
  TrackParametersIdHelper parsIdHelper;

  // create final object
  const TrackParameters* caloEntry = nullptr;
  const TrackParameters* muonEntry = nullptr;
  std::vector<CurvilinearParameters> caloLayers;
  caloLayers.reserve(caloParameters->size() - 1);
  ATH_MSG_DEBUG(" Found calo parameters: " << caloParameters->size()
                                           << "  extrapolation exit ID="
                                           << m_extrapolDetectorID);

  for (const auto& p : *caloParameters) {
    if (!p.first) {
      continue;
    }
    // assign parameters
    // calo aentry muon entry and the crossed calo layers
    if (p.second == 1 && propDir == Trk::alongMomentum) {
      caloEntry = p.first;
    } else if (p.second == 3 && propDir == Trk::oppositeMomentum) {
      caloEntry = p.first;
    } else if (p.second == 3 && propDir == Trk::alongMomentum) {
      muonEntry = p.first;
    } else if (p.second == 4 && propDir == Trk::oppositeMomentum) {
      muonEntry = p.first;
    } else {
      bool isEntry = p.second > 0;
      TrackParametersIdentifier id = parsIdHelper.encode(
        AtlasDetDescr::fFirstAtlasCaloTechnology,
        static_cast<CaloSampling::CaloSample>(abs(p.second) % 1000),
        isEntry);
      /*
       * We construct curvilinear parameters which we push
       * back to the caloLayers.
       * We need to check if the parameters are already
       * curvillinear.
       * And if they are we need to clone the
       * covariance matrix
       */
      if (p.first->type() != Trk::Curvilinear) {
        caloLayers.emplace_back(CurvilinearParameters(p.first->position(),
                                                      p.first->momentum(),
                                                      p.first->charge(),
                                                      std::nullopt,
                                                      id));
        delete p.first;
      } else {
        std::optional<AmgSymMatrix(5)> covariance(std::nullopt);
        if (p.first->covariance()) {
          covariance = AmgSymMatrix(5)(*(p.first->covariance()));
        }
        caloLayers.emplace_back(CurvilinearParameters(p.first->position(),
                                                      p.first->momentum(),
                                                      p.first->charge(),
                                                      std::move(covariance),
                                                      id));
        delete p.first;
      }
    }
  }

  if (!muonEntry && propDir == Trk::oppositeMomentum &&
      fabs(startPars.position().perp() - 4255.) < 1.) {
    // muonEntry is right at the startPars position
    muonEntry = startPars.clone();
  }
  delete caloParameters;

  return std::make_unique<Trk::CaloExtension>(
    caloEntry, muonEntry, std::move(caloLayers));
}

std::vector<std::unique_ptr<Trk::Surface>>
ParticleCaloExtensionTool::caloSurfacesFromLayers(
  const std::vector<CaloSampling::CaloSample>& clusterLayers,
  double eta,
  const CaloDetDescrManager& caloDD) const
{
  // Create surfaces at the layers
  std::vector<std::unique_ptr<Trk::Surface>> caloSurfaces;
  caloSurfaces.reserve(clusterLayers.size());
  for (CaloSampling::CaloSample lay : clusterLayers) {
    auto* surf = m_calosurf->CreateUserSurface(lay, 0., eta, &caloDD);
    if (surf) {
      caloSurfaces.emplace_back(surf);
    }
  }

  return caloSurfaces;
}

std::vector<std::pair<CaloSampling::CaloSample,
                      std::unique_ptr<const Trk::TrackParameters>>>
ParticleCaloExtensionTool::surfaceCaloExtension(
  const EventContext& ctx,
  const TrackParameters& startPars,
  const std::vector<CaloSampling::CaloSample>& clusterLayers,
  const std::vector<std::unique_ptr<Trk::Surface>>& caloSurfaces,
  ParticleHypothesis particleType) const
{
  std::vector<std::pair<CaloSampling::CaloSample,
                        std::unique_ptr<const Trk::TrackParameters>>>
    caloParameters{};
  const auto* lastImpact = &startPars;
  // Go into steps from layer to layer
  size_t numSteps = caloSurfaces.size();
  for (size_t i = 0; i < numSteps; ++i) {
    const auto* nextImpact = m_extrapolator->extrapolate(
      ctx, *lastImpact, *(caloSurfaces[i]), alongMomentum, false, particleType);
    if (nextImpact) {
      caloParameters.emplace_back(clusterLayers[i], nextImpact);
      lastImpact = nextImpact;
    }
  }
  return caloParameters;
}

std::vector<std::pair<CaloSampling::CaloSample,
                      std::unique_ptr<const Trk::TrackParameters>>>
ParticleCaloExtensionTool::layersCaloExtension(
  const EventContext& ctx,
  const TrackParameters& startPars,
  const std::vector<CaloSampling::CaloSample>& clusterLayers,
  double eta,
  const CaloDetDescrManager& caloDD,
  ParticleHypothesis particleType) const
{

  // Create surfaces at the layers
  std::vector<std::unique_ptr<Trk::Surface>> caloSurfaces =
    caloSurfacesFromLayers(clusterLayers, eta, caloDD);

  return surfaceCaloExtension(
    ctx, startPars, clusterLayers, caloSurfaces, particleType);
}

} // end of namespace Trk
