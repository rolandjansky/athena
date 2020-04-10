/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODEventInfo/versions/EventAuxInfo_v2.h"

namespace xAOD {

   EventAuxInfo_v2::EventAuxInfo_v2()
     : AuxInfoBase(),
       runNumber(0),
       eventNumber(0),
       lumiBlock(0),
       timeStamp(0),
       timeStampNSOffset(0),
       bcid(0),
       detectorMask0(0),
       detectorMask1(0),
       detectorMask2(0),
       detectorMask3(0),
       eventTypeBitmask(0),
       statusElement(0),
       extendedLevel1ID(0),
       level1TriggerType(0),
       pixelFlags(0),
       sctFlags(0),
       trtFlags(0),
       larFlags(0),
       tileFlags(0),
       muonFlags(0),
       forwardDetFlags(0),
       coreFlags(0),
       backgroundFlags(0),
       lumiFlags(0),
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
      AUX_VARIABLE( detectorMask2 );
      AUX_VARIABLE( detectorMask3 );

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
   }

} // namespace xAOD
