/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// PixelRandomDisabledCellGenerator.cxx 
//   Implementation file for class PixelRandomDisabledCellGenerator 
/////////////////////////////////////////////////////////////////// 
// (c) ATLAS Detector software 
/////////////////////////////////////////////////////////////////// 

#include "PixelRandomDisabledCellGenerator.h"
#include "SiDigitization/SiHelper.h"

#include "CLHEP/Random/RandFlat.h"

PixelRandomDisabledCellGenerator::PixelRandomDisabledCellGenerator(const std::string& type, const std::string& name,const IInterface* parent):
  PixelProcessorTool(type,name,parent),
  m_disableProbability(9e-3)
{
  declareProperty("TotalBadChannels",m_disableProbability);
}

PixelRandomDisabledCellGenerator::~PixelRandomDisabledCellGenerator() {}

StatusCode PixelRandomDisabledCellGenerator::initialize() {
  CHECK(PixelProcessorTool::initialize());

  ATH_MSG_INFO("\tCreating disabled cell generator with "<<m_disableProbability<<" probability");

  ATH_MSG_DEBUG("PixelRandomDisabledCellGenerator::initialize()");
  return StatusCode::SUCCESS;
}

StatusCode PixelRandomDisabledCellGenerator::finalize() {
  ATH_MSG_DEBUG ("PixelRandomDisabledCellGenerator::finalize()");
  return StatusCode::SUCCESS;
}

void PixelRandomDisabledCellGenerator::process(SiChargedDiodeCollection &collection) {   
  // disabling is applied to all cells even unconnected or below threshold
  // ones to be able to use these cells as well

  for(SiChargedDiodeIterator i_chargedDiode=collection.begin(); i_chargedDiode!=collection.end(); ++i_chargedDiode) {
    // disable the diode randomly
    if (CLHEP::RandFlat::shoot(m_rndmEngine)<m_disableProbability) {
      SiHelper::disabled((*i_chargedDiode).second,true,false);
    }
  }
}

