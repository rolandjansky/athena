/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "JGTowerNoiseAlg.h"
#include "StoreGate/ReadHandle.h"
#include "PathResolver/PathResolver.h"
#include "TFile.h"
#include "TH1F.h"
#include <memory>
#include "GaudiKernel/SystemOfUnits.h"
#include <algorithm>

namespace
{
  const static SG::AuxElement::Decorator<float> decNoise("noise");
}

namespace LVL1
{
  JGTowerNoiseAlg::JGTowerNoiseAlg(const std::string &name, ISvcLocator *pSvcLocator)
      : AthReentrantAlgorithm(name, pSvcLocator)
  {
    declareProperty("InputTowers", m_inputKey);
    declareProperty("NoiseFile", m_noiseFile = "Run3L1CaloSimulation/Noise/jTowergTowerNoise.20210209.r11881.root");
    declareProperty("DoJFEX", m_doJFex = false, "If true, use jFEX noise values, otherwise use gFEX");
    declareProperty("DefaultNoise", m_defaultNoise = 1 * Gaudi::Units::GeV, "Default noise value to use if no file provided");
  }

  JGTowerNoiseAlg::~JGTowerNoiseAlg() {}

  StatusCode JGTowerNoiseAlg::initialize()
  {
    ATH_CHECK(m_inputKey.initialize());

    if (m_noiseFile.empty())
    {
      ATH_MSG_WARNING("No noise file found. Will set all noises to " << m_defaultNoise);
      return StatusCode::SUCCESS;
    }
    std::unique_ptr<TFile> noiseFile(TFile::Open(PathResolver::find_calib_file(m_noiseFile).c_str()));
    if (m_doJFex)
    {
      m_etaBins = {0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2, 2.1, 2.2, 2.3, 2.4, 2.5, 2.7, 2.9, 3.1};
      TProfile *noiseProfileEM = dynamic_cast<TProfile *>(noiseFile->Get("jTowerNoise/pr_eta_EM"));
      TProfile *noiseProfileHAD = dynamic_cast<TProfile *>(noiseFile->Get("jTowerNoise/pr_eta_HAD"));
      TProfile *noiseProfileFCAL = dynamic_cast<TProfile *>(noiseFile->Get("jTowerNoise/pr_eta_FCAL"));
      if (!noiseProfileEM || !noiseProfileHAD || !noiseProfileFCAL)
      {
        ATH_MSG_ERROR("Failed to load noise values from input file!");
        return StatusCode::FAILURE;
      }
      if (m_etaBins.size() != static_cast<std::size_t>(noiseProfileEM->GetNbinsX()) + 1)
      {
        ATH_MSG_ERROR("Number of EM profile eta bins '" << noiseProfileEM->GetNbinsX() << "' does not match expected value '" << m_etaBins.size() - 1 << "'!");
        return StatusCode::FAILURE;
      }
      if (m_etaBins.size() != static_cast<std::size_t>(noiseProfileHAD->GetNbinsX()) + 1)
      {
        ATH_MSG_ERROR("Number of HAD profile eta bins '" << noiseProfileHAD->GetNbinsX() << "' does not match expected value '" << m_etaBins.size() - 1 << "'!");
        return StatusCode::FAILURE;
      }

      // Load the noise values. Start off assigning the default value to all bins (including overflows)
      m_noiseValuesEM.assign(m_defaultNoise, m_etaBins.size() + 1);
      m_noiseValuesHAD.assign(m_defaultNoise, m_etaBins.size() + 1);
      for (std::size_t idx = 1; idx < m_etaBins.size(); ++idx)
      {
        m_noiseValuesEM.at(idx) = noiseProfileEM->GetBinError(idx);
        m_noiseValuesHAD.at(idx) = noiseProfileHAD->GetBinError(idx);
      }

      for (std::size_t idx = 1; idx < static_cast<std::size_t>(noiseProfileFCAL->GetNbinsX() + 1); ++idx)
      {
        std::string label = noiseProfileFCAL->GetXaxis()->GetBinLabel(idx);
        // There are empty bins which have an empty label
        if (label.empty())
          continue;
        if (label.substr(0, 4) != "ieta")
        {
          ATH_MSG_ERROR("Bin label '" << label << "'is not in the expected format");
          return StatusCode::FAILURE;
        }
        std::size_t towerIdx = static_cast<std::size_t>(std::atoi(label.substr(4).c_str()));
        m_noiseValuesFCAL[towerIdx] = noiseProfileFCAL->GetBinError(idx);
      }
    }
    else
    {
      m_etaBins = {0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 1.2, 1.4, 1.6, 1.8, 2.0, 2.2, 2.5, 2.7, 2.9, 3.1, 3.5, 4.0, 4.45, 4.9};
      TProfile *noiseProfile = dynamic_cast<TProfile *>(noiseFile->Get("gTowerNoise/pr_eta"));
      if (!noiseProfile)
      {
        ATH_MSG_ERROR("Failed to load noise values from input file!");
        return StatusCode::FAILURE;
      }
      if (m_etaBins.size() != static_cast<std::size_t>(noiseProfile->GetNbinsX()) + 1)
      {
        ATH_MSG_ERROR("Number of profile eta bins '" << noiseProfile->GetNbinsX() << "' does not match expected value '" << m_etaBins.size() - 1 << "'!");
        return StatusCode::FAILURE;
      }
      // Load the noise values. Start off assigning the default value to all bins (including overflows)
      m_noiseValues.assign(m_defaultNoise, m_etaBins.size() + 1);
      for (std::size_t idx = 1; idx < m_etaBins.size(); ++idx)
        m_noiseValues.at(idx) = noiseProfile->GetBinError(idx);
    }
    return StatusCode::SUCCESS;
  }

