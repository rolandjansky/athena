/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RecTPCnv/MuonSpShowerContainer_p1.h"
#include <cstddef>

MuonSpShowerContainer_p1::MuonSpShowerContainer_p1()
  : m_ownshowers (false)
{
}

MuonSpShowerContainer_p1::~MuonSpShowerContainer_p1()
{
  if (m_ownshowers) {
    for (size_t i = 0; i < m_showers.size(); i++)
      delete m_showers[i];
  }
}
