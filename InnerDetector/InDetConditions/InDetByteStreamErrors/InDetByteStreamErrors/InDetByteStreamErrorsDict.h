/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETBYTESTREAMERRORSDICT_H
#define INDETBYTESTREAMERRORSDICT_H

#include "InDetByteStreamErrors/InDetBSErrContainer.h"
#include "InDetByteStreamErrors/TRT_BSErrContainer.h"
#include "InDetByteStreamErrors/TRT_BSIdErrContainer.h"

namespace {
  // Need this to create the dict entries

  std::pair<uint8_t, uint32_t> dummy_pair_trt;
 
  std::pair<uint8_t, std::pair<uint32_t, uint8_t> > dummy_id_pair;
  std::vector<std::pair<uint8_t, std::pair<uint32_t, uint8_t> >* > dummy_id_ptr_vec;

}

#endif // INDETBYTESTREAMERRORSDICT_H
