/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/HLTMenu.h"
#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfL1Data/CTPConfig.h"
void convertRun2HLTMenuToRun3(const TrigConf::HLTFrame* frame, const std::string& filename);
void convertRun2HLTPrescalesToRun3(const TrigConf::HLTFrame* frame, const std::string& filename);
void convertRun2BunchGroupsToRun3(const TrigConf::CTPConfig* frame, const std::string& filename);
void convertRun2L1PrescalesToRun3(const TrigConf::CTPConfig* frame, const std::string& filename);