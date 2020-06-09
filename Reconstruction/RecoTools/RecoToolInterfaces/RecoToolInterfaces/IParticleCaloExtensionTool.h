/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

/**  IParticleCaloExtensionTool.h
 * 
 * Original by Niels van Eldik (2014)
 * Modified by Christos Anastopoulos (2018) for AthenaMT
 *
 * The purpose of the tool is to handle extensions
 * of TrackParticles attached to an IParticle,
 * or of Neutral/Truth Particles.
 *
 * There are 3 different usage patterns.
 *
 * 1. No-caching
 * std::unique_ptr<Trk::CaloExtension> caloExtension( const xAOD::IParticle& particle) const
 * will provide back a Trk:: CaloExtention wrapped in a unique_ptr.
 * The memory lifetime is determined by the unique_ptr
 *
 * 2. In-Algorithm Caching.
 * An algorithm might extrapolate the same particle multiple times.
 * In this case it can employ a cache of type
 * std::unordered_map<size_t,std::unique_ptr<Trk::CaloExtension>>
 * The method 
 * const Trk::CaloExtension* caloExtension( const xAOD::IParticle& particle,
 *                                          IParticleCaloExtensionTool::Cache& cache ) const
 * fills in the cache with the relevant extension 
 * The extension that is provided as a ptr to a const CaloExtension
 * is just the last element added to that cache.
 * The cache owns the elements.
 *
 * 3. Inter-algorithm caching.
 * In this case an central algorithm A 
 * creates a CaloExtensionCollection in SG
 * via the proper WriteHandle mechanism.
 * Algorithms B, C , D ... 
 * Can retrieve this collection via ReadHandles.
 * This scenario is facilitated by the pair of methods:
 * StatusCode caloExtensionCollection( const xAOD::IParticleContainer& particles,
 *                          const std::vector<bool>& mask,
 *                          CaloExtensionCollection& caloextensions) const
 * and
 * const Trk::CaloExtension* caloExtension( const xAOD::IParticle& particle,
 *                                          const CaloExtensionCollection& cache ) const
 */


#ifndef ITRKTRACKPARTICLECALOEXTENSIONTOOL_H
#define ITRKTRACKPARTICLECALOEXTENSIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "TrkCaloExtension/CaloExtension.h"
#include "TrkCaloExtension/CaloExtensionCollection.h"
#include <vector>
#include <unordered_map>
#include <memory>

namespace Trk 
{
static const InterfaceID IID_IParticleCaloExtensionTool("Trk::IParticleCaloExtensionTool", 1, 0);

/** @brief Interface for extending Particles with calo intersections */
class IParticleCaloExtensionTool : virtual public IAlgTool {
public:
  typedef  std::unordered_map<size_t,std::unique_ptr<Trk::CaloExtension>> Cache;
  
  /** Method returning the calo layers crossed 
   * by the IParticle track or the IParticle itself 
   * if it is  neutral/TruthParticle
   * The memory ownership is handled by the unique_ptr 
   * @param ctx         event context needed for multithreading
   * @param particle    reference to the Particle
   * @param particle    reference to the Particle
   * @return unique_ptr  to a CaloExtension 
   */
  virtual std::unique_ptr<Trk::CaloExtension> caloExtension(const EventContext& ctx,
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
  virtual const Trk::CaloExtension*  caloExtension( const EventContext& ctx,
                                                    const xAOD::IParticle& particle, 
                                                    Cache& cache ) const = 0;


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
  virtual const Trk::CaloExtension*  caloExtension( const xAOD::IParticle& particle, 
                                                    const CaloExtensionCollection& cache ) const = 0;

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
   * @param mask      contains true for the elements for which to permorm an extension, false otherwise          
   * @return caloextension Output to be filled,
   * will contain unfilled CaloExtension where the mask was false, otherwise it contains the relevant
   * result. 
   */  
  virtual StatusCode  caloExtensionCollection( const EventContext& ctx,
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
  virtual std::unique_ptr<Trk::CaloExtension> caloExtension( const EventContext& ctx,
                                                             const TrackParameters& startPars, 
                                                             PropDirection propDir, 
                                                             ParticleHypothesis particleType ) const =0;



    /** old interfaces WITHOUT EventContext ------------------------------------------ */

  /** Method returning the calo layers crossed 
   * by the IParticle track or the IParticle itself 
   * if it is  neutral/TruthParticle
   * The memory ownership is handled by the unique_ptr 
   * @param particle    reference to the Particle
   * @return unique_ptr  to a CaloExtension 
   */
  virtual std::unique_ptr<Trk::CaloExtension> caloExtension(const xAOD::IParticle& particle) const;
  
  /** Method to return the calo layers crossed  (CaloExtension)
   * by the IParticle track or the IParticle itself 
   * if it is  neutral/TruthParticle.
   *
   * An alg looping over a single collection of IParticles  
   * re-using them multiple times can use a local  cache of
   * the form 
   * std::unordered_map<size_t,std::unique_ptr<Trk::CaloExtension>>.
   * The key is the  value of IParticle::index() 
   *
   * This method adds the relevant extension to the cache look-up table 
   * which retains ownership. 
   *
   * @param particle      reference to the Particle
   * @param cache         the look-up table cache 
   * @return ptr to a const CaloExtension (owned by the cache)
   */  
  virtual const Trk::CaloExtension*  caloExtension( const xAOD::IParticle& particle, 
                                                    Cache& cache ) const;


  /** Method that can be used by algorithms that :
   * A. Have an  IParticleCollection
   * B. Define a mask of the size of that collection
   * C. Want back a Calo Extension Collection alligned with the input collection 
   * i.e 1 to 1 correspondance with the input collection.
   * The Collection can then be used as such to retrieve the extensions
   * or manipulated and written to StoreGate.
   *
   * @param particles The input collection
   * @param mask      contains true for the elements for which to permorm an extension, false otherwise          
   * @return caloextension Output to be filled,
   * will contain unfilled CaloExtension where the mask was false, otherwise it contains the relevant
   * result. 
   */  
  virtual StatusCode  caloExtensionCollection( const xAOD::IParticleContainer& particles, 
                                               const std::vector<bool>& mask,
                                               CaloExtensionCollection& caloextensions) const;

  /**
   * Method returning a unique_ptr to the caloExtension given the relevant 
   * starting point parametes , direction and particle hypothesis
   */
  virtual std::unique_ptr<Trk::CaloExtension> caloExtension( const TrackParameters& startPars, 
                                                             PropDirection propDir, 
                                                             ParticleHypothesis particleType ) const;



    
  static const InterfaceID& interfaceID( ) ;
};

inline const InterfaceID& Trk::IParticleCaloExtensionTool::interfaceID() {
  return IID_IParticleCaloExtensionTool; 
}

} // end of namespace

#include "RecoToolInterfaces/IParticleCaloExtensionTool.icc"

#endif 
