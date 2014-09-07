// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElectronxAODHelpers.h 611048 2014-08-09 01:48:26Z christos $
#ifndef XAOD_ELECTRONXAODHELPERS_H
#define XAOD_ELECTRONXAODHELPERS_H

#include "xAODEgamma/Electron.h"

namespace xAOD {

  namespace EgammaHelpers{
  /// Helper functions for getting the "Original" Track Particles (i.e before GSF)

    const xAOD::TrackParticle* getOriginalTrackParticle(const xAOD::Electron* el);
    const xAOD::TrackParticle* getOriginalTrackParticleFromGSF(const xAOD::TrackParticle* trkPar);
    
  } //namaspace EgammaHelpers

} // namespace xAOD

#endif // XAOD_ELECTRONXAODHELPERS_H
