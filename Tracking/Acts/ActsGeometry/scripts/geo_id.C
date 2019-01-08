/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include <iostream>
#include <string>

#include "TFile.h"
#include "TTreeReader.h"

typedef uint64_t geo_id_value;

const static geo_id_value volume_mask    = 0xff00000000000000;
const static geo_id_value boundary_mask  = 0x00ff000000000000;
const static geo_id_value layer_mask     = 0x0000ff0000000000;
const static geo_id_value approach_mask  = 0x000000f000000000;
const static geo_id_value sensitive_mask = 0x0000000ffff00000;
const static geo_id_value channel_mask   = 0x00000000000fffff;

#define ACTS_BIT_SHIFT(mask) (__builtin_ffsl(mask) - 1)
#define ACTS_BIT_ENCODE(value, mask) (value << ACTS_BIT_SHIFT(mask))
#define ACTS_BIT_DECODE(code, mask) ((code & mask) >> ACTS_BIT_SHIFT(mask))

void geo_id(std::string filename) {

  auto infile = TFile::Open(filename.c_str());

  TTreeReader rd("MaterialTracks", infile);

  TTreeReaderValue<std::vector<ULong64_t>> step_geo_id(rd, "step_geo_id");

  while(rd.Next()) {
    for(ULong64_t geo_id : *step_geo_id) {
      std::cout << "geo_id = " << geo_id << std::endl;
      uint64_t vol_id = ACTS_BIT_DECODE(geo_id, volume_mask);
      std::cout << "vol_id = " << vol_id << std::endl;

    }

    std::cout << "----" << std::endl;
  }

}
