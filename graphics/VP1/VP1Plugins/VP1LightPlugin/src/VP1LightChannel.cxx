/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
//                                                             //
//  Header file for class VP1LightChannel                      //
//                                                         	   //
//  Author: Sebastian Merkt <sebastian.andreas.merkt@cern.ch>  //
//                                                             //
//  Initial version: December 2017                             //
//                                                             //
/////////////////////////////////////////////////////////////////

#include "VP1LightPlugin/VP1LightChannel.h"
#include "VP1GeometrySystems/VP1GeometrySystem.h"
#include "VP1GuideLineSystems/VP1GuideLineSystem.h"
#include "VP1AODSystems/VP1AODSystem.h"
#include "VP1Base/VP1QtUtils.h"

VP1LightChannel::VP1LightChannel()
  : IVP13DStandardChannelWidget(VP1CHANNELNAMEINPLUGIN(VP1LightChannel,"VP1Light"),
				"This channel displays the geometry and Analysis Objects (AOD).",
				"Riccardo.Maria.Bianchi@cern.ch;Sebastian.Andreas.Merkt@cern.ch")
{
}

void VP1LightChannel::init()
{
  addSystem(new VP1GuideLineSystem);
  addSystem(new VP1GeometrySystem);
  addSystem(new VP1AODSystem);
  if (VP1QtUtils::environmentVariableIsOn("VP1_SECOND_GEOMSYS")){
    addSystem(new VP1GeometrySystem(VP1GeoFlags::None,"AltGeo"),IVP13DStandardChannelWidget::StartDisabled);
  }
}
