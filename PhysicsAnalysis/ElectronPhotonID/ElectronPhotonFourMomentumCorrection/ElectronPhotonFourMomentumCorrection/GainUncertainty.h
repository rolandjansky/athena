/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ELECTRONPHOTONFOURMOMENTUMCORRECTION_GAINUNCERTAINTY_H
#define ELECTRONPHOTONFOURMOMENTUMCORRECTION_GAINUNCERTAINTY_H

#include <AsgTools/AsgMessaging.h>
#include <PATCore/PATCoreEnums.h>
#include <memory>
#include <string>

class TH1;

namespace egGain {

class GainUncertainty : public asg::AsgMessaging {
public:

  GainUncertainty(const std::string& filename, const std::string& name = "GainUncertainty");
  ~GainUncertainty() {};

  // return relative uncertainty on energy from gain uncertainty
  // input etaCalo_input = eta in Calo frame
  //       et_input = Et in MeV
  //       ptype    = particle type
  //       useUncertainty : instead of the value, if a correction has been applied
  double getUncertainty(double etaCalo_input, double et_input,
			PATCore::ParticleType::Type ptype = PATCore::ParticleType::Electron,
			bool useUncertainty = false) const;

private:

  static const int s_nEtaBins=5;
  std::unique_ptr<TH1> m_alpha_specialGainRun;
  std::unique_ptr<TH1> m_gain_impact_Zee;
  std::unique_ptr<TH1> m_gain_Impact_elec[s_nEtaBins]{};
  std::unique_ptr<TH1> m_gain_Impact_conv[s_nEtaBins]{};
  std::unique_ptr<TH1> m_gain_Impact_unco[s_nEtaBins]{};

};

}

#endif
