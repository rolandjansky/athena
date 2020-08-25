/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TElectronLikelihoodTool.h"
#include "TFile.h" // for TFile
#include "TH1.h"   // for TH1F
#include "TROOT.h"
#include "TString.h" // for TString
#include "TSystem.h"
#include <algorithm> // for min
#include <cmath>
#include <cstdio>  // for sprintf
#include <fstream> // for char_traits

#include "ElectronPhotonSelectorTools/ElectronSelectorHelpers.h"

/**
Author : Kurt Brendlinger <kurb@sas.upenn.edu>
Please see TElectronLikelihoodTool.h for usage.
*/

//=============================================================================
// Constructor
//=============================================================================

//----------------------------------------------------------------------------------------
Root::TElectronLikelihoodTool::TElectronLikelihoodTool(const char* name)
  : asg::AsgMessaging(std::string(name))
  , m_doRemoveF3AtHighEt(false)
  , m_doRemoveTRTPIDAtHighEt(false)
  , m_doSmoothBinInterpolation(false)
  , m_useOneExtraHighETLHBin(false)
  , m_highETBinThreshold(125)
  , m_doPileupTransform(false)
  , m_doCentralityTransform(false)
  , m_discMaxForPileupTransform(2.0)
  , m_pileupMaxForPileupTransform(50)
  , m_variableNames("")
  , m_pdfFileName("")
  , m_name(name)
  , m_variableBitMask(0x0)
  , m_ipBinning("")
  , m_pdfFile(nullptr)
  , m_cutPosition_kinematic(-9)
  , m_cutPosition_NSilicon(-9)
  , m_cutPosition_NPixel(-9)
  , m_cutPosition_NBlayer(-9)
  , m_cutPosition_ambiguity(-9)
  , m_cutPosition_LH(-9)
  , m_cutPositionTrackA0(-9)
  , m_cutPositionTrackMatchEta(-9)
  , m_cutPositionTrackMatchPhiRes(-9)
  , m_cutPositionWstotAtHighET(-9)
  , m_cutPositionEoverPAtHighET(-9)
{
  for (unsigned int varIndex = 0; varIndex < s_fnVariables; varIndex++) {
    for (unsigned int s_or_b = 0; s_or_b < 2; s_or_b++) {
      for (unsigned int ip = 0; ip < IP_BINS; ip++) {
        for (unsigned int et = 0; et < s_fnEtBinsHist; et++) {
          for (unsigned int eta = 0; eta < s_fnEtaBins; eta++) {
            fPDFbins[s_or_b][ip][et][eta][varIndex] = nullptr;
          }
        }
      }
    }
  }
}

//=============================================================================
// Destructor
//=============================================================================
Root::TElectronLikelihoodTool::~TElectronLikelihoodTool()
{
  for (unsigned int varIndex = 0; varIndex < s_fnVariables; varIndex++) {
    for (unsigned int s_or_b = 0; s_or_b < 2; s_or_b++) {
      for (unsigned int ip = 0; ip < IP_BINS; ip++) {
        for (unsigned int et = 0; et < s_fnEtBinsHist; et++) {
          for (unsigned int eta = 0; eta < s_fnEtaBins; eta++) {
            if (fPDFbins[s_or_b][ip][et][eta][varIndex]) {
              delete fPDFbins[s_or_b][ip][et][eta][varIndex];
              fPDFbins[s_or_b][ip][et][eta][varIndex] = nullptr;
            }
          }
        }
      }
    }
  }
}

