/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */
 
#include "SCEmulation.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "PathResolver/PathResolver.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include <memory>
#include <TFile.h>
#include <TKey.h>
#include <TRandom3.h>
#include <regex>
#include <set>
#include <algorithm>
#include <limits>
#include <random>
#include <cstring>

namespace
{
  /**
   * @brief Return the correct bin index for value
   * @param bins Ordered list of bin edges
   * @param value The value to test
   * 
   * Each bin contains its lower edge and not its upper edge
   */
  std::size_t getIndex(const std::vector<float> &bins, float value)
  {
    auto itr = std::upper_bound(bins.begin(), bins.end(), value);
    // upper_bound returns an iterator to the first value larger than value
    // If begin is returned, then value is below the lowest bin edge,
    // If end is returned, then value is above the highest bin edge
    if (itr == bins.begin() || itr == bins.end())
      return SIZE_MAX;
    return std::distance(bins.begin(), itr) - 1;
  }

  /// Get a random number from a binned cdf
  float getRandom(const std::map<float, float>& integrals, std::mt19937_64& generator)
  {
    // The integrals are essentially a binned cdf (though not necessarily properly normalised)
    // We invert this by constructing a random number uniformly distributed between the
    // minimum and maximum values of the cumulative integral (the keys of the map).
    // This allows us to locate the correct bin
    // The values in the map then correspond to upper edges of each bin, so we can get the
    // pair of edges from the iterator found by lower_bound and the previous one.
    // We then perform a simple linear interpolation using how far the random number is into the cdf bin
    // to approximate the timing to be returned
    float start = integrals.begin()->first;
    float end = integrals.rbegin()->first;
    std::uniform_real_distribution<float> distr(start, end);
    float r = distr(generator);
    auto itr = integrals.lower_bound(r);
    if (itr == integrals.end() || itr == integrals.begin())
    {
      // This should never be triggered
      return -1;
    }
    float lowBinEdge, lowBinValue, highBinEdge, highBinValue;
    std::tie(highBinEdge, highBinValue) = *itr;
    std::tie(lowBinEdge, lowBinValue) = *(--itr);
    // Interpolate between the two bin edges
    float pctDiff = (r - lowBinEdge) / (highBinEdge - lowBinEdge);
    return lowBinValue + pctDiff * (highBinValue - lowBinValue);
  }
} // namespace

namespace LVL1
{
  SCEmulation::SCEmulation(const std::string &name, ISvcLocator *pSvcLocator)
      : AthAlgorithm(name, pSvcLocator),
        m_noiseTool("CaloNoiseToolDefault")
  {
    declareProperty("InputCells", m_inputCellsKey = "AllCalo");
    declareProperty("OutputSuperCells", m_outputSuperCellsKey = "SCEmulationNoBCId");
    declareProperty("EventInfo", m_evtInfoKey = "EventInfo");
    declareProperty("SCIDTool", m_scidTool);
    declareProperty("CaloNoiseTool", m_noiseTool);
    declareProperty("LumiBCIDTool", m_caloLumiBCIDTool);
    declareProperty("CompensateForNoise", m_useNoise = true);
    declareProperty("CompensateForBCID", m_useBCID = true);
    declareProperty("CellTimingFile", m_cellTimingFile = "Run3L1CaloSimulation/SuperCells/CellTimingDistributions.MiddleTrain.r11881.20210211.root");
  }

  SCEmulation::~SCEmulation() {}

