/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib__MdtTubeGeomertyRow_h
#define MuonCalib__MdtTubeGeomertyRow_h

#include "TTree.h"

namespace MuonCalib {

class MdtTubeGeomertyRow {
 public:
  UInt_t tube_id;
  Float_t pos_x, pos_y, pos_z;
};

}  //namespace MuonCalib 

#endif
