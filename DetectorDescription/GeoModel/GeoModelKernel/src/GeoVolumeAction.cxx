/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoVolumeAction.h"

GeoVolumeAction::GeoVolumeAction (Type type)
  : m_type(type)
  , m_terminate(false)
{
}

GeoVolumeAction::~GeoVolumeAction()
{
}

void GeoVolumeAction::handleVPhysVol (const GeoVPhysVol *)
{
}

void GeoVolumeAction::terminate ()
{
  m_terminate = true;
}

bool GeoVolumeAction::shouldTerminate () const
{
  return m_terminate;
}

const GeoTraversalState* GeoVolumeAction::getState () const
{
  return &m_traversalState;
}

GeoTraversalState* GeoVolumeAction::getState ()
{
  return &m_traversalState;
}

GeoVolumeAction::Type GeoVolumeAction::getType () const
{
  return m_type;
}
