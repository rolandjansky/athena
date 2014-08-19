/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventAuxInfo_v1.cxx 612175 2014-08-17 01:59:37Z ssnyder $

// Local include(s):
#include "xAODEventInfo/versions/EventAuxInfo_v1.h"

namespace xAOD {

   EventAuxInfo_v1::EventAuxInfo_v1()
     : AuxInfoBase(),
       eventTypeBitmask(0),
       beamPosX(0),
       beamPosY(0),
       beamPosZ(0),
       beamPosSigmaX(0),
       beamPosSigmaY(0),
       beamPosSigmaZ(0),
       beamPosSigmaXY(0),
       beamTiltXZ(0),
       beamTiltYZ(0),
       beamStatus(0)
   {

      // Basic event information:
      AUX_VARIABLE( runNumber );
      AUX_VARIABLE( eventNumber );
      AUX_VARIABLE( lumiBlock );
      AUX_VARIABLE( timeStamp );
      AUX_VARIABLE( timeStampNSOffset );
      AUX_VARIABLE( bcid );
      AUX_VARIABLE( detectorMask0 );
      AUX_VARIABLE( detectorMask1 );

      // Event type information:
      AUX_VARIABLE( detDescrTags );
      AUX_VARIABLE( eventTypeBitmask );

      // Trigger related information:
      AUX_VARIABLE( statusElement );
      AUX_VARIABLE( extendedLevel1ID );
      AUX_VARIABLE( level1TriggerType );
      AUX_VARIABLE( streamTagNames );
      AUX_VARIABLE( streamTagTypes );
      AUX_VARIABLE( streamTagObeysLumiblock );

      // Pileup information:
      AUX_VARIABLE( actualInteractionsPerCrossing );
      AUX_VARIABLE( averageInteractionsPerCrossing );

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

      // Beam spot information:
      AUX_VARIABLE( beamPosX );
      AUX_VARIABLE( beamPosY );
      AUX_VARIABLE( beamPosZ );
      AUX_VARIABLE( beamPosSigmaX );
      AUX_VARIABLE( beamPosSigmaY );
      AUX_VARIABLE( beamPosSigmaZ );
      AUX_VARIABLE( beamPosSigmaXY );
      AUX_VARIABLE( beamTiltXZ );
      AUX_VARIABLE( beamTiltYZ );
      AUX_VARIABLE( beamStatus );
   }

} // namespace xAOD
