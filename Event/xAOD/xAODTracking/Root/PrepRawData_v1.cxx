/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PrepRawData_v1.cxx 609799 2014-08-01 14:58:09Z htorres $

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTracking/versions/PrepRawData_v1.h"

namespace xAOD {

  PrepRawData_v1::PrepRawData_v1()
    : SG::AuxElement() {    
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( PrepRawData_v1, uint64_t, identifier, setIdentifier)
  AUXSTORE_OBJECT_SETTER_AND_GETTER( PrepRawData_v1, std::vector< uint64_t >,
				     rdoIdentifierList,
				     setRdoIdentifierList )

  AUXSTORE_PRIMITIVE_GETTER( PrepRawData_v1, float, localX )
  AUXSTORE_PRIMITIVE_GETTER( PrepRawData_v1, float, localY )
  AUXSTORE_PRIMITIVE_GETTER( PrepRawData_v1, float, localXError )
  AUXSTORE_PRIMITIVE_GETTER( PrepRawData_v1, float, localYError )
  AUXSTORE_PRIMITIVE_GETTER( PrepRawData_v1, float, localXYCorrelation )
  
  void PrepRawData_v1::setLocalPosition(float localX, float localY) {
    static Accessor< float > acc1( "localX" );
    acc1( *this ) = localX;
    static Accessor< float > acc2( "localY" );
    acc2( *this ) = localY;
  }

  void PrepRawData_v1::setLocalPositionError(float localXError, float localYError, float localXYCorrelation) {
    static Accessor< float > acc1( "localXError" );
    acc1( *this ) = localXError;
    static Accessor< float > acc2( "localYError" );
    acc2( *this ) = localYError;
    static Accessor< float > acc3( "localXYCorrelation" );
    acc3( *this ) = localXYCorrelation;
  }

  AUXSTORE_PRIMITIVE_GETTER( PrepRawData_v1, float, globalX )
  AUXSTORE_PRIMITIVE_GETTER( PrepRawData_v1, float, globalY )
  AUXSTORE_PRIMITIVE_GETTER( PrepRawData_v1, float, globalZ )
  
  void PrepRawData_v1::setGlobalPosition(float globalX, float globalY, float globalZ) {
    static Accessor< float > acc1( "globalX" );
    acc1( *this ) = globalX;
    static Accessor< float > acc2( "globalY" );
    acc2( *this ) = globalY;
    static Accessor< float > acc3( "globalZ" );
    acc3( *this ) = globalZ;
  }


} // namespace xAOD
