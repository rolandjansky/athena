/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTaggingAuxContainer_v1.cxx 602796 2014-06-19 21:49:07Z vacavant $

// Local include(s):
#include "xAODBTagging/versions/BTaggingAuxContainer_v1.h"

namespace xAOD {

   BTaggingAuxContainer_v1::BTaggingAuxContainer_v1()
      : AuxContainerBase() {

     AUX_VARIABLE( SV0_significance3D );
     // AUX_VARIABLE( SV0_TrackParticles );

     AUX_VARIABLE( SV1_pb );
     AUX_VARIABLE( SV1_pu );
     AUX_VARIABLE( SV1_pc );
     // AUX_VARIABLE( SV1_TrackParticles );

     AUX_VARIABLE( IP2D_pb );
     AUX_VARIABLE( IP2D_pu );
     AUX_VARIABLE( IP2D_pc );
     // AUX_VARIABLE( IP2D_TrackParticles );
     
     AUX_VARIABLE( IP3D_pb );
     AUX_VARIABLE( IP3D_pu );
     AUX_VARIABLE( IP3D_pc );
     // AUX_VARIABLE( IP3D_TrackParticles );

     AUX_VARIABLE( JetFitter_pb );
     AUX_VARIABLE( JetFitter_pu );
     AUX_VARIABLE( JetFitter_pc );

     AUX_VARIABLE( JetFitterCombNN_pb );
     AUX_VARIABLE( JetFitterCombNN_pu );
     AUX_VARIABLE( JetFitterCombNN_pc );

     AUX_VARIABLE( MV1_discriminant );

   }

} // namespace xAOD
