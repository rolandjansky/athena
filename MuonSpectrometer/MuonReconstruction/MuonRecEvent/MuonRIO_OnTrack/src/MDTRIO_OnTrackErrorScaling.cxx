/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "MuonRIO_OnTrack/MDTRIO_OnTrackErrorScaling.h"
#include <iostream>

const char* const MDTRIO_OnTrackErrorScaling::s_names[MDTRIO_OnTrackErrorScaling::kNParamTypes]={
    "MDT Barrel",
    "MDT Endcap"
  };

namespace {
  inline double square(double a) { return a*a; }
}

CLID MDTRIO_OnTrackErrorScaling::clid() const {
  return ClassID_traits<CondCont<MDTRIO_OnTrackErrorScaling> >::ID();
}

bool MDTRIO_OnTrackErrorScaling::postProcess() {
  checkParameters("MDTRIO_OnTrackErrorScaling", kNParamTypes, s_names, 2);
  return true;
}

Amg::MatrixX MDTRIO_OnTrackErrorScaling::getScaledCovariance(const Amg::MatrixX& cov_input,
                                                             bool is_endcap) const
{
  Amg::MatrixX newCov(cov_input);
  double a = (is_endcap) ? params()[kEndcap][0] : params()[kBarrel][0];
  double b = (is_endcap) ? params()[kEndcap][1] : params()[kBarrel][1];
  newCov(0,0) *= square(a);
  newCov(0,0) += square(b);
  return newCov;
}


