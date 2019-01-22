/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "RecTPCnv/MuonSpShowerContainer_p1.h"
#include <cstddef>

MuonSpShowerContainer_p1::MuonSpShowerContainer_p1()
  : m_ownshowers (false)
{
}

MuonSpShowerContainer_p1::MuonSpShowerContainer_p1(const MuonSpShowerContainer_p1& other)
  : m_showers (other.m_showers),
    m_ownshowers (false)
{
}

MuonSpShowerContainer_p1&
MuonSpShowerContainer_p1::operator=(const MuonSpShowerContainer_p1& other)
{
  if (this != &other) {
    m_showers = other.m_showers;
    m_ownshowers = false;
  }
  return *this;
}

MuonSpShowerContainer_p1::~MuonSpShowerContainer_p1()
{
  if (m_ownshowers) {
    for (size_t i = 0; i < m_showers.size(); i++)
      delete m_showers[i];
  }
}
