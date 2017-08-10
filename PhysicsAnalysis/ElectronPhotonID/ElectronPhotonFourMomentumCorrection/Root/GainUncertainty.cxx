/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <ElectronPhotonFourMomentumCorrection/GainUncertainty.h>
#include "CxxUtils/make_unique.h"
#include "PathResolver/PathResolver.h"
#include <TH1F.h>
#include <TH1D.h>
#include <TFile.h>
#include <iostream>


namespace egGain {


//--------------------------------------

  GainUncertainty::GainUncertainty() : asg::AsgMessaging("GainUncertainty") {

    const std::string filename = PathResolverFindCalibFile("ElectronPhotonFourMomentumCorrection/v8/gain_uncertainty_specialRun.root");

    ATH_MSG_INFO("opening file " << filename);
    m_gainFile = CxxUtils::make_unique<TFile>(filename.c_str());

    if (not (m_alpha_specialGainRun = (TH1F*)(m_gainFile->Get("alpha_specialGainRun")))) ATH_MSG_FATAL("cannot open histogram1");
    if (not (m_gain_impact_Zee = (TH1F*)(m_gainFile->Get("gain_impact_Zee")))) ATH_MSG_FATAL("cannot open histogram2");
    for (int i=0;i<m_NUM_ETA_BINS;i++) {
     char name[60];
     sprintf(name,"gain_Impact_elec_%d",i);
     if (not (m_gain_Impact_elec[i] = (TH1D*)(m_gainFile->Get(name)))) ATH_MSG_FATAL("cannot open histogram3");
     sprintf(name,"gain_Impact_conv_%d",i);
     if (not (m_gain_Impact_conv[i] = (TH1D*)(m_gainFile->Get(name)))) ATH_MSG_FATAL("cannot open histogram4");
     sprintf(name,"gain_Impact_unco_%d",i);
     if (not (m_gain_Impact_unco[i]= (TH1D*)(m_gainFile->Get(name)))) ATH_MSG_FATAL("cannot open histogram5");
    }

  }

//----------------------------------------------

  GainUncertainty::~GainUncertainty() {
      delete m_alpha_specialGainRun;
      delete m_gain_impact_Zee;
      for (int i=0;i<m_NUM_ETA_BINS;i++) {
        delete m_gain_Impact_elec[i];
        delete m_gain_Impact_conv[i];
        delete m_gain_Impact_unco[i];
      }
  }

//----------------------------------------------

    // returns relative uncertainty on energy

  double GainUncertainty::getUncertainty(double etaCalo_input, double et_input,  PATCore::ParticleType::Type ptype) const {
       double aeta=std::fabs(etaCalo_input);
       int ibin=-1;
       if (aeta<0.8) ibin=0;
       else if (aeta<1.37) ibin=1;
       else if (aeta<1.52) ibin=2;
       else if (aeta<1.80) ibin=3;
       else if (aeta<2.50) ibin=4;
       if (ibin<0) return 0.;

//       std::cout << " --- in  GainUncertainty::getUncertainty " << etaCalo_input << " " << et_input << " " << ptype << " ibin " << ibin << std::endl;

       double impact=0.;
       if (ptype==PATCore::ParticleType::Electron) impact = m_gain_Impact_elec[ibin]->GetBinContent(m_gain_Impact_elec[ibin]->FindFixBin(0.001*et_input));
       if (ptype==PATCore::ParticleType::ConvertedPhoton) impact = m_gain_Impact_conv[ibin]->GetBinContent(m_gain_Impact_conv[ibin]->FindFixBin(0.001*et_input));
       if (ptype==PATCore::ParticleType::UnconvertedPhoton) impact = m_gain_Impact_unco[ibin]->GetBinContent(m_gain_Impact_unco[ibin]->FindFixBin(0.001*et_input));

//       std::cout << " impact " << impact << std::endl;
       double_t sigmaE = m_alpha_specialGainRun->GetBinContent(m_alpha_specialGainRun->FindFixBin(aeta))
                        /m_gain_impact_Zee->GetBinContent(m_gain_impact_Zee->FindFixBin(aeta))
                        *impact ;

//       std::cout << " m_alpha_specialGainRun, m_gain_impact_Zee, impact , sigmaE " << m_alpha_specialGainRun->GetBinContent(m_alpha_specialGainRun->FindBin(aeta))
//  << " " << m_gain_impact_Zee->GetBinContent(m_gain_impact_Zee->FindBin(aeta)) << " " << impact << " " << sigmaE << std::endl;

       return sigmaE;

  }


}
