/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_RandomDisabledCellGenerator.h"

#include "SiDigitization/SiHelper.h"

// random number service includes
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandFlat.h"

#include "SiDigitization/SiChargedDiodeCollection.h"

// constructor
SCT_RandomDisabledCellGenerator::SCT_RandomDisabledCellGenerator(const std::string& type, const std::string& name, const IInterface* parent )
  : AthAlgTool(type, name, parent),
    m_myfunc{0},
    m_rndmEngine{0}
{
  declareInterface<ISCT_RandomDisabledCellGenerator>(this);
  declareProperty("TotalBadChannels", m_disableProbability=0.01);
}

// Destructor:
SCT_RandomDisabledCellGenerator::~SCT_RandomDisabledCellGenerator(){
}

StatusCode SCT_RandomDisabledCellGenerator::initialize() {
  StatusCode sc{AthAlgTool::initialize()};
  if (sc.isFailure()) {
    ATH_MSG_INFO("SCT_RandomDisabledCellGenerator::initialize() failed") ;
    return sc ;
  }
  ATH_MSG_DEBUG("SCT_RandomDisabledCellGenerator::initialize()");

  m_myfunc = SiHelper::disconnected ; 

  ATH_MSG_INFO("\tCreating missing bond generator with "<<m_disableProbability<<" probability");

  return sc ;
}

StatusCode SCT_RandomDisabledCellGenerator::finalize() {
  StatusCode sc{AthAlgTool::finalize()};
  if (sc.isFailure()) {
    ATH_MSG_FATAL("SCT_RandomDisabledCellGenerator::finalize() failed");
    return sc;
  }
  ATH_MSG_INFO("SCT_RandomDisabledCellGenerator::finalize()");
  return sc;
}

// process the collection 
void SCT_RandomDisabledCellGenerator::process(SiChargedDiodeCollection& collection) const {
  // disabling is applied to all cells even unconnected or below threshold ones to be able to use these cells as well
  // loop on all charged diodes
  SiChargedDiodeIterator i_chargedDiode{collection.begin()};
  SiChargedDiodeIterator i_chargedDiode_end{collection.end()};
  for (; i_chargedDiode!=i_chargedDiode_end; ++i_chargedDiode) {
    if (CLHEP::RandFlat::shoot(m_rndmEngine)<m_disableProbability) {
      (*m_myfunc)((*i_chargedDiode).second, true, false);
    }
  }
}
