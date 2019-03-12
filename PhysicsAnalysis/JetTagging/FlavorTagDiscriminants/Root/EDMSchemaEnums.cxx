/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "FlavorTagDiscriminants/EDMSchemaEnums.h"

#include <stdexcept>

namespace FlavorTagDiscriminants{

#define RETURN_SCHEMA(schema) \
  if (name == std::string(#schema)) return EDMSchema::schema

  EDMSchema enumFromString(const std::string& name) {
    RETURN_SCHEMA(WINTER_2018);
    RETURN_SCHEMA(FEB_2019);
    throw std::logic_error("b-tagging name schema " + name + " unknown");
  }

#undef RETURN_SCHEMA

}
