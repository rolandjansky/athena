/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef EDM_SCHEMA_ENUMS_HH
#define EDM_SCHEMA_ENUMS_HH

#include <string>

namespace FlavorTagDiscriminants {
  // The old EDM schema is (unfortunately) needed for backward
  // compatibility with older derivations. But we have these
  // centralized so that older naming schemes are easy to remove.
  enum class EDMSchema {WINTER_2018, FEB_2019};
  EDMSchema enumFromString(const std::string&);
}

#endif
