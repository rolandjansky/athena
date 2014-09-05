/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MeasurementStateOnSurface_v1.cxx 614432 2014-09-01 15:16:31Z htorres $

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTracking/versions/MeasurementStateOnSurface_v1.h"

namespace xAOD {

  MeasurementStateOnSurface_v1::MeasurementStateOnSurface_v1()
    : SG::AuxElement() {    
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( MeasurementStateOnSurface_v1, char, type, setType)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( MeasurementStateOnSurface_v1, uint64_t, detElementId, setDetElementId)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( MeasurementStateOnSurface_v1, char, detType, setDetType)


  AUXSTORE_PRIMITIVE_GETTER( MeasurementStateOnSurface_v1, float, localX )
  AUXSTORE_PRIMITIVE_GETTER( MeasurementStateOnSurface_v1, float, localY )
  
  void MeasurementStateOnSurface_v1::setLocalPosition(float localX, float localY) {
    static Accessor< float > acc1( "localX" );
    acc1( *this ) = localX;
    static Accessor< float > acc2( "localY" );
    acc2( *this ) = localY;
  }

  AUXSTORE_PRIMITIVE_GETTER( MeasurementStateOnSurface_v1, float, localTheta )
  AUXSTORE_PRIMITIVE_GETTER( MeasurementStateOnSurface_v1, float, localPhi )

  void MeasurementStateOnSurface_v1::setLocalAngles(float localTheta, float localPhi) {
    static Accessor< float > acc1( "localTheta" );
    acc1( *this ) = localTheta;
    static Accessor< float > acc2( "localPhi" );
    acc2( *this ) = localPhi;
  }

  AUXSTORE_PRIMITIVE_GETTER( MeasurementStateOnSurface_v1, float, unbiasedResidualX )
  AUXSTORE_PRIMITIVE_GETTER( MeasurementStateOnSurface_v1, float, unbiasedResidualY )

  void MeasurementStateOnSurface_v1::setUnbiasedResidual(float unbiasedResidualX, float unbiasedResidualY) {
    static Accessor< float > acc1( "unbiasedResidualX" );
    acc1( *this ) = unbiasedResidualX;
    static Accessor< float > acc2( "unbiasedResidualY" );
    acc2( *this ) = unbiasedResidualY;
  }

  AUXSTORE_PRIMITIVE_GETTER( MeasurementStateOnSurface_v1, float, unbiasedPullX )
  AUXSTORE_PRIMITIVE_GETTER( MeasurementStateOnSurface_v1, float, unbiasedPullY )

  void MeasurementStateOnSurface_v1::setUnbiasedPull(float unbiasedPullX, float unbiasedPullY) {
    static Accessor< float > acc1( "unbiasedPullX" );
    acc1( *this ) = unbiasedPullX;
    static Accessor< float > acc2( "unbiasedPullY" );
    acc2( *this ) = unbiasedPullY;
  }

  AUXSTORE_PRIMITIVE_GETTER( MeasurementStateOnSurface_v1, float, biasedResidualX )
  AUXSTORE_PRIMITIVE_GETTER( MeasurementStateOnSurface_v1, float, biasedResidualY )

  void MeasurementStateOnSurface_v1::setBiasedResidual(float biasedResidualX, float biasedResidualY) {
    static Accessor< float > acc1( "biasedResidualX" );
    acc1( *this ) = biasedResidualX;
    static Accessor< float > acc2( "biasedResidualY" );
    acc2( *this ) = biasedResidualY;
  }

  AUXSTORE_PRIMITIVE_GETTER( MeasurementStateOnSurface_v1, float, biasedPullX )
  AUXSTORE_PRIMITIVE_GETTER( MeasurementStateOnSurface_v1, float, biasedPullY )

  void MeasurementStateOnSurface_v1::setBiasedPull(float biasedPullX, float biasedPullY) {
    static Accessor< float > acc1( "biasedPullX" );
    acc1( *this ) = biasedPullX;
    static Accessor< float > acc2( "biasedPullY" );
    acc2( *this ) = biasedPullY;
  }

  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( MeasurementStateOnSurface_v1, ElementLink<xAOD::PrepRawDataContainer>, prepRawDataLink, setPrepRawDataLink)
  //   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( MeasurementStateOnSurface_v1, ElementLink<xAOD::TrackParticle>, trackParticleLink, setTrackParticleLink)

} // namespace xAOD
