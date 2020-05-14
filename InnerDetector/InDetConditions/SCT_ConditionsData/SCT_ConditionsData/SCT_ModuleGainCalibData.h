// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * SCT_ModuleGainCalibData.h
 * @file header file for data object for gain parameters per module
 * @author Susumu Oda
 * @date 12/10/17
 **/

#ifndef SCT_MODULEGAINCALIBDATA_H
#define SCT_MODULEGAINCALIBDATA_H

#include "SCT_ConditionsData/SCT_ModuleCalibParameter.h"

// Definition of the number of elements
#include "SCT_ConditionsData/SCT_ConditionsParameters.h"

/**
 * @typedef SCT_ModuleGainCalibData
 * @brief Typedef for data object used in SCT_ReadCalibChipGainCondAlg and SCT_ReadCalibChipDataTool.
 **/
typedef std::array<SCT_ModuleCalibParameter, SCT_ConditionsData::N_NPTGAIN> SCT_ModuleGainCalibData;

// Class definition for StoreGate
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SCT_ModuleGainCalibData , 48634490 , 1 )

#endif // SCT_MODULEGAINCALIBDATA_H
