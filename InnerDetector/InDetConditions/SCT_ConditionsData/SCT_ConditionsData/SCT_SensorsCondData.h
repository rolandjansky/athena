// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_SensorsCondData.h
 * @brief header file for data object for SCT_SensorsCondAlg and SCT_SensorsTool.
 * @author Susumu Oda
 * @date 2017-12-04
 **/

#ifndef SCT_SENSORSCONDDATA_H
#define SCT_SENSORSCONDDATA_H

#include "SCT_ConditionsData/SCT_SensorCondData.h"

// Definition of the number of elements
#include "SCT_ConditionsData/SCT_ConditionsParameters.h"

#include <map>

/**
 * @typedef SCT_SensorsCondData
 * @brief Class for data object for SCT_SensorsCondAlg and SCT_SensorsTool.
 **/
typedef std::map<CondAttrListCollection::ChanNum, SCT_SensorCondData> SCT_SensorsCondData;

// Class definition for StoreGate
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SCT_SensorsCondData , 198841041 , 1 )

// Condition container definition for CondInputLoader
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( SCT_SensorsCondData, 137294721 );

#endif // SCT_SENSORSCONDDATA_H
