/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoNodeAction.h"

GeoNameTag::GeoNameTag (const std::string &BaseName)
 : m_name (BaseName)
{
}


GeoNameTag::~GeoNameTag()
{
}

void GeoNameTag::exec (GeoNodeAction *action) const
{
  action->handleNameTag (this);
}