StatusCode
Root::TElectronLikelihoodTool::initialize()
{
  ATH_MSG_DEBUG("TElectronLikelihoodTool initialize.");

  // use an int as a StatusCode
  StatusCode sc(StatusCode::SUCCESS);

  // Check that all needed variables are setup
  if (m_pdfFileName.empty()) {
    ATH_MSG_WARNING("You need to specify the input PDF file name before you "
                    "call initialize() with "
                    "setPDFFileName('your/file/name.root') ");
    sc = StatusCode::FAILURE;
  }

  unsigned int number_of_expected_bin_combinedLH;
  if (m_useOneExtraHighETLHBin)
    number_of_expected_bin_combinedLH = s_fnDiscEtBinsOneExtra * s_fnEtaBins;
  else
    number_of_expected_bin_combinedLH = s_fnDiscEtBins * s_fnEtaBins;
  unsigned int number_of_expected_bin_combinedOther = s_fnDiscEtBins * s_fnEtaBins;

  if (m_cutLikelihood.size() != number_of_expected_bin_combinedLH) {
    ATH_MSG_ERROR("Configuration issue :  cutLikelihood expected size "
                  << number_of_expected_bin_combinedLH << " input size " << m_cutLikelihood.size());
    sc = StatusCode::FAILURE;
  }

  if (!m_discHardCutForPileupTransform.empty()) {
    if (m_discHardCutForPileupTransform.size() != number_of_expected_bin_combinedLH) {
      ATH_MSG_ERROR("Configuration issue :   DiscHardCutForPileupTransform expected size "
                    << number_of_expected_bin_combinedLH << " input size "
                    << m_discHardCutForPileupTransform.size());
      sc = StatusCode::FAILURE;
    }
  }
  if (!m_discHardCutSlopeForPileupTransform.empty()) {
    if (m_discHardCutSlopeForPileupTransform.size() != number_of_expected_bin_combinedLH) {
      ATH_MSG_ERROR("Configuration issue :   "
                    "DiscHardCutSlopeForPileupTransform expected size "
                    << number_of_expected_bin_combinedLH << " input size "
                    << m_discHardCutSlopeForPileupTransform.size());
      sc = StatusCode::FAILURE;
    }
  }
  if (!m_discLooseForPileupTransform.empty()) {
    if (m_discLooseForPileupTransform.size() != number_of_expected_bin_combinedLH) {
      ATH_MSG_ERROR("Configuration issue :   DiscLooseForPileupTransform expected size "
                    << number_of_expected_bin_combinedLH << " input size "
                    << m_discLooseForPileupTransform.size());
      sc = StatusCode::FAILURE;
    }
  }

  // d0 cut
  if (!m_cutA0.empty()) {
    if (m_cutA0.size() != number_of_expected_bin_combinedOther) {
      ATH_MSG_ERROR("Configuration issue :   CutA0  expected size "
                    << number_of_expected_bin_combinedOther << " input size " << m_cutA0.size());
      sc = StatusCode::FAILURE;
    }
  }

  // deltaEta cut
  if (!m_cutDeltaEta.empty()) {
    if (m_cutDeltaEta.size() != number_of_expected_bin_combinedOther) {
      ATH_MSG_ERROR("Configuration issue :  CutDeltaEta  expected size "
                    << number_of_expected_bin_combinedOther << " input size "
                    << m_cutDeltaEta.size());
      sc = StatusCode::FAILURE;
    }
  }

  // deltaPhiRes cut
  if (!m_cutDeltaPhiRes.empty()) {
    if (m_cutDeltaPhiRes.size() != number_of_expected_bin_combinedOther) {
      ATH_MSG_ERROR("Configuration issue :  CutDeltaPhiRes  expected size "
                    << number_of_expected_bin_combinedOther << " input size "
                    << m_cutDeltaPhiRes.size());
      sc = StatusCode::FAILURE;
    }
  }
  if (sc == StatusCode::FAILURE) {
    ATH_MSG_ERROR("Could NOT initialize! Please fix the errors mentioned above...");
    return sc;
  }

  // --------------------------------------------------------------------------
  // Register the cuts and check that the registration worked:
  // NOTE: THE ORDER IS IMPORTANT!!! Cut0 corresponds to bit 0, Cut1 to bit
  // 1,... Cut position for the kineatic pre-selection
  m_cutPosition_kinematic = m_acceptInfo.addCut("kinematic", "pass kinematic");
  if (m_cutPosition_kinematic < 0) {
    sc = StatusCode::FAILURE;
  }

  // NSilicon
  m_cutPosition_NSilicon = m_acceptInfo.addCut("NSilicon", "pass NSilicon");
  if (m_cutPosition_NSilicon < 0) {
    sc = StatusCode::FAILURE;
  }

  // NPixel
  m_cutPosition_NPixel = m_acceptInfo.addCut("NPixel", "pass NPixel");
  if (m_cutPosition_NPixel < 0) {
    sc = StatusCode::FAILURE;
  }

  // NBlayer
  m_cutPosition_NBlayer = m_acceptInfo.addCut("NBlayer", "pass NBlayer");
  if (m_cutPosition_NBlayer < 0) {
    sc = StatusCode::FAILURE;
  }

  // Ambiguity
  m_cutPosition_ambiguity = m_acceptInfo.addCut("ambiguity", "pass ambiguity");
  if (m_cutPosition_ambiguity < 0) {
    sc = StatusCode::FAILURE;
  }

  // Cut position for the likelihood selection - DO NOT CHANGE ORDER!
  m_cutPosition_LH = m_acceptInfo.addCut("passLH", "pass Likelihood");
  if (m_cutPosition_LH < 0) {
    sc = StatusCode::FAILURE;
  }

  // D0
  m_cutPositionTrackA0 = m_acceptInfo.addCut("TrackA0", "A0 (aka d0) wrt beam spot < Cut");
  if (m_cutPositionTrackA0 < 0) {
    sc = StatusCode::FAILURE;
  }

  // deltaeta
  m_cutPositionTrackMatchEta =
    m_acceptInfo.addCut("TrackMatchEta", "Track match deta in 1st sampling < Cut");
  if (m_cutPositionTrackMatchEta < 0) {
    sc = StatusCode::FAILURE;
  }

  // deltaphi
  m_cutPositionTrackMatchPhiRes =
    m_acceptInfo.addCut("TrackMatchPhiRes", "Track match dphi in 2nd sampling, rescaled < Cut");
  if (m_cutPositionTrackMatchPhiRes < 0) {
    sc = StatusCode::FAILURE;
  }

  // Wstot
  m_cutPositionWstotAtHighET =
    m_acceptInfo.addCut("WstotAtHighET", "Above HighETBinThreshold, Wstot < Cut");
  if (m_cutPositionWstotAtHighET < 0) {
    sc = StatusCode::FAILURE;
  }

  // EoverP
  m_cutPositionEoverPAtHighET =
    m_acceptInfo.addCut("EoverPAtHighET", "Above HighETBinThreshold, EoverP < Cut");
  if (m_cutPositionEoverPAtHighET < 0) {
    sc = StatusCode::FAILURE;
  }

  // Check that we got everything OK
  if (sc == StatusCode::FAILURE) {
    ATH_MSG_ERROR("! Something went wrong with the setup of the decision objects...");
    return sc;
  }

  // ----------------------------------
  // Get the correct bit mask for the current likelihood operating point
  m_variableBitMask = getLikelihoodBitmask(m_variableNames);

  //----------File/Histo operation------------------------------------
  // Load the ROOT file containing the PDFs
  TString tmpString(m_pdfFileName);
  gSystem->ExpandPathName(tmpString);
  std::string fname(tmpString.Data());
  m_pdfFile = TFile::Open(fname.c_str(), "READ");
  // Check that we could load the ROOT file
  if (!m_pdfFile) {
    ATH_MSG_ERROR(" No ROOT file found here: " << m_pdfFileName);
    return StatusCode::FAILURE;
  }

  // Load the histograms
  for (unsigned int varIndex = 0; varIndex < s_fnVariables; varIndex++) {
    const std::string& vstr = fVariables[varIndex];
    // Skip the loading of PDFs for variables we don't care about for this
    // operating point. If the string is empty (which is true in the default
    // 2012 case), load all of them.
    if (m_variableNames.find(vstr) == std::string::npos && !m_variableNames.empty()) {
      continue;
    }
    loadVarHistograms(vstr, varIndex);
  }

  // TFile close does not free the memory
  m_pdfFile->Close();
  // We need the destructor to be called
  delete m_pdfFile;
  //----------End File/Histo operation------------------------------------

  ATH_MSG_DEBUG("Initialization complete for a LH tool with these specs:"
                << "\n - pdfFileName                                  : " << m_pdfFileName
                << "\n - Variable bitmask                             : " << m_variableBitMask);

  ATH_MSG_DEBUG(
    "\n - VariableNames                                : "
    << m_variableNames
    << "\n - (bool)CutBL (yes/no)                         : " << (!m_cutBL.empty() ? "yes" : "no")
    << "\n - (bool)CutPi (yes/no)                         : " << (!m_cutPi.empty() ? "yes" : "no")
    << "\n - (bool)CutSi (yes/no)                         : " << (!m_cutSi.empty() ? "yes" : "no")
    << "\n - (bool)CutAmbiguity (yes/no)                  : "
    << (!m_cutAmbiguity.empty() ? "yes" : "no")
    << "\n - (bool)doRemoveF3AtHighEt (yes/no)            : "
    << (m_doRemoveF3AtHighEt ? "yes" : "no")
    << "\n - (bool)doRemoveTRTPIDAtHighEt (yes/no)        : "
    << (m_doRemoveTRTPIDAtHighEt ? "yes" : "no")
    << "\n - (bool)doSmoothBinInterpolation (yes/no)      : "
    << (m_doSmoothBinInterpolation ? "yes" : "no")
    << "\n - (bool)useOneExtraHighETLHBin(yes/no)         : "
    << (m_useOneExtraHighETLHBin ? "yes" : "no")
    << "\n - (double)HighETBinThreshold                   : " << m_highETBinThreshold
    << "\n - (bool)doPileupTransform (yes/no)             : "
    << (m_doPileupTransform ? "yes" : "no")
    << "\n - (bool)doCentralityTransform (yes/no)         : "
    << (m_doCentralityTransform ? "yes" : "no")
    << "\n - (bool)CutLikelihood (yes/no)                 : "
    << (!m_cutLikelihood.empty() ? "yes" : "no")
    << "\n - (bool)CutLikelihoodPileupCorrection (yes/no) : "
    << (!m_cutLikelihoodPileupCorrection.empty() ? "yes" : "no")
    << "\n - (bool)CutA0 (yes/no)                         : " << (!m_cutA0.empty() ? "yes" : "no")
    << "\n - (bool)CutDeltaEta (yes/no)                   : "
    << (!m_cutDeltaEta.empty() ? "yes" : "no")
    << "\n - (bool)CutDeltaPhiRes (yes/no)                : "
    << (!m_cutDeltaPhiRes.empty() ? "yes" : "no")
    << "\n - (bool)CutWstotAtHighET (yes/no)              : "
    << (!m_cutWstotAtHighET.empty() ? "yes" : "no")
    << "\n - (bool)CutEoverPAtHighET (yes/no)             : "
    << (!m_cutEoverPAtHighET.empty() ? "yes" : "no"));
  return sc;
}

