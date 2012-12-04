/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AnalysisTest/ReadVxContainer.h"

StatusCode ReadVxContainer::userInit()
{
  m_ntuplePtr -> addItem (m_prefix+"/chi2", m_nCon,  m_chi2);
  return StatusCode::SUCCESS;
}

StatusCode ReadVxContainer::userExec(const Trk::VxCandidate *val)
{
  m_chi2 [m_nCon] = (val->recVertex()).fitQuality().chiSquared();

  return StatusCode::SUCCESS;
}
