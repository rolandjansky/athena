// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauxAODHelpers.h 699770 2015-10-09 16:29:20Z griffith $
#ifndef XAOD_TAUXAODHELPERS_H
#define XAOD_TAUXAODHELPERS_H

//Truth Includes
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"

#include <cstddef>
#include <set>

namespace xAOD {

  namespace TauHelpers{
    
    // @brief Access to element link to object of type T stored in auxdata
    template<class T>
    const T* getLink(const xAOD::IParticle*, std::string, bool debug = false);
    
    ///@brief return the truthParticle associated to the given IParticle (if any)
    const xAOD::TruthParticle* getTruthParticle(const xAOD::IParticle*, bool debug = false);

  }// TauHelpers

} // namespace xAOD

#include "xAODTau/TauxAODHelpers.icc"
  
#endif // XAOD_TAUXAODHELPERS_H
