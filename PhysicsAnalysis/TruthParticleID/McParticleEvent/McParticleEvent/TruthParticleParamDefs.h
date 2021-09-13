/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// TruthParticleParamDefs.h 
// Header file for parameters definitions for TruthParticle
// Stolen from ElectronParamDefs.h
// Author  : Sebastien Binet : March 2006
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEEVENT_TRUTHPARTICLEPARAMDEFS_H
#define MCPARTICLEEVENT_TRUTHPARTICLEPARAMDEFS_H

// boost includes
#include "boost/array.hpp"

namespace TruthParticleParameters { 

  /// Enum for Cone size indexes (for isolation)
  enum ConeSize {
    etcone10 = 0,//<! ET in a cone of R=0.10
    etcone20 = 1,//<! ET in a cone of R=0.20
    etcone30 = 2,//<! ET in a cone of R=0.30
    etcone40 = 3,//<! ET in a cone of R=0.40
    etcone   = 4,//<! ET in a cone of R=0.45
    etcone50 = 5,//<! ET in a cone of R=0.50
    etcone60 = 6,//<! ET in a cone of R=0.60
    etcone70 = 7,//<! ET in a cone of R=0.70
    
    // keep this enum the last one
    NbrOfCones = 8
  };

  typedef boost::array<double, TruthParticleParameters::NbrOfCones> ConeCuts_t;

  /// The actual definition of delta R cuts for each cone
  inline
  double coneCut( const TruthParticleParameters::ConeSize idx ) {
    static const ConeCuts_t coneCuts = { 
      {
	0.10, // TruthParticleParameters::etcone10
	0.20, // TruthParticleParameters::etcone20
	0.30, // TruthParticleParameters::etcone30
	0.40, // TruthParticleParameters::etcone40
	0.45, // TruthParticleParameters::etcone
	0.50, // TruthParticleParameters::etcone50
	0.60, // TruthParticleParameters::etcone60
	0.70  // TruthParticleParameters::etcone70
      } 
    };
    return coneCuts[idx];
  }
} //> end namespace TruthParticleParameters

namespace McAod {

  /// An array of doubles of fixed size to modelize the Et isolations
  /// for different values of isolation radii' cones.
  typedef boost::array<double, TruthParticleParameters::NbrOfCones>
          EtIsolations;

} //> end namespace McAod

#endif // MCPARTICLEEVENT_TRUTHPARTICLEPARAMDEFS_H

