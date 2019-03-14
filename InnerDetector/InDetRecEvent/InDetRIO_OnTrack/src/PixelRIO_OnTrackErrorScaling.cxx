/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "InDetRIO_OnTrack/PixelRIO_OnTrackErrorScaling.h"
#include "InDetIdentifier/PixelID.h"

const char* const PixelRIO_OnTrackErrorScaling::s_names[PixelRIO_OnTrackErrorScaling::kNParamTypes]={
    "PixPhi Barrel",
    "PixEta Barrel",
    "PixPhi Endcap",
    "PixEta Endcap",
    "PixPhi IBL",
    "PixEta IBL"
  };

namespace {
  inline double square(double a) { return a*a; }

  void scale2by2( Amg::MatrixX& cov,
                  const std::vector<double>& phiTrafo,
                  const std::vector<double>& etaTrafo)
  {
    // careful not to change character of error
    double corr = (std::abs(cov(0,1)) > 1.0e-20)
      ? cov(0,1)/(cov(0,0)*cov(1,1))
      : 0.0;
    cov(0,0) *= square(phiTrafo[0]);
    cov(0,0) += square(phiTrafo[1]);
    cov(1,1) *= square(etaTrafo[0]);
    cov(1,1) += square(etaTrafo[1]);
    if (corr !=0.0) {
      cov(0,1) = corr*cov(0,0)*cov(1,1);
      cov(1,0) = cov(0,1);
    }
  }
}

CLID PixelRIO_OnTrackErrorScaling::clid() const {
  return ClassID_traits<CondCont<PixelRIO_OnTrackErrorScaling> >::ID();
}

bool PixelRIO_OnTrackErrorScaling::postProcess() {
  if (params().size() != kPixIBLPhi && params().size() != kNParamTypes ) {
      std::stringstream message;
      message << " PixelRIO_OnTrackErrorScaling: Expecting parameters for";
      {
        unsigned int idx=0;
        for (; idx < kPixIBLPhi; ++idx) {
          message << " " << s_names[idx];
        }
        message << "(" << kPixIBLPhi << ")" << " and ";
        for (; idx < kNParamTypes; ++idx) {
          message << " " << s_names[idx];
        }
        message << "(" << kNParamTypes << ") for run2.";
      }
      message << " But got " << params().size() << ".";
      throw std::runtime_error( message.str() );
  }
  for (unsigned int idx=0; idx <params().size(); ++idx) {
    if (params()[idx].size()!=2 ) {
      assert( idx < kNParamTypes);
      std::stringstream message;
      message << " PixelRIO_OnTrackErrorScaling: Expected 2 parameters for "  << s_names[idx] << " but got " << params()[idx].size()
              << ".";
      throw std::runtime_error( message.str() );
    }
  }
  return true;
}

Amg::MatrixX PixelRIO_OnTrackErrorScaling::getScaledCovariance(const Amg::MatrixX& cov_input,
                                                               const PixelID &pixel_id,
                                                               const Identifier& id) const
{

  Amg::MatrixX newCov(cov_input);
  assert( &pixel_id );

  // from SiDetectorElement::isEndcap
  bool is_endcap = !(pixel_id.is_barrel(id) || pixel_id.is_dbm(id));
  unsigned int idx=kNParamTypes;

  if (is_endcap) {
    idx=kPixEndcapPhi;
  } else {

    // from  PixelDigitizationTool::getReadoutTech
    int barrel_ec = pixel_id.barrel_ec(id);
    bool is_ibl = abs(barrel_ec)==4 || pixel_id.eta_module_max(id)>6;
    if (is_ibl) {
      idx = kPixIBLPhi;
    }
    else {
      idx = kPixBarrelPhi;
    }
  }

  // checked in postprocess
  assert( params().size()>idx+1);
  assert(   params()[idx].size()>1
         && params()[idx+1].size()>1 );

  scale2by2(newCov,params()[idx] /* phi */ ,params()[idx+1] /* eta */);
  // std::cout << "DEBUG createScaledPixelCovariance region:" << s_names[idx] << " " << cov_input << " -> " << newCov << std::endl;
  return newCov;
}

