/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: getIsolationDecorator.cxx 630627 2014-11-24 14:35:22Z emoyse $

// System include(s):
#include <vector>
#include <memory>

// Local include(s):
#include "xAODPrimitives/tools/getIsolationDecorator.h"

/// The type to be used as the smart pointer
#if __cplusplus >= 201100
typedef std::unique_ptr< SG::AuxElement::Decorator< float > > SmartPointer_t;
#else
typedef std::auto_ptr< SG::AuxElement::Decorator< float > > SmartPointer_t;
#endif // C++11

/// Helper macro for populating the static vector of this file
#define DEFINE_ISO_DECORATOR( TYPE )                                     \
   if( sDecorators.size() <=                                             \
       static_cast< size_t >( xAOD::Iso::TYPE ) ) {                     \
      sDecorators.resize( static_cast< size_t >( xAOD::Iso::TYPE ) +     \
                         1 );                                           \
   }                                                                    \
   sDecorators[ static_cast< size_t >( xAOD::Iso::TYPE ) ] =             \
      SmartPointer_t( new SG::AuxElement::Decorator< float >( #TYPE ) )

namespace xAOD {

   SG::AuxElement::Decorator< float >*
   getIsolationDecorator( Iso::IsolationType type ) {

      /// Decorator objects
      static std::vector< SmartPointer_t > sDecorators;

      // Create the accessor objects if they don't exist yet:
      if( sDecorators.empty() ) {
         // Make the vector large enough for all the accessors:
         sDecorators.resize( Iso::numIsolationTypes );
         // Etcone variables:
         DEFINE_ISO_DECORATOR( etcone20 );
         DEFINE_ISO_DECORATOR( etcone30 );
         DEFINE_ISO_DECORATOR( etcone40 );
         // Ptcone variables:
         DEFINE_ISO_DECORATOR( ptcone20 );
         DEFINE_ISO_DECORATOR( ptcone30 );
         DEFINE_ISO_DECORATOR( ptcone40 );
         // TopoEtcone variables:
         DEFINE_ISO_DECORATOR( topoetcone20 );
         DEFINE_ISO_DECORATOR( topoetcone30 );
         DEFINE_ISO_DECORATOR( topoetcone40 );
         // Mini-Isolation (http://arxiv.org/abs/1007.2221) variables:
         DEFINE_ISO_DECORATOR(ptvarcone20);
         DEFINE_ISO_DECORATOR(ptvarcone30);
         DEFINE_ISO_DECORATOR(ptvarcone40);
         // neutral eflow
         DEFINE_ISO_DECORATOR(neflowisol20);
         DEFINE_ISO_DECORATOR(neflowisol30);
         DEFINE_ISO_DECORATOR(neflowisol40);
      }

      // Look up the element, throwing an exception for too large
      // enum values:
      return sDecorators.at( static_cast< size_t >( type ) ).get();
   }

} // namespace xAOD
