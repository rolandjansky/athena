/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class Example3DChannel2              //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: June 2007                             //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1TestPlugin/Example3DChannel2.h"
#include "VP1TestSystems/Example3DSystem2.h"
#include "VP1GeometrySystems/VP1GeometrySystem.h"

Example3DChannel2::Example3DChannel2()
  : IVP13DStandardChannelWidget(VP1CHANNELNAMEINPLUGIN(Example3DChannel2,"Example 2"),
				"This channel is an example of a"
				" 3D channel which displays the"
				" 3D scene delivered by the"
				" Example3DSystem2 (along with geometry).",
				"Thomas.Kittelmann@cern.ch")
{
}

void Example3DChannel2::init()
{
  addSystem(new Example3DSystem2);
  addSystem(new VP1GeometrySystem(VP1GeoFlags::BeamPipe));
}
