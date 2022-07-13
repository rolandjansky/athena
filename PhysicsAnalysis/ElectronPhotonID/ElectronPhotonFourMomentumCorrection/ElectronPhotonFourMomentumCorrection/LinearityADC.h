/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ELECTRONPHOTONFOURMOMENTUMCORRECTION_LINEARITYADC_H
#define ELECTRONPHOTONFOURMOMENTUMCORRECTION_LINEARITYADC_H
#include <PATCore/PATCoreEnums.h>
#include <AsgTools/AsgMessaging.h>

#include <string>
#include <map>
#include <memory>

#include <TProfile2D.h>

namespace asg {
  class AsgMessaging;
}

/**
  @class LinearityADC
  @brief parameterization of correction for ADC non linearity

  deltaADC vs ADC parameterized from calibration runs in different eta bins for HG/MG/HG by Luca
  applied to single particle samples to parameterize vs eta and Et and particle type
*/

class LinearityADC  : public asg::AsgMessaging {

 public:
   /** @brief constructor (initialization done there reading a root file for the HV maps per period */
  LinearityADC(const std::string& filename, const std::string& name = "ADCNonLinearityCorrection");
  ~LinearityADC() {};

  /** @brief get relative correction factor to apply to energy: corrected energy should be original energy * correction factor */
  /** @brief inputs: etaCalo and Et in MeV, pType (enum) */
  float getCorr(float etaCalo, float et, PATCore::ParticleType::Type ptype = PATCore::ParticleType::Electron) const;

 private:
  std::map<PATCore::ParticleType::Type,std::unique_ptr<TProfile2D>> m_hcorr;

};
#endif
