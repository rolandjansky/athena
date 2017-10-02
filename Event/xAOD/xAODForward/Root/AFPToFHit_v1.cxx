/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODForward/versions/AFPToFHit_v1.h"
//#include "xAODForward/AFPTrackContainer.h"


namespace xAOD
{
  AFPToFHit_v1::AFPToFHit_v1() : SG::AuxElement()
  { }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPToFHit_v1, int, stationID, setStationID)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPToFHit_v1, float, time, setTime)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPToFHit_v1, int, hptdcChannel, setHptdcChannel)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPToFHit_v1, int, barInTrainID, setBarInTrainID)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPToFHit_v1, int, trainID, setTrainID)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPToFHit_v1, float, pulseLength, setPulseLength)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPToFHit_v1, int, hptdcID, setHptdcID)  
}
