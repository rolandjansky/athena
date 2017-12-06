/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * SCT_WaferGoodStripInfo.h
 * @file header file for data object for good strip information of a wafer
 * @author Susumu Oda - 24/11/2017
 **/

#ifndef SCT_WAFERGOODSTRIPINFO_H
#define SCT_WAFERGOODSTRIPINFO_H

// Include boost stuff
#include "boost/array.hpp"

// Definition of the number of elements
#include "SCT_ConditionsServices/SCT_ConditionsParameters.h"

typedef boost::array<bool, SCT_ConditionsServices::STRIPS_PER_WAFER> SCT_WaferGoodStripInfo;

// Class definition
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SCT_WaferGoodStripInfo , 3844565 , 1 )

#endif // SCT_WAFERGOODSTRIPINFO_H
