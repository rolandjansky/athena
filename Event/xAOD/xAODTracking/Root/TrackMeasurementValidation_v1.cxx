/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrackMeasurementValidation_v1.cxx 633476 2014-12-03 18:26:13Z amorley $

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTracking/versions/TrackMeasurementValidation_v1.h"

namespace xAOD {

  TrackMeasurementValidation_v1::TrackMeasurementValidation_v1()
    : SG::AuxElement() {    
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrackMeasurementValidation_v1, uint64_t, identifier, setIdentifier)
  AUXSTORE_OBJECT_SETTER_AND_GETTER( TrackMeasurementValidation_v1, std::vector< uint64_t >,
				     rdoIdentifierList,
				     setRdoIdentifierList )

  AUXSTORE_PRIMITIVE_GETTER( TrackMeasurementValidation_v1, float, localX )
  AUXSTORE_PRIMITIVE_GETTER( TrackMeasurementValidation_v1, float, localY )
  AUXSTORE_PRIMITIVE_GETTER( TrackMeasurementValidation_v1, float, localXError )
  AUXSTORE_PRIMITIVE_GETTER( TrackMeasurementValidation_v1, float, localYError )
  AUXSTORE_PRIMITIVE_GETTER( TrackMeasurementValidation_v1, float, localXYCorrelation )
  
  void TrackMeasurementValidation_v1::setLocalPosition(float localX, float localY) {
    static const Accessor< float > acc1( "localX" );
    acc1( *this ) = localX;
    static const Accessor< float > acc2( "localY" );
    acc2( *this ) = localY;
  }

  void TrackMeasurementValidation_v1::setLocalPositionError(float localXError, float localYError, float localXYCorrelation) {
    static const Accessor< float > acc1( "localXError" );
    acc1( *this ) = localXError;
    static const Accessor< float > acc2( "localYError" );
    acc2( *this ) = localYError;
    static const Accessor< float > acc3( "localXYCorrelation" );
    acc3( *this ) = localXYCorrelation;
  }

  AUXSTORE_PRIMITIVE_GETTER( TrackMeasurementValidation_v1, float, globalX )
  AUXSTORE_PRIMITIVE_GETTER( TrackMeasurementValidation_v1, float, globalY )
  AUXSTORE_PRIMITIVE_GETTER( TrackMeasurementValidation_v1, float, globalZ )
  
  void TrackMeasurementValidation_v1::setGlobalPosition(float globalX, float globalY, float globalZ) {
    static const Accessor< float > acc1( "globalX" );
    acc1( *this ) = globalX;
    static const Accessor< float > acc2( "globalY" );
    acc2( *this ) = globalY;
    static const Accessor< float > acc3( "globalZ" );
    acc3( *this ) = globalZ;
  }


} // namespace xAOD
