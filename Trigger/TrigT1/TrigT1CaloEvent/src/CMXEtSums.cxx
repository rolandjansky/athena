/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/




#ifndef  TRIGGERSPACE
#include "TrigT1CaloEvent/CMXEtSums.h"
#else
#include "CMXEtSums.h"
#endif

namespace LVL1 {

CMXEtSums::CMXEtSums():
  m_crate(0),
  m_source(0),
  m_peak(0),
  m_Et(1),
  m_Ex(1),
  m_Ey(1),
  m_EtError(1),
  m_ExError(1),
  m_EyError(1)
{
}

CMXEtSums::~CMXEtSums(){
}

/** constructs a CMXEtSums object, specifying crate, and data ID. */
CMXEtSums::CMXEtSums(int crate, int source):
  m_crate(crate),
  m_source(source),
  m_peak(0),
  m_Et(1),
  m_Ex(1),
  m_Ey(1),
  m_EtError(1),
  m_ExError(1),
  m_EyError(1)
{
}

/** constructs a CMXEtSums object and fill all data members */
CMXEtSums::CMXEtSums(int crate, int source,
                     const std::vector<unsigned int>& Et, 
		     const std::vector<unsigned int>& Ex, 
		     const std::vector<unsigned int>& Ey,
		     const std::vector<int>& EtError,
		     const std::vector<int>& ExError,
		     const std::vector<int>& EyError,
		     int peak):
  m_crate(crate),
  m_source(source),
  m_peak(peak),
  m_Et(Et),
  m_Ex(Ex),
  m_Ey(Ey),
  m_EtError(EtError),
  m_ExError(ExError),
  m_EyError(EyError)
{
}

} // end of namespace bracket
