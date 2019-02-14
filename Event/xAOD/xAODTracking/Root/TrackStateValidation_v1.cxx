/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrackStateValidation_v1.cxx 717831 2016-01-14 20:03:39Z goetz $

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTracking/versions/TrackStateValidation_v1.h"

namespace xAOD {

  TrackStateValidation_v1::TrackStateValidation_v1()
    : SG::AuxElement() {    
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrackStateValidation_v1, int, type, setType)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrackStateValidation_v1, uint64_t, detElementId, setDetElementId)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrackStateValidation_v1, char, detType, setDetType)


  AUXSTORE_PRIMITIVE_GETTER( TrackStateValidation_v1, float, localX )
  AUXSTORE_PRIMITIVE_GETTER( TrackStateValidation_v1, float, localY )
  
  void TrackStateValidation_v1::setLocalPosition(float localX, float localY) {
    static const Accessor< float > acc1( "localX" );
    acc1( *this ) = localX;
    static const Accessor< float > acc2( "localY" );
    acc2( *this ) = localY;
  }

  AUXSTORE_PRIMITIVE_GETTER( TrackStateValidation_v1, float, localTheta )
  AUXSTORE_PRIMITIVE_GETTER( TrackStateValidation_v1, float, localPhi )

  void TrackStateValidation_v1::setLocalAngles(float localTheta, float localPhi) {
    static const Accessor< float > acc1( "localTheta" );
    acc1( *this ) = localTheta;
    static const Accessor< float > acc2( "localPhi" );
    acc2( *this ) = localPhi;
  }

  AUXSTORE_PRIMITIVE_GETTER( TrackStateValidation_v1, float, unbiasedResidualX )
  AUXSTORE_PRIMITIVE_GETTER( TrackStateValidation_v1, float, unbiasedResidualY )

  void TrackStateValidation_v1::setUnbiasedResidual(float unbiasedResidualX, float unbiasedResidualY) {
    static const Accessor< float > acc1( "unbiasedResidualX" );
    acc1( *this ) = unbiasedResidualX;
    static const Accessor< float > acc2( "unbiasedResidualY" );
    acc2( *this ) = unbiasedResidualY;
  }

  AUXSTORE_PRIMITIVE_GETTER( TrackStateValidation_v1, float, unbiasedPullX )
  AUXSTORE_PRIMITIVE_GETTER( TrackStateValidation_v1, float, unbiasedPullY )

  void TrackStateValidation_v1::setUnbiasedPull(float unbiasedPullX, float unbiasedPullY) {
    static const Accessor< float > acc1( "unbiasedPullX" );
    acc1( *this ) = unbiasedPullX;
    static const Accessor< float > acc2( "unbiasedPullY" );
    acc2( *this ) = unbiasedPullY;
  }

  AUXSTORE_PRIMITIVE_GETTER( TrackStateValidation_v1, float, biasedResidualX )
  AUXSTORE_PRIMITIVE_GETTER( TrackStateValidation_v1, float, biasedResidualY )

  void TrackStateValidation_v1::setBiasedResidual(float biasedResidualX, float biasedResidualY) {
    static const Accessor< float > acc1( "biasedResidualX" );
    acc1( *this ) = biasedResidualX;
    static const Accessor< float > acc2( "biasedResidualY" );
    acc2( *this ) = biasedResidualY;
  }

  AUXSTORE_PRIMITIVE_GETTER( TrackStateValidation_v1, float, biasedPullX )
  AUXSTORE_PRIMITIVE_GETTER( TrackStateValidation_v1, float, biasedPullY )

  void TrackStateValidation_v1::setBiasedPull(float biasedPullX, float biasedPullY) {
    static const Accessor< float > acc1( "biasedPullX" );
    acc1( *this ) = biasedPullX;
    static const Accessor< float > acc2( "biasedPullY" );
    acc2( *this ) = biasedPullY;
  }

  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrackStateValidation_v1, ElementLink<xAOD::TrackMeasurementValidationContainer>, trackMeasurementValidationLink, setTrackMeasurementValidationLink)
  //   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrackStateValidation_v1, ElementLink<xAOD::TrackParticle>, trackParticleLink, setTrackParticleLink)

} // namespace xAOD
