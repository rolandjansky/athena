/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

/**  IParticleCaloExtensionTool.h
 * 
 * Original by Niels van Eldik (2014)
 * Modified by Christos Anastopoulos (2018) for AthenaMT
 *
 * The purpose of the tool is to handle extension
 * of TrackParticles attached to an IParticle
 * or of Neutral or Truth Particles.
 *
 * There are 3 different usage patterns.
 *
 * 1. No-cachhing
 * The following method 
 * bool caloExtension( const xAOD::IParticle& particle, 
 * std::unique_ptr<Trk::CaloExtension>& extension )const
 * will provide back CaloExtention wrapped in a unique_ptr
 * the memory lifetime is determined by the unique_ptr
 *
 * 2. In-Algorithm Caching.
 * An algorithm might extrapolate the same particle multiple times.
 * In this case it can employ a cache of type
 * std::unordered_map<size_t,std::unique_ptr<Trk::CaloExtension>>
 * The method 
 * bool caloExtension( const xAOD::IParticle& particle,
 * const Trk::CaloExtension* extension,
 * Cache& cache ) const
 * then fills in the cache. The extension that is provided
 * is just the last element added to that cache.
 * The cache owns the elements.
 *
 * 3. Among algorithm/domain caching.
 * In this case an central algorithm A 
 * creates a CaloExtensionCollection in SG .
 * Algorithms B, C , D ... 
 * Can retrieve this collection.
 * This scenario is facilitated by the pair of methods:
 * StatusCode caloExtensionCollection( const xAOD::IParticleContainer& particles,
 *                          const std::vector<bool>& mask,
 *                          CaloExtensionCollection& caloextensions) const = 0;
 * and
 * bool caloExtension( const xAOD::IParticle& particle,
 *                       const Trk::CaloExtension* extension,
 *                       const CaloExtensionCollection& cache )*
 *
 *
 */


#ifndef ITRKTRACKPARTICLECALOEXTENSIONTOOL_H
#define ITRKTRACKPARTICLECALOEXTENSIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
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
   * @param particle     reference to the Particle
   * @param extension     unique_ptr ref to a CaloExtension 
   * @return true if the call was successful
   */
  virtual bool caloExtension( const xAOD::IParticle& particle, 
                              std::unique_ptr<Trk::CaloExtension>& extension ) const = 0;
  /** A helper method that  caches in a look-up table 
   * the calo layers crossed 
   * by the IParticle track or the IParticle itself 
   * if it is  neutral/TruthParticle.
   * An alg using the same IParticle multiple times can use a local  cache of
   * the form std::unordered_map<size_t,std::unique_ptr<Trk::CaloExtension>>, 
   * where the key is the  value of IParticle::index().
   * This method adds the relevant element to the cache look-up table 
   * which retains ownership of them. 
   * The ptr to a const CaloExtension points (unique_ptr::get())  
   * is the last element added to the  lookup-table. 
   * Will be deleted when the cache goes out of scope in the algorithm.
   *
   * @param particle      reference to the Particle
   * @param extension     ptr to a const CaloExtesion. 
   * @param cache         the look-up table cache 
   * @return true if the CaloExtension is valid.
   */  
  virtual bool caloExtension( const xAOD::IParticle& particle, 
                              const Trk::CaloExtension* extension, 
                              Cache& cache ) const = 0;


  /** A helper method to return from a cache collection
   * the calo layers crossed 
   * by the IParticle track or the IParticle itself 
   * if it is  neutral/TruthParticle
   * It just returns the result stored in the CaloExtensionCollection
   * created by (or in a similar way as ) the caloExtensionCollection
   * method below.
   * In this case, the CaloExtensionCollection (DataVector<CaloExtension>)
   * owns the element, so a ptr to that const element is returned.
   * You should not delete it.
   *
   * @param particle     reference to the Particle
   * @param extension    ptr to a const CaloExtesion owned by the cache 
   * @param cache        The CaloExtensionCollections   
   * @return true if the CaloExtension in the cache is valid.
   */  
  virtual bool caloExtension( const xAOD::IParticle& particle, 
                              const Trk::CaloExtension* extension, 
                              const CaloExtensionCollection& cache ) const = 0;

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
   * @* @param caloextension Output to be filled,
   * will contain unfilled CaloExtension where the mask was false, otherwise it contains the relevant
   * result. 
   */  
  virtual StatusCode  caloExtensionCollection( const xAOD::IParticleContainer& particles, 
                                               const std::vector<bool>& mask,
                                               CaloExtensionCollection& caloextensions) const = 0;

  /**
   * Method returning a unique_ptr to the caloExtension given the relevant 
   * starting point parametes , direction and particle hypothesis
   */
  virtual std::unique_ptr<Trk::CaloExtension> caloExtension( const TrackParameters& startPars, 
                                                             PropDirection propDir, 
                                                             ParticleHypothesis particleType ) const =0;

  static const InterfaceID& interfaceID( ) ;
};

inline const InterfaceID& Trk::IParticleCaloExtensionTool::interfaceID() {
  return IID_IParticleCaloExtensionTool; 
}

} // end of namespace

#endif 
