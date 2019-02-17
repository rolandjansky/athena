/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: FileMetaDataAccessors_v1.cxx 685184 2015-07-23 20:25:43Z cranshaw $

// System include(s):
#include <iostream>

// Local include(s):
#include "FileMetaDataAccessors_v1.h"

/// Helper macro for implementing the accessor function
#define DECLARE_STRING_ACCESSOR( TYPE )                              \
   case FileMetaData_v1::TYPE:                                       \
   do {                                                              \
      static const SG::AuxElement::Accessor< std::string > acc( #TYPE );   \
      return &acc;                                                   \
   } while( 0 )

/// Helper macro for implementing the accessor function
#define DECLARE_FLOAT_ACCESSOR( TYPE )                               \
   case FileMetaData_v1::TYPE:                                       \
   do {                                                              \
      static const SG::AuxElement::Accessor< float > acc( #TYPE );         \
      return &acc;                                                   \
   } while( 0 )

namespace xAOD {

   const SG::AuxElement::Accessor< std::string >*
   metaDataTypeStringAccessorV1( FileMetaData_v1::MetaDataType type ) {

      switch( type ) {

         DECLARE_STRING_ACCESSOR( productionRelease );
         DECLARE_STRING_ACCESSOR( AODFixVersion );
         DECLARE_STRING_ACCESSOR( AODCalibVersion );
         DECLARE_STRING_ACCESSOR( dataType );
         DECLARE_STRING_ACCESSOR( amiTag );
         DECLARE_STRING_ACCESSOR( geometryVersion );
         DECLARE_STRING_ACCESSOR( conditionsTag );
         DECLARE_STRING_ACCESSOR( beamType );
         DECLARE_STRING_ACCESSOR( simFlavour );

      default:
         std::cerr << "xAOD::FileMetaData_v1    ERROR No string accessor for "
                   << "type: " << type << std::endl;
         return 0;
      }

      // Just to make sure the compiler doesn't complain:
      return 0;
   }

   const SG::AuxElement::Accessor< float >*
   metaDataTypeFloatAccessorV1( FileMetaData_v1::MetaDataType type ) {

      switch( type ) {

         DECLARE_FLOAT_ACCESSOR( beamEnergy );
         DECLARE_FLOAT_ACCESSOR( mcProcID );

      default:
         std::cerr << "xAOD::FileMetaData_v1    ERROR No float accessor for "
                   << "type: " << type << std::endl;
         return 0;
      }

      // Just to make sure the compiler doesn't complain:
      return 0;
   }

} // namespace xAOD
