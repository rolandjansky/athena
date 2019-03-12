/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "InDetRIO_OnTrack/SCTRIO_OnTrackErrorScaling.h"
#include <iostream>

const char * const SCTRIO_OnTrackErrorScaling::s_names[SCTRIO_OnTrackErrorScaling::kNParamTypes]={
    "SCT Barrel",
    "SCT Endcap"
};

namespace {
  inline double square(double a) { return a*a; }
}

CLID SCTRIO_OnTrackErrorScaling::clid() const {
  return ClassID_traits<CondCont<SCTRIO_OnTrackErrorScaling> >::ID();
}

bool SCTRIO_OnTrackErrorScaling::postProcess() {
  checkParameters("SCTRIO_OnTrackErrorScaling", kNParamTypes, s_names, 2);
  return true;
}

Amg::MatrixX SCTRIO_OnTrackErrorScaling::getScaledCovariance(const Amg::MatrixX& cov_input,
                                                             bool is_endcap,
                                                             double sinLocalAngle) const
{
  Amg::MatrixX newCov(cov_input);
  if (is_endcap && newCov.rows() > 1) {
    double Sn      = sinLocalAngle;
    double Sn2     = square(Sn);
    double Cs2     = (1.-Sn)*(1.+Sn);
    double SC      = Sn*sqrt(Cs2);
    double a       = params()[kEndcap][0];
    double b       = params()[kEndcap][1];
    double dV0     = (Cs2*newCov(0,0)+Sn2*newCov(1,1)
                      +2.*SC*newCov(1,0))*(square(a)-1.) + square(b);
    newCov(0,0)+= (Cs2*dV0);
    newCov(1,0)+= (SC *dV0);
    newCov(0,1) = newCov(1,0);
    newCov(1,1)+= (Sn2*dV0);
  } else {
    double a = (is_endcap) ? params()[kEndcap][0] : params()[kBarrel][0];
    double b = (is_endcap) ? params()[kEndcap][1] : params()[kBarrel][1];
    newCov(0,0) *= square(a);
    newCov(0,0) += square(b);
  }
  //std::cout << "DEBUG createScaledSctCovariance endcap:" << is_endcap << " angle=" << sinLocalAngle  << " " << cov_input << " -> " << newCov << std::endl;
  return newCov;
}