  StatusCode SCEmulation::initialize()
  {
    ATH_MSG_INFO("Initializing " << name() << "...");
    if (m_useNoise)
      ATH_CHECK(m_noiseTool.retrieve());
    if (m_useBCID)
      ATH_CHECK(m_caloLumiBCIDTool.retrieve());
    ATH_CHECK(m_evtInfoKey.initialize());
    ATH_CHECK(m_scidTool.retrieve());
    ATH_CHECK(m_inputCellsKey.initialize());
    ATH_CHECK(m_outputSuperCellsKey.initialize());

    ATH_CHECK(detStore()->retrieve(m_scellMgr));
    ATH_CHECK(detStore()->retrieve(m_caloIdMgr));

    std::unique_ptr<TFile> timingFile(TFile::Open(PathResolver::find_calib_file(m_cellTimingFile).c_str()));
    ATH_MSG_INFO("...");
    if (!timingFile || timingFile->IsZombie())
    {
      ATH_MSG_ERROR("Failed to open cell timing file " << m_cellTimingFile);
      return StatusCode::FAILURE;
    }
    TDirectory *tdir = timingFile->GetDirectory("CellTiming");
    if (!tdir)
    {
      ATH_MSG_ERROR(m_cellTimingFile << " has no CellTiming directory!");
      return StatusCode::FAILURE;
    }

    // Regex pattern for histogram names
    std::regex pattern("Layer(\\d+)_([\\d.]+)eta([\\d.]+)_([\\d.-]+)ET([\\d.-]+)_midtrain");
    std::map<CaloSampling::CaloSample, std::set<std::pair<float, float>>> etaBins;
    std::map<CaloSampling::CaloSample, std::set<std::pair<float, float>>> etBins;
    std::map<CaloSampling::CaloSample, std::map<std::pair<std::pair<float, float>, std::pair<float, float>>, TH1F *>> tmpHistMap;
    std::set<CaloSampling::CaloSample> samples;
    for (TObject *obj : *tdir->GetListOfKeys())
    {
      TKey *key = dynamic_cast<TKey *>(obj);
      if (!key)
      {
        ATH_MSG_ERROR(obj->GetName() << " is not a TKey");
        return StatusCode::FAILURE;
      }
      std::cmatch match;
      if (!std::regex_match(key->GetName(), match, pattern))
      {
        continue;
      }
      if (std::strcmp(key->GetClassName(), "TH1F") != 0)
      {
        ATH_MSG_ERROR("Object " << key->GetName() << " not histogram as expected!");
        return StatusCode::FAILURE;
      }
      CaloSampling::CaloSample sample = static_cast<CaloSampling::CaloSample>(std::stoi(match.str(1)));
      samples.insert(sample);

      auto etaBin = std::make_pair(std::stof(match.str(2)), std::stof(match.str(3)));
      auto etBin = std::make_pair(std::stof(match.str(4)), std::stof(match.str(5)));

      etaBins[sample].insert(etaBin);
      etBins[sample].insert(etBin);
      tmpHistMap[sample][std::make_pair(etaBin, etBin)] = (TH1F *)key->ReadObj();
    }
    // Now regularise the binning
    for (CaloSampling::CaloSample sample : samples)
    {
      auto itr = etaBins[sample].begin();
      auto end = etaBins[sample].end();
      m_etaBins[sample].reserve(std::distance(itr, end) + 1);
      m_etaBins[sample].push_back(itr->first);
      m_etaBins[sample].push_back(itr->second);
      ++itr;
      for (; itr != end; ++itr)
      {
        // Make sure that the bin edges match up
        if (itr->first != m_etaBins[sample].back())
        {
          ATH_MSG_ERROR("Eta bins do not match up for sample " << sample << "(" << m_etaBins[sample].back() << ", " << itr->first << ")");
          return StatusCode::FAILURE;
        }
        m_etaBins[sample].push_back(itr->second);
      }

      itr = etBins[sample].begin();
      end = etBins[sample].end();
      m_etBins[sample].reserve(std::distance(itr, end) + 1);
      m_etBins[sample].push_back(itr->first);
      m_etBins[sample].push_back(itr->second);
      ++itr;
      for (; itr != end; ++itr)
      {
        // Make sure that the bin edges match up
        if (itr->first != m_etBins[sample].back())
        {
          ATH_MSG_ERROR("Et bins do not match up for sample " << sample);
          return StatusCode::FAILURE;
        }
        m_etBins[sample].push_back(itr->second);
      }

      // Now copy the histograms over
      for (const auto &p : tmpHistMap[sample])
      {
        // Use the lower bounds of each bins to get the index
        std::size_t etaIndex = getEtaIndex(sample, p.first.first.first);
        std::size_t etIndex = getEtIndex(sample, p.first.second.first);
        auto mapKey = std::make_tuple(sample, etaIndex, etIndex);
        std::map<float, float>& integrals = m_timingSamples[mapKey];
        float cumulativeSum = 0;
        TAxis* axis = p.second->GetXaxis();
        integrals[cumulativeSum] = axis->GetBinLowEdge(1);
        for (int idx = 1; idx < axis->GetNbins(); ++idx)
        {
          cumulativeSum += p.second->GetBinContent(idx);
          integrals[cumulativeSum] = axis->GetBinUpEdge(idx);
        }
      }
    }
    return StatusCode::SUCCESS;
  }

