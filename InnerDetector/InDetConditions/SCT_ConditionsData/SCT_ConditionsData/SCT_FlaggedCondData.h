// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_FlaggedCondData.h
 * @brief header file for data object for SCT_Clusterization and SCT_FlaggedConditionTool.
 * @author Susumu Oda
 * @date 21/02/2018
 **/

#ifndef SCT_FLAGGEDCONDDATA_H
#define SCT_FLAGGEDCONDDATA_H

#include <map>
#include <string>

#include "Identifier/IdentifierHash.h"

/**
 * @typedef SCT_FlaggedCondData
 * @brief Typedef for data object used in SCT_Clusterization and SCT_FlaggedConditionTool.
 */
typedef std::map<IdentifierHash, std::string> SCT_FlaggedCondData;

// Class definition for StoreGate
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SCT_FlaggedCondData , 188972386 , 1 )

#endif // SCT_FLAGGEDCONDDATA_H
