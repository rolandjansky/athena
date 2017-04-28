/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ELECTRONPHOTONFOURMOMENTUMCORRECTION_GAINUNCERTAINTYY_H
#define ELECTRONPHOTONFOURMOMENTUMCORRECTION_GAINUNCERTAINTYY_H

#include <AsgTools/AsgMessaging.h>
#include <PATCore/PATCoreEnums.h>
#include <memory>
#include <string>


class TFile;
class TH1F;
class TH1D;

namespace egGain{

class GainUncertainty : public asg::AsgMessaging {

public:

  GainUncertainty();
  ~GainUncertainty();

 // return relative uncertainty on energy from gain uncertainty
 // input etaCalo_input = eta in Calo frame
 //       et_input = Et in MeV
 //       ptype    = particle type

  double getUncertainty(double etaCalo_input, double et_input, PATCore::ParticleType::Type ptype=PATCore::ParticleType::Electron) const;

private:

  static const int m_NUM_ETA_BINS=5;

  std::unique_ptr<TFile> m_gainFile;

  TH1F* m_alpha_specialGainRun;
  TH1F* m_gain_impact_Zee;
  TH1D* m_gain_Impact_elec[5];
  TH1D* m_gain_Impact_conv[5];
  TH1D* m_gain_Impact_unco[5];

};

}

#endif