int
Root::TElectronLikelihoodTool::loadVarHistograms(const std::string& vstr, unsigned int varIndex)
{
  for (unsigned int s_or_b = 0; s_or_b < 2; s_or_b++) {
    for (unsigned int ip = 0; ip < IP_BINS; ip++) {
      for (unsigned int et = 0; et < s_fnEtBinsHist; et++) {
        for (unsigned int eta = 0; eta < s_fnEtaBins; eta++) {

          std::string sig_bkg = (s_or_b == 0) ? "sig" : "bkg";
          // Because eta bins in the root file don't match up exactly with cut
          // menu definitions, the second eta bin is an exact copy of the first,
          // and all subsequent eta bins are pushed back by one.
          unsigned int eta_tmp = (eta > 0) ? eta - 1 : eta;
          // The 7-10 GeV, crack bin uses the 10-15 Gev pdfs. WE DO NOT DO THIS
          // ANYMORE! unsigned int et_tmp = (eta == 5 && et == 1) ? 1 : et;
          unsigned int et_tmp = et;
          char binname[200];
          getBinName(binname, et_tmp, eta_tmp, ip, m_ipBinning);

          if (((std::string(binname).find("2.37") != std::string::npos)) &&
              (vstr.find("el_f3") != std::string::npos))
            continue;

          if (((std::string(binname).find("2.01") != std::string::npos) ||
               (std::string(binname).find("2.37") != std::string::npos)) &&
              (vstr.find("TRT") != std::string::npos))
            continue;

          char pdfdir[500];
          snprintf(pdfdir, 500, "%s/%s", vstr.c_str(), sig_bkg.c_str());
          char pdf[500];
          snprintf(
            pdf, 500, "%s_%s_smoothed_hist_from_KDE_%s", vstr.c_str(), sig_bkg.c_str(), binname);
          char pdf_newname[500];
          snprintf(pdf_newname,
                   500,
                   "%s_%s_%s_LHtool_copy_%s",
                   m_name.c_str(),
                   vstr.c_str(),
                   sig_bkg.c_str(),
                   binname);

          if (!m_pdfFile->GetListOfKeys()->Contains(vstr.c_str())) {
            ATH_MSG_INFO("Warning: skipping variable " << vstr
                                                       << " because the folder does not exist.");
            return 1;
          }
          if (!((TDirectory*)m_pdfFile->Get(vstr.c_str()))
                 ->GetListOfKeys()
                 ->Contains(sig_bkg.c_str())) {
            ATH_MSG_INFO("Warning: skipping variable " << vstr
                                                       << " because the folder does not exist.");
            return 1;
          }

          // We only need to load PDFs
          // up to a certain ET value (40 GeV)
          if (et > s_fnEtBinsHist - 1) {
            continue;
          }

          // If the 0th et bin (4-7 GeV) histogram does not exist in the root
          // file, then just use the 7-10 GeV bin histogram. This should
          // preserve backward compatibility
          if (et == 0 && !((TDirectory*)m_pdfFile->Get(pdfdir))->GetListOfKeys()->Contains(pdf)) {
            getBinName(binname, et_tmp + 1, eta_tmp, ip, m_ipBinning);
            snprintf(
              pdf, 500, "%s_%s_smoothed_hist_from_KDE_%s", vstr.c_str(), sig_bkg.c_str(), binname);
            snprintf(pdf_newname,
                     500,
                     "%s_%s_%s_LHtool_copy4GeV_%s",
                     m_name.c_str(),
                     vstr.c_str(),
                     sig_bkg.c_str(),
                     binname);
          }
          if (((TDirectory*)m_pdfFile->Get(pdfdir))->GetListOfKeys()->Contains(pdf)) {
            TH1F* hist = (TH1F*)(((TDirectory*)m_pdfFile->Get(pdfdir))->Get(pdf));
            fPDFbins[s_or_b][ip][et][eta][varIndex] = new EGSelectors::SafeTH1(hist);
            delete hist;
          } else {
            ATH_MSG_INFO("Warning: Object " << pdf << " does not exist.");
            ATH_MSG_INFO("Skipping all other histograms with this variable.");
            return 1;
          }
        }
      }
    }
  }
  return 1;
}

asg::AcceptData
Root::TElectronLikelihoodTool::accept(double likelihood,
                                      double eta,
                                      double eT,
                                      int nSiHitsPlusDeadSensors,
                                      int nPixHitsPlusDeadSensors,
                                      bool passBLayerRequirement,
                                      uint8_t ambiguityBit,
                                      double d0,
                                      double deltaEta,
                                      double deltaphires,
                                      double wstot,
                                      double EoverP,
                                      double ip) const
{
  LikeEnum::LHAcceptVars_t vars;
  vars.likelihood = likelihood;
  vars.eta = eta;
  vars.eT = eT;
  vars.nSiHitsPlusDeadSensors = nSiHitsPlusDeadSensors;
  vars.nPixHitsPlusDeadSensors = nPixHitsPlusDeadSensors;
  vars.passBLayerRequirement = passBLayerRequirement;
  vars.ambiguityBit = ambiguityBit;
  vars.d0 = d0;
  vars.deltaEta = deltaEta;
  vars.deltaphires = deltaphires;
  vars.wstot = wstot;
  vars.EoverP = EoverP;
  vars.ip = ip;

  return accept(vars);
}

