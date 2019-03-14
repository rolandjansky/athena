/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARBADCHANNEL_LARBADCHANNELDICT_H
# define LARBADCHANNEL_LARBADCHANNELDICT_H

#include "Identifier/HWIdentifier.h"
#include "LArBadChannelTool/HWIdentifier32.h"
#include "LArRecConditions/LArBadChannel.h"
#include "LArRecConditions/LArBadFeb.h"
#include "LArBadChannelTool/LArBadChannelDBTools.h"
#include "LArRecConditions/LArBadChannelEnum.h"
#include "LArRecConditions/LArBadChanBitPacking.h"

#include "LArBadChannelTool/LArBadChannelMasker.h"

std::pair<bool,LArBadChannelEnum::ProblemType> dummy1;
std::pair<HWIdentifier,LArBadFeb> dummy2;
std::pair<HWIdentifier32,LArBadFeb> dummy22;
std::pair<HWIdentifier32,LArBadChannel> dummy4;
std::vector<std::pair<HWIdentifier,LArBadFeb> > dummy3;
std::vector<std::pair<HWIdentifier32,LArBadFeb> > dummy33;
std::vector<std::pair<HWIdentifier32,LArBadChannel>> dummy44;

#endif
