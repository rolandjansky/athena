/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODTracking/versions/PrepRawDataAuxContainer_v1.h"
 
namespace xAOD {
 
  PrepRawDataAuxContainer_v1::PrepRawDataAuxContainer_v1()
    : AuxContainerBase() {
      AUX_VARIABLE( identifier );
      AUX_VARIABLE( rdoIdentifierList );

      AUX_VARIABLE( localX );
      AUX_VARIABLE( localY );
      AUX_VARIABLE( localXError );
      AUX_VARIABLE( localYError );
      AUX_VARIABLE( localXYCorrelation );

      AUX_VARIABLE( globalX );
      AUX_VARIABLE( globalY );
      AUX_VARIABLE( globalZ );
  }

} // namespace xAOD
