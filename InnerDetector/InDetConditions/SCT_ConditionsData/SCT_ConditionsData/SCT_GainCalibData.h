// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_GainCalibData.h
 * @brief header file for data object for gain parameters for all modules
 * @author Susumu Oda
 * @date 12/10/17
 **/

#ifndef SCT_GAINCALIBDATA_H
#define SCT_GAINCALIBDATA_H

#include "SCT_ConditionsData/SCT_ModuleGainCalibData.h"

// Definition of the number of elements
#include "SCT_ConditionsData/SCT_ConditionsParameters.h"

/**
 * @typedef SCT_GainCalibData
 * @brief Typedef for data object used in SCT_ReadCalibChipGainCondAlg and SCT_ReadCalibChipDataTool.
 **/
typedef std::array<SCT_ModuleGainCalibData, SCT_ConditionsData::NUMBER_OF_MODULES> SCT_GainCalibData;

// Class definition for StoreGate
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SCT_GainCalibData , 221029672 , 1 )

// Condition container definition for CondInputLoader
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( SCT_GainCalibData, 46251018 );

#endif // SCT_GAINCALIBDATA_H