// This method calculates if the current electron passes the requested
// likelihood cut
asg::AcceptData
Root::TElectronLikelihoodTool::accept(LikeEnum::LHAcceptVars_t& vars_struct) const
{
  // Setup return accept with AcceptInfo
  asg::AcceptData acceptData(&m_acceptInfo);

  // Set up the individual cuts
  bool passKine(true);
  bool passNSilicon(true);
  bool passNPixel(true);
  bool passNBlayer(true);
  bool passAmbiguity(true);
  bool passLH(true);
  bool passTrackA0(true);
  bool passDeltaEta(true);
  bool passDeltaPhiRes(true);
  bool passWstotAtHighET(true);
  bool passEoverPAtHighET(true);

  if (std::abs(vars_struct.eta) > 2.47) {
    ATH_MSG_DEBUG("This electron is std::abs(eta)>2.47 Returning False.");
    passKine = false;
  }

  unsigned int etbinLH = getLikelihoodEtDiscBin(vars_struct.eT, true);
  unsigned int etbinOther = getLikelihoodEtDiscBin(vars_struct.eT, false);
  unsigned int etabin = getLikelihoodEtaBin(vars_struct.eta);

  // sanity
  if (etbinLH >= s_fnDiscEtBinsOneExtra) {
    ATH_MSG_WARNING("Cannot evaluate likelihood for Et " << vars_struct.eT
                                                         << ". Returning false..");
    passKine = false;
  }
  // sanity
  if (etbinOther >= s_fnDiscEtBins) {
    ATH_MSG_WARNING("Cannot evaluate likelihood for Et " << vars_struct.eT
                                                         << ". Returning false..");
    passKine = false;
  }

  // Return if the kinematic requirements are not fulfilled
  acceptData.setCutResult(m_cutPosition_kinematic, passKine);
  if (!passKine) {
    return acceptData;
  }

  // ambiguity bit
  if (!m_cutAmbiguity.empty()) {
    if (!ElectronSelectorHelpers::passAmbiguity(
          (xAOD::AmbiguityTool::AmbiguityType)vars_struct.ambiguityBit, m_cutAmbiguity[etabin])) {
      ATH_MSG_DEBUG("Likelihood macro: ambiguity Bit Failed.");
      passAmbiguity = false;
    }
  }

  // blayer cut
  if (!m_cutBL.empty()) {
    if (m_cutBL[etabin] == 1 && !vars_struct.passBLayerRequirement) {
      ATH_MSG_DEBUG("Likelihood macro: Blayer cut failed.");
      passNBlayer = false;
    }
  }
  // pixel cut
  if (!m_cutPi.empty()) {
    if (vars_struct.nPixHitsPlusDeadSensors < m_cutPi[etabin]) {
      ATH_MSG_DEBUG("Likelihood macro: Pixels Failed.");
      passNPixel = false;
    }
  }
  // silicon cut
  if (!m_cutSi.empty()) {
    if (vars_struct.nSiHitsPlusDeadSensors < m_cutSi[etabin]) {
      ATH_MSG_DEBUG("Likelihood macro: Silicon Failed.");
      passNSilicon = false;
    }
  }

  double cutDiscriminant;
  unsigned int ibin_combinedLH =
    etbinLH * s_fnEtaBins + etabin; // Must change if number of eta bins
                                    // changes!. Also starts from 7-10 GeV bin.
  unsigned int ibin_combinedOther =
    etbinOther * s_fnEtaBins + etabin; // Must change if number of eta bins changes!. Also
                                       // starts from 7-10 GeV bin.

  if (!m_cutLikelihood.empty()) {
    // To protect against a binning mismatch, which should never happen
    if (ibin_combinedLH >= m_cutLikelihood.size()) {
      ATH_MSG_ERROR("The desired eta/pt bin "
                    << ibin_combinedLH << " is outside of the range specified by the input"
                    << m_cutLikelihood.size() << "This should never happen!");
      return acceptData;
    }

    if (m_doSmoothBinInterpolation) {
      cutDiscriminant =
        InterpolateCuts(m_cutLikelihood, m_cutLikelihood4GeV, vars_struct.eT, vars_struct.eta);
      if (!m_doPileupTransform && !m_cutLikelihoodPileupCorrection.empty() &&
          !m_cutLikelihoodPileupCorrection4GeV.empty())
        cutDiscriminant += vars_struct.ip * InterpolateCuts(m_cutLikelihoodPileupCorrection,
                                                            m_cutLikelihoodPileupCorrection4GeV,
                                                            vars_struct.eT,
                                                            vars_struct.eta);
    } else {
      if (vars_struct.eT > 7000. || m_cutLikelihood4GeV.empty()) {
        cutDiscriminant = m_cutLikelihood[ibin_combinedLH];
        // If doPileupTransform, then correct the discriminant itself instead of
        // the cut value
        if (!m_doPileupTransform && !m_cutLikelihoodPileupCorrection.empty()) {
          cutDiscriminant += vars_struct.ip * m_cutLikelihoodPileupCorrection[ibin_combinedLH];
        }
      } else {
        cutDiscriminant = m_cutLikelihood4GeV[etabin];
        if (!m_doPileupTransform && !m_cutLikelihoodPileupCorrection4GeV.empty())
          cutDiscriminant += vars_struct.ip * m_cutLikelihoodPileupCorrection4GeV[etabin];
      }
    }

    // Determine if the calculated likelihood value passes the cut
    ATH_MSG_DEBUG("Likelihood macro: Discriminant: ");
    if (vars_struct.likelihood < cutDiscriminant) {
      ATH_MSG_DEBUG("Likelihood macro: Disciminant Cut Failed.");
      passLH = false;
    }
  }

  // d0 cut
  if (!m_cutA0.empty()) {
    if (std::abs(vars_struct.d0) > m_cutA0[ibin_combinedOther]) {
      ATH_MSG_DEBUG("Likelihood macro: D0 Failed.");
      passTrackA0 = false;
    }
  }

  // deltaEta cut
  if (!m_cutDeltaEta.empty()) {
    if (std::abs(vars_struct.deltaEta) > m_cutDeltaEta[ibin_combinedOther]) {
      ATH_MSG_DEBUG("Likelihood macro: deltaEta Failed.");
      passDeltaEta = false;
    }
  }

  // deltaPhiRes cut
  if (!m_cutDeltaPhiRes.empty()) {
    if (std::abs(vars_struct.deltaphires) > m_cutDeltaPhiRes[ibin_combinedOther]) {
      ATH_MSG_DEBUG("Likelihood macro: deltaphires Failed.");
      passDeltaPhiRes = false;
    }
  }

  // Only do this above HighETBinThreshold [in GeV]
  if (vars_struct.eT > m_highETBinThreshold * 1000) {
    // wstot cut
    if (!m_cutWstotAtHighET.empty()) {
      if (std::abs(vars_struct.wstot) > m_cutWstotAtHighET[etabin]) {
        ATH_MSG_DEBUG("Likelihood macro: wstot Failed.");
        passWstotAtHighET = false;
      }
    }

    // EoverP cut
    if (!m_cutEoverPAtHighET.empty()) {
      if (std::abs(vars_struct.EoverP) > m_cutEoverPAtHighET[etabin]) {
        ATH_MSG_DEBUG("Likelihood macro: EoverP Failed.");
        passEoverPAtHighET = false;
      }
    }
  }

  // Set the individual cut bits in the return object
  acceptData.setCutResult(m_cutPosition_NSilicon, passNSilicon);
  acceptData.setCutResult(m_cutPosition_NPixel, passNPixel);
  acceptData.setCutResult(m_cutPosition_NBlayer, passNBlayer);
  acceptData.setCutResult(m_cutPosition_ambiguity, passAmbiguity);
  acceptData.setCutResult(m_cutPosition_LH, passLH);
  acceptData.setCutResult(m_cutPositionTrackA0, passTrackA0);
  acceptData.setCutResult(m_cutPositionTrackMatchEta, passDeltaEta);
  acceptData.setCutResult(m_cutPositionTrackMatchPhiRes, passDeltaPhiRes);
  acceptData.setCutResult(m_cutPositionWstotAtHighET, passWstotAtHighET);
  acceptData.setCutResult(m_cutPositionEoverPAtHighET, passEoverPAtHighET);
  return acceptData;
}

