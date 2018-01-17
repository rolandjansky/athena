/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * SCT_AllGoodStripInfo.h
 * @file header file for data object for good strips for all wafers
 * @author Susumu Oda - 24/11/2017
 **/

#ifndef SCT_ALLGOODSTRIPINFO_H
#define SCT_ALLGOODSTRIPINFO_H

#include "SCT_ConditionsData/SCT_WaferGoodStripInfo.h"

// Definition of the number of elements
#include "SCT_ConditionsServices/SCT_ConditionsParameters.h"

typedef boost::array<SCT_WaferGoodStripInfo, SCT_ConditionsServices::NUMBER_OF_WAFERS> SCT_AllGoodStripInfo;

// Class definition
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SCT_AllGoodStripInfo , 140934533 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( SCT_AllGoodStripInfo, 7553175 );

#endif // SCT_ALLGOODSTRIPINFO_H
