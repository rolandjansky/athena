// Emacs -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGNAVSTRUCTURE_IHOLDERFACTORY
#define TRIGNAVSTRUCTURE_IHOLDERFACTORY

#include "TrigNavStructure/Types.h"
#include <vector>

namespace HLT {
  class BaseHolder;
  struct ITrigHolderFactory {
    virtual BaseHolder* fromSerialized(int version, const std::vector<uint32_t>::const_iterator& start, const std::vector<uint32_t>::const_iterator& end) = 0;    
    virtual BaseHolder* createHolder(class_id_type clid, const std::string& label, sub_index_type index) = 0;    
  };
}
#endif
