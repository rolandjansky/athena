/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: getIsolationAccessor.cxx 582418 2014-02-10 15:03:46Z krasznaa $

// System include(s):
#include <vector>

// Local include(s):
#include "xAODPrimitives/tools/getIsolationAccessor.h"

/// Helper macro for populating the static vector of this file
#define DEFINE_ISO_ACCESSOR( TYPE )                                     \
   if( sAccessors.size() <=                                             \
       static_cast< size_t >( xAOD::Iso::TYPE ) ) {                     \
      sAccessors.resize( static_cast< size_t >( xAOD::Iso::TYPE ) +     \
                         1 );                                           \
   }                                                                    \
   sAccessors[ static_cast< size_t >( xAOD::Iso::TYPE ) ] =             \
      new SG::AuxElement::Accessor< float >( #TYPE )

namespace xAOD {

   /// Accessor objects
   static std::vector< SG::AuxElement::Accessor< float >* > sAccessors;

   SG::AuxElement::Accessor< float >*
   getIsolationAccessor( Iso::IsolationType type ) {

      // Create the accessor objects if they don't exist yet:
      if( ! sAccessors.size() ) {
         // Make the vector large enough for all the accessors:
         sAccessors.resize( 90 );
         // Etcone variables:
         DEFINE_ISO_ACCESSOR( etcone10 );
         DEFINE_ISO_ACCESSOR( etcone15 );
         DEFINE_ISO_ACCESSOR( etcone20 );
         DEFINE_ISO_ACCESSOR( etcone25 );
         DEFINE_ISO_ACCESSOR( etcone30 );
         DEFINE_ISO_ACCESSOR( etcone35 );
         DEFINE_ISO_ACCESSOR( etcone40 );
         DEFINE_ISO_ACCESSOR( etcone45 );
         // Ptcone variables:
         DEFINE_ISO_ACCESSOR( ptcone10 );
         DEFINE_ISO_ACCESSOR( ptcone15 );
         DEFINE_ISO_ACCESSOR( ptcone20 );
         DEFINE_ISO_ACCESSOR( ptcone25 );
         DEFINE_ISO_ACCESSOR( ptcone30 );
         DEFINE_ISO_ACCESSOR( ptcone35 );
         DEFINE_ISO_ACCESSOR( ptcone40 );
         DEFINE_ISO_ACCESSOR( ptcone45 );
         // Nucone variables:
         DEFINE_ISO_ACCESSOR( nucone10 );
         DEFINE_ISO_ACCESSOR( nucone15 );
         DEFINE_ISO_ACCESSOR( nucone20 );
         DEFINE_ISO_ACCESSOR( nucone25 );
         DEFINE_ISO_ACCESSOR( nucone30 );
         DEFINE_ISO_ACCESSOR( nucone35 );
         DEFINE_ISO_ACCESSOR( nucone40 );
         DEFINE_ISO_ACCESSOR( nucone45 );
         // EtconeNoEm variables:
         DEFINE_ISO_ACCESSOR( etconeNoEm10 );
         DEFINE_ISO_ACCESSOR( etconeNoEm15 );
         DEFINE_ISO_ACCESSOR( etconeNoEm20 );
         DEFINE_ISO_ACCESSOR( etconeNoEm25 );
         DEFINE_ISO_ACCESSOR( etconeNoEm30 );
         DEFINE_ISO_ACCESSOR( etconeNoEm35 );
         DEFINE_ISO_ACCESSOR( etconeNoEm40 );
         DEFINE_ISO_ACCESSOR( etconeNoEm45 );
         // Etcone_ptcorrected variables:
         DEFINE_ISO_ACCESSOR( etcone10_ptcorrected );
         DEFINE_ISO_ACCESSOR( etcone15_ptcorrected );
         DEFINE_ISO_ACCESSOR( etcone20_ptcorrected );
         DEFINE_ISO_ACCESSOR( etcone25_ptcorrected );
         DEFINE_ISO_ACCESSOR( etcone30_ptcorrected );
         DEFINE_ISO_ACCESSOR( etcone35_ptcorrected );
         DEFINE_ISO_ACCESSOR( etcone40_ptcorrected );
         DEFINE_ISO_ACCESSOR( etcone45_ptcorrected );
         // Etcone_corrected variables:
         DEFINE_ISO_ACCESSOR( etcone10_corrected );
         DEFINE_ISO_ACCESSOR( etcone15_corrected );
         DEFINE_ISO_ACCESSOR( etcone20_corrected );
         DEFINE_ISO_ACCESSOR( etcone25_corrected );
         DEFINE_ISO_ACCESSOR( etcone30_corrected );
         DEFINE_ISO_ACCESSOR( etcone35_corrected );
         DEFINE_ISO_ACCESSOR( etcone40_corrected );
         DEFINE_ISO_ACCESSOR( etcone45_corrected );
         // TopoEtcone variables:
         DEFINE_ISO_ACCESSOR( topoetcone10 );
         DEFINE_ISO_ACCESSOR( topoetcone15 );
         DEFINE_ISO_ACCESSOR( topoetcone20 );
         DEFINE_ISO_ACCESSOR( topoetcone25 );
         DEFINE_ISO_ACCESSOR( topoetcone30 );
         DEFINE_ISO_ACCESSOR( topoetcone35 );
         DEFINE_ISO_ACCESSOR( topoetcone40 );
         DEFINE_ISO_ACCESSOR( topoetcone45 );
         // TopoEtcone_ptcorrected variables:
         DEFINE_ISO_ACCESSOR( topoetcone10_ptcorrected );
         DEFINE_ISO_ACCESSOR( topoetcone15_ptcorrected );
         DEFINE_ISO_ACCESSOR( topoetcone20_ptcorrected );
         DEFINE_ISO_ACCESSOR( topoetcone25_ptcorrected );
         DEFINE_ISO_ACCESSOR( topoetcone30_ptcorrected );
         DEFINE_ISO_ACCESSOR( topoetcone35_ptcorrected );
         DEFINE_ISO_ACCESSOR( topoetcone40_ptcorrected );
         DEFINE_ISO_ACCESSOR( topoetcone45_ptcorrected );
         // TopoEtcone_corrected variables:
         DEFINE_ISO_ACCESSOR( topoetcone10_corrected );
         DEFINE_ISO_ACCESSOR( topoetcone15_corrected );
         DEFINE_ISO_ACCESSOR( topoetcone20_corrected );
         DEFINE_ISO_ACCESSOR( topoetcone25_corrected );
         DEFINE_ISO_ACCESSOR( topoetcone30_corrected );
         DEFINE_ISO_ACCESSOR( topoetcone35_corrected );
         DEFINE_ISO_ACCESSOR( topoetcone40_corrected );
         DEFINE_ISO_ACCESSOR( topoetcone45_corrected );
      }

      // Look up the element, throwing an exception for too large
      // enum values:
      return sAccessors.at( static_cast< size_t >( type ) );
   }

} // namespace xAOD
