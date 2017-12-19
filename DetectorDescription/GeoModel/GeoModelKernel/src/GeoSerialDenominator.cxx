/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoSerialDenominator.h"
#include "GeoModelKernel/GeoNodeAction.h"

GeoSerialDenominator::GeoSerialDenominator (const std::string &BaseName)
  : m_baseName (BaseName)
{
}

GeoSerialDenominator::~GeoSerialDenominator()
{
}

void GeoSerialDenominator::exec (GeoNodeAction *action) const
{
  action->handleSerialDenominator (this);
}
