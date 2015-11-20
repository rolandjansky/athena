// Emacs -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGNAVSTRUCTURE_BASEHOLDER_H
#define TRIGNAVSTRUCTURE_BASEHOLDER_H
#include <string>
#include <vector>
#include <TrigNavStructure/Types.h>

namespace HLT {
  class BaseHolder {
  public:
    virtual ~BaseHolder();
    virtual const std::string& label() const = 0;
    virtual class_id_type typeClid() const = 0;
    virtual sub_index_type subTypeIndex() const = 0;
    

    static bool enquireSerialized(std::vector<uint32_t>::const_iterator& fromHere,
				  const std::vector<uint32_t>::const_iterator& end,
				  class_id_type& c, std::string& label,
				  sub_index_type& subtypeIndex );

    virtual bool serialize(std::vector<uint32_t>& data) const;

  };
}

#endif
