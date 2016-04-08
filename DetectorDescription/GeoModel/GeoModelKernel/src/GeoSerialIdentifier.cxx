/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoNodeAction.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"

GeoSerialIdentifier::GeoSerialIdentifier(int BaseId)
:
m_baseId (BaseId)
{
}


GeoSerialIdentifier::~GeoSerialIdentifier()
{
}



void GeoSerialIdentifier::exec(GeoNodeAction *action) const
{
  action->handleSerialIdentifier (this);
}





