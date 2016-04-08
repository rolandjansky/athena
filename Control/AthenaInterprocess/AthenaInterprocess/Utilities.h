/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAINTERPROCESS_UTILITIES_H
#define ATHENAINTERPROCESS_UTILITIES_H

#include <uuid/uuid.h>
#include <string>
#define SSIZE 37

namespace AthenaInterprocess {
  inline std::string randString()
  {
    uuid_t uuid;
    char out[SSIZE];
    uuid_generate(uuid);
    uuid_unparse(uuid, out);
    return std::string(out);
  }
}

#endif