  StatusCode JGTowerNoiseAlg::execute(const EventContext& ctx) const
  {
    auto inputTowers = SG::makeHandle(m_inputKey, ctx);
    if (!inputTowers.isValid())
    {
      ATH_MSG_ERROR("Could not retrieve input towers " << m_inputKey.key());
      return StatusCode::FAILURE;
    }
    for (const xAOD::JGTower *iTower : *inputTowers)
    {
      float absEta = std::abs(iTower->eta());
      if (m_doJFex)
      {
        if (absEta > m_etaBins.back())
        {
          float noise = m_noiseValuesFCAL.at(iTower->index());
          decNoise(*iTower) = noise > 0 ? noise : m_defaultNoise;
        }
        else
        {
          // Use the eta-binned histograms
          // std::upper_bound returns an iterator to the first element larger than the provided value
          // This means that it maps onto ROOT binning exactly: if the value is smaller than the lowest bin edge
          // (underflow) it returns 0, right on up to if it's larger than the largest bin edge where it returns the size of the edge vector
          // which is the overflow bin
          std::size_t iEta = std::distance(m_etaBins.begin(), std::upper_bound(m_etaBins.begin(), m_etaBins.end(), absEta));
          if (iEta == 0 || iEta == m_etaBins.size())
            ATH_MSG_WARNING("Eta value " << absEta << " falls in the under/overflow?");
          if (iTower->sampling() == 0)
            decNoise(*iTower) = m_noiseValuesEM.at(iEta);
          else
            decNoise(*iTower) = m_noiseValuesHAD.at(iEta);
        }
      }
      else
      {
        // std::upper_bound returns an iterator to the first element larger than the provided value
        // This means that it maps onto ROOT binning exactly: if the value is smaller than the lowest bin edge
        // (underflow) it returns 0, right on up to if it's larger than the largest bin edge where it returns the size of the edge vector
        // which is the overflow bin
        std::size_t iEta = std::distance(m_etaBins.begin(), std::upper_bound(m_etaBins.begin(), m_etaBins.end(), absEta));
        if (iEta == 0 || iEta == m_etaBins.size())
          ATH_MSG_WARNING("Eta value " << absEta << " falls in the under/overflow?");
        decNoise(*iTower) = m_noiseValues.at(iEta);
      }
    }
    return StatusCode::SUCCESS;
  }
} // namespace LVL1
