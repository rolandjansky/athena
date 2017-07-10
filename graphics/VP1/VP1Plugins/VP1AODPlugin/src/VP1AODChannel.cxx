/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1AODPlugin/VP1AODChannel.h"
#include "VP1AODSystems/VP1AODSystem.h"
#include "VP1GuideLineSystems/VP1GuideLineSystem.h"
VP1AODChannel::VP1AODChannel()
  : IVP13DStandardChannelWidget(VP1CHANNELNAMEINPLUGIN(VP1AODChannel,"AOD"),
				"This channel displays various systems related to Analysis Objects (AOD).",
				"Edward Moyse <edward.moyse@cern.ch>") {}

void VP1AODChannel::init()
{
  VP1AODSystem * aodsys = new VP1AODSystem;
  addSystem(aodsys);
  VP1GuideLineSystem * guidelinesystem = new VP1GuideLineSystem;
  addSystem(guidelinesystem);
}

