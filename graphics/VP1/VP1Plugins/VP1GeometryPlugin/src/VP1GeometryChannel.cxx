/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP1GeometryChannel             //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: May 2007                              //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1GeometryPlugin/VP1GeometryChannel.h"
#include "VP1GeometrySystems/VP1GeometrySystem.h"
#include "VP1GuideLineSystems/VP1GuideLineSystem.h"
#include "VP1Base/VP1QtUtils.h"

VP1GeometryChannel::VP1GeometryChannel()
  : IVP13DStandardChannelWidget(VP1CHANNELNAMEINPLUGIN(VP1GeometryChannel,"Geometry"),
				"This channel shows the geometry as defined in GeoModel.",
				"Thomas.Kittelmann@cern.ch;Riccardo.Maria.Bianchi@cern.ch")
{
}

void VP1GeometryChannel::init()
{
  addSystem(new VP1GuideLineSystem);
  addSystem(new VP1GeometrySystem);
  if (VP1QtUtils::environmentVariableIsOn("VP1_SECOND_GEOMSYS"))
    addSystem(new VP1GeometrySystem(VP1GeoFlags::None,"AltGeo"),IVP13DStandardChannelWidget::StartDisabled);
}
