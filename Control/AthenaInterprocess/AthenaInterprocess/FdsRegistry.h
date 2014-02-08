/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAINTERPROCESS_FDSREGISTRY_H
#define ATHENAINTERPROCESS_FDSREGISTRY_H

#include <string>
#include <vector>

namespace AthenaInterprocess {

  struct FdsRegistryEntry {
    FdsRegistryEntry(int _fd, std::string _name)
    : fd(_fd)
    , name(_name) {}

    int fd;
    std::string name;
  };

  typedef std::vector<FdsRegistryEntry> FdsRegistry;

}

#endif
