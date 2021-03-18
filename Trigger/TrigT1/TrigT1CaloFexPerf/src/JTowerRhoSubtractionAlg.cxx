/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "JTowerRhoSubtractionAlg.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "xAODCore/ShallowAuxContainer.h"
#include "xAODCore/ShallowCopy.h"
#include <memory>
#include <numeric>
#include <TFile.h>
#include <TH1.h>
#include "PathResolver/PathResolver.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODTrigger/EnergySumRoIAuxInfo.h"

namespace
{
  const static SG::AuxElement::ConstAccessor<float> accArea("area");
  const static SG::AuxElement::Decorator<std::vector<float>> decRhos("rhoValues");
  std::vector<std::size_t> createVectorFromRanges(const std::vector<std::pair<std::size_t, std::size_t>> &ranges)
  {
    // Work out the size
    std::size_t total = 0;
    for (const std::pair<std::size_t, std::size_t> &range : ranges)
      total += (range.second - range.first);
    std::vector<std::size_t> result(total);
    auto startItr = result.begin();
    auto endItr = result.begin();
    for (const std::pair<std::size_t, std::size_t> &range : ranges)
    {
      startItr = endItr;
      endItr = startItr + (range.second - range.first);
      std::iota(startItr, endItr, range.first);
    }
    return result;
  }

  std::vector<std::size_t> findBinNumbers(
      float eta, float phi,
      const std::vector<std::pair<float, float>> &etaBins,
      const std::vector<std::pair<float, float>> &phiBins,
      float phiOffset)
  {
    std::vector<std::size_t> binNumbers;
    std::size_t iBin = 0;
    phi = TVector2::Phi_0_2pi(phi - phiOffset);
    for (const std::pair<float, float> &etaBin : etaBins)
    {
      for (const std::pair<float, float> &phiBin : phiBins)
      {
        if (etaBin.first < eta && eta < etaBin.second)
        {
          if (phiBin.first < phi && phi < phiBin.second)
            binNumbers.push_back(iBin);
        }
        ++iBin;
      }
    }
    return binNumbers;
  }
} // namespace

namespace LVL1
{
  JTowerRhoSubtractionAlg::JTowerRhoSubtractionAlg(const std::string &name, ISvcLocator *pSvcLocator)
      : AthReentrantAlgorithm(name, pSvcLocator)
  {
    declareProperty("InputTowers", m_inputKey = "JTower");
    declareProperty("OutputTowers", m_outputKey = "JTowerRhoSubtracted");
    declareProperty("OutputRho", m_outputRhoKey = "JFEXRho");
    declareProperty("MinTowerRho", m_minTowerRho = 0.0, "Minimum tower rho to enter the calculation of bin rho");
    declareProperty("MaxTowerRho", m_maxTowerRho = 1.0 * Gaudi::Units::GeV, "Maximum tower rho to enter the calculation of bin rho");
    declareProperty("MinOutputTowerRho", m_minOutputTowerRho = 100 * Gaudi::Units::MeV, "Minimum tower rho to be output (below this set 0)");
  }

  JTowerRhoSubtractionAlg::~JTowerRhoSubtractionAlg() {}

  StatusCode JTowerRhoSubtractionAlg::initialize()
  {
    ATH_CHECK(m_inputKey.initialize());
    ATH_CHECK(m_outputKey.initialize());
    ATH_CHECK(m_outputRhoKey.initialize());
    return StatusCode::SUCCESS;
  }

