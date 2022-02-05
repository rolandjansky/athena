/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

/*
 * ParticleCaloExtensionTool.h  - implements the IParticleCaloExtenions
 * Interface begin : Summer 2014 updated : 2018-2019 for AthenaMT authors :
 * Niels van Eldik (CERN PH-ATC),Christos Anastopoulos
 */

#ifndef TRKPARTICLECREATOR_PARTICLECALOEXTENSIONTOOL_H
#define TRKPARTICLECREATOR_PARTICLECALOEXTENSIONTOOL_H
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

/*interface */
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"

/* interfce for the extrapolator tool*/

#include "CaloTrackingGeometry/ICaloSurfaceBuilder.h"
#include "TrkDetDescrUtils/GeometrySignature.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkExInterfaces/IExtrapolator.h"
/*
 * xAOD includes
 */
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTracking/NeutralParticle.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"

class AtlasDetectorID;
class CaloDetDescrManager;
namespace Trk {

class IExtrapolator;

class ParticleCaloExtensionTool final
  : virtual public IParticleCaloExtensionTool
  , public AthAlgTool
{
public:
  ParticleCaloExtensionTool(const std::string&,
                            const std::string&,
                            const IInterface*);
  virtual ~ParticleCaloExtensionTool();

  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;

  /*
   * Implement the IParticleCaloExtension methods
   * see IParticleCaloExtension.h for
   * documentation
   */
  virtual std::unique_ptr<Trk::CaloExtension> caloExtension(
    const EventContext& ctx,
    const xAOD::IParticle& particle) const override final;

  virtual const Trk::CaloExtension* caloExtension(
    const EventContext& ctx,
    const xAOD::IParticle& particle,
    IParticleCaloExtensionTool::Cache& cache) const override final;

  virtual const Trk::CaloExtension* caloExtension(
    const xAOD::IParticle& particle,
    const CaloExtensionCollection& cache) const override final;

  virtual StatusCode caloExtensionCollection(
    const EventContext& ctx,
    const xAOD::IParticleContainer& particles,
    const std::vector<bool>& mask,
    CaloExtensionCollection& caloextensions) const override final;

  virtual std::unique_ptr<Trk::CaloExtension> caloExtension(
    const EventContext& ctx,
    const TrackParameters& startPars,
    PropDirection propDir,
    ParticleHypothesis particleType) const override final;

  std::vector<std::unique_ptr<Trk::Surface>> caloSurfacesFromLayers(
    const std::vector<CaloSampling::CaloSample>& clusterLayers,
    double eta,
    const CaloDetDescrManager& caloDD) const override final;

  virtual std::vector<std::pair<CaloSampling::CaloSample,
                                std::unique_ptr<const Trk::TrackParameters>>>
  surfaceCaloExtension(
    const EventContext& ctx,
    const TrackParameters& startPars,
    const std::vector<CaloSampling::CaloSample>& clusterLayers,
    const std::vector<std::unique_ptr<Trk::Surface>>& caloSurfaces,
    ParticleHypothesis particleType) const override final;

  virtual std::vector<std::pair<CaloSampling::CaloSample,
                                std::unique_ptr<const Trk::TrackParameters>>>
  layersCaloExtension(
    const EventContext& ctx,
    const TrackParameters& startPars,
    const std::vector<CaloSampling::CaloSample>& clusterLayers,
    double eta,
    const CaloDetDescrManager& caloDD,
    ParticleHypothesis particleType) const override final;

private:
  /*
   * Internal methods for different IParticle types
   */
  std::unique_ptr<Trk::CaloExtension> caloExtension(
    const EventContext& ctx,
    const xAOD::TruthParticle& particle) const;
  std::unique_ptr<Trk::CaloExtension> caloExtension(
    const EventContext& ctx,
    const xAOD::NeutralParticle& particle) const;
  std::unique_ptr<Trk::CaloExtension> caloExtension(
    const EventContext& ctx,
    const xAOD::TrackParticle& particle) const;

  ToolHandle<Trk::IExtrapolator> m_extrapolator{ this, "Extrapolator", "" };

  /** @brief Tool to build calorimeter layer surfaces */
  ToolHandle<ICaloSurfaceBuilder> m_calosurf{
    this,
    "CaloSurfaceBuilder",
    "",
    "Tool to build calorimeter layer surfaces"
  };
  Gaudi::Property<std::string> m_particleTypeName{
    this,
    "ParticleType",
    "muon",
    "The particle type used to decide for the extrapolation strategy"
    ": muon, pion, electron,nonInteracting"
  };
  Gaudi::Property<bool> m_startFromPerigee{ this,
                                            "StartFromPerigee",
                                            false,
                                            "Start from Perigee" };
  const AtlasDetectorID* m_detID;
  ParticleHypothesis m_particleStrategy;

  Gaudi::Property<unsigned int> m_extrapolDetectorID{
    this,
    "ExtrapolationDetectorID",
    Trk::Calo,
    "The detector this tool should extrapolate through. Expects a "
    "Trk::GeometrySignature enum value."
  };
};
} // namespace Trk

#endif
