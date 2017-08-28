/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERPERFORMANCEFUNCTIONS_CXX
#define TRIGGERPERFORMANCEFUNCTIONS_CXX

#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"

float UpgradePerformanceFunctions::getSingleElectronTriggerEfficiency(float ptMeV, float eta) {
  float minPt = 22000.;
  float minPtHighEta = 35000;
  float maxEta = 4.0;
  float eff = 0.95;
  float effHighEta = 0.90;
  if ( m_layout == LoI ) {
    minPt = 25000.;
    maxEta = 2.5;
    eff = 0.88;
  } else if ( m_layout == LoIVF ) {
    minPt = 25000.;
    eff = 0.88;
    effHighEta = 0.88;
  } else if ( m_layout == silver ) {
    minPt = 28000.;
    minPtHighEta = 40000.;
    maxEta = 3.2;
  } else if ( m_layout == bronze ) {
    minPt = 28000.;
    maxEta = 2.5;
    eff = 0.91;
  }

  // HGTD forward trigger 5 GeV improvement
  if (m_bUseHGTD0 || m_bUseHGTD1)
    minPtHighEta = 30000.;

  if ( ptMeV > 50000. && fabs(eta) < 2.5 ) return 1.0;
  if ( ptMeV > 35000. && fabs(eta) < 2.5 && m_layout == gold) return 1.0;
  if ( ptMeV > minPt && fabs(eta) < 2.5 )
    return eff;
  if ( ptMeV > minPtHighEta && fabs(eta) < maxEta )
    return effHighEta;
  //  std::cout << "WARNING: no trigger efficiency is returned for pT "<<ptMeV/1000.<<" GeV, eta="<<eta<<", since it is assumed this will be the first unprescaled single electron trigger threshold !" << std::endl;
  return 0.0;
}

float UpgradePerformanceFunctions::getDiElectronTriggerEfficiency(float pt1MeV, float pt2MeV, float eta1, float eta2) {
  if (pt1MeV<15000. || pt2MeV<15000. || fabs(eta1)>2.5 || fabs(eta2)>2.5) {
    //std::cout << "WARNING: no trigger efficiency is returned for pT<15 GeV or eta>2.5, since it is assumed this will be the lowest unprescaled di-lepton trigger threshold !" << std::endl;
    return 0.0;
  }
  float eff=0.95; //single lepton efficiency
  if ( m_layout == LoI || m_layout == LoIVF) {
    eff = 0.88;
  } else if (m_layout == bronze ) {
    eff = 0.91;
  }
  return eff*eff;
}

float UpgradePerformanceFunctions::getSinglePhotonTriggerEfficiency(float etMeV, float eta) {
  if (etMeV<120000. || fabs(eta)>2.4 ) {
    std::cout << "WARNING: no trigger efficiency is returned for PT<120 GeV or eta>2.4 since it is assumed this will be the lowest photon trigger threshold !" << std::endl;
    return 0.;
  }
  return 1.0;
}

float UpgradePerformanceFunctions::getDiPhotonTriggerEfficiency(float et1MeV, float et2MeV, float eta1, float eta2) {
  if (et1MeV<25000. || et2MeV<25000. || fabs(eta1)>2.4 || fabs(eta2)>2.4) {
    std::cout << "WARNING: no trigger efficiency is returned for PT<25 GeV or eta >2.4 since it is assumed this will be the lowest diphoton trigger threshold !" << std::endl;
    return 0.;
  }
  return 1.00;
}

float UpgradePerformanceFunctions::muonEtaTriggerEfficiency(float eta) {

    // rpc L0 efficiency data  22 bins for 0<eta<1.1
    const float eta_bin = 0.05;
    const float eff_gold[22] = {0.790656, 0.930483, 0.98033, 0.992508, 0.974555, 0.981241, 0.985142, 0.947444, 0.960144, 0.98223, 0.983938, 0.984972, 0.972907, 0.982902, 0.919753, 0.899409, 0.970952, 0.960322, 0.946016, 0.868755, 0.619748,0};
    const float eff_silver[22] = {0.466976, 0.640636, 0.78031, 0.875155, 0.786891, 0.817936, 0.857857, 0.735092, 0.614015, 0.732087, 0.820762, 0.799978, 0.911093, 0.971162, 0.911653, 0.899409, 0.970952, 0.960322, 0.946016, 0.868755, 0.619748, 0 };
    const float eff_bronze[22] = {0.466303, 0.638691, 0.776874, 0.874595, 0.785156, 0.816267, 0.856739, 0.73399, 0.610921, 0.730391, 0.820209, 0.798315, 0.82698, 0.716125, 0.565287, 0.486861, 0.529895, 0.562725, 0.48137, 0.238961, 0.116867, 0};
    //=======
    
    float eff = 0.98*0.98; //TGC*MDT efficiency
    if (fabs(eta)>2.4) return 0.;
    if (fabs(eta)>1.05) {
        if ( m_layout == LoI || m_layout == LoIVF) eff=0.86; //obsolete numbers
        if ( m_layout == bronze) eff*=0.98; //additional 2% loss from L1Track
    } else {
        int ibin=fabs(eta)/eta_bin;
        float eff_rpc= eff=0.70; //obsolete numbers
        if ( m_layout == gold ) {
            eff_rpc=eff_gold[ibin];
        } else if   ( m_layout == silver ) {
            eff_rpc=eff_silver[ibin];
        } else if (m_layout == bronze) {
            eff_rpc=eff_bronze[ibin];
        } 
        
        if ( m_layout == gold )   eff=eff_rpc*0.98; //RPC recovery with BI RPC chambers
        if ( m_layout == silver ) eff=eff_rpc*0.98; //only partial recovery with BI RPC chambers
        if ( m_layout == bronze ) eff=eff_rpc*0.98*0.98; //inefficient RPC, and additional 2% L1Track inefficiency
    }
    return eff;
}

