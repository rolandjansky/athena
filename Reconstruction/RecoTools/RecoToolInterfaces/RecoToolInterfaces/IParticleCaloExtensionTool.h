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
#include "xAODTracking/TrackParticle.h"
#include "TrkCaloExtension/CaloExtension.h"
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
   * The cache is an argument that should be handled by the calling algorithm
   * One could foresee futher changes on the caching for sharing between algs
   * provided that they use the same extrapolator configuration.
   * For example e/gamma does no eloss in calo while muons typically will do eloss. 
   */


  /** Method returning the calo layers crossed 
   * by an IParticle track or itself if it is a TruthParticle
   * @param IParticle     reference to the Particle
   * @param extension     unique_ptr ref to a CaloExtension 
   * @return true if the call was successful
   */
  virtual bool caloExtension( const xAOD::IParticle& particle, std::unique_ptr<Trk::CaloExtension>& extension ) const = 0;
  
  /** Method returning the calo layers crossed 
   * by an IParticle track or itself if it is a TruthParticle
   * @param IParticle     reference to the Particle
   * @param extension     ptr to a CaloExtesion. 
   * @param cache         An alg using the same IParticle multiple times can pass a cache of
   * the form std::unordered_map<size_t,std::unique_ptr<Trk::CaloExtension>> , where the key is the  
   * value of IParticle::index().
   * In this case it is the cache that retains ownership of the pointers
   * This makes sense if one expects repeated extrapolation for the same trackParticles from
   * a single algorithm, and wants to have a cache outside the relevant loops. 
   * @return true if the call was successful
   */  
  virtual bool caloExtension( const xAOD::IParticle& particle, Trk::CaloExtension* extension, 
                              std::unordered_map<size_t,std::unique_ptr<Trk::CaloExtension>>& cache ) const = 0;

  /**
   * Method returning a unique_ptr to the caloExtension
   */
  virtual std::unique_ptr<Trk::CaloExtension> caloExtension( const TrackParameters& startPars, 
                                                             PropDirection propDir, ParticleHypothesis particleType ) const =0;

  static const InterfaceID& interfaceID( ) ;
};

inline const InterfaceID& Trk::IParticleCaloExtensionTool::interfaceID() {
  return IID_IParticleCaloExtensionTool; 
}

} // end of namespace

#endif 
