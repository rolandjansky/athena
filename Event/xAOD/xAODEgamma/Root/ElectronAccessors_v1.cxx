/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElectronAccessors_v1.cxx 575466 2013-12-13 16:40:29Z amorley $

// System include(s):
#include <iostream>

// Local include(s):
#include "ElectronAccessors_v1.h"

/// Helper macro for Accessor objects
#define DEFINE_ACCESSOR(TYPE, NAME )                               \
  case xAOD::EgammaParameters::NAME:				   \
   {                                                               \
     const static SG::AuxElement::Accessor< TYPE > a( #NAME );	   \
     return &a;							   \
   }                                                               \
   break;

namespace xAOD {

   const SG::AuxElement::Accessor< float >*
   trackCaloMatchAccessorV1( xAOD::EgammaParameters::TrackCaloMatchType type ) 
   {   
      switch( type ) {
        DEFINE_ACCESSOR( float,deltaEta0);
        DEFINE_ACCESSOR( float,deltaEta1);
        DEFINE_ACCESSOR( float,deltaEta2);
        DEFINE_ACCESSOR( float,deltaEta3);
        DEFINE_ACCESSOR( float,deltaPhi0);
        DEFINE_ACCESSOR( float,deltaPhi1);
        DEFINE_ACCESSOR( float,deltaPhi2);
        DEFINE_ACCESSOR( float,deltaPhi3);
        DEFINE_ACCESSOR( float,deltaPhiFromLastMeasurement);
        DEFINE_ACCESSOR( float,deltaPhiRescaled0);
        DEFINE_ACCESSOR( float,deltaPhiRescaled1);
        DEFINE_ACCESSOR( float,deltaPhiRescaled2);
        DEFINE_ACCESSOR( float,deltaPhiRescaled3);;
      default:                  
         std::cerr << "xAOD::Electron  ERROR Unknown float TrackCaloMatchType ("
                   << type << ") requested" << std::endl;
         return 0;
      }
   
   }     
} // namespace xAOD
