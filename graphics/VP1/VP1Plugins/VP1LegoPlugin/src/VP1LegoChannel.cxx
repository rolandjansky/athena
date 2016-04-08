/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1LegoPlugin/VP1LegoChannel.h"
#include "VP1LegoSystems/VP1CaloLegoSystem.h"
#include "VP1LegoSystems/VP1CaloHitLegoSystem.h"
#include "VP1LegoSystems/VP1HVLegoSystem.h"

VP1LegoChannel::VP1LegoChannel()
  : IVP13DStandardChannelWidget(VP1CHANNELNAMEINPLUGIN(VP1LegoChannel,"Lego"),
                                "Lego Plot Display of the ATLAS Detector",
                                "Joe Boudreau", false/*disable detector buttons*/)
{
}

void VP1LegoChannel::init()
{
  addSystem(new VP1CaloLegoSystem);
  addSystem(new VP1CaloHitLegoSystem);
  addSystem(new VP1HVLegoSystem,IVP13DStandardChannelWidget::StartDisabled);
}

