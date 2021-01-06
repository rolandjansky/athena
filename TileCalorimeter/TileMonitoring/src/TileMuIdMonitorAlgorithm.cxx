/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TileMuIdMonitorAlgorithm.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

#include "StoreGate/ReadHandle.h"
#include "AthenaKernel/Units.h"

using Athena::Units::GeV;
using Athena::Units::ns;

StatusCode TileMuIdMonitorAlgorithm::initialize() {

  ATH_MSG_INFO("in initialize()");

  ATH_CHECK( m_muContainerKey.initialize() );

  using namespace Monitored;

  int nL1Triggers = getNumberOfL1Triggers();

  m_muIdTagsGroups = buildToolMap<int>(m_tools, "TileMuIdTags", nL1Triggers);
  m_muIdEnergyGroups = buildToolMap<int>(m_tools, "TileMuIdEnergy", nL1Triggers);
  m_muIdEtaGroups = buildToolMap<int>(m_tools, "TileMuIdEta", nL1Triggers);
  m_muIdPhiGroups = buildToolMap<int>(m_tools, "TileMuIdPhi", nL1Triggers);
  m_muIdQualityGroups = buildToolMap<int>(m_tools, "TileMuIdQuality", nL1Triggers);
  m_muIdEtaPhiGroups = buildToolMap<int>(m_tools, "TileMuIdEtaPhi", nL1Triggers);
  m_muIdEneEtaGroups = buildToolMap<int>(m_tools, "TileMuIdEneEta", nL1Triggers);
  m_muIdEnePhiGroups = buildToolMap<int>(m_tools, "TileMuIdEnePhi", nL1Triggers);
  m_muIdEneQualityGroups = buildToolMap<int>(m_tools, "TileMuIdEneQuality", nL1Triggers);

  m_muId1TagEnergyGroups = buildToolMap<int>(m_tools, "TileMuId1TagEnergy", nL1Triggers);
  m_muId1TagQualityGroups = buildToolMap<int>(m_tools, "TileMuId1TagQuality", nL1Triggers);
  m_muId1TagEtaPhiGroups = buildToolMap<int>(m_tools, "TileMuId1TagEtaPhi", nL1Triggers);
  m_muId1TagEtaGroups = buildToolMap<int>(m_tools, "TileMuId1TagEta", nL1Triggers);
  m_muId1TagPhiGroups = buildToolMap<int>(m_tools, "TileMuId1TagPhi", nL1Triggers);
  m_muId1TagEneQualityGroups = buildToolMap<int>(m_tools, "TileMuId1TagEneQuality", nL1Triggers);

  m_muId2TagEnergyGroups = buildToolMap<int>(m_tools, "TileMuId2TagEnergy", nL1Triggers);
  m_muId2TagQualityGroups = buildToolMap<int>(m_tools, "TileMuId2TagQuality", nL1Triggers);
  m_muId2TagEtaPhiGroups = buildToolMap<int>(m_tools, "TileMuId2TagEtaPhi", nL1Triggers);
  m_muId2TagEtaGroups = buildToolMap<int>(m_tools, "TileMuId2TagEta", nL1Triggers);
  m_muId2TagPhiGroups = buildToolMap<int>(m_tools, "TileMuId2TagPhi", nL1Triggers);
  m_muId2TagEneQualityGroups = buildToolMap<int>(m_tools, "TileMuId2TagEneQuality", nL1Triggers);

  return TileMonitorAlgorithm::initialize();
}


StatusCode TileMuIdMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {


    // In case you want to measure the execution time
  auto timer = Monitored::Timer("TIME_execute");

  const xAOD::EventInfo* eventInfo = GetEventInfo(ctx).get();

  // Indices of L1 trigger histograms to be filled in the current event
  std::vector<int> l1TriggersIndices = getL1TriggerIndices(eventInfo->level1TriggerType());

  SG::ReadHandle<TileMuContainer> muContainer(m_muContainerKey, ctx);
  ATH_CHECK( muContainer.isValid() );

  int nTags = muContainer->size();
  ATH_MSG_DEBUG( "Tile MuId nubmer of tags in the event: " << nTags );

  auto monNTags = Monitored::Scalar<int>("nTags", nTags);
  for (int l1TriggerIdx : l1TriggersIndices) {
    fill(m_tools[m_muIdTagsGroups[l1TriggerIdx]], monNTags);
  }

  auto monEta = Monitored::Collection("eta", *muContainer, [] (const TileMu* mu) {return mu->eta();});
  auto monPhi = Monitored::Collection("phi", *muContainer, [] (const TileMu* mu) {return mu->phi();});
  auto monQuality = Monitored::Collection("quality", *muContainer, [] (const TileMu* mu) {return mu->quality();});
  auto monEnergy = Monitored::Collection("energy", *muContainer,
                                         [] (const TileMu* mu) {
                                           return mu->enedep()[0] + mu->enedep()[1] + mu->enedep()[2];
                                         });

  for (int l1TriggerIdx : l1TriggersIndices) {
    fill(m_tools[m_muIdEnergyGroups[l1TriggerIdx]], monEnergy);
    fill(m_tools[m_muIdEtaGroups[l1TriggerIdx]], monEta);
    fill(m_tools[m_muIdPhiGroups[l1TriggerIdx]], monPhi);
    fill(m_tools[m_muIdQualityGroups[l1TriggerIdx]], monQuality);
    fill(m_tools[m_muIdEtaPhiGroups[l1TriggerIdx]], monEta, monPhi);
    fill(m_tools[m_muIdEneEtaGroups[l1TriggerIdx]], monEta, monEnergy);
    fill(m_tools[m_muIdEnePhiGroups[l1TriggerIdx]], monPhi, monEnergy);
    fill(m_tools[m_muIdEneQualityGroups[l1TriggerIdx]], monEnergy, monQuality);
  }

  if (nTags == 1) {
    for (int l1TriggerIdx : l1TriggersIndices) {
      fill(m_tools[m_muId1TagEnergyGroups[l1TriggerIdx]], monEnergy);
      fill(m_tools[m_muId1TagQualityGroups[l1TriggerIdx]], monQuality);
      fill(m_tools[m_muId1TagEtaPhiGroups[l1TriggerIdx]], monEta, monPhi);
      fill(m_tools[m_muId1TagEtaGroups[l1TriggerIdx]], monEta, monEnergy);
      fill(m_tools[m_muId1TagPhiGroups[l1TriggerIdx]], monPhi, monEnergy);
      fill(m_tools[m_muId1TagEneQualityGroups[l1TriggerIdx]], monEnergy, monQuality);
    }
  } else if (nTags == 2) {
    for (int l1TriggerIdx : l1TriggersIndices) {
      fill(m_tools[m_muId2TagEnergyGroups[l1TriggerIdx]], monEnergy);
      fill(m_tools[m_muId2TagQualityGroups[l1TriggerIdx]], monQuality);
      fill(m_tools[m_muId2TagEtaPhiGroups[l1TriggerIdx]], monEta, monPhi);
      fill(m_tools[m_muId2TagEtaGroups[l1TriggerIdx]], monEta, monEnergy);
      fill(m_tools[m_muId2TagPhiGroups[l1TriggerIdx]], monPhi, monEnergy);
      fill(m_tools[m_muId2TagEneQualityGroups[l1TriggerIdx]], monEnergy, monQuality);
    }
  }

  fill("TileMuIdMonExecuteTime", timer);

  return StatusCode::SUCCESS;
}
