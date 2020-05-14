// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_WaferGoodStripInfo.h
 * @brief header file for data object for good strip information of a wafer
 * @author Susumu Oda
 * @date 24/11/2017
 **/

#ifndef SCT_WAFERGOODSTRIPINFO_H
#define SCT_WAFERGOODSTRIPINFO_H

// Definition of the number of elements
#include "SCT_ConditionsData/SCT_ConditionsParameters.h"

// STL
#include <array>

/**
 * @typedef SCT_WaferGoodStripInfo
 * @brief Class for data object used in SCT_ReadCalibDataCondAlg and SCT_AllGoodStripInfo.
 **/
typedef std::array<bool, SCT_ConditionsData::STRIPS_PER_WAFER> SCT_WaferGoodStripInfo;

// Class definition for StoreGate
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SCT_WaferGoodStripInfo , 3844565 , 1 )

#endif // SCT_WAFERGOODSTRIPINFO_H
