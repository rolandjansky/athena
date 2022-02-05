/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/HLTMenu.h"
#include "TrigConfHLTData/HLTFrame.h"

void convertRun2HLTMenuToRun3(const TrigConf::HLTFrame* frame, const std::string& filename);
void convertRun2HLTPrescalesToRun3(const TrigConf::HLTFrame* frame, const std::string& filename);
