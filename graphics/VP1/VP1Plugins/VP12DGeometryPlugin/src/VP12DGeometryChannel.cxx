/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP12DGeometryChannel           //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: June 2007                             //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP12DGeometryPlugin/VP12DGeometryChannel.h"
#include "VP12DGeometrySystems/VP12DGeometrySystem.h"

VP12DGeometryChannel::VP12DGeometryChannel()
  : IVP12DDetViewsChannelWidget(VP1CHANNELNAMEINPLUGIN(VP12DGeometryChannel,"2D Geometry"),
				"This channel shows a stylized 2D geometry based GeoModel information.",
				"Thomas.Kittelmann@cern.ch")
{
}

void VP12DGeometryChannel::init()
{
  addSystem(new VP12DGeometrySystem, 1);
}
