/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGNAVSTRUCTURE_TYPES_H
#define TRIGNAVSTRUCTURE_TYPES_H
#include <stdint.h>
#include <boost/variant.hpp>
namespace HLT {
  typedef uint32_t te_id_type;
  typedef uint16_t sub_index_type;
  typedef uint32_t index_type;
  typedef uint32_t class_id_type;      

  typedef boost::variant<sub_index_type,std::string> index_or_label_type;

  const static sub_index_type invalid_sub_index = 0xffff;
  const static index_type     invalid_index = 0xffffffff;
  const static class_id_type  invalid_class_id = 0;
}
#endif 
