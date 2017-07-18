/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetReadoutGeometry/TRT_Numerology.h"
namespace InDetDD {

TRT_Numerology::TRT_Numerology() :
  _nringBarrel(0),
  _nphiBarrel(0),
  _nwheelEndcap(0),
  _nphiEndcap(0)
{
  // Zero out the arrays
  for (int i = 0; i < 3; i++) {
    _nlayerBarrel[i] = 0;
  }
  for (int j = 0; j < 18; j++) {
    _nlayerEndcap[j] = 0;
  }    
}

TRT_Numerology::~TRT_Numerology() {
}

void TRT_Numerology::setNBarrelLayers(unsigned int module, unsigned int nLayers) {
  _nlayerBarrel[module]=nLayers;
}

void TRT_Numerology::setNEndcapLayers(unsigned int wheel, unsigned int nLayers) {
  _nlayerEndcap[wheel]=nLayers;
}

void TRT_Numerology::setNBarrelRings(unsigned int ring) {
  _nringBarrel = ring;
}

void TRT_Numerology::setNBarrelPhi(unsigned int phi) {
  _nphiBarrel = phi;
}

void TRT_Numerology::setNEndcapWheels(unsigned int wheel) {
  _nwheelEndcap = wheel;
}

void TRT_Numerology::setNEndcapPhi(unsigned int phi) {
  _nphiEndcap = phi;
}
}

