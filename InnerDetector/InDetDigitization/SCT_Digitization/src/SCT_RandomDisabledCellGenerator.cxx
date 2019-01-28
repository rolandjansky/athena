/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_RandomDisabledCellGenerator.h"

#include "SiDigitization/SiChargedDiodeCollection.h"

#include "SiDigitization/SiHelper.h"

#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandFlat.h"

// constructor
SCT_RandomDisabledCellGenerator::SCT_RandomDisabledCellGenerator(const std::string& type, const std::string& name, const IInterface* parent)
  : base_class(type, name, parent)
{
  declareProperty("TotalBadChannels", m_disableProbability=0.01);
}

StatusCode SCT_RandomDisabledCellGenerator::initialize() {
  ATH_MSG_DEBUG("SCT_RandomDisabledCellGenerator::initialize()");
  ATH_MSG_INFO("\tCreating missing bond generator with "<<m_disableProbability<<" probability");
  return StatusCode::SUCCESS;
}

StatusCode SCT_RandomDisabledCellGenerator::finalize() {
  ATH_MSG_INFO("SCT_RandomDisabledCellGenerator::finalize()");
  return StatusCode::SUCCESS;
}

// process the collection 
void SCT_RandomDisabledCellGenerator::process(SiChargedDiodeCollection& collection, CLHEP::HepRandomEngine * rndmEngine) const {
  // disabling is applied to all cells even unconnected or below threshold ones to be able to use these cells as well
  // loop on all charged diodes
  for (std::pair<const InDetDD::SiCellId, SiChargedDiode>& chargedDiode: collection) {
    if (CLHEP::RandFlat::shoot(rndmEngine)<m_disableProbability) {
      SiHelper::disconnected(chargedDiode.second, true, false);
    }
  }
}
