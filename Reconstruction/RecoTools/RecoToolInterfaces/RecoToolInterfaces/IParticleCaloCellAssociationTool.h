/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

///////////////////////////////////////////////////////////////////
// IParticleCaloCellAssociationTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ITRKTRACKPARTICLECALOCELLASSOCIATIONTOOL_H
#define ITRKTRACKPARTICLECALOCELLASSOCIATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODBase/IParticle.h"
#include "ParticleCaloExtension/ParticleCellAssociation.h"
#include "TrkCaloExtension/CaloExtensionCollection.h"
#include <memory>
class CaloCellContainer;

namespace Rec 
{

static const InterfaceID IID_IParticleCaloCellAssociationTool("Rec::IParticleCaloCellAssociationTool", 1, 0);

/** @brief Interface for extending Particles with calo cells */
class IParticleCaloCellAssociationTool : virtual public IAlgTool {
public:

  /** Method to get the ParticleCellAssociation for a given Particle
   * @param particle       input particle
   * @param dr             cone size used for the association
   * @param container      cell container to be used if provided 
   * @param extensionCache A calo extension cache to be used if provided
   * @return std::unique_ptr<ParticleCellAssociation>  
   */
  virtual std::unique_ptr< ParticleCellAssociation > particleCellAssociation( const xAOD::IParticle& particle, float dr, 
                                                                              const CaloCellContainer* container = nullptr,
                                                                              const CaloExtensionCollection* extensionCache = nullptr) const = 0;

  /** Method to get the ParticleCellAssociation for a given Particle
   * @param particle       input particle
   * @param dr             cone size used for the association
   * @ param cache         cache for keeping previous results
   * @param container      cell container to be used if provided 
   * @param extensionCache A calo extension cache to be used if provided
   * @return ParticleCellAssociation* (plain ptr cache has ownership)
   *
   * An alg looping over a single collection of IParticles  
   * re-using them multiple times can use a local  cache of
   * the form 
   * std::unordered_map<size_t,std::unique_ptr<ParticleCellAssociation >>.
   * where the key is the  value of IParticle::index() 
   *
   * This method adds the ParticleCellAssociation to the cache look-up table 
   * which retains ownership. 
   */  

  typedef std::unordered_map<size_t,std::unique_ptr< ParticleCellAssociation >> Cache;
  virtual ParticleCellAssociation* particleCellAssociation( const xAOD::IParticle& particle, float dr, Cache& cache,
                                                            const CaloCellContainer* container = nullptr,
                                                            const CaloExtensionCollection* extensionCache = nullptr) const = 0;
  

  static const InterfaceID& interfaceID( ) ;
};

inline const InterfaceID& IParticleCaloCellAssociationTool::interfaceID() {
  return IID_IParticleCaloCellAssociationTool; 
}

} // end of namespace

#endif 
