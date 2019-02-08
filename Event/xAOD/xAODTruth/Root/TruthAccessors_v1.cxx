/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthAccessors_v1.cxx 623284 2014-10-22 14:07:48Z krasznaa $

// System include(s):
#include <iostream>

// Local include(s):
#include "TruthAccessors_v1.h"

/// Helper macro for managing cluster moment Accessor objects
#define DEFINE_ACCESSOR( PARENT, TYPE, NAME )                \
   case PARENT::NAME:                                        \
      {                                                      \
         static const SG::AuxElement::Accessor< TYPE > a( #NAME ); \
         return &a;                                          \
      }                                                      \
      break

namespace xAOD {

   const SG::AuxElement::Accessor< float >*
   polarizationAccessorV1( TruthParticle_v1::PolParam type ) {

      switch( type ) {

         DEFINE_ACCESSOR( TruthParticle_v1, float, polarizationTheta );
         DEFINE_ACCESSOR( TruthParticle_v1, float, polarizationPhi );

      default:
         std::cerr << "xAOD::polarizationAccessorV1 ERROR Unknown PolParam ("
                   << type << ") requested" << std::endl;
         return 0;
      }

      return 0;
   }

   const SG::AuxElement::Accessor< int >*
   pdfInfoAccessorV1Int( TruthEvent_v1::PdfParam type ) {

      switch( type ) {

         DEFINE_ACCESSOR( TruthEvent_v1, int, PDGID1 );
         DEFINE_ACCESSOR( TruthEvent_v1, int, PDGID2 );
         DEFINE_ACCESSOR( TruthEvent_v1, int, PDFID1 );
         DEFINE_ACCESSOR( TruthEvent_v1, int, PDFID2 );

      default:
         std::cerr << "xAOD::pdfInfoAccessorV1Int ERROR Unknown PdfParam ("
                   << type << ") requested" << std::endl;
         return 0;
      }

      return 0;
   }

   const SG::AuxElement::Accessor< float >*
   pdfInfoAccessorV1Float( TruthEvent_v1::PdfParam type ) {

      switch( type ) {

         DEFINE_ACCESSOR( TruthEvent_v1, float, X1 );
         DEFINE_ACCESSOR( TruthEvent_v1, float, X2 );
         DEFINE_ACCESSOR( TruthEvent_v1, float, Q );
         DEFINE_ACCESSOR( TruthEvent_v1, float, XF1 );
         DEFINE_ACCESSOR( TruthEvent_v1, float, XF2 );

      default:
         std::cerr << "xAOD::pdfInfoAccessorV1Float ERROR Unknown ParamDef ("
                   << type << ") requested" << std::endl;
         return 0;
      }

      return 0;
   }

   const SG::AuxElement::Accessor< int >*
   heavyIonAccessorV1Int( TruthEvent_v1::HIParam type ) {

      switch( type ) {

         DEFINE_ACCESSOR( TruthEvent_v1, int, NCOLLHARD );
         DEFINE_ACCESSOR( TruthEvent_v1, int, NPARTPROJ );
         DEFINE_ACCESSOR( TruthEvent_v1, int, NPARTTARG );
         DEFINE_ACCESSOR( TruthEvent_v1, int, NCOLL );
         DEFINE_ACCESSOR( TruthEvent_v1, int, SPECTATORNEUTRONS );
         DEFINE_ACCESSOR( TruthEvent_v1, int, SPECTATORPROTONS );
         DEFINE_ACCESSOR( TruthEvent_v1, int, NNWOUNDEDCOLLISIONS );
         DEFINE_ACCESSOR( TruthEvent_v1, int, NWOUNDEDNCOLLISIONS );
         DEFINE_ACCESSOR( TruthEvent_v1, int, NWOUNDEDNWOUNDEDCOLLISIONS );

      default:
         std::cerr << "xAOD::heavyIonAccessorV1Int ERROR Unknown HIParam ("
                   << type << ") requested" << std::endl;
         return 0;
      }

      return 0;
   }

   const SG::AuxElement::Accessor< float >*
   heavyIonAccessorV1Float( TruthEvent_v1::HIParam type ) {

      switch( type ) {

         DEFINE_ACCESSOR( TruthEvent_v1, float, IMPACTPARAMETER );
         DEFINE_ACCESSOR( TruthEvent_v1, float, EVENTPLANEANGLE );
         DEFINE_ACCESSOR( TruthEvent_v1, float, ECCENTRICITY );
         DEFINE_ACCESSOR( TruthEvent_v1, float, SIGMAINELNN );
         DEFINE_ACCESSOR( TruthEvent_v1, float, CENTRALITY );

      default:
         std::cerr << "xAOD::heavyIonAccessorV1Float ERROR Unknown HIParam ("
                   << type << ") requested" << std::endl;
         return 0;
      }

      return 0;
   }

} // namespace xAOD
