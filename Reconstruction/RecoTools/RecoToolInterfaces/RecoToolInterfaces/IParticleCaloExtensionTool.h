/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

/**  IParticleCaloExtensionTool.h
 *
 * Original by Niels van Eldik (2014)
 * Modified by Christos Anastopoulos for AthenaMT
 * and additional calo layer methods
 *
 * The purpose of the tool is to handle Particles
 * extrapolated to the calorimeter creating so
 * called extensions
 *
 * Methods accepting TrackParameters
 *
 * - caloExtension : extrapolate typically from parameters
 *   until the exit of the calorimeter (can be changed to be
 *   entry in the configuration). This typically uses the STEP
 *   propagator via the Extrapolator. This is the most
 *   generic case if one wants all intersections and
 *   the full path inside the calo.
 *
 * - layersCaloExtension : Extrapolate to particular
 *   layers using the configured Propagator in the
 *   Extrapolator. This can be faster but assumes
 *   explicit knowledge of which layer intersection to
 *   target
 *
 *
 * Methods accepting  IParticle :
 *
 * These will extract the needed information and form
 * the proper track parameters. Then will call the
 * caloExtension method from above.
 *
 * Since we might want to repeat the same extrapolation
 * from different domains or multiple times from the
 * same domain we allow for the following strategies
 * for caching.
 *
 * 1. No-caching
 * std::unique_ptr<Trk::CaloExtension> caloExtension( const xAOD::IParticle&
 * particle) const will provide back a Trk:: CaloExtention wrapped in a
 * unique_ptr. The memory lifetime is determined by the unique_ptr
 *
 * 2. In-Algorithm Caching.
 * An algorithm might extrapolate the same particle multiple times.
 * In this case it can employ a cache of type
 * std::unordered_map<size_t,std::unique_ptr<Trk::CaloExtension>>
 * The method
 * const Trk::CaloExtension* caloExtension( const xAOD::IParticle& particle,
 *                                          IParticleCaloExtensionTool::Cache&
 * cache ) const fills in the cache with the relevant extension The extension
 * that is provided as a ptr to a const CaloExtension is just the last element
 * added to that cache. The cache owns the elements.
 *
 * 3. Inter-algorithm caching.
 * In this case an central algorithm A
 * creates a CaloExtensionCollection in SG
 * via the proper WriteHandle mechanism.
 * Algorithms B, C , D ...
 * Can retrieve this collection via ReadHandles.
 * This scenario is facilitated by the pair of methods:
 * StatusCode caloExtensionCollection( const xAOD::IParticleContainer&
 * particles, const std::vector<bool>& mask, CaloExtensionCollection&
 * caloextensions) const and const Trk::CaloExtension* caloExtension( const
 * xAOD::IParticle& particle, const CaloExtensionCollection& cache ) const
 */

#ifndef ITRKTRACKPARTICLECALOEXTENSIONTOOL_H
#define ITRKTRACKPARTICLECALOEXTENSIONTOOL_H

#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "TrkCaloExtension/CaloExtension.h"
#include "TrkCaloExtension/CaloExtensionCollection.h"
#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTracking/TrackParticle.h"
#include <memory>
#include <unordered_map>
#include <vector>

class CaloDetDescrManager;
namespace Trk {
static const InterfaceID
  IID_IParticleCaloExtensionTool("Trk::IParticleCaloExtensionTool", 1, 0);

/** @brief Interface for extending Particles with calo intersections */
class IParticleCaloExtensionTool : virtual public IAlgTool
{
public:
  typedef std::unordered_map<size_t, std::unique_ptr<Trk::CaloExtension>> Cache;

  /** Method returning the calo layers crossed
   * by the IParticle track or the IParticle itself
   * if it is  neutral/TruthParticle
   * The memory ownership is handled by the unique_ptr
   * @param ctx         event context needed for multithreading
   * @param particle    reference to the Particle
   * @return unique_ptr  to a CaloExtension
   */
  virtual std::unique_ptr<Trk::CaloExtension> caloExtension(
    const EventContext& ctx,
    const xAOD::IParticle& particle) const = 0;

  /** Method to return the calo layers crossed  (CaloExtension)
   * by the IParticle track or the IParticle itself
   * if it is  neutral/TruthParticle.
   *
   * An alg looping over a single collection of IParticles
   * re-using them multiple times can use a local cache of
   * the form
   * std::unordered_map<size_t,std::unique_ptr<Trk::CaloExtension>>.
   * The key is the  value of IParticle::index()
   *
   * This method adds the relevant extension to the cache look-up table
   * which retains ownership.
   *
   * @param ctx           event context needed for multithreading
   * @param particle      reference to the Particle
   * @param cache         the look-up table cache
   * @return ptr to a const CaloExtension (owned by the cache)
   */
  virtual const Trk::CaloExtension* caloExtension(
    const EventContext& ctx,
    const xAOD::IParticle& particle,
    Cache& cache) const = 0;

