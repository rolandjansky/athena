/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "JJetBuilder.h"
#include "TVector2.h"
#include "xAODTrigger/JetRoIAuxContainer.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODTrigger/JetRoI.h"
#include <cstdint>

namespace
{
  /**
   * Check if two towers are on the edges of the ring
   * 
   * For the rings we don't want to include the towers which are exactly on the
   * edges i.e. the separation of the two is exactly R. Because the radii we're 
   * interested in (2, 4 and 8) are not the hypotenuse of any Pythagorean triple
   * these only occur when the two towers are in either an eta or phi line
   */
  bool edgeOfRingPhi(const std::vector<JGTowerHelper> &helpers, std::size_t lowIdx, std::size_t highIdx, std::size_t size)
  {
    for (std::size_t step = 0; step < size; ++step)
      {
	lowIdx = helpers.at(lowIdx).nextPhiIndex();
	if (lowIdx == SIZE_MAX)
	  return false;
      }
    return lowIdx == highIdx;
  }

  bool edgeOfRingEta(const std::vector<JGTowerHelper> &helpers, std::size_t lowIdx, std::size_t highIdx, std::size_t size)
  {
    for (std::size_t step = 0; step < size; ++step)
      {
	lowIdx = helpers.at(lowIdx).nextEtaIndex();
	if (lowIdx == SIZE_MAX)
	  return false;
      }
    return lowIdx == highIdx;
  }
  /// Return a set containing the index of the supplied tower and all behind it                                                         
  std::set<std::size_t> allIndicesBehind(const std::vector<JGTowerHelper> &helpers, std::size_t idx)
  {
    std::set<std::size_t> ret;
    while (idx != SIZE_MAX)
      {
	ret.insert(idx);
	idx = helpers.at(idx).indexBehind();
      }
    return ret; 
  }
  bool edgeOfRing(const std::vector<JGTowerHelper> &helpers, std::size_t idx1, std::size_t idx2, std::size_t size)
  {
    float dEta = helpers.at(idx1).Eta() - helpers.at(idx2).Eta();
    float dPhi = TVector2::Phi_mpi_pi(helpers.at(idx1).Phi() - helpers.at(idx2).Phi());
    // We don't need a small tolerance given that we're dealing with floats on the order of 0.1

    // Check if the two towers are on either an eta or a phi line
    // If they are send them to the right function with the indices correctly
    // ordered.
    static constexpr float tolerance = 1e-3;
    if (std::abs(dEta) < tolerance)
    {
      if (dEta > 0)
        return edgeOfRingEta(helpers, idx2, idx1, size);
      else
        return edgeOfRingEta(helpers, idx1, idx2, size);
    }
    else if (std::abs(dPhi) < tolerance)
    {
      if (dPhi > 0)
        return edgeOfRingPhi(helpers, idx2, idx1, size);
      else
        return edgeOfRingPhi(helpers, idx1, idx2, size);
    }
    else
      // Otherwise they aren't on a line
      return false;
  }
} // namespace

