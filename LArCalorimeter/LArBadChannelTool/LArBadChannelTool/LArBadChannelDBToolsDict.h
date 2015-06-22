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
#include "LArBadChannelTool/LArBadChanTool.h"

std::pair<bool,LArBadChannelEnum::ProblemType> m_dummy1;
std::pair<HWIdentifier,LArBadFeb> m_dummy2;
std::pair<HWIdentifier32,LArBadFeb> m_dummy22;
std::pair<HWIdentifier32,LArBadChannel> m_dummy4;
std::vector<std::pair<HWIdentifier,LArBadFeb> > m_dummy3;
std::vector<std::pair<HWIdentifier32,LArBadFeb> > m_dummy33;
std::vector<std::pair<HWIdentifier32,LArBadChannel>> m_dummy44;

#endif