  StatusCode SCEmulation::execute()
  {
    const EventContext &ctx = Gaudi::Hive::currentContext();

    // Prepare output container
    auto superCells = std::make_unique<CaloCellContainer>();
    // Get input
    auto cells = SG::makeHandle(m_inputCellsKey, ctx);
    if (!cells.isValid())
    {
      ATH_MSG_ERROR("Failed to retrieve input cells " << m_inputCellsKey.key());
      return StatusCode::FAILURE;
    }

    uint32_t bcid = -1;
    auto evtInfo = SG::makeHandle(m_evtInfoKey, ctx);
    if (!evtInfo.isValid())
    {
      ATH_MSG_ERROR("Failed to retrieve " << m_evtInfoKey.key());
      return StatusCode::FAILURE;
    }
    std::mt19937_64 generator;
    // seed the generator for the whole event
    generator.seed(evtInfo->eventNumber() * evtInfo->runNumber());
    if (m_useBCID)
      bcid = evtInfo->bcid();

    const CaloCell_SuperCell_ID *superCellIDHelper = m_caloIdMgr->getCaloCell_SuperCell_ID();
    const TileID *tileCellID = m_caloIdMgr->getTileID();
    const Tile_SuperCell_ID *tileSuperCellID = m_caloIdMgr->getTile_SuperCell_ID();

    // Prepare the output values
    std::size_t nSuperCells = superCellIDHelper->calo_cell_hash_max();
    std::vector<float> energies(nSuperCells, 0.0);
    std::vector<float> enTime(nSuperCells, 0.0);
    std::vector<float> enForTime(nSuperCells, 0.0);
    std::vector<char> timeDef(nSuperCells, false);
    std::vector<uint16_t> qualities(nSuperCells, 0);
    std::vector<float> sigmaNoisePerSuperCell(nSuperCells, 0.0);

    for (const CaloCell *cell : *cells)
    {
      Identifier cellID = cell->ID();
      // map to super cell ID
      Identifier superCellID = m_scidTool->offlineToSuperCellID(cellID);
      const CaloDetDescrElement *cdde = cell->caloDDE();
      if (!superCellID.is_valid())
      {
        continue;
      }
      IdentifierHash scIDHash = superCellIDHelper->calo_cell_hash(superCellID);
      float energy = cell->energy();
      if (m_useBCID)
        energy += m_caloLumiBCIDTool->average(cell, bcid);
      energies[scIDHash] += energy;
      if (m_useNoise && !cdde->is_tile() && cell->gain() == CaloGain::LARHIGHGAIN)
      {
        std::vector<float> noise = m_noiseTool->elecNoiseRMS3gains(cdde);
        float sigma = noise.at(1)*noise.at(1) - noise.at(0)*noise.at(0);
        sigmaNoisePerSuperCell[scIDHash] += (sigma > 0 ? std::sqrt(sigma) : 0.0f);
      }
      // This is a bad definition, but it's needed to be consistent with the other code (for now...)
      CaloSampling::CaloSample s = cell->caloDDE()->getSampling();
      bool isTile_BAD = s >= 9 && s <21;
      if (cell->provenance() & 0x2000)
      {
        if (cell->energy() > 256)
        {
          // We have the timing values correctly for above 256
          timeDef[scIDHash] |= true;
          enForTime[scIDHash] += cell->energy();
          enTime[scIDHash] += cell->energy() * cell->time();
        }
        else if (!isTile_BAD)
        {
          // Use the random sampling from timing histograms (only midtrain)
          
          CaloSampling::CaloSample sample = cell->caloDDE()->getSampling();
          // Locate the correct eta/et bins
          std::size_t iEta = getEtaIndex(sample, std::abs(cell->eta()));
          if (iEta == SIZE_MAX)
          {
            ATH_MSG_ERROR("Eta value " << cell->eta() << " for sampling " << sample << " does not fall in a bin");
            return StatusCode::FAILURE;
          }
          std::size_t iEt = getEtIndex(sample, cell->et());
          if (iEt == SIZE_MAX)
          {
            ATH_MSG_ERROR("Et value " << cell->et() << " for sampling " << sample << " does not fall in a bin");
            return StatusCode::FAILURE;
          }
          float cellTime = getRandom(m_timingSamples.at(std::make_tuple(sample, iEta, iEt)), generator);
          timeDef.at(scIDHash) |= true;
          enForTime.at(scIDHash) += cell->energy();
          enTime.at(scIDHash) += cell->energy() * cellTime;
        }
      } //> if (provenance & 0x2000)
      uint16_t &quality = qualities.at(scIDHash);
      // Add the qualities such that you don't overflow the storage
      if ((std::numeric_limits<uint16_t>::max() + 1 - quality) < cell->quality())
        quality = std::numeric_limits<uint16_t>::max();
      else
        quality += cell->quality();
      //ATH_MSG_INFO("Quality is " << quality);
      // Special case for SAMP_D in tile. The signal is split into two SCs
      if (cdde->is_tile() && tileCellID->sampling(cellID) == TileID::SAMP_D)
      {
        int section = tileCellID->section(cellID);
        int side = tileCellID->side(cellID);
        int module = tileCellID->module(cellID);
        int tower = tileCellID->tower(cellID);

        // Get the parameters for the new SCs
        int section1 = section;
        int section2 = section;
        int side1 = side;
        int side2 = side;
        int tower1 = tower;
        int tower2 = tower - 1;
        if (tower == 0)
        {
          side1 = -1;
          side2 = 1;
          tower1 = 0;
          tower2 = 0;
        }
        else if (tower == 10)
        {
          section1 = TileID::EXTBAR;
          section2 = TileID::BARREL;
        }

        Identifier scID1 = tileSuperCellID->cell_id(section1, side1, module, tower1, 0);
        Identifier scID2 = tileSuperCellID->cell_id(section2, side2, module, tower2, 0);

        // Split the energy between the SCs
        energies.at(superCellIDHelper->calo_cell_hash(scID1)) += cell->energy() * 0.5;
        energies.at(superCellIDHelper->calo_cell_hash(scID2)) += cell->energy() * 0.5;
      }
    } //> end loop over cells

    for (std::size_t idx = 0; idx < energies.size(); ++idx)
    {
      const CaloDetDescrElement *dde = m_scellMgr->get_element(idx);
      if (!dde)
      {
        ATH_MSG_WARNING("Invalid DDE for hash index " << idx);
        continue;
      }
      // Only push LAr supercells
      CaloSampling::CaloSample s = dde->getSampling();
      bool isTile_BAD = s >= 9 && s <21;
      if (isTile_BAD)
        continue;
      float energy = energies.at(idx);
      float sigmaNoise = sigmaNoisePerSuperCell.at(dde->calo_hash());
      if (!dde->is_tile() && sigmaNoise > 0.0)
      {
        std::normal_distribution<double> distribution(0.0, sigmaNoise);
        energy += distribution(generator);
      }
      auto superCell = std::make_unique<CaloCell>();
      superCell->setCaloDDE(dde);
      superCell->setEnergy(energy);
      uint16_t prov = 0;
      if (timeDef.at(idx) && enForTime.at(idx) != 0)
      {
        float time = enTime.at(idx) / enForTime.at(idx);
        superCell->setTime(time);
        float et = superCell->et();
        prov = 0x2000;
        if ((et > 10e3 && time > -8 && time < 18) || (et <= 10e3 && std::abs(time) < 8))
          prov |= 0x200;
      }
      else
        superCell->setTime(999.0);
      superCell->setProvenance(prov);
      superCell->setGain(CaloGain::LARHIGHGAIN);
      superCells->push_back(superCell.release());
    }
    auto superCellHandle = SG::makeHandle(m_outputSuperCellsKey, ctx);
    ATH_CHECK(superCellHandle.record(std::move(superCells)));
    return StatusCode::SUCCESS;
  }

  std::size_t SCEmulation::getEtaIndex(CaloSampling::CaloSample sample, float eta)
  {
    return getIndex(m_etaBins.at(sample), eta);
  }

  std::size_t SCEmulation::getEtIndex(CaloSampling::CaloSample sample, float et)
  {
    return getIndex(m_etBins.at(sample), et);
  }
} // namespace LVL1