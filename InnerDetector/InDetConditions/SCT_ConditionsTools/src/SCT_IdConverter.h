/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * SCT_IdConverter.h
 * @file Header file for converter functions for Identifiers 64 / 32 bit
 * @author Shaun Roe (shaun.roe@cern.ch)
 **/
#include "Identifier/Identifier.h"
 
namespace SCT_ConditionsTools {
  inline unsigned int castId(const Identifier& identifier) {
    return identifier.get_identifier32().get_compact();
  }
} //end of namespace
