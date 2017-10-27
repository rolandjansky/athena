/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * SCT_GainCalibData.h
 * @file header file for data object for gain parameters for all modules
 * @author Susumu Oda - 12/10/17
 **/

#ifndef SCT_GAINCALIBDATA_H
#define SCT_GAINCALIBDATA_H

#include "SCT_ConditionsData/SCT_ModuleGainCalibData.h"

// Definition of the number of elements
#include "SCT_ConditionsServices/SCT_ConditionsParameters.h"

typedef boost::array<SCT_ModuleGainCalibData, SCT_ConditionsServices::NUMBER_OF_MODULES> SCT_GainCalibData;

// Class definition
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SCT_GainCalibData , 221029672 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( SCT_GainCalibData, 46251018 );

#endif // SCT_GAINCALIBDATA_H
