/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: VertexAuxContainer_v1.cxx 575751 2013-12-16 16:45:36Z krasznaa $

// Local include(s):
#include "xAODTracking/versions/VertexAuxContainer_v1.h"
 
namespace xAOD {
 
   VertexAuxContainer_v1::VertexAuxContainer_v1()
      : AuxContainerBase() {
 
      AUX_VARIABLE( chiSquared );
      AUX_VARIABLE( numberDoF );

      AUX_VARIABLE( x );
      AUX_VARIABLE( y );
      AUX_VARIABLE( z );

      AUX_VARIABLE( covariance );
      AUX_VARIABLE( vertexType );

      AUX_VARIABLE( trackParticleLinks );
      AUX_VARIABLE( trackWeights );

      AUX_VARIABLE( neutralParticleLinks );
      AUX_VARIABLE( neutralWeights );
   }
 
} // namespace xAOD
