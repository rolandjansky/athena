/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoNodeAction.h"

GeoIdentifierTag::GeoIdentifierTag (int id)
  : m_identifier(id)
{
}

GeoIdentifierTag::~GeoIdentifierTag()
{
}

void GeoIdentifierTag::exec (GeoNodeAction *action) const
{
  action->handleIdentifierTag (this);
}
