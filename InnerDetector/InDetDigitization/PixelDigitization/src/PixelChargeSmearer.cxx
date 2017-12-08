/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelChargeSmearer.cxx
//   Implementation file for class PixelChargeSmearer
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "PixelChargeSmearer.h"
#include "SiDigitization/SiChargedDiodeCollection.h"

PixelChargeSmearer::PixelChargeSmearer(const std::string& type, const std::string& name,const IInterface* parent):
  PixelProcessorTool(type,name,parent),
  m_amplitude(0)
{  
	declareProperty("ThermalNoise",m_amplitude,"Thermal noise amplitude");
}

PixelChargeSmearer::~PixelChargeSmearer() {}

StatusCode PixelChargeSmearer::initialize() {
  CHECK(PixelProcessorTool::initialize());
  ATH_MSG_DEBUG("PixelChargeSmearer::initialize()");
	return StatusCode::SUCCESS;
}

StatusCode PixelChargeSmearer::finalize() {
  ATH_MSG_DEBUG("PixelChargeSmearer::finalize()");
  return StatusCode::SUCCESS;
}

void PixelChargeSmearer::process(SiChargedDiodeCollection &collection) {

  // if the noise amplitude if 0, do nothing
  if (0==m_amplitude) return;

  // loop on all charged diodes
  for (SiChargedDiodeIterator i_chargedDiode=collection.begin(); i_chargedDiode!=collection.end(); ++i_chargedDiode) {

    // create noise charge
    SiCharge charge(m_amplitude*CLHEP::RandGaussZiggurat::shoot(m_rndmEngine),0,SiCharge::noise);

    // add it to the charged diode
    (*i_chargedDiode).second.add(charge);
  }
}