double
Root::TElectronLikelihoodTool::calculate(double eta,
                                         double eT,
                                         double f3,
                                         double rHad,
                                         double rHad1,
                                         double Reta,
                                         double w2,
                                         double f1,
                                         double eratio,
                                         double deltaEta,
                                         double d0,
                                         double d0sigma,
                                         double rphi,
                                         double deltaPoverP,
                                         double deltaphires,
                                         double TRT_PID,
                                         double ip) const
{

  LikeEnum::LHCalcVars_t vars;

  vars.eta = eta;
  vars.eT = eT;
  vars.f3 = f3;
  vars.rHad = rHad;
  vars.rHad1 = rHad1;
  vars.Reta = Reta;
  vars.w2 = w2;
  vars.f1 = f1;
  vars.eratio = eratio;
  vars.deltaEta = deltaEta;
  vars.d0 = d0;
  vars.d0sigma = d0sigma;
  vars.rphi = rphi;
  vars.deltaPoverP = deltaPoverP;
  vars.deltaphires = deltaphires;
  vars.TRT_PID = TRT_PID;
  vars.ip = ip;

  return calculate(vars);
}

// The main public method to actually calculate the likelihood value
double
Root::TElectronLikelihoodTool::calculate(LikeEnum::LHCalcVars_t& vars_struct) const
{
  // Reset the results to defaul values
  double result = -999;

  unsigned int etabin = getLikelihoodEtaBin(vars_struct.eta);
  double rhad_corr;
  if (etabin == 3 || etabin == 4) {
    rhad_corr = vars_struct.rHad;
  } else {
    rhad_corr = vars_struct.rHad1;
  }
  double d0significance =
    vars_struct.d0sigma == 0 ? 0. : std::abs(vars_struct.d0) / vars_struct.d0sigma;

  std::vector<double> vec = { d0significance,     vars_struct.eratio,      vars_struct.deltaEta,
                              vars_struct.f1,     vars_struct.f3,          vars_struct.Reta,
                              rhad_corr,          vars_struct.rphi,        vars_struct.d0,
                              vars_struct.w2,     vars_struct.deltaPoverP, vars_struct.deltaphires,
                              vars_struct.TRT_PID };
  // Calculate the actual likelihood value and fill the return object
  result = this->evaluateLikelihood(vec, vars_struct.eT, vars_struct.eta, vars_struct.ip);

  return result;
}

double
Root::TElectronLikelihoodTool::evaluateLikelihood(const std::vector<float>& varVector,
                                                  double et,
                                                  double eta,
                                                  double ip) const
{
  std::vector<double> vec;
  for (unsigned int var = 0; var < s_fnVariables; var++) {
    vec.push_back(varVector[var]);
  }
  return evaluateLikelihood(vec, et, eta, ip);
}

