// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * SCT_ModuleNoiseCalibData.h
 * @file header file for data object for noise parameters per module
 * @author Susumu Oda
 * @date 12/10/17
 **/

#ifndef SCT_MODULENOISECALIBDATA_H
#define SCT_MODULENOISECALIBDATA_H

#include "SCT_ConditionsData/SCT_ModuleCalibParameter.h"

// Definition of the number of elements
#include "SCT_ConditionsData/SCT_ConditionsParameters.h"

/**
 * @typedef SCT_ModuleNoiseCalibData
 * @brief Typedef for data object used in SCT_ReadCalibChipNoiseCondAlg and SCT_ReadCalibChipDataTool.
 **/
typedef std::array<SCT_ModuleCalibParameter, SCT_ConditionsData::N_NOISEOCC> SCT_ModuleNoiseCalibData;

// Class definition for StoreGate
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SCT_ModuleNoiseCalibData , 59343966 , 1 )

#endif // SCT_MODULENOISECALIBDATA_H
