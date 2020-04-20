/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AnalysisTest/ReadTrackParticle.h"

StatusCode ReadTrackParticle::userInit()
{
  return m_ntuplePtr -> addItem (m_prefix+"/charge", m_nCon,  m_charge);
}

StatusCode ReadTrackParticle::userExec(const Rec::TrackParticle *val)
{
  m_charge [m_nCon] = val->charge();

  return StatusCode::SUCCESS;
}