double
Root::TElectronLikelihoodTool::evaluateLikelihood(const std::vector<double>& varVector,
                                                  double et,
                                                  double eta,
                                                  double ip) const
{

  const double GeV = 1000;
  unsigned int etbin = getLikelihoodEtHistBin(et); // hist binning
  unsigned int etabin = getLikelihoodEtaBin(eta);
  unsigned int ipbin = getIpBin(ip);

  ATH_MSG_DEBUG("et: " << et << " eta: " << eta << " etbin: " << etbin << " etabin: " << etabin);

  if (etbin >= s_fnEtBinsHist) {
    ATH_MSG_WARNING("skipping etbin " << etbin << ", et " << et);
    return -999.;
  }
  if (etabin >= s_fnEtaBins) {
    ATH_MSG_WARNING("skipping etabin " << etabin << ", eta " << eta);
    return -999.;
  }

  if (varVector.size() != s_fnVariables) {
    ATH_MSG_WARNING("Error! Variable vector size mismatch! Check your vector!");
  }

  double SigmaS = 1.;
  double SigmaB = 1.;

  // define some string constants
  const std::string TRT_string = "TRT";
  const std::string el_f3_string = "el_f3";
  const std::string el_TRT_PID_string = "el_TRT_PID";

  for (unsigned int var = 0; var < s_fnVariables; var++) {

    const std::string& varstr = fVariables[var];

    // Skip variables that are masked off (not used) in the likelihood
    if (!(m_variableBitMask & (0x1 << var))) {
      continue;
    }
    // Don't use TRT for outer eta bins (2.01,2.37)
    if (((etabin == 8) || (etabin == 9)) && (varstr.find(TRT_string) != std::string::npos)) {
      continue;
    }
    // Don't use f3 for outer eta bin (2.37)
    if ((etabin == 9) && (varstr.find(el_f3_string) != std::string::npos)) {
      continue;
    }
    // Don't use f3 for high et (>80 GeV)
    if (m_doRemoveF3AtHighEt && (et > 80 * GeV) &&
        (varstr.find(el_f3_string) != std::string::npos)) {
      continue;
    }
    // Don't use TRTPID for high et (>80 GeV)
    if (m_doRemoveTRTPIDAtHighEt && (et > 80 * GeV) &&
        (varstr.find(el_TRT_PID_string) != std::string::npos)) {
      continue;
    }
    for (unsigned int s_or_b = 0; s_or_b < 2; s_or_b++) {

      int bin = fPDFbins[s_or_b][ipbin][etbin][etabin][var]->FindBin(varVector[var]);

      double prob = 0;
      if (m_doSmoothBinInterpolation) {
        prob = InterpolatePdfs(s_or_b, ipbin, et, eta, bin, var);
      } else {
        double integral = double(fPDFbins[s_or_b][ipbin][etbin][etabin][var]->Integral());
        if (integral == 0) {
          ATH_MSG_WARNING("Error! PDF integral == 0!");
          return -1.35;
        }

        prob = double(fPDFbins[s_or_b][ipbin][etbin][etabin][var]->GetBinContent(bin)) / integral;
      }

      if (s_or_b == 0) {
        SigmaS *= prob;
      } else if (s_or_b == 1) {
        SigmaB *= prob;
      }
    }
  }

  return TransformLikelihoodOutput(SigmaS, SigmaB, ip, et, eta);
}

// --------------------------------------------
double
Root::TElectronLikelihoodTool::TransformLikelihoodOutput(double ps,
                                                         double pb,
                                                         double ip,
                                                         double et,
                                                         double eta) const
{
  // returns transformed or non-transformed output
  // (Taken mostly from TMVA likelihood code)
  double fEpsilon = 1e-99;
  // If both signal and bkg are 0, we want it to fail.
  if (ps < fEpsilon)
    ps = 0;
  if (pb < fEpsilon)
    pb = fEpsilon;
  double disc = ps / double(ps + pb);

  if (disc >= 1.0) {
    disc = 1. - 1.e-15;
  } else if (disc <= 0.0) {
    disc = fEpsilon;
  }

  double tau = 15.0;
  disc = -log(1.0 / disc - 1.0) * (1. / double(tau));

  // Linearly transform the discriminant as a function of pileup, rather than
  // the old scheme of changing the cut value based on pileup. This is simpler
  // for the tuning, as well as ensuring subsets / making discriminants more
  // transparent. In the HI case, a quadratic centrality transform is applied
  // instead.
  if (m_doPileupTransform) {

    // The variables used by the transform:
    //
    // - hard_cut_ref = extremely tight discriminant cut as reference to ensure
    // pileup correction for looser menus is less than for tighter menus.
    // - loose_ref = a loose menu with no pileup correction. Any tighter
    // menu with same inputs will necessarily have pileup dependence built in
    // - disc_max = max disc value for which pileup correction is desired.
    // - pileup_max = max nvtx or mu for calculating the transform. Any larger
    // pileup values will use this maximum value in the transform.

    if (m_discHardCutForPileupTransform.empty() || m_discHardCutSlopeForPileupTransform.empty() ||
        m_discLooseForPileupTransform.empty()) {
      ATH_MSG_WARNING("Vectors needed for pileup-dependent transform not correctly filled! "
                      "Skipping the transform.");
      return disc;
    }

    if (m_doCentralityTransform && m_discHardCutQuadForPileupTransform.empty()) {
      ATH_MSG_WARNING("Vectors needed for centrality-dependent transform not "
                      "correctly filled! "
                      "Skipping the transform.");
      return disc;
    }

    unsigned int etabin = getLikelihoodEtaBin(eta);

    double disc_hard_cut_ref = 0;
    double disc_hard_cut_ref_slope = 0;
    double disc_hard_cut_ref_quad = 0; // only used for heavy ion implementation of the LH
    double disc_loose_ref = 0;
    double disc_max = m_discMaxForPileupTransform;
    double pileup_max = m_pileupMaxForPileupTransform;

    if (m_doSmoothBinInterpolation) {
      disc_hard_cut_ref = InterpolateCuts(
        m_discHardCutForPileupTransform, m_discHardCutForPileupTransform4GeV, et, eta);
      disc_hard_cut_ref_slope = InterpolateCuts(
        m_discHardCutSlopeForPileupTransform, m_discHardCutSlopeForPileupTransform4GeV, et, eta);
      if (m_doCentralityTransform)
        disc_hard_cut_ref_quad = InterpolateCuts(
          m_discHardCutQuadForPileupTransform, m_discHardCutQuadForPileupTransform4GeV, et, eta);
      disc_loose_ref =
        InterpolateCuts(m_discLooseForPileupTransform, m_discLooseForPileupTransform4GeV, et, eta);
    } else {
      // default situation, in the case where 4-7 GeV bin is not defined
      if (et > 7000. || m_discHardCutForPileupTransform4GeV.empty()) {
        unsigned int etfinebinLH = getLikelihoodEtDiscBin(et, true);
        unsigned int ibin_combined = etfinebinLH * s_fnEtaBins + etabin;
        disc_hard_cut_ref = m_discHardCutForPileupTransform[ibin_combined];
        disc_hard_cut_ref_slope = m_discHardCutSlopeForPileupTransform[ibin_combined];
        if (m_doCentralityTransform)
          disc_hard_cut_ref_quad = m_discHardCutQuadForPileupTransform[ibin_combined];
        disc_loose_ref = m_discLooseForPileupTransform[ibin_combined];
      } else {
        if (m_discHardCutForPileupTransform4GeV.empty() ||
            m_discHardCutSlopeForPileupTransform4GeV.empty() ||
            m_discLooseForPileupTransform4GeV.empty()) {
          ATH_MSG_WARNING("Vectors needed for pileup-dependent transform not "
                          "correctly filled for 4-7 GeV "
                          "bin! Skipping the transform.");
          return disc;
        }
        if (m_doCentralityTransform && m_discHardCutQuadForPileupTransform4GeV.empty()) {
          ATH_MSG_WARNING("Vectors needed for centrality-dependent transform "
                          "not correctly filled for 4-7 "
                          "GeV bin! Skipping the transform.");
          return disc;
        }
        disc_hard_cut_ref = m_discHardCutForPileupTransform4GeV[etabin];
        disc_hard_cut_ref_slope = m_discHardCutSlopeForPileupTransform4GeV[etabin];
        if (m_doCentralityTransform)
          disc_hard_cut_ref_quad = m_discHardCutQuadForPileupTransform4GeV[etabin];
        disc_loose_ref = m_discLooseForPileupTransform4GeV[etabin];
      }
    }

    double ip_for_corr = std::min(ip, pileup_max); // turn off correction for values > pileup_max
    double disc_hard_cut_ref_prime = disc_hard_cut_ref + disc_hard_cut_ref_slope * ip_for_corr +
                                     disc_hard_cut_ref_quad * ip_for_corr * ip_for_corr;

    if (disc <= disc_loose_ref) {
      // Below threshold for applying pileup correction
    } else if (disc <= disc_hard_cut_ref_prime) {
      // Between the loose and hard cut reference points for pileup correction
      double denom = double(disc_hard_cut_ref_prime - disc_loose_ref);
      if (denom < 0.001)
        denom = 0.001;
      disc =
        disc_loose_ref + (disc - disc_loose_ref) * (disc_hard_cut_ref - disc_loose_ref) / denom;
    } else if (disc_hard_cut_ref_prime < disc && disc <= disc_max) {
      // Between the hard cut and max reference points for pileup correction
      double denom = double(disc_max - disc_hard_cut_ref_prime);
      if (denom < 0.001)
        denom = 0.001;
      disc = disc_hard_cut_ref +
             (disc - disc_hard_cut_ref_prime) * (disc_max - disc_hard_cut_ref) / denom;
    }
  }

  ATH_MSG_DEBUG("disc is " << disc);
  return disc;
}

