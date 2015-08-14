/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGNAVSTRUCTURE_TYPES_H
#define TRIGNAVSTRUCTURE_TYPES_H
namespace HLT {
  typedef uint32_t teid_type;
  typedef uint16_t sub_index_type;
  typedef uint32_t index_type;
  typedef uint32_t class_id_type;      
  

  const static class_id_type invalid_class_id = 0;
  const static index_type invalid_index = static_cast<uint32_t>(-1);
  const static index_type invalid_sub_index = static_cast<uint16_t>(-1);

}
#endif 
