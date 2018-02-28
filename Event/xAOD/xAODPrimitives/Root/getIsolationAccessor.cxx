/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: getIsolationAccessor.cxx 630627 2014-11-24 14:35:22Z emoyse $

// System include(s):
#include <vector>
#include <memory>

// Local include(s):
#include "xAODPrimitives/tools/getIsolationAccessor.h"

/// The type to be used as the smart pointer
#if __cplusplus >= 201100
typedef std::unique_ptr< SG::AuxElement::Accessor< float > > SmartPointer_t;
#else
typedef std::auto_ptr< SG::AuxElement::Accessor< float > > SmartPointer_t;
#endif // C++11

/// Helper macro for populating the static vector of this file
#define DEFINE_ISO_ACCESSOR( TYPE )                                     \
   if( sAccessors.size() <=                                             \
       static_cast< size_t >( xAOD::Iso::TYPE ) ) {                     \
      sAccessors.resize( static_cast< size_t >( xAOD::Iso::TYPE ) +     \
                         1 );                                           \
   }                                                                    \
   sAccessors[ static_cast< size_t >( xAOD::Iso::TYPE ) ] =             \
      SmartPointer_t( new SG::AuxElement::Accessor< float >( #TYPE ) )

namespace xAOD {

   SG::AuxElement::Accessor< float >*
   getIsolationAccessor( Iso::IsolationType type ) {

      /// Accessor objects
      static std::vector< SmartPointer_t > sAccessors;

      // Create the accessor objects if they don't exist yet:
      if( sAccessors.empty() ) {
         // Make the vector large enough for all the accessors:
         sAccessors.resize( Iso::numIsolationTypes );
         // Etcone variables:
         DEFINE_ISO_ACCESSOR( etcone20 );
         DEFINE_ISO_ACCESSOR( etcone30 );
         DEFINE_ISO_ACCESSOR( etcone40 );
         // Ptcone variables:
         DEFINE_ISO_ACCESSOR( ptcone20 );
         DEFINE_ISO_ACCESSOR( ptcone30 );
         DEFINE_ISO_ACCESSOR( ptcone40 );
         // TopoEtcone variables:
         DEFINE_ISO_ACCESSOR( topoetcone20 );
         DEFINE_ISO_ACCESSOR( topoetcone30 );
         DEFINE_ISO_ACCESSOR( topoetcone40 );
         // Mini-Isolation (http://arxiv.org/abs/1007.2221) variables:
         DEFINE_ISO_ACCESSOR(ptvarcone20);
         DEFINE_ISO_ACCESSOR(ptvarcone30);
         DEFINE_ISO_ACCESSOR(ptvarcone40);
         // Mini-Isolation (http://arxiv.org/abs/1007.2221) variables for high mu:
         DEFINE_ISO_ACCESSOR(ptvarcone20_TightTTVA_pt500);
         DEFINE_ISO_ACCESSOR(ptvarcone30_TightTTVA_pt500);
         DEFINE_ISO_ACCESSOR(ptvarcone40_TightTTVA_pt500);
         // neutral eflow
         DEFINE_ISO_ACCESSOR(neflowisol20);
         DEFINE_ISO_ACCESSOR(neflowisol30);
         DEFINE_ISO_ACCESSOR(neflowisol40);
      }

      // Look up the element, throwing an exception for too large
      // enum values:
      return sAccessors.at( static_cast< size_t >( type ) ).get();
   }

} // namespace xAOD
