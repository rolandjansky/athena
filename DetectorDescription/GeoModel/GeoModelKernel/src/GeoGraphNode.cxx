/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoGraphNode.h"
#include "GeoModelKernel/GeoNodeAction.h"

GeoGraphNode::GeoGraphNode ()
{
}

GeoGraphNode::~GeoGraphNode()
{
}

void GeoGraphNode::exec (GeoNodeAction *action) const
{
  action->handleNode (this);
}

void GeoGraphNode::dockTo (GeoVPhysVol* )
{
}
