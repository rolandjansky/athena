/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingerKnownParticles.h 732386 2016-03-25 03:21:48Z ssnyder $
#ifndef CALORINGERTOOLS_RINGERKNOWNPARTICLES_H
#define CALORINGERTOOLS_RINGERKNOWNPARTICLES_H

#if 0
#ifndef INCLUDE_HEADER_ONLY
#define INCLUDE_HEADER_ONLY
#endif
#include "CaloRingerReaderUtils.h"
#undef INCLUDE_HEADER_ONLY
#endif

// Add the particles
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"

namespace Ringer {
/**
 * @brief template to get particle_t name and if it has cluster access.
 *
 *  - Use GetParticleProp< particle_t >::name to retrieve the particle name. 
 *
 *  - Use GetParticleProp< particle_t >::hasCluster to retrieve whether particle
 * has cluster access. 
 *
 * The particle must have its class name declared using
 * DECLARE_RINGER_KNOWN_PARTICLE macro.
 **/
template < typename particle_t > struct GetParticleProp;
} // End of namespace Ringer

/**
 * @brief macro for declaring particles known to Ringer algorithm.
 *
 * Use GetParticleName< particle_t >::name to retrieve the particle name. The
 * particle must have its class name declared using DECLARE_NAMED_CLASS macro.
 **/
#define DECLARE_RINGER_KNOWN_PARTICLE( _name, _hasCluster ) \
  namespace Ringer { \
    template < > struct GetParticleProp< _name > { \
      static constexpr const char *name = #_name; \
      static constexpr bool hasCluster = _hasCluster; \
      };\
  }

// Declare Ringer known particles:
DECLARE_RINGER_KNOWN_PARTICLE( const xAOD::Electron  , true  ) 
DECLARE_RINGER_KNOWN_PARTICLE( const xAOD::Photon    , true  ) 

// Ringer can be run with IParticles, but it won't use cluster information
// since it is not available:
DECLARE_RINGER_KNOWN_PARTICLE( const xAOD::IParticle , false ) 

#undef DECLARE_RINGER_KNOWN_PARTICLE

#endif // CALORINGERTOOLS_RINGERKNOWNPARTICLES_H
