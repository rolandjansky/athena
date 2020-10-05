// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ModuleCalibParameter.h
 * @brief header file for data object for a calibration parameter per chip of a module
 * @author Susumu Oda
 * @date 12/10/17
 **/

#ifndef SCT_MODULECALIBPARAMETER_H
#define SCT_MODULECALIBPARAMETER_H

// Definition of the number of elements
#include "SCT_ConditionsData/SCT_ConditionsParameters.h"

// Include STL stuff
#include <array>

/**
 * @typedef SCT_ModuleCalibParameter
 * @brief typedef for SCT_ReadCalibChipGainCondAlg, SCT_ReadCalibChipNoiseCondAlg, SCT_ReadCalibChipDataTool.
 **/
typedef std::array<float, SCT_ConditionsData::CHIPS_PER_MODULE> SCT_ModuleCalibParameter;

// Class definition for StoreGate
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SCT_ModuleCalibParameter , 119421667 , 1 )

#endif // SCT_MODULECALIBPARAMETER_H
