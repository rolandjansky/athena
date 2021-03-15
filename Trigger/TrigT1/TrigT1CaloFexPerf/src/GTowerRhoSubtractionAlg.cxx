/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "GTowerRhoSubtractionAlg.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include <memory>
#include "xAODCore/ShallowAuxContainer.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODBase/IParticleHelpers.h"
#include "TH1F.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GTowerHelpers.h"
#include "xAODTrigger/EnergySumRoIAuxInfo.h"
#include <algorithm>

namespace
{
  const static SG::AuxElement::ConstAccessor<float> accArea("area");
  const static SG::AuxElement::Decorator<std::vector<float>> decRho("FPGARhos");
} // namespace

namespace LVL1
{
  GTowerRhoSubtractionAlg::GTowerRhoSubtractionAlg(const std::string &name, ISvcLocator *pSvcLocator)
      : AthReentrantAlgorithm(name, pSvcLocator)
  {
    declareProperty("InputTowers", m_inputKey = "GCaloTower");
    declareProperty("OutputTowers", m_outputKey = "GCaloTowerRhoSubtracted");
    declareProperty("OutputRho", m_outputRhoKey = "GFEXRho");
    declareProperty("UseNegativeTowers", m_useNegativeTowers = true);
    declareProperty("MaxTowerEt", m_maxTowerEt = 10 * Gaudi::Units::GeV);
    declareProperty("ForcePositiveRho", m_forcePosRho = false);
  }

  GTowerRhoSubtractionAlg::~GTowerRhoSubtractionAlg() {}

  StatusCode GTowerRhoSubtractionAlg::initialize()
  {
    ATH_CHECK(m_inputKey.initialize());
    ATH_CHECK(m_outputKey.initialize());
    ATH_CHECK(m_outputRhoKey.initialize());
    return StatusCode::SUCCESS;
  }

  StatusCode GTowerRhoSubtractionAlg::execute(const EventContext& ctx) const
  {
    auto inputTowers = SG::makeHandle(m_inputKey, ctx);
    if (!inputTowers.isValid())
    {
      ATH_MSG_ERROR("Failed to retrieve input towers " << m_inputKey.key());
      return StatusCode::FAILURE;
    }
    auto shallowCopy = xAOD::shallowCopyContainer(*inputTowers);
    std::unique_ptr<xAOD::JGTowerContainer> outputTowers(shallowCopy.first);
    std::unique_ptr<xAOD::ShallowAuxContainer> outputTowersAux(shallowCopy.second);
    xAOD::setOriginalObjectLink(*inputTowers, *outputTowers);

    // Split towers into FPGAs
    constexpr static std::size_t nFPGAs = static_cast<std::size_t>(gFEX::FPGA::N_FPGAS);
    std::array<std::vector<const xAOD::JGTower *>, nFPGAs> fpgas;
    // Use histograms to calculate RMSs of tower Ets for each FPGA
    std::array<TH1F, nFPGAs> fpgaHistograms;
    std::vector<float> fpgaRhos(3, 0.0);
    for (std::size_t i = 0; i < nFPGAs; ++i)
      fpgaHistograms.at(i) = TH1F(("hFPGA" + std::to_string(i)).c_str(), "", 50, 0, 5000);
    for (const xAOD::JGTower *tower : *inputTowers)
    {
      gFEX::FPGA fpga = gFEX::getFPGA(tower->eta());
      if (fpga == gFEX::FPGA::N_FPGAS)
      {
        ATH_MSG_WARNING("Could not classify tower as an FPGA!");
        continue;
      }
      fpgas.at(static_cast<std::size_t>(fpga)).push_back(tower);
      fpgaHistograms.at(static_cast<std::size_t>(fpga)).Fill(tower->et());
    }

    // Now perform the subtraction
    for (std::size_t i = 0; i < nFPGAs; ++i)
    {
      float threshold = 3 * fpgaHistograms.at(i).GetRMS();
      const std::vector<const xAOD::JGTower *> &towers = fpgas.at(i);
      float rho = calculateRho(towers);
      fpgaRhos.at(i) = rho;
      for (const xAOD::JGTower *tower : towers)
      {
        float area = accArea(*tower);
        float etSub = tower->et() - area * rho;
        // output is a shallow copy of input so the indices are guaranteed to be parallel
        outputTowers->at(tower->index())->setEt(etSub < threshold ? 0 : etSub);
      }
    }

    auto outputHandle = SG::makeHandle(m_outputKey, ctx);
    ATH_CHECK(outputHandle.record(std::move(outputTowers), std::move(outputTowersAux)));

    // Create an EnergySumRoI container to store the rho values
    auto rhoCont = std::make_unique<xAOD::EnergySumRoI>();
    auto rhoContAux = std::make_unique<xAOD::EnergySumRoIAuxInfo>();
    rhoCont->setStore(rhoContAux.get());
    decRho(*rhoCont) = fpgaRhos;
    auto outputRhoHandle = SG::makeHandle(m_outputRhoKey, ctx);
    ATH_CHECK(outputRhoHandle.record(std::move(rhoCont), std::move(rhoContAux)));
    return StatusCode::SUCCESS;
  }

  float GTowerRhoSubtractionAlg::calculateRho(const std::vector<const xAOD::JGTower *> &towers) const
  {
    float totalEt = 0;
    float totalArea = 0;
    for (const xAOD::JGTower *tower : towers)
    {
      if ((m_useNegativeTowers || tower->et() > 0) && tower->et() <= m_maxTowerEt)
      {
        totalEt += tower->et();
        totalArea += accArea(*tower);
      }
    }
    float rho = (totalArea == 0 ? 0.0f : totalEt / totalArea);
    if (m_forcePosRho && rho < 0)
      rho = 0;
    return rho;
  }

} // namespace LVL1