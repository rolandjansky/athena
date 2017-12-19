/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * SCT_NoiseCalibData.h
 * @file header file for data object for noise parameters for all modules
 * @author Susumu Oda - 12/10/17
 **/

#ifndef SCT_NOISECALIBDATA_H
#define SCT_NOISECALIBDATA_H

#include "SCT_ConditionsData/SCT_ModuleNoiseCalibData.h"

// Definition of the number of elements
#include "SCT_ConditionsServices/SCT_ConditionsParameters.h"

typedef boost::array<SCT_ModuleNoiseCalibData, SCT_ConditionsServices::NUMBER_OF_MODULES> SCT_NoiseCalibData;

// Class definition
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SCT_NoiseCalibData , 58952480 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( SCT_NoiseCalibData , 29066680 );

#endif // SCT_NOISECALIBDATA_H
