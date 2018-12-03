/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETPERFORMANCEFUNCTIONS_CXX
#define JETPERFORMANCEFUNCTIONS_CXX

#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"
#include "PathResolver/PathResolver.h"

#include <TH2D.h>
#include <TFile.h>
#include <Math/QuantFuncMathCore.h>

namespace Upgrade {
bool comparePt(Jet a, Jet b) { return a.Pt() > b.Pt(); }

void UpgradePerformanceFunctions::setJetAlgorithm(JetAlgorithm jetAlgo) {
  m_jetAlgo = jetAlgo;
}

void UpgradePerformanceFunctions::setConstantTermPenaltyFactor(double CtermPenalty) {
  if (CtermPenalty < -0.02 || CtermPenalty > 0.1)
    ATH_MSG_FATAL(Form("JER C-term penalty must be in range -0.02 to 0.1 - i.e. 2%% improved vs Run-2 (crazy good!) or 10%% degraded."));
  m_JER_Cterm_penalty = CtermPenalty;
}

void UpgradePerformanceFunctions::setJetPtRapidiyCuts(double ptMeV, double absY) {
  m_jet_pTcut = ptMeV;
  m_jet_absYcut = absY;
}

void UpgradePerformanceFunctions::printJetSmearingSettings() {
  ATH_MSG_INFO("----");
  ATH_MSG_INFO("HLLHC jet smearing will be applied parametrized for");
  if (m_jetAlgo == TopoEM) {
    ATH_MSG_INFO("EM+JES topo jets with a JER constant-term penalty of " << m_JER_Cterm_penalty);
  }
  else {
    ATH_MSG_INFO("Particle-flow jets with a JER constant-term penalty of " << m_JER_Cterm_penalty);
  }
  ATH_MSG_INFO("  Jet pT threshold [GeV] :" << m_jet_pTcut / 1000);
  ATH_MSG_INFO("  Jet rapidity cut |y| < " << m_jet_absYcut);
  ATH_MSG_INFO("  Jet random seed: " << m_jetRandom.GetSeed());
  ATH_MSG_INFO("  AvgIntPerXing: " << m_avgMu);
  if (m_pueff == PU) {
    ATH_MSG_INFO("  JVT operating point: " << m_fEff * 100 << "% flat PU-jet efficiency");
  }
  else {
    ATH_MSG_INFO("  JVT operating point: " << m_fEff * 100 << "% flat HS-jet efficiency");
  }
  ATH_MSG_INFO("----");
  std::cout << std::flush;
}


float UpgradePerformanceFunctions::getJetSmearedEnergy(float ptMeV, float eta, bool useJetArea) {
  /*
  static int nwarn = 0;
  if (++nwarn<5)
    std::cout << "WARNING: getJetSmearedEnergy is obsolete. Please update your code as described on "
        << "https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/UpgradePerformanceFunctions"
        << std::endl;
  */

  if (ptMeV < 5000. || ptMeV > 1500000.) {
    ATH_MSG_WARNING(" getJetSmearedEnergy valid only in range 10-1500 GeV)");
    return 0;
  }
  if (fabs(eta) > 4.5) {
    //    std::cout << " getJetSmearedEnergy valid only in eta range (-4.5 - 4.5), current eta: " << eta << std::endl;
    return 0;
  }

  double mysigma = getJetEnergyResolution(ptMeV, eta, useJetArea);
  double smearFactor = m_jetRandom.Gaus(1, mysigma);
  return std::max(ptMeV * smearFactor, 1E-08);
}

double UpgradePerformanceFunctions::getJER(double ptMeV, double eta, double mu, JetAlgorithm jetAlgo) {
  double absEta = std::abs(eta), pT = ptMeV / 1000;

  // Simplified Run-2 JER after EM+JES and GSC. This is for 2016 conditions, so mu~22
  // Derived by Jeff Dandoy and parmetrized (simplified) by Dag Gillberg
  static TGraph *g_N      = makeGraph({ 0.0, 0.7, 1.0, 1.35, 1.45, 1.9, 3.2, 3.35, 3.5, 3.8, 5.0},
  { 3.8, 3.8, 4.5, 4.5, 5.6, 4.0, 3.0, 6.2, 2.75, 2.75, 2.75});
  static TGraph *g_pflowN = makeGraph({ 0.0, 0.7, 1.0, 1.35, 1.45, 1.9, 3.2, 3.35, 3.5, 3.8, 5.0},
  { -2.0, -2.0, -0.5, 2.5, 3.2, 1.5, 1.5, 4.0, 1.8, 2.75, 2.75});
  static TGraph *g_S      = makeGraph({ 0.0, 0.6, 1.4, 2.25, 3.00, 3.25, 3.5, 5.0},
  {0.75, 0.75, 0.85, 0.65, 0.80, 1.05, 0.8, 0.8});
  static TGraph *g_pflowS = makeGraph({ 0.0, 0.6, 1.4, 2.25, 3.00, 3.25, 3.5, 5.0},
  {0.88, 0.88, 1.05, 0.80, 0.80, 1.05, 0.80, 0.80});
  static TGraph *g_C      = makeGraph({0.0, 1.3, 1.5, 2.8, 3.1, 3.4, 5.0},
  {0.023, 0.023, 0.03, 0.03, 0.07, 0.045, 0.045});

  // For HL-LHC, the S and C terms are assumed unchanged, while the noise term is degraded
  // with mu. The noise term above is for mu=22 (2016 Run-2 data with GSC).
  // A change DeltaN to this term is calculated by extrapolation using Run-2 data @ pT~=33 GeV (30-40)
  // For TopoEM jets, this gives very similar results to the ones derived by Chris Delitzsch using
  // Step 1.9 sampels.
  // eta bins and extrapolated resolutions to mu=200 @ pT=33 GeV
  static std::vector<double> etas({0.5, 1.2, 2.1, 2.85, 3.8});
  static std::vector<double> jer200_33GeV({0.362, 0.381, 0.364, 0.385, 0.39});
  static std::vector<double> PFjer200_33GeV({0.275, 0.28, 0.27, 0.36, 0.39});
  static TGraph *g_DN      = new TGraph(5); // TopoEM
  static TGraph *g_pflowDN = new TGraph(5); // PFlow

  static bool first = true;
  static int s_mu = mu;
  if (first || s_mu != mu) {
    // (re)calculate the degradation of the noise term
    for (size_t i = 0; i < etas.size(); ++i) {
      // TopoEM
      double eta = etas[i], N22 = g_N->Eval(eta), S = g_S->Eval(eta), C = g_C->Eval(eta);
      double jer22 = sqrt(pow(N22 / 33, 2) + S * S / 33 + C * C), jer200 = jer200_33GeV[i], jer = jer22 + (jer200 - jer22) * (mu - 22) / 178;
      double N = jer * jer > S * S / 33 + C * C ? sqrt(jer * jer - S * S / 33 - C * C) * 33 : -sqrt(-jer * jer + S * S / 33 + C * C) * 33, DN = N - N22;
      //printf("eta=%.1f TopoEM has nNoise term %.2f @ mu=22, %.2f @ mu=%.0f, Delta: %.2f\n",eta,N22,N,mu,DN);
      g_DN->SetPoint(i, eta, DN);

      // P-flow
      N22 = g_pflowN->Eval(eta); S = g_pflowS->Eval(eta);
      jer22 = sqrt(pow(N22 / 33, 2) + S * S / 33 + C * C); jer200 = PFjer200_33GeV[i]; jer = jer22 + (jer200 - jer22) * (mu - 22) / 178;
      N = jer * jer > S * S / 33 + C * C ? sqrt(jer * jer - S * S / 33 - C * C) * 33 : -sqrt(-jer * jer + S * S / 33 + C * C) * 33; DN = N - N22;
      //printf("eta=%.1f PFlow has nNoise term %.2f @ mu=22, %.2f @ mu=%.0f, Delta: %.2f\n",eta,N22,N,mu,DN);
      g_pflowDN->SetPoint(i, eta, DN);
    }

    first = false;
    s_mu = mu;
  }

  if (jetAlgo == TopoEM) {
    double N22 = g_N->Eval(absEta), S = g_S->Eval(absEta), C = g_C->Eval(absEta) + m_JER_Cterm_penalty;
    double N = N22 + g_DN->Eval(TMath::Range(0.5, 3.8, absEta));
    return sqrt(pow(N / pT, 2) + S * S / pT + C * C);
  }

  // p-flow
  double N22 = g_pflowN->Eval(absEta), S = g_pflowS->Eval(absEta), C = g_C->Eval(absEta) + m_JER_Cterm_penalty;
  double N = N22 + g_pflowDN->Eval(TMath::Range(0.5, 3.8, absEta));
  // Noise term for p-flow can go negative in central region at low mu
  return sqrt(pow(N / pT, 2) * N / std::abs(N) + S * S / pT + C * C);
}

double UpgradePerformanceFunctions::getJER(double ptMeV, double eta) {
  return getJER(ptMeV, eta, m_avgMu, m_jetAlgo);
}

float UpgradePerformanceFunctions::getJetEnergyResolution(float ptMeV, float eta, bool useJetArea) {
  // This currently returns relative energy resolution.

  //eta binning
  //double Etamin[4]={0,0.8,1.2,2.8};
  //double Etamax[4]={0.8,1.2,2.8,4.5};
  eta = fabs(eta);

  double sigma = 0;
  double N = 0; double S = 0; double C = 0;
  double A = 0; double B = 0;
  if ( getLayout() == UpgradeLayout::run2 ) {
    //use RUN2 settings: as from the latest recommendations from JetEtmiss @ https://twiki.cern.ch/twiki/bin/view/AtlasProtected/JetResolution2015Prerecom
    //coded here :  https://svnweb.cern.ch/trac/atlasoff/browser/Reconstruction/Jet/JetResolution/trunk/Root/JERTool.cxx

    // JER::AKt4EM, Cone of 0.4
    if (eta >= 0   && eta < 0.8) {  N = 3.34;     S = 0.627;     C = 0.0234; }
    if (eta >= 0.8 && eta < 1.2) {  N = 3.05;     S = 0.693;     C = 0.0224; }
    if (eta >= 1.2 && eta < 2.1) {  N = 3.29;     S = 0.658;     C = 0.0300; }
    if (eta >= 2.1 && eta < 2.8) {  N = 2.56;     S = 0.607;     C = 0.0250; }
    if (eta >= 2.8 && eta < 3.2) {  N = 0.988;    S = 0.753;     C = 0.0228; }
    if (eta >= 3.2 && eta < 3.6) {  N = 2.74;     S = 0.783;     C = 0.0465; }
    if (eta >= 3.6 && eta < 4.5) {  N = 2.80;     S = 0.623;     C = 0.0000; }

    sigma = sqrt( pow(N / (ptMeV * 0.001), 2) + S * S / (ptMeV * 0.001) + C * C );
    return sigma;
  }

  if ( 0 <= eta && eta < 0.8) {
    C = 0.055;
    S = 0.74;
    if (!useJetArea) {
      A = 3.16;
      B = 0.07;
    }
    else {
      A = 3.2;
      B = 0.035;
    }
  }
  else if (0.8 <= eta && eta < 1.2) {
    C = 0.053;
    S = 0.85;
    if (!useJetArea) {
      A = 3.03;
      B = 0.07;
    }
    else {
      A = 2.9;
      B = 0.04;
    }
  }
  else if ( 1.2 <= eta && eta < 2.8) {
    C = 0.056;
    S = 0.56;
    if (!useJetArea) {
      A = 3.32;
      B = 0.08;
    }
    else {
      A = 3.5;
      B = 0.03;
    }
  }
  else if ( 2.8 <= eta && eta < 4.5) {
    C = 0.054;
    S = 0.83;
    if (!useJetArea) {
      A = 2.84;
      B = 0.11;
    }
    else {
      A = 2.9;
      B = 0.06;
    }
  }
  else {
    ATH_MSG_WARNING("getJetEnergyResolution: eta value not allowed: " << eta );
  }

  N = A + B * m_avgMu; //same as old f_N function
  sigma = sqrt(N * N / (ptMeV / 1000. * ptMeV / 1000.) + S * S / (ptMeV / 1000.) + C * C); //same as old f_reso
  return sigma;
}

// float UpgradePerformanceFunctions::getTrackJetConfirmEff(float ptGeV, float eta) {
//   if (m_avgMu!=150) {
//     std::cout << "WARNING : track-jet confirmation not implemented for mu!=150 !" << std::endl;
//     return 1e6;
//   }
//   if (fabs(eta)>2.1) {
//     std::cout << "WARNING : track-jet confirmation not implemented for abs(eta)>2.1 !" << std::endl;
//     return 1e6;
//   }

//   if (ptGeV<20.) {
//     std::cout << "WARNING : track-jet confirmation not implemented for pT<20000 MeV !" << std::endl;
//     return 0.;
//   }
//   else if (ptGeV<25.)
//     return 0.75;
//   else if (ptGeV<35.)
//     return 0.80;
//   return 0.95;
//}

float UpgradePerformanceFunctions::getJetPtThresholdForFakeRate(float eta, float fakeRate, bool useTrackConfirmation) {
  // Note output is now in MeV
  if (m_avgMu != 150) {
    ATH_MSG_WARNING("fake rate parametrization not implemented for mu!=150 !");
    return 1e6;
  }

  float a = 0, b = 0;
  if (fabs(eta) < 2.1) {
    if (!useTrackConfirmation) {
      a = 36.;
      b = -0.16;
    }
    else {
      a = 18.;
      b = -0.18;
    }
  }
  else {
    if (useTrackConfirmation) {
      ATH_MSG_WARNING("Track Confirmation not available for fake rate parametrization about eta=2.1 !");
      return 1e6;
    }
    if (fabs(eta) < 2.8) {
      a = 32.;
      b = -0.19;
    }
    else if (fabs(eta) < 3.2) {
      a = 31.;
      b = -0.19;
    }
    else if (fabs(eta) < 4.5) {
      a = 19.;
      b = -0.21;
    }
  }
  return (a * pow(fakeRate, b)) * 1000.;
}

float UpgradePerformanceFunctions::getJetSmearedMass(float ptMeV, float massMeV, FatJetType jetType) {
  // Allow the user to load all of the mass smearing functions at the same time
  static TH2D* hJetMassResolutions[UpgradePerformanceFunctions::NUM_FAT_JET_TYPES];

  if (m_avgMu != 200) {
    ATH_MSG_ERROR("getJetSmearedMass available only for mu=200.  Returning -1.");
    return -1.;
  }

  if (ptMeV < 200.*1000. || ptMeV > 3000.*1000.) {
    ATH_MSG_ERROR("getJetSmearedMass available only for 200 < pT < 3000 GeV.  Returning -1.");
    return -1.;
  }

  std::string label = "";
  if      (jetType == UpgradePerformanceFunctions::QCD) label = "QCD";
  else if (jetType == UpgradePerformanceFunctions::Top) label = "Top";
  else if (jetType == UpgradePerformanceFunctions::WZ)  label = "WZ";

  if (! hJetMassResolutions[jetType]) {
    // Load the functions when needed for the first time
    std::string filename = "UpgradePerformanceFunctions/CalibArea-00-01/" + label + "_JMRSmearingFunction_mu200_ITkStep1p9.root";
    std::string resolvedFilename = PathResolverFindCalibFile(filename);
    TFile *m_infile = new TFile(resolvedFilename.c_str(), "READ");
    // Assume that we may access any FatJetType smearing at any time; keep all of them in memory
    TString histoNameInFile("h2_fit_IQRMed_jms_vs_pt_vs_mOpt_etadetbin1_smoothed");
    hJetMassResolutions[jetType] = (TH2D*)m_infile->Get(histoNameInFile);
  }

  int bin = (hJetMassResolutions[jetType])->FindBin(ptMeV / 1000., massMeV / ptMeV);
  double mySigma = (hJetMassResolutions[jetType])->GetBinContent(bin);
  double smearingFactor = m_jetRandom.Gaus(1, mySigma);
  double smearedMassMeV = massMeV * smearingFactor;
  return std::max(smearedMassMeV, 0.);
}

std::vector<double> UpgradePerformanceFunctions::getLargeRjet_JER_JMS_JMR(double ptMeV, double massMeV, LargeRjetTopology jetTopo) {

  // 1. A bunch of health checks
  if (m_avgMu != 200) ATH_MSG_FATAL("Large-R jet smearing only availalbe for mu = 200.");
  if ( jetTopo != Multijet && jetTopo != Wprime && jetTopo != Zprime)
    ATH_MSG_FATAL("Large-R jet smearing only availalbe for QCD, Wprime and Zprime topologies.");

  static int nwarn = 0;
  if ( (ptMeV < 200e3 || ptMeV > 3000e3 ) && ++nwarn < 5 )
    ATH_MSG_WARNING("large-R jet smearing only derived for pT in 200-3000 GeV. Current jet has pT = " << ptMeV * 1e-3 << " GeV.");

  // 2. If it's the first time this method is called, then load the smearing histograms from root files
  static std::map<LargeRjetTopology, std::vector<TH2F*> > largeR_JER_JMS_JMR;
  static bool first = true;
  if (first) {
    TFile *file = TFile::Open(PathResolverFindCalibFile("UpgradePerformanceFunctions/CalibArea-00-01/HLLHC_largeR_jets_smoothSmear_Feb2018.root").c_str());
    largeR_JER_JMS_JMR[Multijet] = std::vector<TH2F*>({(TH2F*)file->Get("QCD_JER"), (TH2F*)file->Get("QCD_JMS"), (TH2F*)file->Get("QCD_JMR")});
    largeR_JER_JMS_JMR[Wprime]   = std::vector<TH2F*>({(TH2F*)file->Get("Wprime_JER"), (TH2F*)file->Get("Wprime_JMS"), (TH2F*)file->Get("Wprime_JMR")});
    largeR_JER_JMS_JMR[Zprime]   = std::vector<TH2F*>({(TH2F*)file->Get("Zprime_JER"), (TH2F*)file->Get("Zprime_JMS"), (TH2F*)file->Get("Zprime_JMR")});
    first = false;
  }

  // 3. Extract the parametrized resolution for the current jet
  double massOpT = massMeV / ptMeV;
  ptMeV = TMath::Range(200e3, 2999e3, ptMeV); // smearing only determined in range 200-3000 GeV
  return { largeR_JER_JMS_JMR[jetTopo][0]->Interpolate(ptMeV / 1000, massOpT),
           largeR_JER_JMS_JMR[jetTopo][1]->Interpolate(ptMeV / 1000, massOpT),
           largeR_JER_JMS_JMR[jetTopo][2]->Interpolate(ptMeV / 1000, massOpT) };
}

TLorentzVector UpgradePerformanceFunctions::getSmearedLargeRjet(double ptMeV, double eta, double phi, double massMeV, LargeRjetTopology jetTopo) {
  TLorentzVector jet;
  auto smear = getLargeRjet_JER_JMS_JMR(ptMeV, massMeV, jetTopo);

  // For the scale of the jet, sample from a Gaussian with mean 1 and width=JER (relative pT resolution()
  double scaleSmear = m_jetRandom.Gaus(1.0, smear[0]); // JER

  // For the mass resolution of a jet is measured in bins of truth m/pT and is treated as
  // uncorrelated to the scale (pT,E). The mass resposne distribution does not follow a Gaussian.
  // Log normal is a much better approximation.
  double median = smear[1], stdDev = smear[2], var = stdDev * stdDev;
  // log normal propoerties. Extract m and s paramters from median and standadDeviation
  double m = log(median), s = sqrt(log(1 + sqrt(1.0 + 4.0 * var * exp(-2 * m))) - log(2));
  double massSmear  = ROOT::Math::lognormal_quantile(m_jetRandom.Rndm(), m, s); // smeargin of m/pT
  jet.SetPtEtaPhiM(ptMeV * scaleSmear, eta, phi, massMeV * massSmear);
  return jet;
}

TLorentzVector UpgradePerformanceFunctions::getSmearedLargeRjet(TLorentzVector jet, LargeRjetTopology jetTopo) {
  return getSmearedLargeRjet(jet.Pt(), jet.Eta(), jet.Phi(), jet.M(), jetTopo);
}

TLorentzVector UpgradePerformanceFunctions::getSmearedJet(double ptMeV, double eta, double phi, double massMeV) {
  TLorentzVector jet;
  jet.SetPtEtaPhiM(ptMeV, eta, phi, massMeV);
  return getSmearedJet(jet);
}

TLorentzVector UpgradePerformanceFunctions::getSmearedJet(TLorentzVector tjet_MeV) {
  // Access the relative JER and smear!
  //   Full jet four vector scaled by Gaussian smearing factor
  double jer = getJER(tjet_MeV.Pt(), tjet_MeV.Eta());
  double smearFactor = m_jetRandom.Gaus(1.0, jer);
  if (smearFactor < 1e-3) smearFactor = 1e-3; // avoid negative energy
  return tjet_MeV * smearFactor;
}

Upgrade::Jets UpgradePerformanceFunctions::getSmearedJets(Upgrade::Jets hsJets) {
  Upgrade::Jets all_jets;

  // first time method is used, print the settings
  static bool first = true;
  if (first) {
    printJetSmearingSettings();
    first = false;
  }

  // a. smear the HS jets and insert them to all_jets;
  for (Upgrade::Jet hsJet : hsJets) {
    Upgrade::Jet jet = getSmearedJet(hsJet);
    jet.truthIsHS = true;
    // copy flavour, in case user provided. Default is -99 = unknown
    jet.truthFlavour = hsJet.truthFlavour;
    //jet.print(Form("HS jet, JVT eff: %.3f",getJVTeff(jet)));
    all_jets.push_back(jet);
  }

  // b. Access PU jets and insert them to all_jets
  auto PUjets = getPileupJets();
  for (auto pujet : PUjets) {
    Upgrade::Jet jet = pujet;
    jet.truthIsHS = false;
    all_jets.push_back(jet);
    //jet.print(Form("PU jet, JVT eff: %.3f",getJVTeff(jet)));
  }

  // sort the jets by pT
  std::sort(all_jets.begin(), all_jets.end(), Upgrade::comparePt);

  // c. Apply pT and y selection and
  // d. JVT selection
  Upgrade::Jets jets;
  for (auto jet : all_jets) {

    // Apply jet pT threshold
    if ( jet.Pt() < m_jet_pTcut ) continue;
    // And rapidity selection
    if ( std::abs(jet.Rapidity()) > m_jet_absYcut) continue;

    // See if the jet pass the JVT cut
    if ( m_jetRandom.Rndm() > getJVTeff(jet) ) continue;

    // If we get here, we survivided pT, y and JVT selection
    jets.push_back(jet);
  }
  // return selected, smeared and PU overlayed jets
  return jets;
}

/*
// Also b-tag jets
Upgrade::Jets UpgradePerformanceFunctions::getSmearedBtaggedJets(Upgrade::Jets hsJets, TString btagAlgo, int btagWP) {
  Upgrade::Jets jets = getSmearedJets(hsJets);
  for (auto &jet : jets) {
    // extract the truth flavour
    const int &flav = jet.truthFlavour;
    char flavChar = flav == 5 ? 'B' : flav == 4 ? 'C' : 'L';
    if (!jet.truthIsHS) flavChar = 'P';
    double btagEff = getFlavourTagEfficiency(jet.Pt(),jet.Eta(),flavChar,btagAlgo,btagWP,getPileupTrackConfSetting());
    jet.isBtagged = m_jetRandom.Rndm() < btagEff;
  }
  return jets;
}
*/
}

#endif
