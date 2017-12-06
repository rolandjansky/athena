/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * SCT_ModuleCalibParameter.h
 * @file header file for data object for a calibration parameter per chip of a module
 * @author Susumu Oda - 12/10/17
 **/

#ifndef SCT_MODULECALIBPARAMETER_H
#define SCT_MODULECALIBPARAMETER_H

// Include boost stuff
#include "boost/array.hpp"

// Definition of the number of elements
#include "SCT_ConditionsServices/SCT_ConditionsParameters.h"

typedef boost::array<float, SCT_ConditionsServices::CHIPS_PER_MODULE> SCT_ModuleCalibParameter;

// Class definition
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SCT_ModuleCalibParameter , 119421667 , 1 )

#endif // SCT_MODULECALIBPARAMETER_H
