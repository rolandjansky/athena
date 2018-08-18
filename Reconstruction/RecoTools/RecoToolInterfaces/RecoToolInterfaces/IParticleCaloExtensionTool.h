/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

///////////////////////////////////////////////////////////////////
// IParticleCaloExtensionTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ITRKTRACKPARTICLECALOEXTENSIONTOOL_H
#define ITRKTRACKPARTICLECALOEXTENSIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "TrkCaloExtension/CaloExtension.h"
#include <vector>
#include <unordered_map>
#include <memory>


namespace Trk 
{
static const InterfaceID IID_IParticleCaloExtensionTool("Trk::IParticleCaloExtensionTool", 1, 0);

/** @brief Interface for extending Particles with calo intersections */
class IParticleCaloExtensionTool : virtual public IAlgTool {
public:

  /* 
   * Notes on the AthenaMT migration 
   * The tool was using direct StoreGate access both
   * as a way to implement the cache
   * and as a way to handle the ownership/lifetime of the caloExtension.
   * For a 0th order migration the semantics of the cache and the ownership
   * were changed.
   */


  /** Method returning the calo layers crossed 
   * by an IParticle track or itself if it is a TruthParticle
   * @param particle     reference to the Particle
   * @param extension     unique_ptr ref to a CaloExtension 
   * @return true if the call was successful
   */
  virtual bool caloExtension( const xAOD::IParticle& particle, 
                              std::unique_ptr<Trk::CaloExtension>& extension ) const = 0;
  
  /** Method returning the calo layers crossed 
   * by an IParticle track or itself if it is a TruthParticle
   * An alg using the same IParticle multiple times can pass a cache of
   * the form std::unordered_map<size_t,std::unique_ptr<Trk::CaloExtension>>, 
   * where the key is the  value of IParticle::index().
   * This method can add elements to the cache.  
   * The cache retains ownership of the pointers (unique_ptr) 
   *
   * @param particle     reference to the Particle
   * @param extension     ptr to a CaloExtesion. 
   * @param cache         the cache 
   * @return true if the call was successful
   */  
  virtual bool caloExtension( const xAOD::IParticle& particle, 
                              Trk::CaloExtension* extension, 
                              std::unordered_map<size_t,std::unique_ptr<Trk::CaloExtension>>& cache ) const = 0;

  /** Method that can be used by algorithms that :
   * A. Have an  IParticleCollection
   * B. Define a mask of the size of that collection
   * C. Want back a vector alligned with the input collection 
   * i.e 1 to 1 correspondance with the input collection.
   * The vector can then be used as such to retrieve the extensions
   * or manipulated and written to storegate.
   *
   * @param particles The input collection
   * @param mask      contains true for the elements for which to permorm an extension, false otherwise          
   * @* @param caloextension Output to be filled,
   * will contain nullptr where the mask was false, otherwise it contains the relevant
   * result. 
   */  
   virtual StatusCode  caloExtensionCollection( const xAOD::IParticleContainer& particles, 
                                                const std::vector<bool>& mask,
                                                std::vector<std::unique_ptr<CaloExtension>>& caloextensions) const = 0;

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