  /** Method to return the calo layers crossed  (CaloExtension)
   * by the IParticle track or the IParticle itself
   * if it is  neutral/TruthParticle
   * It returns the result stored in the CaloExtensionCollection
   * created by the caloExtensionCollection method below.
   * In this case, the CaloExtensionCollection (DataVector<CaloExtension>)
   * owns the caloExtension.
   *
   * @param particle     reference to the Particle
   * @param cache        The CaloExtensionCollections
   * @return ptr to a const CaloExtension (owned by the cache)
   */
  virtual const Trk::CaloExtension* caloExtension(
    const xAOD::IParticle& particle,
    const CaloExtensionCollection& cache) const = 0;

  /** Method that can be used by algorithms that :
   * A. Have an  IParticleCollection
   * B. Define a mask of the size of that collection
   * C. Want back a Calo Extension Collection alligned with the input collection
   * i.e 1 to 1 correspondance with the input collection.
   * The Collection can then be used as such to retrieve the extensions
   * or manipulated and written to StoreGate.
   *
   * @param ctx       event context needed for multithreading
   * @param particles The input collection
   * @param mask      contains true for the elements for which to permorm an
   * extension, false otherwise
   * @return caloextension Output to be filled,
   * will contain unfilled CaloExtension where the mask was false, otherwise it
   * contains the relevant result.
   */
  virtual StatusCode caloExtensionCollection(
    const EventContext& ctx,
    const xAOD::IParticleContainer& particles,
    const std::vector<bool>& mask,
    CaloExtensionCollection& caloextensions) const = 0;

  /**
   * Method returning a unique_ptr to the caloExtension given the relevant
   * starting point parameters , direction and particle hypothesis
   *
   * @param ctx          event context needed for multithreading
   * @param startPars    the starting track parameters
   * @param propDir      extrapolation direction
   * @param particleType type of particle
   */
  virtual std::unique_ptr<Trk::CaloExtension> caloExtension(
    const EventContext& ctx,
    const TrackParameters& startPars,
    PropDirection propDir,
    ParticleHypothesis particleType) const = 0;

  /**
   * Method returning a vector of calo surfaces give calo
   * layers/samplings
   * @param ctx          event context needed for multithreading
   * @param clusterLayers  the layers (should be ordered) we aim to
   * @param eta            eta used for the depth
   */
  virtual std::vector<std::unique_ptr<Trk::Surface>> caloSurfacesFromLayers(
    const std::vector<CaloSampling::CaloSample>& clusterLayers,
    double eta,
    const CaloDetDescrManager& caloDD) const = 0;

  /**
   * Method returning a vector of the Track Parameters at
   * layers/samplings
   * @param ctx          event context needed for multithreading
   * @param startPars    the starting track parameters
   * @param clusterLayers  the layers (should be ordered) we aim to
   * @param clusterSurfaces  the surfaces for each layer
   * @param particleType type of particle
   *
   * The clusterLayers and clusterSurfaces need to be 1-1
   * this can be done via the caloSurfacesFromLayers
   * method above.
   *
   */
  virtual std::vector<std::pair<CaloSampling::CaloSample,
                                std::unique_ptr<const Trk::TrackParameters>>>
  surfaceCaloExtension(
    const EventContext& ctx,
    const TrackParameters& startPars,
    const std::vector<CaloSampling::CaloSample>& clusterLayers,
    const std::vector<std::unique_ptr<Trk::Surface>>& caloSurfaces,
    ParticleHypothesis particleType) const = 0;

  /**
   * Method returning a vector of the Track Parameters at
   * layers/samplings
   * @param ctx          event context needed for multithreading
   * @param startPars    the starting track parameters
   * @param clusterLayers  the layers (should be ordered)  we aim to
   * @param eta            eta used for the depth
   * @param particleType type of particle
   */

  virtual std::vector<std::pair<CaloSampling::CaloSample,
                                std::unique_ptr<const Trk::TrackParameters>>>
  layersCaloExtension(
    const EventContext& ctx,
    const TrackParameters& startPars,
    const std::vector<CaloSampling::CaloSample>& clusterLayers,
    double eta,
    const CaloDetDescrManager& caloDD,
    ParticleHypothesis particleType = Trk::nonInteracting) const = 0;

  static const InterfaceID& interfaceID();
};

inline const InterfaceID&
Trk::IParticleCaloExtensionTool::interfaceID()
{
  return IID_IParticleCaloExtensionTool;
}

} // end of namespace

#endif
