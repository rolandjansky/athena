/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEvent/RingerRings.h"

RingerRings::RingerRings()
  : m_rings() {
}

RingerRings::RingerRings(const std::vector<float> &rings)
  : m_rings(rings) {
}

RingerRings::RingerRings(const unsigned int nRings, const float *rings) {
  if (!rings)
    return;
  for (unsigned int i = 0; i < nRings; ++i) {
    m_rings.push_back(rings[i]);
  }
}

std::vector<float> &RingerRings::rings() {
  return m_rings;
}

const std::vector<float> &RingerRings::rings() const {
  return m_rings;
}

void RingerRings::rings(const std::vector<float> &r) {
  m_rings = r;
}

float &RingerRings::at(const unsigned int i) {
  return m_rings[i];
}

const float &RingerRings::at(const unsigned int i) const {
  return m_rings[i];
}

unsigned int RingerRings::size() const {
  return m_rings.size();
}

const float &RingerRings::operator [](const unsigned int i) const {
  return m_rings[i];
}

float &RingerRings::operator [](const unsigned int i) {
  return m_rings[i];
}

void RingerRings::print(MsgStream &log) const {
  log << MSG::DEBUG << "Dumping RingerRings information for debugging. There are " << m_rings.size()  << " rings." << endreq;
  log << MSG::DEBUG << "Ringer values: ";
  for (std::vector<float>::const_iterator i = m_rings.begin(); i != m_rings.end(); ++i) {
    log << (*i) << "   ";
  }
  log << endreq;
}

