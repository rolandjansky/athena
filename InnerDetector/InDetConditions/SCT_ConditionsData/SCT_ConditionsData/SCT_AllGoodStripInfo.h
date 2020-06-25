// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * SCT_AllGoodStripInfo.h
 * @file header file for data object for good strips for all wafers
 * @author Susumu Oda
 * @date 24/11/2017
 **/

#ifndef SCT_ALLGOODSTRIPINFO_H
#define SCT_ALLGOODSTRIPINFO_H

#include "SCT_ConditionsData/SCT_WaferGoodStripInfo.h"

// Definition of the number of elements
#include "SCT_ConditionsData/SCT_ConditionsParameters.h"

/**
 * @typedef SCT_AllGoodStripInfo
 * @brief SCT_AllGoodStripInfo is std::array of SCT_WaferGoodStripInfo
 * and is used in SCT_ReadCalibDataCondAlg and SCT_ReadCalibDataTool.
 **/

typedef std::array<SCT_WaferGoodStripInfo, SCT_ConditionsData::NUMBER_OF_WAFERS> SCT_AllGoodStripInfo;

// Class definition for StoreGate
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SCT_AllGoodStripInfo , 140934533 , 1 )

// Condition container definition for CondInputLoader
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( SCT_AllGoodStripInfo, 7553175 );

#endif // SCT_ALLGOODSTRIPINFO_H
