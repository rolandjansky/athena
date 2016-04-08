/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Thresholds.cxx 667905 2015-05-18 19:07:55Z wsfreund $
#include "RingerSelectorTools/procedures/Thresholds.icc"

namespace Ringer {

namespace Discrimination {

// =============================================================================
void UniqueThreshold::print(MSG::Level lvl) const
{
  if ( !this->isStreamAvailable() ) {
    std::cerr << "Cannot print " << this->name() << ", stream unavailable"
      << std::endl;
  }
  if ( this->level() > lvl ) {
    return;
  }
  msg() << lvl << "Threshold: " << m_threshold << endreq;
}

// =============================================================================
void UniqueThreshold::read(UniqueThreshold *newObj, 
    TDirectory *configDir, 
    unsigned /*version*/ )
{
  IOHelperFcns::readVar(configDir, "threshold", newObj->m_threshold); 
}

// =============================================================================
void UniqueThreshold::write(TDirectory *configDir, const char *) const
{
  IOHelperFcns::writeVar(configDir, "threshold",    m_threshold );
}

} // namespace Discrimination

} // namespace Ringer
