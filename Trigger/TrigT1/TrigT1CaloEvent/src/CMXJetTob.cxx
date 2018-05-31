/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/




#ifndef  TRIGGERSPACE
#include "TrigT1CaloEvent/CMXJetTob.h"
#else
#include "CMXJetTob.h"
#endif

namespace LVL1 {

CMXJetTob::CMXJetTob():
  m_crate(0),
  m_jem(0),
  m_frame(0),
  m_location(0),
  m_peak(0),
  m_energyLarge(1),
  m_energySmall(1),
  m_error(1),
  m_presenceMap(1)
{
}

CMXJetTob::~CMXJetTob(){
}

/** constructs a CMXJetTob object, specifying crate, jem etc. */
CMXJetTob::CMXJetTob(int crate, int jem, int frame, int loc):
  m_crate(crate),
  m_jem(jem),
  m_frame(frame),
  m_location(loc),
  m_peak(0),
  m_energyLarge(1),
  m_energySmall(1),
  m_error(1),
  m_presenceMap(1)
{
}

/** constructs a CMXJetTob object and fill all data members */
CMXJetTob::CMXJetTob(int crate, int jem, int frame, int loc,
                     const std::vector<int>& energyLarge,
                     const std::vector<int>& energySmall,
		     const std::vector<int>& error,
		     const std::vector<unsigned int>& presenceMap,
		     int peak):
  m_crate(crate),
  m_jem(jem),
  m_frame(frame),
  m_location(loc),
  m_peak(peak),
  m_energyLarge(energyLarge),
  m_energySmall(energySmall),
  m_error(error),
  m_presenceMap(presenceMap)
{
}

} // end of namespace bracket
