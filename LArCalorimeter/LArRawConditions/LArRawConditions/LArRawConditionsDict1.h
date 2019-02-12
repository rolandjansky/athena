/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-


#include "LArRawConditions/LArRawConditionsDictCommon.h"


//=== Base classes

//=== LArAutoCorrP
#include "LArRawConditions/LArAutoCorrMC.h"
#include "LArRawConditions/LArAutoCorrP.h"
#include "LArRawConditions/LArAutoCorrP.h"
#include "LArRawConditions/LArAutoCorrP1.h"
INSTAN_CC(LArAutoCorrP1);
INSTAN_CS(LArAutoCorrP);
//PYGATE(LArAutoCorrComplete);
//PYGATE(LArAutoCorrMC);


#include "LArRawConditions/LArCaliPulseParamsComplete.h"
#include "LArRawConditions/LArCaliPulseParamsP.h"
INSTAN_BOTH(LArCaliPulseParamsP);


//=== LArDAC2uAP
#include "LArRawConditions/LArDAC2uAMC.h"
#include "LArRawConditions/LArDAC2uAP.h"
INSTAN_BOTH(LArDAC2uAP);
//PYGATE(LArDAC2uAMC);


//=== LArDetCellParams
#include "LArRawConditions/LArDetCellParamsP.h"
#include "LArRawConditions/LArDetCellParamsComplete.h"
INSTAN_BOTH(LArDetCellParamsP);


//=== LArfSamplP
#include "LArRawConditions/LArfSamplP.h"
#include "LArRawConditions/LArfSamplMC.h"
INSTAN_BOTH(LArfSamplP);
//PYGATE(LArfSamplMC);


//=== LArH6Oscillation
#include "LArRawConditions/LArH6OscillationP.h"
#include "LArRawConditions/LArH6OscillationComplete.h"
INSTAN_BOTH(LArH6OscillationP);


//=== LArMinBiasP
#include "LArRawConditions/LArMinBiasP.h"
#include "LArRawConditions/LArMinBiasMC.h"
INSTAN_BOTH(LArMinBiasP);
//PYGATE(LArMinBiasMC);


//=== LArMinBiasAverageP
#include "LArRawConditions/LArMinBiasAverageP.h"
#include "LArRawConditions/LArMinBiasAverageComplete.h"
#include "LArRawConditions/LArMinBiasAverageMC.h"
INSTAN_BOTH(LArMinBiasAverageP);
//PYGATE(LArMinBiasAverageMC);


//=== LArMphysOverMcalP
#include "LArRawConditions/LArMphysOverMcalP1.h"
#include "LArRawConditions/LArMphysOverMcalP.h"
#include "LArRawConditions/LArMphysOverMcalMC.h"
INSTAN_CC(LArMphysOverMcalP1);
INSTAN_CS(LArMphysOverMcalP);
//PYGATE(LArMphysOverMcalComplete);


//=== LArNoiseP
#include "LArRawConditions/LArNoiseP.h"
#include "LArRawConditions/LArNoiseMC.h"
#include "LArRawConditions/LArNoiseComplete.h"
INSTAN_BOTH(LArNoiseP);
//PYGATE(LArNoiseMC);


//=== LArOFCP
#include "LArRawConditions/LArOFCP1.h"
#include "LArRawConditions/LArOFCComplete.h"
#include "LArRawConditions/LArOFCMC.h"
#include "LArRawConditions/LArOFCP.h"
INSTAN_CC(LArOFCP1);
INSTAN_CS(LArOFCP);
//PYGATE(LArOFCComplete);


//=== LArPedestalP
#include "LArRawConditions/LArPedestalP.h"
#include "LArRawConditions/LArPedestalP1.h"
#include "LArRawConditions/LArPedestalComplete.h"
#include "LArRawConditions/LArPedestalMC.h"
INSTAN_CC(LArPedestalP1);
INSTAN_CS(LArPedestalP);
//PYGATE(LArPedestalComplete);
//PYGATE(LArPedestalMC);
//PYGATE(ILArPedestal);


//=== LArPhysCaliTdiffP
#include "LArRawConditions/LArPhysCaliTdiffP.h"
#include "LArRawConditions/LArPhysCaliTdiffComplete.h"
INSTAN_BOTH(LArPhysCaliTdiffP);


//=== LArRampP
#include "LArRawConditions/LArRampP.h"
#include "LArRawConditions/LArRampP1.h"
#include "LArRawConditions/LArRampComplete.h"
#include "LArRawConditions/LArRampMC.h"
INSTAN_CC(LArRampP1);
INSTAN_CS(LArRampP);
//PYGATE(LArRampComplete);
//PYGATE(LArRampMC);


//=== LArSingleFloatP
#include "LArRawConditions/LArSingleFloatP.h"
INSTAN_BOTH(LArSingleFloatP);
//PYGATE(LArSingleFloatP);

#include "LArRawConditions/LArHVScaleCorrComplete.h"
//PYGATE(LArHVScaleCorrComplete);

//=== LAruA2MeVP
// (Depends on LArConditionsContainer<LArSingleFloatP>)
#include "LArRawConditions/LAruA2MeVP.h"
#include "LArRawConditions/LAruA2MeVMC.h"
INSTAN_BOTH(LAruA2MeVP);
//PYGATE(LAruA2MeVMC);



//===LArDSPThresholds
#include "LArRawConditions/LArDSPThresholdsComplete.h"
