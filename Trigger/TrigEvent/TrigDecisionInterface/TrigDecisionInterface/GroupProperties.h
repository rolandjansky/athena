/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGDECISIONINTERFACE_GROUPPROPERTIES_H
#define TRIGDECISIONINTERFACE_GROUPPROPERTIES_H

#include "CxxUtils/bitmask.h"

namespace TrigDefs {

  /// Properties of a chain group
  enum class Group {
    Default = 0,
    NoRegex = 1,        //!< Do not use regular expressions
    ATH_BITMASK
  };
}

#endif
