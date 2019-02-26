// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODBaseDict.h 618909 2014-09-29 10:16:52Z krasznaa $
#ifndef XAODBASE_XAODBASEDICT_H
#define XAODBASE_XAODBASEDICT_H

// STL include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODBase/ObjectType.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODBase/IParticleHelpers.h"

namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODBASE {
      xAOD::IParticleContainer c1;
      DataLink< xAOD::IParticleContainer > l1;
      std::vector< DataLink< xAOD::IParticleContainer > > l2;
      ElementLink< xAOD::IParticleContainer > l3;
      std::vector< ElementLink< xAOD::IParticleContainer > > l4;
      std::vector< std::vector< ElementLink< xAOD::IParticleContainer > > > l5;
   };
}

template class DataVector<xAOD::IParticle>;

template
bool& xAOD::IParticle::auxdata< bool >( const std::string& name,
                                        const std::string& clsname = "" );

template
float& xAOD::IParticle::auxdata< float >( const std::string& name,
                                          const std::string& clsname = "" );

template
int& xAOD::IParticle::auxdata< int >( const std::string& name,
                                      const std::string& clsname = "" );
template
unsigned int&
xAOD::IParticle::auxdata< unsigned int >( const std::string& name,
                                          const std::string& clsname = "" );

template
uint8_t& xAOD::IParticle::auxdata< uint8_t >( const std::string& name,
                                              const std::string& clsname = "" );

template
const bool&
xAOD::IParticle::auxdata< bool >( const std::string& name,
                                  const std::string& clsname = "" ) const;

template
const float&
xAOD::IParticle::auxdata< float >( const std::string& name,
                                   const std::string& clsname = "" ) const;

template
const int&
xAOD::IParticle::auxdata< int >( const std::string& name,
                                 const std::string& clsname = "" ) const;

template
const unsigned int&
xAOD::IParticle::auxdata< unsigned int >( const std::string& name,
                                          const std::string& clsname = "" ) const;

template
const uint8_t&
xAOD::IParticle::auxdata< uint8_t >( const std::string& name,
                                     const std::string& clsname = "" ) const;

template
bool
xAOD::IParticle::isAvailable< bool >( const std::string& name,
                                      const std::string& clsname = "" ) const;

template
bool
xAOD::IParticle::isAvailable< float >( const std::string& name,
                                       const std::string& clsname = "" ) const;

template
bool
xAOD::IParticle::isAvailable< int >( const std::string& name,
                                     const std::string& clsname = "" ) const;

template
bool
xAOD::IParticle::isAvailable< unsigned int >( const std::string& name,
                                              const std::string& clsname = "" ) const;

template
bool
xAOD::IParticle::isAvailable< uint8_t >( const std::string& name,
                                         const std::string& clsname = "" ) const;

#endif // XAODBASE_XAODBASEDICT_H
