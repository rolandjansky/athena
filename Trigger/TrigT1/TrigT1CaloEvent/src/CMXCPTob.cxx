/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/




#ifndef  TRIGGERSPACE
#include "TrigT1CaloEvent/CMXCPTob.h"
#else
#include "CMXCPTob.h"
#endif

namespace LVL1 {

CMXCPTob::CMXCPTob():
  m_crate(0),
  m_cmx(0),
  m_cpm(0),
  m_chip(0),
  m_location(0),
  m_peak(0),
  m_energy(1),
  m_isolation(1),
  m_error(1),
  m_presenceMap(1)
{
}

CMXCPTob::~CMXCPTob(){
}

/** constructs a CMXCPTob object, specifying crate, cmx, cpm etc. */
CMXCPTob::CMXCPTob(int crate, int cmx, int cpm, int chip, int loc):
  m_crate(crate),
  m_cmx(cmx),
  m_cpm(cpm),
  m_chip(chip),
  m_location(loc),
  m_peak(0),
  m_energy(1),
  m_isolation(1),
  m_error(1),
  m_presenceMap(1)
{
}

/** constructs a CMXCPTob object and fill all data members */
CMXCPTob::CMXCPTob(int crate, int cmx, int cpm, int chip, int loc,
                   const std::vector<int>& energy,
                   const std::vector<int>& isolation,
		   const std::vector<int>& error,
		   const std::vector<unsigned int>& presenceMap,
		   int peak):
  m_crate(crate),
  m_cmx(cmx),
  m_cpm(cpm),
  m_chip(chip),
  m_location(loc),
  m_peak(peak),
  m_energy(energy),
  m_isolation(isolation),
  m_error(error),
  m_presenceMap(presenceMap)
{
}

} // end of namespace bracket
