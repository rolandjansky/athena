/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-


#include "LArRawConditions/LArRawConditionsDictCommon.h"


//=== LArShapeP
#include "LArRawConditions/LArShape32MC.h"
#include "LArRawConditions/LArShapeP.h"
#include "LArRawConditions/LArShapeP1.h"
INSTAN_CC(LArShapeP1);
INSTAN_CS(LArShapeP);
//PYGATE(LArShape32MC);


//=== LArTdriftP
#include "LArRawConditions/LArTdriftP.h"
#include "LArRawConditions/LArTdriftComplete.h"
INSTAN_BOTH(LArTdriftP);


//=== LArRinjP
#include "LArRawConditions/LArRinjP.h"
#include "LArRawConditions/LArRinjComplete.h"
INSTAN_BOTH(LArRinjP);


//=== LArCableLengthP
#include "LArRawConditions/LArCableLengthP.h"
#include "LArRawConditions/LArCableLengthComplete.h"
INSTAN_BOTH(LArCableLengthP);


//=== LArCableAttenuationP
#include "LArRawConditions/LArCableAttenuationP.h"
#include "LArRawConditions/LArCableAttenuationComplete.h"
INSTAN_BOTH(LArCableAttenuationP);


//=== LArTShaperP
#include "LArRawConditions/LArTshaperP.h"
#include "LArRawConditions/LArTshaperComplete.h"
INSTAN_BOTH(LArTshaperP);


//=== LArOFCBinP
#include "LArRawConditions/LArOFCBinComplete.h"


//=== LArEMEC_CphiP
#include "LArRawConditions/LArEMEC_CphiP.h"
#include "LArRawConditions/LArEMEC_CphiComplete.h"
INSTAN_BOTH(LArEMEC_CphiP);


//=== LArEMEC_HValphaP
#include "LArRawConditions/LArEMEC_HValphaP.h"
#include "LArRawConditions/LArEMEC_HValphaComplete.h"
INSTAN_BOTH(LArEMEC_HValphaP);


//=== LArEMEC_HVbetaP
#include "LArRawConditions/LArEMEC_HVbetaP.h"
#include "LArRawConditions/LArEMEC_HVbetaComplete.h"
INSTAN_BOTH(LArEMEC_HVbetaP);


//=== LArCaliWave
#include "LArRawConditions/LArCaliWave.h"
#include "LArRawConditions/LArCaliWaveContainer.h"
INSTAN_BOTH(LArCaliWaveVec);
//PYGATE(LArCaliWaveContainer);


//=== LArPhysWave
#include "LArRawConditions/LArPhysWave.h"
#include "LArRawConditions/LArPhysWaveContainer.h"
INSTAN_BOTH(LArPhysWave);
//PYGATE(LArPhysWaveContainer);


//=== Others

#include "LArRawConditions/LArFEBTimeOffset.h"
#include "LArRawConditions/LArCalibParams.h"
#include "LArRawConditions/LArGlobalTimeOffset.h"

namespace LArRawConditionsDict {
  std::pair< HWIdentifier, LArCalibParams::CalibBoard>     pair0;
}

