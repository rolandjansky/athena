/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/VDetectorNode.h"

VDetectorNode::VDetectorNode()
{
}

VDetectorNode::~VDetectorNode()
{
}

const GeoVDetectorManager * VDetectorNode::getDetectorManager() const
{
  return this;
}



