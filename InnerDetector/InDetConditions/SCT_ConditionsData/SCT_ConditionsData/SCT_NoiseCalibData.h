// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_NoiseCalibData.h
 * @brief header file for data object for noise parameters for all modules
 * @author Susumu Oda
 * @date 12/10/17
 **/

#ifndef SCT_NOISECALIBDATA_H
#define SCT_NOISECALIBDATA_H

#include "SCT_ConditionsData/SCT_ModuleNoiseCalibData.h"

// Definition of the number of elements
#include "SCT_ConditionsData/SCT_ConditionsParameters.h"

/**
 * @typedef SCT_NoiseCalibData
 * @brief Class for data object used in SCT_ReadCalibChipNoiseCondAlg and SCT_ReadCalibChipDataTool.
 **/
typedef std::array<SCT_ModuleNoiseCalibData, SCT_ConditionsData::NUMBER_OF_MODULES> SCT_NoiseCalibData;

// Class definition for StoreGate
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SCT_NoiseCalibData , 58952480 , 1 )

// Condition container definition for CondInputLoader
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( SCT_NoiseCalibData , 29066680 );

#endif // SCT_NOISECALIBDATA_H
