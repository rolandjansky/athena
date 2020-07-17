/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TileCalCellMonAlg.h"
#include "CaloIdentifier/TileID.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadCondHandle.h"

#include "AthenaKernel/Units.h"

using Athena::Units::GeV;

StatusCode TileCalCellMonAlg::initialize() {

  ATH_CHECK( detStore()->retrieve(m_tileID) );

  ATH_CHECK( m_cellContainerKey.initialize() );
  ATH_CHECK( m_caloNoiseKey.initialize() );

  // Excluding histogram for Tile E sample, but including histogram for all samples instead
  m_noiseEtaPhiGroups = Monitored::buildToolMap<int>(m_tools, "CellsNoiseXEtaVSPhi", N_TILE_SAMPLES);
  // Histogams per Tile sample
  m_energyModuleGroups = Monitored::buildToolMap<int>(m_tools, "CellsXModule", N_TILE_SAMPLES);

  return CaloMonAlgBase::initialize();
}

StatusCode TileCalCellMonAlg::fillHistograms( const EventContext& ctx ) const {

  // In case you want to measure the execution time
  auto timer = Monitored::Timer("TIME_execute");

  bool ifPass = true;
  bool passBeamBackgroundRemoval = false;
  StatusCode sc = checkFilters(ifPass, passBeamBackgroundRemoval, m_monGroupName, ctx);
  if(sc.isFailure() || !ifPass) return StatusCode::SUCCESS;

  ATH_MSG_VERBOSE("::checkFilters() passed");

  const int nTileCells = 5184;
  std::vector<float> cellEnergyToNoiseRatio;
  cellEnergyToNoiseRatio.reserve(nTileCells);

  std::vector<float> cellNoiseDB;
  std::vector<float> cellNoiseDBeta;
  std::vector<float> cellNoiseDBphi;
  cellNoiseDB.reserve(nTileCells);
  cellNoiseDBeta.reserve(nTileCells);
  cellNoiseDBphi.reserve(nTileCells);

  std::vector<float> sampleEta[N_TILE_SAMPLES];
  std::vector<float> samplePhi[N_TILE_SAMPLES];
  std::vector<IdentifierHash> hashes;

  const int nCellsOverThr = 500;
  std::vector<float> overThrEta;
  std::vector<float> overThrPhi;
  std::vector<float> overThrTower;
  std::vector<float> overThrEnergyGeV;
  overThrEta.reserve(nCellsOverThr);
  overThrPhi.reserve(nCellsOverThr);
  overThrTower.reserve(nCellsOverThr);
  overThrEnergyGeV.reserve(nCellsOverThr);

  std::vector<float> overThrModule[N_TILE_SAMPLES];
  std::vector<float> overThrModuleEnergy[N_TILE_SAMPLES];

  SG::ReadCondHandle<CaloNoise> caloNoise{m_caloNoiseKey, ctx};
  SG::ReadHandle<CaloCellContainer> cellContainer{m_cellContainerKey, ctx};
  ATH_CHECK( cellContainer.isValid() );

  for (const CaloCell* cell : *cellContainer) {
    Identifier id = cell->ID();
    if (m_tileID->is_tile(id)) {
      IdentifierHash hash = m_tileID->cell_hash(id);
      int sample = m_tileID->sample(id);
      int module = m_tileID->module(id);
      int tower = m_tileID->tower(id);
      float energy = cell->energy();
      double eta = cell->eta();
      double phi = cell->phi();
      bool isCellGood = !(cell->badcell());

      if (isCellGood && sample < TileID::SAMP_E) {
        float cellNoise = (m_twoGaussianNoise) ? caloNoise->getEffectiveSigma(id, cell->gain(), energy)
                                               : caloNoise->getNoise(id, cell->gain());

        if (std::isfinite(cellNoise) && cellNoise > 0 && energy != 0) {
          float energyToNoiseRatio = energy / cellNoise;
          cellEnergyToNoiseRatio.push_back(energyToNoiseRatio);

          if (std::abs(energyToNoiseRatio) > 4.0) {
            hashes.push_back(hash);
            sampleEta[sample].push_back(eta);
            samplePhi[sample].push_back(phi);

            ATH_MSG_VERBOSE( "Cell rs=" << energyToNoiseRatio << " e=" << energy << " eta=" << eta << " phi="<< phi  );
            ATH_MSG_VERBOSE( "hash=" << hash << " module= " << module + 1 << " sample=" << sample );
          }

          cellNoiseDB.push_back(cellNoise);
          cellNoiseDBeta.push_back(eta);
          cellNoiseDBphi.push_back(phi);
        }
      }

      if (energy > m_energyThreshold) {
        float energyGeV = energy / GeV;

        overThrEta.push_back(eta);
        overThrPhi.push_back(phi);
        overThrTower.push_back(tower + 1);
        overThrEnergyGeV.push_back(energyGeV);
        overThrModule[sample].push_back(module + 1);
        overThrModuleEnergy[sample].push_back(energyGeV);
      }
    }
  }


  // Excluding Tile E sample, but using all samples instead
  int allSamples = TileID::SAMP_E;
  for (int sample = 0; sample < TileID::SAMP_E; ++sample) {
    if (!sampleEta[sample].empty()) {
       auto monEta = Monitored::Collection("eta", sampleEta[sample]);
       auto monPhi = Monitored::Collection("phi", samplePhi[sample]);
       fill(m_tools[m_noiseEtaPhiGroups[sample]], monEta, monPhi);
       fill(m_tools[m_noiseEtaPhiGroups[allSamples]], monEta, monPhi);
    }
  }

  if (!hashes.empty()) {
    auto monHash = Monitored::Collection("hash", hashes);
    fill("CellsXNoiseXHash", monHash);
  }

  if (!cellEnergyToNoiseRatio.empty()) {
    auto monCellEnergyToNoiseRatio = Monitored::Collection("noise", cellEnergyToNoiseRatio);
    fill("CellsNoiseTile", monCellEnergyToNoiseRatio);
  }

  if (!cellNoiseDB.empty()) {
    auto monCellNoise = Monitored::Collection("noise", cellNoiseDB);
    auto monEta = Monitored::Collection("eta", cellNoiseDBeta);
    auto monPhi = Monitored::Collection("phi", cellNoiseDBphi);
    fill("CellsNoiseXEta", monEta, monCellNoise);
    fill("CellsNoiseXPhi", monPhi, monCellNoise);
  }


  if (!overThrEnergyGeV.empty()) {

    auto monCellsNumber = Monitored::Scalar<unsigned int>("nCells", overThrEnergyGeV.size());
    fill("CellsXN", monCellsNumber);

    auto monEnergy = Monitored::Collection("energy", overThrEnergyGeV);
    fill("CellsXE", monEnergy);

    auto monEta = Monitored::Collection("eta", overThrEta);
    fill("CellsXEta", monEta, monEnergy);

    auto monPhi = Monitored::Collection("phi", overThrPhi);
    fill("CellsXPhi", monPhi, monEnergy);

    auto monTower = Monitored::Collection("tower", overThrTower);
    fill("CellsXTower", monTower, monEnergy);

    fill("CellsXEtaVSPhi", monEta, monPhi);

    for (int sample = 0; sample <= TileID::SAMP_E; ++sample) {
      if (!overThrModuleEnergy[sample].empty()) {
        auto monModule = Monitored::Collection("module", overThrModule[sample]);
        auto monEnergy = Monitored::Collection("energy", overThrModuleEnergy[sample]);
        fill(m_tools[m_energyModuleGroups[sample]], monModule, monEnergy);
      }
    }
  }

  fill("TileCalCellMonExecuteTime", timer);

  return StatusCode::SUCCESS;
}
