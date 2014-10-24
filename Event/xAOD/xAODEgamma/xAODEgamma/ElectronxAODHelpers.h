// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElectronxAODHelpers.h 617854 2014-09-20 18:49:22Z christos $
#ifndef XAOD_ELECTRONXAODHELPERS_H
#define XAOD_ELECTRONXAODHELPERS_H


#include "xAODEgamma/ElectronFwd.h"
#include "xAODTracking/TrackParticleFwd.h"

#include <cstddef>
#include <set>

namespace xAOD {

  namespace EgammaHelpers{

    ///@brief Helper function for getting the "Original" Track Particles (i.e before GSF) via the electron
    const xAOD::TrackParticle* getOriginalTrackParticle(const xAOD::Electron* el);
    ///@brief Helper function for getting the "Original" Track Particles (i.e before GSF) via the GSF Track Particle
    const xAOD::TrackParticle* getOriginalTrackParticleFromGSF(const xAOD::TrackParticle* trkPar);

    /** Return a list of all or only the best TrackParticle associated to the object. 
      * (used for track isolation)
      * If useBremAssoc is set, get the original TrackParticle **/
    const std::set<const xAOD::TrackParticle*> getTrackParticles(const xAOD::Electron* el,
      bool useBremAssoc = true, bool allParticles = true);


    ///@brief return the number of Si hits in the track particle
    std::size_t numberOfSiHits(const xAOD::TrackParticle *tp);
    
  } //namaspace EgammaHelpers

} // namespace xAOD

#endif // XAOD_ELECTRONXAODHELPERS_H
