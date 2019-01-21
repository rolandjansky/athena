/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelEnvelopes/ForDetEnvelopeManager.h"

ForDetEnvelopeManager::ForDetEnvelopeManager()
{
  setName("ForDetEnvelope");
}


ForDetEnvelopeManager::~ForDetEnvelopeManager()
{
  for (unsigned int i = 0; i < m_volume.size(); i++) {
    m_volume[i]->unref();
  }
}


unsigned int ForDetEnvelopeManager::getNumTreeTops() const
{
  return m_volume.size();
}

PVConstLink ForDetEnvelopeManager::getTreeTop(unsigned int i) const
{
  return m_volume[i];
}

void  ForDetEnvelopeManager::addTreeTop(PVLink vol)
{
  vol->ref();
  m_volume.push_back(vol);
}



