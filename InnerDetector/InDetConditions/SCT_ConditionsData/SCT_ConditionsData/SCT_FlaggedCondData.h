/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * SCT_FlaggedCondData.h
 * @file header file for data object for flagged conditions
 * @author Susumu Oda - 21/02/2018
 **/

#ifndef SCT_FLAGGEDCONDDATA_H
#define SCT_FLAGGEDCONDDATA_H

#include <map>
#include <string>

#include "Identifier/IdentifierHash.h"

typedef std::map<IdentifierHash, std::string> SCT_FlaggedCondData;

// Class definition
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SCT_FlaggedCondData , 188972386 , 1 )

#endif // SCT_FLAGGEDCONDDATA_H
