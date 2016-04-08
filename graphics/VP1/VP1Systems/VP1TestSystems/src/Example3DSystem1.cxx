/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Implementation of class Example3DSystem1                           //
//                                                                     //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>              //
//                                                                     //
//  Initial version: June 2007                                         //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VP1TestSystems/Example3DSystem1.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCube.h>

//_____________________________________________________________________________________
Example3DSystem1::Example3DSystem1()
  : IVP13DSystemSimple("Ex3DSys1",
		       "This is an illustration of an extremely basic 3D system.\n "
		       "It simply builds a cube each event. Admittedly not very useful",
		       "Thomas.Kittelmann@cern.ch")
{
}

//_____________________________________________________________________________________
void Example3DSystem1::buildEventSceneGraph(StoreGateSvc* /*sg*/, SoSeparator *root)
{
  //Make a message appear in the VP1 messagebox:
  message("Building a (useless) 3D scenegraph consisting of a cube");

  //Potentially the worlds simplest and most useless scenegraph:
  root->addChild(new SoCube);
}