float UpgradePerformanceFunctions::getSingleMuonTriggerEfficiency(float etMeV, float eta) {
  //single-mu trigger efficiency w.r.t. reconstruction efficiency (tight=true)
  //using 2012 values from K. Nagano
  float minPt=25000.;
  if ( m_layout == gold ) minPt=20000.;
  if (etMeV > minPt) return muonEtaTriggerEfficiency(eta);
  return 0.;
}

float UpgradePerformanceFunctions::getDiMuonTriggerEfficiency(float et1MeV, float et2MeV, float eta1, float eta2) {
  float eff=muonEtaTriggerEfficiency(eta1)*muonEtaTriggerEfficiency(eta2);
  float minPt=15000.;
  if (m_layout == gold) minPt=11000.;
  
  // Result is product of individual muon efficiencies 
  if (et1MeV > minPt && et2MeV > minPt) return eff;
  return 0.;
}

float UpgradePerformanceFunctions::getElectronMuonTriggerEfficiency(float elecEtMeV, float muonEtMeV, float elecEta, float muonEta) {
  float minPt = 15000.;
  float muonEff = muonEtaTriggerEfficiency(muonEta);
  float elecEff = 0.95;
  if (m_layout == bronze) elecEff = 0.91;
  if (fabs(elecEta) > 2.5) elecEff = 0.;
  // Result is product of individual efficiencies 
  if (elecEtMeV > minPt && muonEtMeV > minPt) return muonEff*elecEff;
  return 0.;
}

float UpgradePerformanceFunctions::getSingleTauTriggerEfficiency(float etMeV, float eta, short prong) {
  if (etMeV<150000. || fabs(eta)>2.5 || prong>3) {
    std::cout << "WARNING: no trigger efficiency is returned for ET<150 GeV or |eta|>2.5. since it is assumed this will be the first unprescaled single tau trigger threshold !" << std::endl;
    return 0.;
  }
  return 0.80;
}

float UpgradePerformanceFunctions::getDiTauTriggerEfficiency(float et1MeV, float et2MeV, 
							     float eta1, float eta2,
							     short prong1, short prong2) {
  float et1 = et1MeV;
  float et2 = et2MeV;
  if ( et1 < et2 ) {
    et1 = et2MeV;
    et2 = et1MeV;
  }
  float minPt1 = 40000.;
  float minPt2 = 30000.;
  float eff = 0.80; //single tau efficiency
  if ( m_layout == silver || m_layout == bronze) {
    minPt1 = 50000.;
    minPt2 = 50000.;
  }
  if ( m_layout == bronze ) eff = 0.75;
  if ( fabs(eta1)>2.5 || fabs(eta2)>2.5 || prong1>3 || prong2>3 ) return 0.;
  if ( et1 > minPt1 && et2 > minPt2 ) return eff*eff;
  return 0;
}

// Hadronic trigger efficiencies from A. Sfyrla and S. Demers
// All input arguments are the "smeared" jet/MET/HT energies.
// (Users must have done the smearing before calling these methods.)

float UpgradePerformanceFunctions::getSingleJetTriggerEfficiency(float smearedEtMeV, float eta) {
  if (smearedEtMeV<m_JetTrig1jMin || fabs(eta)>m_JetTrigMaxEta) {
    return 0.;
  } else {
    return 0.95;
  }
}

float UpgradePerformanceFunctions::getThreeJetTriggerEfficiency(float smearedEtMeV1, float eta1, float smearedEtMeV2, float eta2, float smearedEtMeV3, float eta3) {
  if (smearedEtMeV1<m_JetTrig3jMin || fabs(eta1)>m_JetTrigMaxEta ||
      smearedEtMeV2<m_JetTrig3jMin || fabs(eta2)>m_JetTrigMaxEta ||
      smearedEtMeV3<m_JetTrig3jMin || fabs(eta3)>m_JetTrigMaxEta) {
    return 0.;
  } else {
    return 0.95;
  }
}

float UpgradePerformanceFunctions::getFourJetTriggerEfficiency(float smearedEtMeV1, float eta1, float smearedEtMeV2, float eta2, float smearedEtMeV3, float eta3, float smearedEtMeV4, float eta4) {
  if (smearedEtMeV1<m_JetTrig4jMin || fabs(eta1)>m_JetTrigMaxEta ||
      smearedEtMeV2<m_JetTrig4jMin || fabs(eta2)>m_JetTrigMaxEta ||
      smearedEtMeV3<m_JetTrig4jMin || fabs(eta3)>m_JetTrigMaxEta ||
      smearedEtMeV4<m_JetTrig4jMin || fabs(eta4)>m_JetTrigMaxEta) {
    return 0.;
  } else {
    return 0.95;
  }
}

float UpgradePerformanceFunctions::getHTTriggerEfficiency(float smearedHTMeV) {
  if (smearedHTMeV < 500000.) {
    return 0.;
  } else {
    return 0.95;
  }
}

float UpgradePerformanceFunctions::getEtmissTriggerEfficiency(float smearedEtmissMeV) {
  if (smearedEtmissMeV < 200000.) {
    return 0.;
  } else {
    return 0.95;
  }
}

float UpgradePerformanceFunctions::getJetEtmissTriggerEfficiency(float smearedJetEtMeV, float jetEta, float smearedEtmissMeV) {
  if (smearedJetEtMeV < 140000. || fabs(jetEta) < m_JetTrigMaxEta || 
      smearedEtmissMeV < 125000.) {
    return 0.;
  } else {
    return 0.95;
  }
}

#endif
