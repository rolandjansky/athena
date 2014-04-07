/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaEvent/CaloRings.h"

///////////////
/// RingSet ///
///////////////

float &RingSet::at(unsigned int i) 
{ 
    return m_rings.at(i); 
}


float &RingSet::operator [](unsigned int i) 
{ 
    return m_rings[i];
}

const float &RingSet::at(unsigned int i) const 
{ 
    return m_rings.at(i); 
}


const float &RingSet::operator [](unsigned int i) const 
{ 
    return m_rings[i]; 
}

size_t RingSet::size() const 
{ 
    return m_rings.size(); 
}


void RingSet::clearRings() 
{ 
    m_rings.clear(); 
}

void RingSet::addRing(float r) 
{ 
    m_rings.push_back(r); 
}


size_t RingSet::numberOfLayers() const 
{
    return m_layers.size(); 
}

CaloCell_ID::CaloSample &RingSet::layer(unsigned int i) 
{ 
    return m_layers[i]; 
}

const CaloCell_ID::CaloSample &RingSet::layer(unsigned int i) const 
{ 
    return m_layers[i]; 
}

void RingSet::clearLayers() 
{ 
    m_layers.clear(); 
}


void RingSet::addLayer(CaloCell_ID::CaloSample c) 
{ 
    m_layers.push_back(c); 
}

float &RingSet::deltaEta() 
{ 
    return m_deltaEta; 
}

float &RingSet::deltaPhi() 
{ 
    return m_deltaPhi; 
}

const float &RingSet::deltaEta() const 
{ 
    return m_deltaEta; 
}

const float &RingSet::deltaPhi() const 
{ 
    return m_deltaPhi; 
}


RingSet::const_iterator RingSet::begin() const
{
  return m_rings.begin();
}


RingSet::const_iterator RingSet::end() const
{
  return m_rings.end();
}


//////////////////
/// Calo Rings ///
//////////////////

CaloRings::CaloRings()
 : m_ringsets(0)
{
}

void CaloRings::addRingSet(unsigned int numberOfRings,
                    std::vector<CaloCell_ID::CaloSample> &layers,
                    float deltaEta, float deltaPhi) {

  RingSet r;
  r.m_rings.clear();
  r.m_layers = layers;
  r.m_deltaEta = deltaEta;
  r.m_deltaPhi = deltaPhi;

  for (unsigned int j = 0; j < numberOfRings; ++j) {
    r.m_rings.push_back(0);
  }
  m_ringsets.push_back(r);
}

void CaloRings::clear() 
{
    m_ringsets.clear();
}

size_t CaloRings::size() const 
{ 
    return m_ringsets.size(); 
}

std::vector<RingSet>::iterator CaloRings::begin() 
{ 
    return m_ringsets.begin(); 
}

std::vector<RingSet>::iterator CaloRings::end() 
{ 
    return m_ringsets.end(); 
}

std::vector<RingSet>::const_iterator CaloRings::begin() const 
{ 
    return m_ringsets.begin(); 
}

std::vector<RingSet>::const_iterator CaloRings::end() const 
{ 
    return m_ringsets.end(); 
}

RingSet &CaloRings::at(unsigned int i) {
  return m_ringsets.at(i);
}

RingSet &CaloRings::operator [](unsigned int i) {
  return m_ringsets[i];
}

const RingSet &CaloRings::at(unsigned int i) const {
  return m_ringsets.at(i);
}

const RingSet &CaloRings::operator [](unsigned int i) const {
  return m_ringsets[i];
}

float CaloRings::ringAt(unsigned int nRingSet, unsigned int nRing) {
   return m_ringsets.at(nRingSet).at(nRing);
}

