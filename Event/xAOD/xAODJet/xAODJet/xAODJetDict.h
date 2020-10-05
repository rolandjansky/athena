// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODJET_XAODJETDICT_H
#define XAODJET_XAODJETDICT_H

// Local include(s).
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODJet/versions/Jet_v1.h"
#include "xAODJet/versions/JetContainer_v1.h"
#include "xAODJet/versions/JetAuxContainer_v1.h"

#if !defined(SIMULATIONBASE) and !defined(GENERATIONBASE)
#include "xAODJet/versions/JetTrigAuxContainer_v1.h"
#include "xAODJet/versions/JetTrigAuxContainer_v2.h"
#endif // not SIMULATIONBASE or GENERATIONBASE

#include "xAODJet/JetTypes.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

// System include(s).
#include <vector>

// Instantiate all necessary types for the dictionary.
namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODJET {
      // Local type(s).
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, JetContainer_v1 );
      // Type(s) needed for the dictionary generation to succeed.
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, IParticleContainer );
      xAOD::JetFourMom_t dummy;
   };
}

/// Helper macro for instantiating the "attribute function(s)"
#define INST_JET_ATTRIB( TYPE )                                                \
   template TYPE                                                               \
   xAOD::Jet::getAttribute< TYPE >( xAOD::JetAttribute::AttributeID ) const;   \
   template TYPE xAOD::Jet::getAttribute< TYPE >( const std::string& ) const;  \
   template void                                                               \
   xAOD::Jet::setAttribute< TYPE >( xAOD::JetAttribute::AttributeID,           \
                                    const TYPE& );                             \
   template void xAOD::Jet::setAttribute< TYPE >( const std::string&,          \
                                                  const TYPE& )

INST_JET_ATTRIB( float );
INST_JET_ATTRIB( int );
INST_JET_ATTRIB( std::vector< float > );
INST_JET_ATTRIB( std::vector< int > );
INST_JET_ATTRIB( xAOD::JetFourMom_t );

// Clean up.
#undef INST_JET_ATTRIB

template std::vector< const xAOD::IParticle* >
xAOD::Jet::getAssociatedObjects< xAOD::IParticle >( xAOD::JetAttribute::AssoParticlesID ) const;
template std::vector< const xAOD::IParticle* >
xAOD::Jet::getAssociatedObjects< xAOD::IParticle >( const std::string& ) const;
template void
xAOD::Jet::setAssociatedObjects< xAOD::IParticle >( const std::string&,
                                                    const std::vector< const xAOD::IParticle* >& );

template const xAOD::IParticle*
xAOD::Jet::getAssociatedObject< xAOD::IParticle >( xAOD::JetAttribute::AssoParticlesID ) const;
template const xAOD::IParticle*
xAOD::Jet::getAssociatedObject< xAOD::IParticle >( const std::string& ) const;
template void
xAOD::Jet::setAssociatedObject< xAOD::IParticle >( const std::string&,
                                                   const xAOD::IParticle* );

#endif // XAODJET_XAODJETDICT_H
