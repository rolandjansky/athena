/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetReadoutGeometry/TRT_Numerology.h"
namespace InDetDD {

TRT_Numerology::TRT_Numerology() :
  m_nringBarrel(0),
  m_nphiBarrel(0),
  m_nwheelEndcap(0),
  m_nphiEndcap(0)
{
  // Zero out the arrays
  for (int i = 0; i < 3; i++) {
    m_nlayerBarrel[i] = 0;
  }
  for (int j = 0; j < 18; j++) {
    m_nlayerEndcap[j] = 0;
  }    
}

TRT_Numerology::~TRT_Numerology() {
}

void TRT_Numerology::setNBarrelLayers(unsigned int module, unsigned int nLayers) {
  m_nlayerBarrel[module]=nLayers;
}

void TRT_Numerology::setNEndcapLayers(unsigned int wheel, unsigned int nLayers) {
  m_nlayerEndcap[wheel]=nLayers;
}

void TRT_Numerology::setNBarrelRings(unsigned int ring) {
  m_nringBarrel = ring;
}

void TRT_Numerology::setNBarrelPhi(unsigned int phi) {
  m_nphiBarrel = phi;
}

void TRT_Numerology::setNEndcapWheels(unsigned int wheel) {
  m_nwheelEndcap = wheel;
}

void TRT_Numerology::setNEndcapPhi(unsigned int phi) {
  m_nphiEndcap = phi;
}
}

