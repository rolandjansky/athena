/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Thresholds.cxx 770805 2016-08-30 14:03:33Z ssnyder $
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
  msg() << lvl << "Threshold: " << m_threshold << endmsg;
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