  StatusCode JTowerRhoSubtractionAlg::buildFexBins(const xAOD::JGTowerContainer *towers) const
  {
    // Create the bin lists as a list of tower indices
    // eta bins
    std::vector<std::pair<float, float>> etaBins{
        {-1.6, 0.8},
        {-0.8, 1.6},
        {-2.4, 0.0},
        {0.0, 2.4},
        {-4.9, -0.8},
        {0.8, 4.9}};
    std::vector<std::pair<float, float>> etaBinsCore{
        {-0.8, 0.0},
        {0.0, 0.8},
        {-1.6, -0.8},
        {0.8, 1.6},
        {-4.9, -1.6},
        {1.6, 4.9}};
    // phi bins (in multiples of pi)
    std::vector<std::pair<float, float>> phiBins{
        {0.0, TMath::Pi()}, {TMath::Pi(), 2 * TMath::Pi()}};
    float phiOffset = 0.25 * TMath::Pi();

    std::size_t nBins = etaBins.size() * phiBins.size();

    std::vector<std::size_t> EMTowers = createVectorFromRanges(
        {{1, 1696}, {3392, 5088}, {6784, 6816}, {6848, 6880}, {6912, 6976}});
    std::vector<std::size_t> hadTowers = createVectorFromRanges(
        {{1696, 3392}, {5088, 6784}, {6816, 6848}, {6880, 6912}});
    std::vector<std::size_t> FCALTowers = createVectorFromRanges({{6976, 7744}});

    // Split the hadronic calorimeter into 3 regions
    std::vector<std::size_t> had1Towers;
    std::vector<std::size_t> had2Towers;
    std::vector<std::size_t> had3Towers;
    for (std::size_t idx : hadTowers)
    {
      float eta = std::abs(towers->at(idx)->eta());
      if (eta < 1.5)
        had1Towers.push_back(idx);
      else if (eta < 1.6)
        had2Towers.push_back(idx);
      else
        had3Towers.push_back(idx);
    }

    // Create the necessary regions
    std::vector<std::vector<std::size_t>> regions;
    regions.reserve(5);
    regions.push_back(std::move(had1Towers));
    regions.push_back(std::move(had2Towers));
    regions.push_back(std::move(had3Towers));
    regions.push_back(std::move(EMTowers));
    regions.push_back(std::move(FCALTowers));

    m_jFEXBins.clear();
    m_jFEXBinsCore.clear();
    m_jFEXBins.resize(regions.size() * nBins);
    m_jFEXBinsCore.resize(regions.size() * nBins);

    for (std::size_t regionIdx = 0; regionIdx < regions.size(); ++regionIdx)
    {
      for (std::size_t towerIdx : regions.at(regionIdx))
      {
        const xAOD::JGTower &tower = *towers->at(towerIdx);
        for (std::size_t binIdx : findBinNumbers(tower.eta(), tower.phi(), etaBins, phiBins, phiOffset))
          m_jFEXBins.at(binIdx + regionIdx * nBins).push_back(towerIdx);
        for (std::size_t binIdx : findBinNumbers(tower.eta(), tower.phi(), etaBinsCore, phiBins, phiOffset))
          m_jFEXBinsCore.at(binIdx + regionIdx * nBins).push_back(towerIdx);
      }
    }

    // If any core bin is empty, also empty the corresponding larger bin
    for (std::size_t binIdx = 0; binIdx < m_jFEXBins.size(); ++binIdx)
      if (m_jFEXBinsCore.at(binIdx).size() == 0)
        m_jFEXBins.at(binIdx).clear();

    m_builtFexBins = true;
    return StatusCode::SUCCESS;
  }

  StatusCode JTowerRhoSubtractionAlg::execute(const EventContext &ctx) const
  {
    auto inputTowers = SG::makeHandle(m_inputKey, ctx);
    if (!inputTowers.isValid())
    {
      ATH_MSG_ERROR("Failed to retrieve " << m_inputKey.key());
      return StatusCode::FAILURE;
    }
    auto shallowCopy = xAOD::shallowCopyContainer(*inputTowers);
    std::unique_ptr<xAOD::JGTowerContainer> outputTowers(shallowCopy.first);
    std::unique_ptr<xAOD::ShallowAuxContainer> outputTowersAux(shallowCopy.second);
    xAOD::setOriginalObjectLink(*inputTowers, *outputTowers);

    if (!m_builtFexBins)
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_builtFexBins)
        ATH_CHECK(buildFexBins(inputTowers.cptr()));
    }

    // Iterate over the bins and calculate the average energy in the towers
    std::vector<float> rhos;
    rhos.reserve(m_jFEXBins.size());
    for (const std::vector<std::size_t> &binTowerIndices : m_jFEXBins)
    {
      std::size_t count = 0;
      float rhoSum = 0;
      for (std::size_t towerIdx : binTowerIndices)
      {
        const xAOD::JGTower *tower = inputTowers->at(towerIdx);
        float area = accArea(*tower);
        float towerRho = tower->et() / area;
        if (towerRho > m_minTowerRho && towerRho <= m_maxTowerRho)
        {
          ++count;
          rhoSum += towerRho;
        }
      }
      if (count == 0)
        rhos.push_back(0);
      else
        rhos.push_back(rhoSum / count);
    }

    std::vector<float> subtractedTowerEnergies(inputTowers->size(), 0.0);
    for (std::size_t binIdx = 0; binIdx < m_jFEXBinsCore.size(); ++binIdx)
    {
      for (std::size_t towerIdx : m_jFEXBinsCore.at(binIdx))
      {
        const xAOD::JGTower *tower = inputTowers->at(towerIdx);
        float area = accArea(*tower);
        float etSub = tower->et() - rhos.at(binIdx) * area;
        if (etSub < area * m_minOutputTowerRho)
          etSub = 0;
        subtractedTowerEnergies.at(towerIdx) = etSub;
      }
    }
    for (std::size_t idx = 0; idx < subtractedTowerEnergies.size(); ++idx)
      outputTowers->at(idx)->setEt(subtractedTowerEnergies.at(idx));

    auto outputHandle = SG::makeHandle(m_outputKey, ctx);

    ATH_CHECK(outputHandle.record(std::move(outputTowers), std::move(outputTowersAux)));

    // Create an EnergySumRoI container to store the rho values
    auto rhoCont = std::make_unique<xAOD::EnergySumRoI>();
    auto rhoContAux = std::make_unique<xAOD::EnergySumRoIAuxInfo>();
    rhoCont->setStore(rhoContAux.get());
    decRhos(*rhoCont) = rhos;
    auto outputRhoHandle = SG::makeHandle(m_outputRhoKey, ctx);
    ATH_CHECK(outputRhoHandle.record(std::move(rhoCont), std::move(rhoContAux)));
    return StatusCode::SUCCESS;
  }
} // namespace LVL1