/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUPERFORMANCEFUNCTIONS_CXX
#define TAUPERFORMANCEFUNCTIONS_CXX

#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"

void UpgradePerformanceFunctions::setTauRandomSeed(unsigned seed) {
  m_tauRandom.SetSeed(seed);
}

float UpgradePerformanceFunctions::getTauEfficiency(float etMeV, float eta_in, short prong, short wp) {
  // prong variable must be 1 or 3
  // wp variable determines the working point used: 0 = loose, 1 = medium, 2 = tight

  float Reco_Pt_cut = 20000.;
  float Reco_eta_cut = 4.0;
  if (etMeV < Reco_Pt_cut || fabs(eta_in) > Reco_eta_cut) {
    ATH_MSG_WARNING("No Reco SF for these taus - either pt or eta are out of the acceptance !");
    return 0;
  }

  float a, b;
  a = 0;
  b = 0;

  float eta = abs(eta_in);
  float eta1 = 0.9;
  float eta2 = 4.0;

  if (wp == 0) {
    //loose working point (reco eff of 85% for 1-prong and 75% for 3-prong)
    if (prong == 1) {
      if (eta < eta1) {  a = 0.72;     b = 0; }
      else if (eta >= eta1 && eta < eta2) {  a = 0.86;     b = -0.19; }
      else {
        ATH_MSG_WARNING("getTauEfficiency: no efficiency available for eta=" << eta);
        return 0;
      }
    } else if (prong == 3) {
      if (eta < eta1) {  a = 0.67;     b = 0; }
      else if (eta >= eta1 && eta < eta2) {  a = 0.75;     b = -0.13; }
      else {
        ATH_MSG_WARNING("getTauEfficiency: no efficiency available for eta=" << eta);
        return 0;
      }
    } else {
      ATH_MSG_WARNING("getTauEfficiency: no efficiency available for prong=" << prong);
      return 0;
    }

  } else if (wp == 1) {
    //medium working point (reco eff of 75% for 1-prong and 60% for 3-prong)
    if (prong == 1) {
      if (eta < eta1) {  a = 0.63;     b = 0; }
      else if (eta >= eta1 && eta < eta2) {  a = 0.77;     b = -0.18; }
      else {
        ATH_MSG_WARNING("getTauEfficiency: no efficiency available for eta=" << eta);
        return 0;
      }
    } else if (prong == 3) {
      if (eta < eta1) {  a = 0.58;     b = 0; }
      else if (eta >= eta1 && eta < eta2) {  a = 0.66;     b = -0.13; }
      else {
        ATH_MSG_WARNING("getTauEfficiency: no efficiency available for eta=" << eta);
        return 0;
      }
    } else {
      ATH_MSG_WARNING("getTauEfficiency: no efficiency available for prong=" << prong);
      return 0;
    }

  } else if (wp == 2) {
    //tight working point (reco eff of 60% for 1-prong and 45% for 3-prong)

    if (prong == 1) {
      if (eta < eta1) {  a = 0.53;     b = 0; }
      else if (eta >= eta1 && eta < eta2) {  a = 0.63;     b = -0.16; }
      else {
        ATH_MSG_WARNING("getTauEfficiency: no efficiency available for eta=" << eta);
        return 0;
      }
    } else if (prong == 3) {
      if (eta < eta1) {  a = 0.48;     b = 0; }
      else if (eta >= eta1 && eta < eta2) {  a = 0.55;     b = -0.12; }
      else {
        ATH_MSG_WARNING("getTauEfficiency: no efficiency available for eta=" << eta);
        return 0;
      }
    } else {
      ATH_MSG_WARNING("getTauEfficiency: no efficiency available for prong=" << prong);
      return 0;
    }

  } else {
    ATH_MSG_WARNING("getTauEfficiency: no efficiency available for this working point =" << wp);
    return 0;
  }

  float efficiency = a + b * eta;
  return efficiency;
}