const double Root::TElectronLikelihoodTool::fIpBounds[IP_BINS + 1] = { 0., 500. };

//---------------------------------------------------------------------------------------
// Gets the IP bin
unsigned int
Root::TElectronLikelihoodTool::getIpBin(double ip) const
{
  for (unsigned int ipBin = 0; ipBin < IP_BINS; ++ipBin) {
    if (ip < fIpBounds[ipBin + 1])
      return ipBin;
  }
  return 0;
}

//---------------------------------------------------------------------------------------
// Gets the Eta bin [0-9] given the eta
unsigned int
Root::TElectronLikelihoodTool::getLikelihoodEtaBin(double eta) const
{
  const unsigned int nEtaBins = s_fnEtaBins;
  const double etaBins[nEtaBins] = { 0.1, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47 };

  for (unsigned int etaBin = 0; etaBin < nEtaBins; ++etaBin) {
    if (std::abs(eta) < etaBins[etaBin])
      return etaBin;
  }

  return 9;
}
//---------------------------------------------------------------------------------------
// Gets the histogram Et bin given the et (MeV) -- corrresponds to fnEtBinsHist
unsigned int
Root::TElectronLikelihoodTool::getLikelihoodEtHistBin(double eT) const
{
  const double GeV = 1000;

  const unsigned int nEtBins = s_fnEtBinsHist;
  const double eTBins[nEtBins] = { 7 * GeV,  10 * GeV, 15 * GeV, 20 * GeV,
                                   30 * GeV, 40 * GeV, 50 * GeV };

  for (unsigned int eTBin = 0; eTBin < nEtBins; ++eTBin) {
    if (eT < eTBins[eTBin]) {
      return eTBin;
    }
  }

  return nEtBins - 1; // Return the last bin if > the last bin.
}

//---------------------------------------------------------------------------------------
// Gets the Et bin [0-10] given the et (MeV)
unsigned int
Root::TElectronLikelihoodTool::getLikelihoodEtDiscBin(double eT, const bool isLHbinning) const
{
  const double GeV = 1000;

  if (m_useOneExtraHighETLHBin && isLHbinning) {
    const unsigned int nEtBins = s_fnDiscEtBinsOneExtra;
    const double eTBins[nEtBins] = { 10 * GeV,  15 * GeV, 20 * GeV,
                                     25 * GeV,  30 * GeV, 35 * GeV,
                                     40 * GeV,  45 * GeV, m_highETBinThreshold * GeV,
                                     6000 * GeV };

    for (unsigned int eTBin = 0; eTBin < nEtBins; ++eTBin) {
      if (eT < eTBins[eTBin])
        return eTBin;
    }

    return nEtBins - 1; // Return the last bin if > the last bin.
  }

  const unsigned int nEtBins = s_fnDiscEtBins;
  const double eTBins[nEtBins] = { 10 * GeV, 15 * GeV, 20 * GeV, 25 * GeV, 30 * GeV,
                                   35 * GeV, 40 * GeV, 45 * GeV, 50 * GeV };

  for (unsigned int eTBin = 0; eTBin < nEtBins; ++eTBin) {
    if (eT < eTBins[eTBin])
      return eTBin;
  }

  return nEtBins - 1; // Return the last bin if > the last bin.
}

//---------------------------------------------------------------------------------------
// Gets the bin name. Given the HISTOGRAM binning (fnEtBinsHist)
void
Root::TElectronLikelihoodTool::getBinName(char* buffer,
                                          int etbin,
                                          int etabin,
                                          int ipbin,
                                          const std::string& iptype) const
{
  double eta_bounds[9] = { 0.0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37 };
  int et_bounds[s_fnEtBinsHist] = { 4, 7, 10, 15, 20, 30, 40 };
  if (!iptype.empty()) {
    snprintf(buffer,
             200,
             "%s%det%02deta%0.2f",
             iptype.c_str(),
             int(fIpBounds[ipbin]),
             et_bounds[etbin],
             eta_bounds[etabin]);
  } else {
    snprintf(buffer, 200, "et%deta%0.2f", et_bounds[etbin], eta_bounds[etabin]);
  }
}
//----------------------------------------------------------------------------------------
unsigned int
Root::TElectronLikelihoodTool::getLikelihoodBitmask(const std::string& vars) const
{
  unsigned int mask = 0x0;
  ATH_MSG_DEBUG("Variables to be used: ");
  for (unsigned int var = 0; var < s_fnVariables; var++) {
    if (vars.find(fVariables[var]) != std::string::npos) {
      ATH_MSG_DEBUG(fVariables[var]);
      mask = mask | 0x1 << var;
    }
  }
  ATH_MSG_DEBUG("mask: " << mask);
  return mask;
}

