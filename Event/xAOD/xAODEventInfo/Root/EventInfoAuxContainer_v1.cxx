/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventInfoAuxContainer_v1.cxx 636390 2014-12-16 21:52:18Z cranshaw $

// Local include(s):
#include "xAODEventInfo/versions/EventInfoAuxContainer_v1.h"

namespace xAOD {

   EventInfoAuxContainer_v1::EventInfoAuxContainer_v1()
      : AuxContainerBase() {

      // Basic event information:
      AUX_VARIABLE( runNumber );
      AUX_VARIABLE( eventNumber );
      AUX_VARIABLE( lumiBlock );
      AUX_VARIABLE( timeStamp );
      AUX_VARIABLE( timeStampNSOffset );
      AUX_VARIABLE( bcid );
      AUX_VARIABLE( detectorMask0 );
      AUX_VARIABLE( detectorMask1 );
      AUX_VARIABLE( detectorMask2 );
      AUX_VARIABLE( detectorMask3 );

      // Event type information:
      AUX_VARIABLE( detDescrTags );
      AUX_VARIABLE( eventTypeBitmask );

      // Detector flags:
      AUX_VARIABLE( pixelFlags );
      AUX_VARIABLE( sctFlags );
      AUX_VARIABLE( trtFlags );
      AUX_VARIABLE( larFlags );
      AUX_VARIABLE( tileFlags );
      AUX_VARIABLE( muonFlags );
      AUX_VARIABLE( forwardDetFlags );
      AUX_VARIABLE( coreFlags );
      AUX_VARIABLE( backgroundFlags );
      AUX_VARIABLE( lumiFlags );

      // MC information:
      AUX_VARIABLE( mcChannelNumber );
      AUX_VARIABLE( mcEventNumber );
      AUX_VARIABLE( mcEventWeights );
      
      // Pileup information:
      AUX_VARIABLE( pileUpMixtureIDLowBits );
      AUX_VARIABLE( pileUpMixtureIDHighBits );
   }

} // namespace xAOD
