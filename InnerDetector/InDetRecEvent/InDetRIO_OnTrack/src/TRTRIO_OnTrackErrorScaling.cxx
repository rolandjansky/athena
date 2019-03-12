/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "InDetRIO_OnTrack/TRTRIO_OnTrackErrorScaling.h"
#include <iostream>

const char* const TRTRIO_OnTrackErrorScaling::s_names[TRTRIO_OnTrackErrorScaling::kNParamTypes]={
    "TRT Barrel",
    "TRT Endcap"
  };

namespace {
  inline double square(double a) { return a*a; }
}

CLID TRTRIO_OnTrackErrorScaling::clid() const {
  return ClassID_traits<CondCont<TRTRIO_OnTrackErrorScaling> >::ID();
}

bool TRTRIO_OnTrackErrorScaling::postProcess() {
  if (params().size() != kNParamTypes ) {
      std::stringstream message;
      message << " TRTRIO_OnTrackErrorScaling: Expecting parameters for et least 2 parameters for";
      for (unsigned int idx=0; idx < kNParamTypes; ++idx) {
        message << " " << s_names[idx];
      }
      message << "(" << kNParamTypes << ") for run2.";
      message << " But got " << params().size() << ".";
      throw std::runtime_error( message.str() );
  }
  for (unsigned int idx=0; idx<params().size(); ++idx) {
    if (params()[idx].size() != 3) {
      if (params()[idx].size() == 2) {
        params()[idx].push_back(0.);
      }
      else {
        assert( idx < kNParamTypes );
        std::stringstream message;
        message << " TRTRIO_OnTrackErrorScaling: Expected 2 or 3 parameters for "  << s_names[idx] << " but got " << params()[idx].size()
                << ".";
        throw std::runtime_error( message.str() );
      }
    }
  }
  return true;
}

Amg::MatrixX TRTRIO_OnTrackErrorScaling::getScaledCovariance(const Amg::MatrixX& cov_input,
                                                             bool is_endcap,
                                                             double mu) const
{
  Amg::MatrixX newCov(cov_input);
  double a = (is_endcap) ? params()[kEndcap][0] : params()[kBarrel][0];
  double b = (is_endcap) ? params()[kEndcap][1] : params()[kBarrel][1];
  double c = (is_endcap) ? params()[kEndcap][2] : params()[kBarrel][2];
  newCov(0,0) *= square(a);
  newCov(0,0) += square(b);
  newCov(0,0) *= (1. + mu * c);
  // std::cout << "DEBUG createScaledTrtCovariance endcap:" << is_endcap << " mu=" << mu  << " " << cov_input << " -> " << newCov << std::endl;
  return newCov;
}


