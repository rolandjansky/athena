/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventInfoAccessors_v1.cxx 579930 2014-01-24 10:36:54Z krasznaa $

// System include(s):
#include <iostream>

// Local include(s):
#include "EventInfoAccessors_v1.h"

/// Helper macro for defining the different accessor objects
#define DEFINE_ACCESSOR( ENUM_NAME, AUX_NAME )                           \
   case EventInfo_v1::ENUM_NAME:                                         \
   {                                                                     \
      static const SG::AtomicDecorator< uint32_t > a( #AUX_NAME );       \
      return &a;                                                         \
   }                                                                     \
   break

/// Helper macro for defining the different accessor objects
#define DEFINE_CONST_ACCESSOR( ENUM_NAME, AUX_NAME )                     \
   case EventInfo_v1::ENUM_NAME:                                         \
   {                                                                     \
      static const SG::AtomicConstAccessor< uint32_t > a( #AUX_NAME );   \
      return &a;                                                         \
   }                                                                     \
   break

namespace xAOD {

   const SG::AtomicDecorator< uint32_t >*
   eventFlagsAccessorsV1( EventInfo_v1::EventFlagSubDet subDet ) {

      switch( subDet ) {

         DEFINE_ACCESSOR( Pixel, pixelFlags );
         DEFINE_ACCESSOR( SCT, sctFlags );
         DEFINE_ACCESSOR( TRT, trtFlags );
         DEFINE_ACCESSOR( LAr, larFlags );
         DEFINE_ACCESSOR( Tile, tileFlags );
         DEFINE_ACCESSOR( Muon, muonFlags );
         DEFINE_ACCESSOR( ForwardDet, forwardDetFlags );
         DEFINE_ACCESSOR( Core, coreFlags );
         DEFINE_ACCESSOR( Background, backgroundFlags );
         DEFINE_ACCESSOR( Lumi, lumiFlags );

      default:
         std::cerr << "xAOD::EventInfo_v1 ERROR Unknown sub-detector ("
                   << subDet << ") requested" << std::endl;
         return nullptr;
      }
   }


   const SG::AtomicConstAccessor< uint32_t >*
   eventFlagsConstAccessorsV1( EventInfo_v1::EventFlagSubDet subDet ) {

      switch( subDet ) {

         DEFINE_CONST_ACCESSOR( Pixel, pixelFlags );
         DEFINE_CONST_ACCESSOR( SCT, sctFlags );
         DEFINE_CONST_ACCESSOR( TRT, trtFlags );
         DEFINE_CONST_ACCESSOR( LAr, larFlags );
         DEFINE_CONST_ACCESSOR( Tile, tileFlags );
         DEFINE_CONST_ACCESSOR( Muon, muonFlags );
         DEFINE_CONST_ACCESSOR( ForwardDet, forwardDetFlags );
         DEFINE_CONST_ACCESSOR( Core, coreFlags );
         DEFINE_CONST_ACCESSOR( Background, backgroundFlags );
         DEFINE_CONST_ACCESSOR( Lumi, lumiFlags );

      default:
         std::cerr << "xAOD::EventInfo_v1 ERROR Unknown sub-detector ("
                   << subDet << ") requested" << std::endl;
         return nullptr;
      }
   }

} // namespace xAOD
