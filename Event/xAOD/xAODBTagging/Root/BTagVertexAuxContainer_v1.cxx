/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTaggingAuxContainer_v1.cxx 584372 2014-02-20 10:51:12Z jerdmann $

// Local include(s):
#include "xAODBTagging/versions/BTagVertexAuxContainer_v1.h"

namespace xAOD {

   BTagVertexAuxContainer_v1::BTagVertexAuxContainer_v1()
      : AuxContainerBase() {


     AUX_VARIABLE( chi2 );
     AUX_VARIABLE( NDF );
     AUX_VARIABLE( pv_compatibility );
     AUX_VARIABLE( compToOtherSV );
//     AUX_VARIABLE( reco_vertexLink );

     AUX_VARIABLE( track_chi2 );
     AUX_VARIABLE( track_NDF );
     AUX_VARIABLE( track_refPx );
     AUX_VARIABLE( track_refPy );
     AUX_VARIABLE( track_refPz );
     AUX_VARIABLE( track_links );

   }

} // namespace xAOD
