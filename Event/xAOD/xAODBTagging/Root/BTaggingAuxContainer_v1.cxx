/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTaggingAuxContainer_v1.cxx 796222 2017-02-08 14:55:59Z guirriec $

// Local include(s):
#include "xAODBTagging/versions/BTaggingAuxContainer_v1.h"

namespace xAOD {

   BTaggingAuxContainer_v1::BTaggingAuxContainer_v1()
      : AuxContainerBase() {

     AUX_VARIABLE( SV1_pb );
     AUX_VARIABLE( SV1_pu );
     AUX_VARIABLE( SV1_pc );

     AUX_VARIABLE( IP2D_pb );
     AUX_VARIABLE( IP2D_pu );
     AUX_VARIABLE( IP2D_pc );
     
     AUX_VARIABLE( IP3D_pb );
     AUX_VARIABLE( IP3D_pu );
     AUX_VARIABLE( IP3D_pc );

     AUX_VARIABLE( JetFitter_pb );
     AUX_VARIABLE( JetFitter_pu );
     AUX_VARIABLE( JetFitter_pc );

   }

} // namespace xAOD
