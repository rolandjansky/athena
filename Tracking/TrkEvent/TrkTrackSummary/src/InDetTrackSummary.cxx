/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrackSummary/InDetTrackSummary.h"

#include "GaudiKernel/MsgStream.h"
#include <iostream>

Trk::InDetTrackSummary::InDetTrackSummary()
  : m_likelihoodspixeldedx(3, -1)
  , m_massdedx(-1)
{}


std::ostream&
Trk::operator<<(std::ostream&out, const InDetTrackSummary&)
{
  return out;
}

MsgStream&
Trk::operator<<(MsgStream&out, const InDetTrackSummary&)
{
  return out;
}