namespace LVL1
{
JJetBuilder::JJetBuilder(const std::string &name, ISvcLocator *pSvcLocator)
    : AthAlgorithm(name, pSvcLocator)
{
  declareProperty("MappingMaker", m_mappingMaker);
  declareProperty("InputTowers", m_jtowersKey);
  declareProperty("InputTowerNoise", m_noiseName = "", "Input JG tower noise values (if used)");
  declareProperty("OutputSmallJets",m_outputSmallJetContainerKey); 
  declareProperty("OutputLargeJets",m_outputLargeJetContainerKey);
  declareProperty("OutputSeedJets",m_outputSeedJetContainerKey);
  declareProperty("jRoundJetMinET",m_jRoundJetMinET = 5000.0); 
  declareProperty("jRoundJetSeedMinET",m_jRoundJetSeedMinET = 500.0);
}

StatusCode JJetBuilder::initialize()
{
  ATH_MSG_DEBUG("Initializing JJetBuilder"); 
  if (!m_noiseName.empty())
    m_noiseAcc = std::make_unique<SG::AuxElement::ConstAccessor<float>>(m_noiseName);

  //Initialize keys 
  ATH_CHECK(m_outputSmallJetContainerKey.initialize() );
  ATH_CHECK(m_outputLargeJetContainerKey.initialize() ); 
  ATH_CHECK(m_outputSeedJetContainerKey.initialize() ); 


  ATH_MSG_DEBUG("get mapping: " << m_mappingMaker);
  ATH_CHECK(m_jtowersKey.initialize());
  const std::vector<JGTowerHelper> &mapping = m_mappingMaker->getMapping();
  m_seedHelpers.clear();
  m_seedHelpers.reserve(mapping.size());
  ATH_MSG_DEBUG("Loop over mapping");
  // Fill in our vector of seed helpers
  for (std::size_t idx = 0; idx < mapping.size(); ++idx)
  {
    const JGTowerHelper &tower = mapping.at(idx);
    if (!tower.isFrontTower())
      continue;
    m_seedHelpers.push_back(JJetSeedHelper(idx, tower.Eta(), tower.Phi()));
    std::set<std::size_t> towerIndices = allIndicesBehind(mapping, idx);
    m_seedHelpers.back().seedIndices().insert(towerIndices.begin(), towerIndices.end());
  }
  ATH_MSG_DEBUG("Iterate and fill index lists");
  // Now iterate through and fill the index lists
  for (std::size_t thisIdx = 0; thisIdx < m_seedHelpers.size(); ++thisIdx)
  {
    ATH_MSG_DEBUG("thisIdx " << thisIdx);
    JJetSeedHelper &helper = m_seedHelpers.at(thisIdx);
    std::set<std::size_t> towerIndices = allIndicesBehind(mapping, helper.centreIdx());
    for (std::size_t otherIdx = 0; otherIdx < thisIdx; ++otherIdx)
    {
      ATH_MSG_DEBUG("otherIdx" << otherIdx);
      JJetSeedHelper &other = m_seedHelpers.at(otherIdx);
      std::set<std::size_t> otherTowerIndices = allIndicesBehind(mapping, other.centreIdx());
      float dEta = helper.eta() - other.eta();
      float dPhi = TVector2::Phi_mpi_pi(helper.phi() - other.phi());
      float dR = std::sqrt(dEta * dEta + dPhi * dPhi);
      static constexpr int seedRadius = 2;
      static constexpr int firstRingRadius = 4;
      static constexpr int secondRingRadius = 8;
      static constexpr int comparisonRadius = 3;

      ATH_MSG_DEBUG("Delta R:" << dR ); 

      if (dR < seedRadius * 0.1 && !edgeOfRing(mapping, helper.centreIdx(), other.centreIdx(), seedRadius))
      {
	ATH_MSG_DEBUG("seed Radius: Inserting towers for seed radius");
        helper.seedIndices().insert(otherTowerIndices.begin(), otherTowerIndices.end());
        other.seedIndices().insert(towerIndices.begin(), towerIndices.end());
      }
      else if (dR < firstRingRadius * 0.1 && !edgeOfRing(mapping, helper.centreIdx(), other.centreIdx(), firstRingRadius))
      {
	ATH_MSG_DEBUG("firstRingRadius. Now inserting towers" ); 
        helper.firstRingIndices().insert(otherTowerIndices.begin(), otherTowerIndices.end());
        other.firstRingIndices().insert(towerIndices.begin(), towerIndices.end());
      }
      else if (dR < secondRingRadius * 0.1 && !edgeOfRing(mapping, helper.centreIdx(), other.centreIdx(), secondRingRadius))
      {
	ATH_MSG_DEBUG("secondRingRadius. Now inserting towers"); 
        helper.secondRingIndices().insert(otherTowerIndices.begin(), otherTowerIndices.end());
        other.secondRingIndices().insert(towerIndices.begin(), towerIndices.end());
      }

      ATH_MSG_DEBUG("Comparison Indicies, deta " << dEta << " dphi: " << dPhi);
      // Also calculate how we will perform the comparisons
      if (dR < comparisonRadius * 0.1 && !edgeOfRing(mapping, helper.centreIdx(), other.centreIdx(), comparisonRadius))
      {
        if (dEta >= 0 ? dPhi <= -dEta : dPhi < -dEta)
          helper.comparisonIndices().insert(otherIdx);
        else
          other.comparisonIndices().insert(thisIdx);
      }
    }
  }

  return StatusCode::SUCCESS;
}

float JJetBuilder::passNoiseCutET(const xAOD::JGTower *tower){
  
  if (m_noiseAcc)
    {
      //If we are reading sigma noise from the file per eta bin, then use 2 sigma 
      if (tower->et() < 2 * (*m_noiseAcc)(*tower)) return 0.; 
    }
  else if (tower->et() < 0 ) return 0.; 
  //If not using the file, then we are using pileup corrections, and just take the tower with ET > 0 (with the intention that we have subtracted some pileup) 

  return tower->et(); 

}

StatusCode JJetBuilder::execute()
{
  ATH_MSG_DEBUG("JJetBuilder execute");
  const EventContext &ctx = Gaudi::Hive::currentContext();

  auto jtowers = SG::makeHandle(m_jtowersKey);
  std::vector<float> seedEts;
  seedEts.reserve(m_seedHelpers.size());
  std::vector<float> smallREts;
  smallREts.reserve(m_seedHelpers.size());
  std::vector<float> largeREts;
  largeREts.reserve(m_seedHelpers.size());
  for (const JJetSeedHelper &helper : m_seedHelpers)
  {
    float et = 0;
    for (std::size_t idx : helper.seedIndices())
      et += passNoiseCutET(jtowers->at(idx));
    seedEts.push_back(et);
    for (std::size_t idx : helper.firstRingIndices())
      et += passNoiseCutET(jtowers->at(idx));
    smallREts.push_back(et);
    for (std::size_t idx : helper.secondRingIndices())
      et += passNoiseCutET(jtowers->at(idx));
    largeREts.push_back(et);
  }
  // Decide what to do with close together seeds
  std::vector<char> mask(m_seedHelpers.size(), true);
  for (std::size_t idx1 = 0; idx1 < m_seedHelpers.size(); ++idx1)
    for (std::size_t idx2 : m_seedHelpers.at(idx1).comparisonIndices())
      mask[seedEts.at(idx1) >= seedEts.at(idx2) ? idx2 : idx1] = false;

  // Now at this point we have the different energies and the mask values (true) if passing the seed step

  auto smallJets = std::make_unique<xAOD::JetRoIContainer>();
  auto smallJetsAux = std::make_unique<xAOD::JetRoIAuxContainer>();
  smallJets->setStore(smallJetsAux.get()); 
  
  auto largeJets = std::make_unique<xAOD::JetRoIContainer>();
  auto largeJetsAux = std::make_unique<xAOD::JetRoIAuxContainer>();
  largeJets->setStore(largeJetsAux.get());

  auto seedJets = std::make_unique<xAOD::JetRoIContainer>();
  auto seedJetsAux = std::make_unique<xAOD::JetRoIAuxContainer>();
  seedJets->setStore(seedJetsAux.get());

  for (std::size_t idx = 0; idx < m_seedHelpers.size(); ++idx)
    {
      if (!mask.at(idx))
	continue;

      float seedEt = seedEts.at(idx); 

      if(seedEt < m_jRoundJetSeedMinET) continue; 

      float smallEt = smallREts.at(idx);
      float largeEt = smallREts.at(idx); 
      float eta = m_seedHelpers.at(idx).eta();
      float phi = m_seedHelpers.at(idx).phi();

      //Requirements to write out jets to AOD 
      if(smallEt>=m_jRoundJetMinET){
	xAOD::JetRoI *Jet = new xAOD::JetRoI;
	smallJets->push_back(Jet); 
	Jet->initialize(0x0,eta,phi); 
	Jet->setEt8x8(smallEt); 
      }
      if(seedEt>0){
	//Only save positive ET seeds
	xAOD::JetRoI *Jet = new xAOD::JetRoI;
	seedJets->push_back(Jet);
	Jet->initialize(0x0,eta,phi);
	Jet->setEt8x8(seedEt);
      }
      if(largeEt>=m_jRoundJetMinET){
	xAOD::JetRoI *Jet = new xAOD::JetRoI;
	largeJets->push_back(Jet);
	Jet->initialize(0x0,eta,phi);
	Jet->setEt8x8(largeEt);
      }
    }

  std::sort(smallJets->begin(), smallJets->end(), [] (const auto* lhs, const auto* rhs) { return lhs->et8x8() > rhs->et8x8();});
  std::sort(largeJets->begin(), largeJets->end(), [] (const auto* lhs, const auto* rhs) { return lhs->et8x8() > rhs->et8x8();});
  std::sort(seedJets->begin(),  seedJets->end(), [] (const auto* lhs, const auto* rhs) { return lhs->et8x8() > rhs->et8x8();});

  auto writeHandleSmall = SG::makeHandle(m_outputSmallJetContainerKey, ctx);
  ATH_CHECK(writeHandleSmall.record(std::move(smallJets), 
				    std::move(smallJetsAux)));

  auto writeHandleLarge = SG::makeHandle(m_outputLargeJetContainerKey, ctx);
  ATH_CHECK(writeHandleLarge.record(std::move(largeJets),
				    std::move(largeJetsAux)));

  auto writeHandleSeeds = SG::makeHandle(m_outputSeedJetContainerKey, ctx);
  ATH_CHECK(writeHandleSeeds.record(std::move(seedJets),
				    std::move(seedJetsAux)));

  return StatusCode::SUCCESS;
}
}
