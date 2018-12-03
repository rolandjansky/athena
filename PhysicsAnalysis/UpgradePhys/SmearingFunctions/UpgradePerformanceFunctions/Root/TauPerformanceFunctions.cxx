/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUPERFORMANCEFUNCTIONS_CXX
#define TAUPERFORMANCEFUNCTIONS_CXX

#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"

namespace Upgrade {

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

  float eta = fabs(eta_in);
  float eta1 = 1.2;
  float eta2 = 2.5;
  float eta3 = 4.0;

  float pt = etMeV;
  float pt1 = 50000.;
  float pt2 = 100000.;

  if (wp == 0) {
    //loose working point (ID eff of 85% for 1-prong and 75% for 3-prong)
    if (prong == 1) {
      if (eta < eta1) {
	if (pt < pt1) {a = 0.80;     b = -0.019; }
	else if (pt>= pt1 && pt<pt2) {a = 0.85; b = -0.025;}
	else {a = 0.90; b = -0.024;}
      } else if (eta >= eta1 && eta < eta2) {
      	if (pt < pt1) {a = 0.84;     b = -0.040; }
	else if (pt>= pt1 && pt<pt2) {a = 0.96; b = -0.082;}
	else {a = 1.16; b = -0.168;}
      } else if (eta >= eta2 && eta < eta3) {
	if (pt < pt1) {a = 1.11;     b = -0.153; }
	else if (pt>= pt1 && pt<pt2) {a = 1.17; b = -0.170;}
	else {a = 1.07; b = -0.182;}
      } else {
        ATH_MSG_WARNING("getTauEfficiency: no efficiency available for eta=" << eta);
        return 0;
      }
    } else if (prong == 3) {
      if (eta < eta1) {
	if (pt < pt1) {a = 0.78;     b = -0.057; }
	else if (pt>= pt1 && pt<pt2) {a = 0.80; b = -0.022;}
	else {a = 0.84; b = 0;}
      } else if (eta >= eta1 && eta < eta2) {
      	if (pt < pt1) {a = 0.80;     b = -0.084; }
	else if (pt>= pt1 && pt<pt2) {a = 0.84; b = -0.064;}
	else {a = 0.97; b = -0.149;}
      } else if (eta >= eta2 && eta < eta3) {
	if (pt < pt1) {a = 1.04;     b = -0.165; }
	else if (pt>= pt1 && pt<pt2) {a = 1.15; b = -0.197;}
	else {a = 1.15; b = -0.238;}
    } else {
        ATH_MSG_WARNING("getTauEfficiency: no efficiency available for eta=" << eta);
        return 0;
      }
    } else {
      ATH_MSG_WARNING("getTauEfficiency: no efficiency available for prong=" << prong);
      return 0;
    }

  } else if (wp == 1) {
    //medium working point (ID eff of 75% for 1-prong and 60% for 3-prong)

    if (prong == 1) {
      if (eta < eta1) {
        if (pt < pt1) {a = 0.72;     b = -0.017; }
        else if (pt>= pt1 && pt<pt2) {a = 0.78; b = -0.032;}
        else {a = 0.82; b = -0.042;}
      } else if (eta >= eta1 && eta < eta2) {
        if (pt < pt1) {a = 0.76;     b = -0.048; }
        else if (pt>= pt1 && pt<pt2) {a = 0.91; b = -0.094;}
        else {a = 1.18; b = -0.227;}
      } else if (eta >= eta2 && eta < eta3) {
        if (pt < pt1) {a = 1.09;     b = -0.185; }
        else if (pt>= pt1 && pt<pt2) {a = 1.25; b = -0.244;}
        else {a = 1.03; b = -0.229;}
      } else {
        ATH_MSG_WARNING("getTauEfficiency: no efficiency available for eta=" << eta);
        return 0;
      }
    } else if (prong == 3) {
      if (eta < eta1) {
        if (pt < pt1) {a = 0.63;     b = -0.036; }
        else if (pt>= pt1 && pt<pt2) {a = 0.67; b = -0.044;}
        else {a = 0.67; b = 0;}
      } else if (eta >= eta1 && eta < eta2) {
        if (pt < pt1) {a = 0.66;     b = -0.090; }
        else if (pt>= pt1 && pt<pt2) {a = 0.72; b = -0.085;}
        else {a = 0.89; b = -0.179;}
      } else if (eta >= eta2 && eta < eta3) {
        if (pt < pt1) {a = 1.21;     b = -0.275; }
        else if (pt>= pt1 && pt<pt2) {a = 1.05; b = 0.221;}
        else {a = 0.93; b = -0.219;}
    } else {
        ATH_MSG_WARNING("getTauEfficiency: no efficiency available for eta=" << eta);
        return 0;
      }
    } else {
      ATH_MSG_WARNING("getTauEfficiency: no efficiency available for prong=" << prong);
      return 0;
    }
 
  } else if (wp == 2) {
    //tight working point (ID eff of 60% for 1-prong and 45% for 3-prong)

    if (prong == 1) {
      if (eta < eta1) {
        if (pt < pt1) {a = 0.59;     b = -0.019; }
        else if (pt>= pt1 && pt<pt2) {a = 0.65; b = -0.039;}
        else {a = 0.71; b = -0.071;}
      } else if (eta >= eta1 && eta < eta2) {
        if (pt < pt1) {a = 0.62;     b = -0.046; }
        else if (pt>= pt1 && pt<pt2) {a = 0.80; b = -0.109;}
        else {a = 1.10; b = -0.259;}
      } else if (eta >= eta2 && eta < eta3) {
        if (pt < pt1) {a = 1.02;     b = -0.204; }
        else if (pt>= pt1 && pt<pt2) {a = 1.10; b = -0.251;}
        else {a = 0.68; b = -0.166;}
    } else {
        ATH_MSG_WARNING("getTauEfficiency: no efficiency available for eta=" << eta);
        return 0;
      }
    } else if (prong == 3) {
      if (eta < eta1) {
        if (pt < pt1) {a = 0.49;     b = -0.039; }
        else if (pt>= pt1 && pt<pt2) {a = 0.53; b = -0.038;}
        else {a = 0.57; b = -0.011;}
      } else if (eta >= eta1 && eta < eta2) {
        if (pt < pt1) {a = 0.50;     b = -0.079; }
        else if (pt>= pt1 && pt<pt2) {a = 0.59; b = -0.088;}
        else {a = 0.85; b = -0.218;}
      } else if (eta >= eta2 && eta < eta3) {
        if (pt < pt1) {a = 0.68;     b = -0.145; }
        else if (pt>= pt1 && pt<pt2) {a = 0.84; b = -0.198;}
        else {a = 0.62; b = -0.152;}
    } else {
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

  float eta = fabs(eta_in);
  float eta1 = 2.5;
  float eta2 = 4.0;

  float a,b;
  a = 0;
  b = 0;

  float pt = etMeV;
  float pt1 = 50000.;
  float pt2 = 100000.;

  // Fake SFs are binned in pT (1-5), working point (L,M,T)
  //  and an overall SF is added to account for the jet efficiency
  //  of the 1- and 3-prong selections

  float Fake_SF_eff;

  if(prong == 1) Fake_SF_eff = 0.278;
  if(prong == 3) Fake_SF_eff = 0.091;

  if (wp == 0) {
    //loose working point (ID eff of 85% for 1-prong and 75% for 3-prong)
    if (prong == 1) {
      if (eta < eta1) {
	if (pt < pt1) {a = 0.090;     b = 0; }
	else if (pt>= pt1 && pt<pt2) {a = 0.059; b = 0;}
	else {a = 0.022; b = 0;}
      } else if (eta >= eta1 && eta < eta2) {
      	if (pt < pt1) {a = 0.031;     b = 0.015; }
	else if (pt>= pt1 && pt<pt2) {a = -0.167; b = 0.096;}
	else {a = -0.049; b = 0.029;}
      } else {
        ATH_MSG_WARNING("getTauEfficiency: no efficiency available for eta=" << eta);
        return 0;
      }
    } else if (prong == 3) {
      if (eta < eta1) {
	if (pt < pt1) {a = 0.017;     b = 0; }
	else if (pt>= pt1 && pt<pt2) {a = 0.009; b = 0;}
	else {a = 0.0008; b = 0;}
      } else if (eta >= eta1 && eta < eta2) {
      	if (pt < pt1) {a = 0.010;     b = 0.006; }
	else if (pt>= pt1 && pt<pt2) {a = 0.024; b = -0.003;}
	else {a = -0.004; b = 0.002;}
    } else {
        ATH_MSG_WARNING("getTauEfficiency: no efficiency available for eta=" << eta);
        return 0;
      }
    } else {
      ATH_MSG_WARNING("getTauEfficiency: no efficiency available for prong=" << prong);
      return 0;
    }

  } else if (wp == 1) {
    //medium working point (ID eff of 75% for 1-prong and 60% for 3-prong)

    if (prong == 1) {
      if (eta < eta1) {
        if (pt < pt1) {a = 0.049;     b = 0; }
        else if (pt>= pt1 && pt<pt2) {a = 0.032; b = 0;}
        else {a = 0.010; b = 0;}
      } else if (eta >= eta1 && eta < eta2) {
        if (pt < pt1) {a = 0.027;     b = 0.0047; }
        else if (pt>= pt1 && pt<pt2) {a = -0.023; b = 0.028;}
        else {a = 0.002; b = 0.004;}
      } else {
        ATH_MSG_WARNING("getTauEfficiency: no efficiency available for eta=" << eta);
        return 0;
      }
    } else if (prong == 3) {
      if (eta < eta1) {
        if (pt < pt1) {a = 0.006;     b = 0; }
        else if (pt>= pt1 && pt<pt2) {a = 0.003; b = 0;}
        else {a = 0.0001; b = 0;}
      } else if (eta >= eta1 && eta < eta2) {
        if (pt < pt1) {a = 0.006;     b = 0.0007; }
        else if (pt>= pt1 && pt<pt2) {a = 0.01; b = -0.003;}
        else {a = -0.003; b = 0.001;}
    } else {
        ATH_MSG_WARNING("getTauEfficiency: no efficiency available for eta=" << eta);
        return 0;
      }
    } else {
      ATH_MSG_WARNING("getTauEfficiency: no efficiency available for prong=" << prong);
      return 0;
    }
 
  } else if (wp == 2) {
    //tight working point (ID eff of 60% for 1-prong and 45% for 3-prong)

    if (prong == 1) {
      if (eta < eta1) {
        if (pt < pt1) {a = 0.023;     b = 0; }
        else if (pt>= pt1 && pt<pt2) {a = 0.014; b = 0;}
        else {a = 0.004; b = 0;}
      } else if (eta >= eta1 && eta < eta2) {
        if (pt < pt1) {a = 0.021;     b = 0; }
        else if (pt>= pt1 && pt<pt2) {a = 0.028; b = -0.001;}
        else {a = 0.009; b = -0.002;}
    } else {
        ATH_MSG_WARNING("getTauEfficiency: no efficiency available for eta=" << eta);
        return 0;
      }
    } else if (prong == 3) {
      if (eta < eta1) {
        if (pt < pt1) {a = 0.002;     b = 0; }
        else if (pt>= pt1 && pt<pt2) {a = 0.001; b = 0;}
        else {a = 0.0001; b = 0;}
      } else if (eta >= eta1 && eta < eta2) {
        if (pt < pt1) {a = 0.004;     b = 0; }
        else if (pt>= pt1 && pt<pt2) {a = 0.005; b = -0.001;}
        else {a = 0.0001; b = 0;}
    } else {
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

  float rate = (a + b*eta) * Fake_SF_eff;
  return rate;

  ATH_MSG_WARNING("getTauFakeRate: no fake rate available for this tau");
  return 0.;
  }


float UpgradePerformanceFunctions::getTauEnergyResolution(float eMeV, float eta_in, short prong) {
  float a, b, c;
  a = 0;
  b = 0;
  c = 0;
  float eta = fabs(eta_in);

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

}
#endif
