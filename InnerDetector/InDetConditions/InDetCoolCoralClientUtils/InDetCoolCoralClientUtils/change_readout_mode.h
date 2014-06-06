/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CHANGE_READOUT_MODE_H
#define CHANGE_READOUT_MODE_H

#include <iostream>
#include <vector>
#include <string>

#include "DB_Item_Naming_Rules.h"
#include "TRTDatabase/ConfDBif.h"

typedef struct {
  int detector;
  int phi;
  int side;
  int ttc_addr;
  int ttc_group;
  int rod_addr;
  int rod_half;
} sector;


int change_part_TRT();
int change_TRT_segment();
int change_Barrel_Endcap_Segments(std::string);
int process_config(std::vector<sector*>*, std::string);

#endif
