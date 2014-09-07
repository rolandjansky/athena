/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PhotonAccessors_v1.cxx 582601 2014-02-11 15:46:49Z christos $

// System include(s):
#include <iostream>

// Local include(s):
#include "PhotonAccessors_v1.h"

/// Helper macro for Accessor objects
#define DEFINE_ACCESSOR(TYPE, NAME )                               \
  case xAOD::EgammaParameters::NAME:				   \
   {                                                               \
     static SG::AuxElement::Accessor< TYPE > a( #NAME );	   \
     return &a;							   \
   }                                                               \
   break;

namespace xAOD {

   SG::AuxElement::Accessor< float >*
   vertexCaloMatchAccessorV1( xAOD::EgammaParameters::VertexCaloMatchType type ) 
   {   
      switch( type ) {
	DEFINE_ACCESSOR( float,convMatchDeltaEta1);
	DEFINE_ACCESSOR( float,convMatchDeltaEta2);    
	DEFINE_ACCESSOR( float,convMatchDeltaPhi1);    
	DEFINE_ACCESSOR( float,convMatchDeltaPhi2);
      default:                  
         std::cerr << "xAOD::Photon_v1 ERROR Unknown float VertexCaloMatchType ("
                   << type << ") requested" << std::endl;
         return 0;
      }
   
   }     
} // namespace xAOD
