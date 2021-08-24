/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrackSummary/InDetTrackSummary.h"

#include "GaudiKernel/MsgStream.h"
#include <iostream>

Trk::InDetTrackSummary::InDetTrackSummary()
  : m_likelihoodspixeldedx(3, -1)
  , m_massdedx(-1)
{}


std::ostream&
Trk::operator<<(std::ostream& out, const InDetTrackSummary& trackSum)
{
  out << "InDetTrackSummary: likelihoods from pixel dEdx (pion/kaon/proton): "
      << trackSum.likelihoodsPixeldEdx()[0] << " "
      << trackSum.likelihoodsPixeldEdx()[1] << " "
      << trackSum.likelihoodsPixeldEdx()[2] << std::endl;
  out << "InDetTrackSummary: mass calculated using pixel dEdx: "
      << trackSum.massPixeldEdx() << std::endl;
  return out;
}

MsgStream&
Trk::operator<<(MsgStream& out, const InDetTrackSummary& trackSum)
{
  out << "InDetTrackSummary: likelihoods from pixel dEdx (pion/kaon/proton): "
      << trackSum.likelihoodsPixeldEdx()[0] << " "
      << trackSum.likelihoodsPixeldEdx()[1] << " "
      << trackSum.likelihoodsPixeldEdx()[2] << endmsg;
  out << "InDetTrackSummary: mass calculated using pixel dEdx: "
      << trackSum.massPixeldEdx() << endmsg;
  return out;
}