//----------------------------------------------------------------------------------------
// Note that this will only perform the cut interpolation up to ~45 GeV, so
// no smoothing is done above this for the high ET LH binning yet
double
Root::TElectronLikelihoodTool::InterpolateCuts(const std::vector<double>& cuts,
                                               const std::vector<double>& cuts_4gev,
                                               double et,
                                               double eta) const
{
  int etbinLH = getLikelihoodEtDiscBin(et, true);
  int etabin = getLikelihoodEtaBin(eta);
  unsigned int ibin_combinedLH = etbinLH * s_fnEtaBins + etabin;
  double cut = cuts.at(ibin_combinedLH);
  if (!cuts_4gev.empty() && et < 7000.) {
    cut = cuts_4gev.at(etabin);
  }
  if (et > 47500.) {
    return cut;
  } // interpolation stops here.
  if (cuts_4gev.empty() && et < 8500.) {
    return cut;
  } // stops here
  if (!cuts_4gev.empty() && et < 6000.) {
    return cut;
  } // stops here
  double bin_width = 5000.;
  if (7000. < et && et < 10000.) {
    bin_width = 3000.;
  }
  if (et < 7000.) {
    bin_width = 2000.;
  }
  const double GeV = 1000;
  const double eTBins[9] = { 8.5 * GeV,  12.5 * GeV, 17.5 * GeV, 22.5 * GeV, 27.5 * GeV,
                             32.5 * GeV, 37.5 * GeV, 42.5 * GeV, 47.5 * GeV };
  double bin_center = eTBins[etbinLH];
  if (et > bin_center) {
    double cut_next = cut;
    if (etbinLH + 1 <= 8)
      cut_next = cuts.at((etbinLH + 1) * s_fnEtaBins + etabin);
    return cut + (cut_next - cut) * (et - bin_center) / (bin_width);
  }
  // or else if et < bin_center :
  double cut_before = cut;
  if (etbinLH - 1 >= 0) {
    cut_before = cuts.at((etbinLH - 1) * s_fnEtaBins + etabin);
  } else if (etbinLH == 0 && !cuts_4gev.empty()) {
    cut_before = cuts_4gev.at(etabin);
  }

  return cut - (cut - cut_before) * (bin_center - et) / (bin_width);
}

//----------------------------------------------------------------------------------------
// Note that this will only perform the PDF interpolation up to ~45 GeV, so
// no smoothing is done above this for the high ET LH binning yet
double
Root::TElectronLikelihoodTool::InterpolatePdfs(unsigned int s_or_b,
                                               unsigned int ipbin,
                                               double et,
                                               double eta,
                                               int bin,
                                               unsigned int var) const
{
  // histograms exist for the following bins: 4, 7, 10, 15, 20, 30, 40.
  // Interpolation between histograms must follow fairly closely the
  // interpolation scheme between cuts - so be careful!
  int etbin = getLikelihoodEtHistBin(et); // hist binning
  int etabin = getLikelihoodEtaBin(eta);
  double integral = double(fPDFbins[s_or_b][ipbin][etbin][etabin][var]->Integral());
  double prob = double(fPDFbins[s_or_b][ipbin][etbin][etabin][var]->GetBinContent(bin)) / integral;

  int Nbins = fPDFbins[s_or_b][ipbin][etbin][etabin][var]->GetNbinsX();
  if (et > 42500.) {
    return prob; // interpolation stops here.
  }
  if (et < 6000.) {
    return prob; // interpolation stops here.
  }
  if (22500. < et && et < 27500.) {
    return prob; // region of non-interpolation for pdfs
  }
  if (32500. < et && et < 37500.) {
    return prob; // region of non-interpolation for pdfs
  }
  double bin_width = 5000.;
  if (7000. < et && et < 10000.) {
    bin_width = 3000.;
  }
  if (et < 7000.) {
    bin_width = 2000.;
  }
  const double GeV = 1000;
  const double eTHistBins[7] = { 6. * GeV,   8.5 * GeV,  12.5 * GeV, 17.5 * GeV,
                                 22.5 * GeV, 32.5 * GeV, 42.5 * GeV };
  double bin_center = eTHistBins[etbin];
  if (etbin == 4 && et >= 27500.) {
    bin_center = 27500.; // special: interpolate starting from 27.5 here
  }
  if (etbin == 5 && et >= 37500.) {
    bin_center = 37500.; // special: interpolate starting from 37.5 here
  }
  if (et > bin_center) {
    double prob_next = prob;
    if (etbin + 1 <= 6) {
      // account for potential histogram bin inequalities
      int NbinsPlus = fPDFbins[s_or_b][ipbin][etbin + 1][etabin][var]->GetNbinsX();
      int binplus = bin;
      if (Nbins < NbinsPlus) {
        binplus = int(round(bin * (Nbins / NbinsPlus)));
      } else if (Nbins > NbinsPlus) {
        binplus = int(round(bin * (NbinsPlus / Nbins)));
      }
      // do interpolation
      double integral_next = double(fPDFbins[s_or_b][ipbin][etbin + 1][etabin][var]->Integral());
      prob_next = double(fPDFbins[s_or_b][ipbin][etbin + 1][etabin][var]->GetBinContent(binplus)) /
                  integral_next;
      return prob + (prob_next - prob) * (et - bin_center) / (bin_width);
    }
  }
  // or else if et < bin_center :
  double prob_before = prob;
  if (etbin - 1 >= 0) {
    // account for potential histogram bin inequalities
    int NbinsMinus = fPDFbins[s_or_b][ipbin][etbin - 1][etabin][var]->GetNbinsX();
    int binminus = bin;
    if (Nbins < NbinsMinus) {
      binminus = int(round(bin * (Nbins / NbinsMinus)));
    } else if (Nbins > NbinsMinus) {
      binminus = int(round(bin * (NbinsMinus / Nbins)));
    }
    double integral_before = double(fPDFbins[s_or_b][ipbin][etbin - 1][etabin][var]->Integral());
    prob_before = double(fPDFbins[s_or_b][ipbin][etbin - 1][etabin][var]->GetBinContent(binminus)) /
                  integral_before;
  }
  return prob - (prob - prob_before) * (bin_center - et) / (bin_width);
}

//----------------------------------------------------------------------------------------

// These are the variables availalble in the likelihood.
const std::string Root::TElectronLikelihoodTool::fVariables[s_fnVariables] = {
  "el_d0significance",
  "el_eratio",
  "el_deltaeta1",
  "el_f1",
  "el_f3",
  "el_reta",
  "el_rhad",
  "el_rphi",
  "el_trackd0pvunbiased",
  "el_weta2",
  "el_DeltaPoverP",
  "el_deltaphiRescaled",
  "el_TRT_PID"
};