float UpgradePerformanceFunctions::getTauFakeRate(float etMeV, float eta_in, short prong, short wp) {
  // Fake rates in bins of pT and eta, for 1- and 3-prong taus
  //  prong variable must be 1 or 3
  //  wp variable determines the working point used: 0 = loose, 1 = medium, 2 = tight

  float Jet_Pt_cut = 20000.;
  float Jet_eta_cut = 4.0;

  if (etMeV < Jet_Pt_cut || fabs(eta_in) > Jet_eta_cut) {
    ATH_MSG_WARNING("getTauFakeRate: no fake rate for these taus with pT=" << etMeV << ", eta=" << eta_in << " -- either pt or eta are out of the acceptance !");
    return 0;
  }

  float eta = abs(eta_in);

  float pt1 = 20000.;
  float pt2 = 30000.;
  float pt3 = 50000.;
  float pt4 = 80000.;
  float pt5 = 120000.;

  // Fake SFs are binned in pT (1-5), working point (L,M,T)
  //  and an overall SF is added to account for the jet efficiency
  //  of the 1- and 3-prong selections

  float Fake_SF_1, Fake_SF_2, Fake_SF_3, Fake_SF_4, Fake_SF_5;
  float Fake_SF_M, Fake_SF_T;
  float Fake_SF_eff;

  if (prong == 1) {
    Fake_SF_M = 0.5;
    Fake_SF_T = 0.2;
    Fake_SF_eff = 0.142;

    if (eta >= 0 && eta < 1.2) {
      Fake_SF_1 = 0.02;
      Fake_SF_2 = 0.03;
      Fake_SF_3 = 0.05;
      Fake_SF_4 = 0.08;
      Fake_SF_5 = 0.10;
    } else if (eta >= 1.2 && eta < 2.5) {
      Fake_SF_1 = 0.02;
      Fake_SF_2 = 0.04;
      Fake_SF_3 = 0.07;
      Fake_SF_4 = 0.08;
      Fake_SF_5 = 0.09;
    } else if (eta >= 2.5 && eta < 4.0) {
      Fake_SF_1 = 0.03;
      Fake_SF_2 = 0.05;
      Fake_SF_3 = 0.07;
      Fake_SF_4 = 0.10;
      Fake_SF_5 = 0.14;
    } else {
      ATH_MSG_WARNING("getTauFakeRate: no fake rate available for prong=" << prong << ", eta=" << eta);
      return 0;
    }

  } else if (prong == 3) {
    Fake_SF_M = 0.35;
    Fake_SF_T = 0.13;
    Fake_SF_eff = 0.138;

    if (eta >= 0 && eta < 1.2) {
      Fake_SF_1 = 0.018;
      Fake_SF_2 = 0.024;
      Fake_SF_3 = 0.021;
      Fake_SF_4 = 0.017;
      Fake_SF_5 = 0.007;
    } else if (eta >= 1.2 && eta < 2.5) {
      Fake_SF_1 = 0.018;
      Fake_SF_2 = 0.029;
      Fake_SF_3 = 0.028;
      Fake_SF_4 = 0.023;
      Fake_SF_5 = 0.010;
    } else if (eta >= 2.5 && eta < 4.0) {
      Fake_SF_1 = 0.035;
      Fake_SF_2 = 0.049;
      Fake_SF_3 = 0.056;
      Fake_SF_4 = 0.030;
      Fake_SF_5 = 0.015;
    } else {
      ATH_MSG_WARNING("getTauFakeRate: no fake rate available for prong=" << prong << ", eta" << eta);
      return 0;
    }
  } else {
    ATH_MSG_WARNING("getTauFakeRate: no fake rate available for prong=" << prong);
    return 0;
  }

  //loose working point (reco eff of 85% for 1-prong and 75% for 3-prong)
  //medium working point (reco eff of 75% for 1-prong and 60% for 3-prong)
  //tight working point (reco eff of 60% for 1-prong and 45% for 3-prong)

  if (etMeV >= pt1 && etMeV < pt2) {
    if (wp == 0) return Fake_SF_eff * Fake_SF_1;
    if (wp == 1) return Fake_SF_eff * Fake_SF_M * Fake_SF_1;
    if (wp == 2) return Fake_SF_eff * Fake_SF_T * Fake_SF_1;
  } else if (etMeV >= pt2 && etMeV < pt3) {
    if (wp == 0) return Fake_SF_eff * Fake_SF_2;
    if (wp == 1) return Fake_SF_eff * Fake_SF_M * Fake_SF_2;
    if (wp == 2) return Fake_SF_eff * Fake_SF_T * Fake_SF_2;
  } else if (etMeV >= pt3 && etMeV < pt4) {
    if (wp == 0) return Fake_SF_eff * Fake_SF_3;
    if (wp == 1) return Fake_SF_eff * Fake_SF_M * Fake_SF_3;
    if (wp == 2) return Fake_SF_eff * Fake_SF_T * Fake_SF_3;
  } else if (etMeV >= pt4 && etMeV < pt5) {
    if (wp == 0) return Fake_SF_eff * Fake_SF_4;
    if (wp == 1) return Fake_SF_eff * Fake_SF_M * Fake_SF_4;
    if (wp == 2) return Fake_SF_eff * Fake_SF_T * Fake_SF_4;
  } else if (etMeV >= pt5) {
    if (wp == 0) return Fake_SF_eff * Fake_SF_5;
    if (wp == 1) return Fake_SF_eff * Fake_SF_M * Fake_SF_5;
    if (wp == 2) return Fake_SF_eff * Fake_SF_T * Fake_SF_5;
  }
  ATH_MSG_WARNING("getTauFakeRate: no fake rate available for this tau");
  return 0.;
}


