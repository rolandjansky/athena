/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODCutFlow/versions/CutBookkeeperAuxContainer_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {

  CutBookkeeperAuxContainer_v1::CutBookkeeperAuxContainer_v1()
  : AuxContainerBase() {

    AUX_VARIABLE( nameIdentifier );
    AUX_VARIABLE( uniqueIdentifier );
    AUX_VARIABLE( name );
    AUX_VARIABLE( description );
    AUX_VARIABLE( cutLogic );
    AUX_VARIABLE( isTopFilter );
    AUX_VARIABLE( cycle );
    AUX_VARIABLE( inputStream );
    AUX_VARIABLE( outputStreams );
    AUX_VARIABLE( parentLink );
    AUX_VARIABLE( childrenLinks );
    AUX_VARIABLE( othersLinks );
    AUX_VARIABLE( siblingsLinks );

    AUX_VARIABLE( nAcceptedEvents );
    AUX_VARIABLE( sumOfEventWeights );
    AUX_VARIABLE( sumOfEventWeightsSquared );

  }

} // namespace xAOD
