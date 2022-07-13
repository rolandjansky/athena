/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <ElectronPhotonFourMomentumCorrection/GainUncertainty.h>
#include <TH1.h>
#include <TFile.h>

template<typename TargetPtr, typename SourcePtr>
TargetPtr
checked_cast(SourcePtr ptr)
{
  static_assert(std::is_pointer<TargetPtr>::value, "attempt to cast to no ptr object");
  static_assert(std::is_pointer<SourcePtr>::value, "attempt to cast from no ptr object");
  if(!ptr){
    throw std::runtime_error("Attempt to cast from nullptr");
  }
  TargetPtr obj = dynamic_cast<TargetPtr>(ptr);
  if (not obj) {
    throw std::runtime_error("failed dynamic cast for " + std::string(ptr->GetName())
                             +" in egGain::GainUncertainty");
  }
  return obj;
}

namespace egGain {

//--------------------------------------

  GainUncertainty::GainUncertainty(const std::string& filename, const std::string& name) : asg::AsgMessaging(name) {

    ATH_MSG_INFO("opening file " << filename);
    std::unique_ptr<TFile> gainFile(TFile::Open( filename.c_str(), "READ"));

    m_alpha_specialGainRun.reset(checked_cast<TH1*>(gainFile->Get("alpha_specialGainRun")));
    m_alpha_specialGainRun->SetDirectory(nullptr);
    m_gain_impact_Zee.reset(checked_cast<TH1*>(gainFile->Get("gain_impact_Zee")));
    m_gain_impact_Zee->SetDirectory(nullptr);
    for (int i=0;i<s_nEtaBins;i++) {
      char name[60];
      sprintf(name,"gain_Impact_elec_%d",i);
      m_gain_Impact_elec[i].reset(checked_cast<TH1*>(gainFile->Get(name)));
      m_gain_Impact_elec[i]->SetDirectory(nullptr);
      sprintf(name,"gain_Impact_conv_%d",i);
      m_gain_Impact_conv[i].reset(checked_cast<TH1*>(gainFile->Get(name)));
      m_gain_Impact_conv[i]->SetDirectory(nullptr);
      sprintf(name,"gain_Impact_unco_%d",i);
      m_gain_Impact_unco[i].reset(checked_cast<TH1*>(gainFile->Get(name)));
      m_gain_Impact_unco[i]->SetDirectory(nullptr);
    }

  }

  //----------------------------------------------
  // returns relative uncertainty on energy

  double GainUncertainty::getUncertainty(double etaCalo_input, double et_input,
					 PATCore::ParticleType::Type ptype,
					 bool useL2GainUncertainty) const {
    double aeta = std::abs(etaCalo_input);
    int ibin = -1;
    if (aeta<0.8) ibin=0;
    else if (aeta<1.37) ibin=1;
    else if (aeta<1.52) ibin=2;
    else if (aeta<1.80) ibin=3;
    else if (aeta<2.50) ibin=4;
    if (ibin<0) return 0.;

    //Protection needed as the histograms stops at 1 TeV
    if(et_input>999999.) et_input = 999999.;

    ATH_MSG_VERBOSE("GainUncertainty::getUncertainty "
		    << etaCalo_input << " "
		    << et_input << " "
		    << ptype << " ibin " << ibin);

    TH1 *hImpact = nullptr;
    if (ptype == PATCore::ParticleType::Electron)
      hImpact = m_gain_Impact_elec[ibin].get();
    else if (ptype == PATCore::ParticleType::ConvertedPhoton)
      hImpact = m_gain_Impact_conv[ibin].get();
    else if (ptype == PATCore::ParticleType::UnconvertedPhoton)
      hImpact = m_gain_Impact_unco[ibin].get();
    if (hImpact == nullptr) {
      ATH_MSG_WARNING("Trying to get Gain correction of not allowed particle type");
      return 0;
    }
    double impact = hImpact->GetBinContent(hImpact->FindFixBin(0.001*et_input));

    int ieta = m_alpha_specialGainRun->FindFixBin(aeta);
    double alphaG = useL2GainUncertainty ?
      m_alpha_specialGainRun->GetBinError(ieta) :
      m_alpha_specialGainRun->GetBinContent(ieta);

    double impactZee = m_gain_impact_Zee->GetBinContent(m_gain_impact_Zee->FindFixBin(aeta));

    double_t sigmaE = alphaG*impact/impactZee;

    ATH_MSG_VERBOSE("alpha_specialGainRun, gain_impact_Zee, impact, sigmaE = "
		    << alphaG << " " << impactZee << " " << impact << " " << sigmaE);

    return sigmaE;
  }

}