float UpgradePerformanceFunctions::getTauEnergyResolution(float eMeV, float eta_in, short prong) {
  float a, b, c;
  a = 0;
  b = 0;
  c = 0;
  float eta = abs(eta_in);

  if (prong == 1) {
    if (eta >= 0 && eta < 0.3) {  a = 0.037;     b = 0.70;     c = 6.21; }
    else if (eta >= 0.3 && eta < 0.8) {  a = 0.036;     b = 0.70;     c = 7.11; }
    else if (eta >= 0.8 && eta < 1.3) {  a = 0.051;     b = 1.16;     c = 9.77; }
    else if (eta >= 1.3 && eta < 1.6) {  a = 0.077;     b = 1.42;     c = 12.47; }
    else if (eta >= 1.6 && eta < 2.5) {  a = 0.010;     b = 1.87;     c = 17.08; }
    else if (eta >= 2.5 && eta < 4.0) {  a = 0.10;     b = 0.14;     c = 95.43; }
    else {
      ATH_MSG_WARNING("getTauEnergyResolution: no energy resolution available for prong=" << prong << " eta=" << eta);
      return 0;
    }
  } else if (prong == 3) {
    if (eta >= 0 && eta < 0.3) {  a = 0.046;     b = 0.60;     c = 7.00; }
    else if (eta >= 0.3 && eta < 0.8) {  a = 0.036;     b = 0.74;     c = 7.51; }
    else if (eta >= 0.8 && eta < 1.3) {  a = 0.043;     b = 1.26;     c = 10.03; }
    else if (eta >= 1.3 && eta < 1.6) {  a = 0.075;     b = 1.16;     c = 14.24; }
    else if (eta >= 1.6 && eta < 2.5) {  a = 0.039;     b = 1.14;     c = 21.36; }
    else if (eta >= 2.5 && eta < 4.0) {  a = 0.0078;     b = 4.31;     c = 86.33; }
    else {
      ATH_MSG_WARNING("getTauEnergyResolution: no energy resolution available for prong=" << prong << " eta=" << eta);
      return 0;
    }
  } else {
    ATH_MSG_WARNING("getTauEnergyResolution: no energy resolution available for prong=" << prong);
    return 0;
  }

  double relativeUncertainty = hypot( hypot(c / (eMeV / 1000.), b / sqrt(eMeV / 1000.)) , a);
  return eMeV * relativeUncertainty;
}


float UpgradePerformanceFunctions::getTauSmearedEnergy(float eMeV, float eta, short prong) {
  return (eMeV + m_tauRandom.Gaus(0., getTauEnergyResolution(eMeV, eta, prong)));
}

#endif
