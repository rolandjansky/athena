/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "MuonRIO_OnTrack/MuonEtaPhiRIO_OnTrackErrorScaling.h"
#include <iostream>

const char* const MuonEtaPhiRIO_OnTrackErrorScaling::s_names[MuonEtaPhiRIO_OnTrackErrorScaling::kNParamTypes]={
    "Phi",
    "Eta"
  };

namespace {
  inline double square(double a) { return a*a; }
}

CLID MuonEtaPhiRIO_OnTrackErrorScaling::clid() const {
  return ClassID_traits<CondCont<MuonEtaPhiRIO_OnTrackErrorScaling> >::ID();
}

bool MuonEtaPhiRIO_OnTrackErrorScaling::postProcess() {
  checkParameters("MuonEtaPhiRIO_OnTrackErrorScaling", kNParamTypes, s_names, 2);
  return true;
}

Amg::MatrixX MuonEtaPhiRIO_OnTrackErrorScaling::getScaledCovariance(const Amg::MatrixX& cov_input,
                                                                    const Trk::ParamDefs measuredCoord) const
{
  Amg::MatrixX newCov(cov_input);
  double a,b = 0.0;
  if (measuredCoord == Trk::distPhi) {
    a = params()[kPhi][0];
    b = params()[kPhi][1];
  } else if (measuredCoord == Trk::distEta) {
    a = params()[kEta][0];
    b = params()[kEta][1];
  } else {
    throw std::runtime_error("Invalid measured coordinate. Only Trk::distPhi and Trk::distEta are supported.");
  }
  newCov(0,0) *= square(a);
  newCov(0,0) += square(b);
  return newCov;
}


