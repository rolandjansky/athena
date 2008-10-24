/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class Example3DChannel3              //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: June 2007                             //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1TestPlugin/Example3DChannel3.h"
#include "VP1TestSystems/Example3DSystem3.h"
#include "VP1GeometrySystems/VP1GeometrySystem.h"

Example3DChannel3::Example3DChannel3()
  : IVP13DStandardChannelWidget(VP1CHANNELNAMEINPLUGIN(Example3DChannel3,"Example 3"),
				"This channel is an example of a 3D"
				" channel which displays the 3D scene"
				" delivered by the Example3DSystem3 (along with geometry).",
				"Thomas.Kittelmann@cern.ch")
{
}

void Example3DChannel3::init()
{
  addSystem(new Example3DSystem3);
  addSystem(new VP1GeometrySystem(VP1GeoFlags::BeamPipe));
}
