// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODBASE_XAODBASEDICT_H
#define XAODBASE_XAODBASEDICT_H

// STL include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"
#include "AthContainers/tools/AuxTypeVectorFactory.h"

// Local include(s):
#include "xAODBase/IParticleContainer.h"
#include "xAODBase/IParticleHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODBASE {
      xAOD::IParticleContainer c1;
      DataLink< xAOD::IParticleContainer > l1;
      std::vector< DataLink< xAOD::IParticleContainer > > l2;
      ElementLink< xAOD::IParticleContainer > l3;
      std::vector< ElementLink< xAOD::IParticleContainer > > l4;
      std::vector< std::vector< ElementLink< xAOD::IParticleContainer > > > l5;
      SG::AuxTypeVectorFactory< DataLink< xAOD::IParticleContainer > > l6;
      SG::AuxTypeVectorFactory< ElementLink< xAOD::IParticleContainer > > l7;
      SG::AuxTypeVectorFactory< std::vector< ElementLink< xAOD::IParticleContainer > > > l8;
   };
}

// Not sure if this line is necessary but it shouldn't hurt...
template class DataVector<xAOD::IParticle>;

/// Helper macro for instantiating all the template functions that we need a
/// dictionary for.
#define INST_AUXFUNC( TYPE )                                                   \
   template TYPE&                                                              \
   xAOD::IParticle::auxdata< TYPE >( const std::string& name,                  \
                                     const std::string& clsname = "" );        \
   template const TYPE&                                                        \
   xAOD::IParticle::auxdata< TYPE >( const std::string& name,                  \
                                     const std::string& clsname = "" ) const;  \
   template bool                                                               \
   xAOD::IParticle::isAvailable< TYPE >( const std::string& name,              \
                                         const std::string& clsname = "" ) const

// Instantiate the functions for a couple of regularly used types.
INST_AUXFUNC( bool );
INST_AUXFUNC( float );
INST_AUXFUNC( double );
INST_AUXFUNC( int );
INST_AUXFUNC( unsigned int );
INST_AUXFUNC( uint8_t );

// Clean up.
#undef INST_AUXFUNC

#endif // XAODBASE_XAODBASEDICT_H
