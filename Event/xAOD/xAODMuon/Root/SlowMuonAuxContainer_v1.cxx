/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODMuon/versions/SlowMuonAuxContainer_v1.h"
 
namespace xAOD {
 
  SlowMuonAuxContainer_v1::SlowMuonAuxContainer_v1()
    : AuxContainerBase() {

    AUX_VARIABLE( beta );
    AUX_VARIABLE( betaT );
    AUX_VARIABLE( ann );
    
    AUX_VARIABLE( nRpcHits );
    AUX_VARIABLE( nTileCells );

    AUX_VARIABLE( rpcBetaAvg );
    AUX_VARIABLE( rpcBetaRms );
    AUX_VARIABLE( rpcBetaChi2 );
    AUX_VARIABLE( rpcBetaDof );

    AUX_VARIABLE( mdtBetaAvg );
    AUX_VARIABLE( mdtBetaRms );
    AUX_VARIABLE( mdtBetaChi2 );
    AUX_VARIABLE( mdtBetaDof );

    AUX_VARIABLE( caloBetaAvg );
    AUX_VARIABLE( caloBetaRms );
    AUX_VARIABLE( caloBetaChi2 );
    AUX_VARIABLE( caloBetaDof );

    AUX_VARIABLE( dEdxPixel );
    AUX_VARIABLE( dEdxCalo );
    AUX_VARIABLE( dEdxNClusters );

    AUX_VARIABLE( muonLink );

  }

} // namespace xAOD
