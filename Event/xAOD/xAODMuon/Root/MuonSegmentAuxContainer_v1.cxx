/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODMuon/versions/MuonSegmentAuxContainer_v1.h"
 
namespace xAOD {
 
  MuonSegmentAuxContainer_v1::MuonSegmentAuxContainer_v1()
    : AuxContainerBase() {
    AUX_VARIABLE( x );
    AUX_VARIABLE( y );
    AUX_VARIABLE( z );

    AUX_VARIABLE( px );
    AUX_VARIABLE( py );
    AUX_VARIABLE( pz );

    AUX_VARIABLE( t0      );
    AUX_VARIABLE( t0error );

    AUX_VARIABLE( chiSquared );
    AUX_VARIABLE( numberDoF  );

    AUX_VARIABLE( sector );
    AUX_VARIABLE( chamberIndex );
    AUX_VARIABLE( etaIndex );
    AUX_VARIABLE( technology );

    AUX_VARIABLE( nPrecisionHits );
    AUX_VARIABLE( nPhiLayers     );
    AUX_VARIABLE( nTrigEtaLayers );
  }

} // namespace xAOD
