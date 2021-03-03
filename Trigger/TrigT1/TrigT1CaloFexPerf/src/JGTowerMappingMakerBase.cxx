#include "JGTowerMappingMakerBase.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "TMath.h"
#include "TVector2.h"
#include "PathResolver/PathResolver.h"
#include "TFile.h"
#include "TH1F.h"
#include <memory>
#include "GaudiKernel/SystemOfUnits.h"
#include <map>
#include <utility>
#include <algorithm>
#include <fstream>

namespace LVL1
{
  JGTowerMappingMakerBase::JGTowerMappingMakerBase(
      const std::string &type,
      const std::string &name,
      const IInterface *parent)
      : AthAlgTool(type, name, parent)
  {
    declareInterface<IJGTowerMappingMaker>(this);
    declareProperty("MapTileCells", m_mapTileCells = false, "If false, don't map tile cells to the helpers");
  }

  JGTowerMappingMakerBase::~JGTowerMappingMakerBase() {}

  StatusCode JGTowerMappingMakerBase::initialize()
  {
    ATH_CHECK(detStore()->retrieve(m_scDetMgr));
    ATH_CHECK(detStore()->retrieve(m_ccid));
    ATH_CHECK(detStore()->retrieve(m_scid));
    return StatusCode::SUCCESS;
  }

  const std::vector<JGTowerHelper> &JGTowerMappingMakerBase::getMapping() const
  {
    return m_mapping;
  }

  StatusCode JGTowerMappingMakerBase::buildMapping(const JGTowerBase_ID *jgTowerID)
  {
    ATH_MSG_INFO("Build tower mapping");
    if (m_mapping.size() > 0)
    {
      ATH_MSG_WARNING("JTowerMappingMaker::buildMapping called multiple times!");
      return StatusCode::SUCCESS;
    }

    // Store the towers in a map by their location to identify ones which are in front/behind each other
    // tuple elements: pos_neg, region, eta, phi
    std::map<std::tuple<int, int, int, int>, std::vector<std::size_t>> towerGrid;
    // For speed later, also store the max phi index and min/max eta indices per (int) region code
    std::map<int, std::tuple<int, int, int>> regionParams;

    for (std::size_t towerIdx = 0; towerIdx < jgTowerID->tower_hash_max(); ++towerIdx)
    {
      Identifier towerID = jgTowerID->tower_id(towerIdx);
      Identifier regionID = jgTowerID->region_id(towerID);

      int detSide = jgTowerID->pos_neg(towerID) < 0 ? -1 : +1;

      // Get the phi and eta boundaries of the towers (only advisable for barrel)
      float jDEta = jgTowerID->etaGranularity(regionID);
      float jDPhi = jgTowerID->phiGranularity(regionID);
      int nTowers = (int)std::ceil(TMath::Pi() / jDPhi);
      jDPhi = TMath::Pi() / nTowers;

      float jEta = detSide * ((jgTowerID->eta(towerID) + 0.5) * jDEta + jgTowerID->eta0(regionID));
      float jPhi = TVector2::Phi_mpi_pi((jgTowerID->phi(towerID) + 0.5) * jDPhi + jgTowerID->phi0(regionID));

      m_mapping.emplace_back(jEta, jDEta, jPhi, jDPhi);
      JGTowerHelper &helper = m_mapping.back();
      helper.SetSampling(jgTowerID->sampling(towerID));

      std::vector<std::size_t> &gridLoc = towerGrid[std::make_tuple(
          jgTowerID->pos_neg(towerID),
          jgTowerID->region(towerID),
          jgTowerID->eta(towerID),
          jgTowerID->phi(towerID))];
      regionParams[jgTowerID->region(towerID)] = std::make_tuple(
          jgTowerID->phi_max(regionID),
          jgTowerID->eta_min(regionID),
          jgTowerID->eta_max(regionID));

      // Now insert our index into this such that the vector remains sorted in ascending order of sampling
      gridLoc.insert(std::lower_bound(
                         gridLoc.begin(), gridLoc.end(), towerIdx,
                         [this](std::size_t lhs, std::size_t rhs) { return m_mapping.at(lhs).sampling() < m_mapping.at(rhs).sampling(); }),
                     towerIdx);

      for (std::size_t scIdx = 0; scIdx < m_scid->calo_cell_hash_max(); ++scIdx)
      {
        Identifier scID = m_scid->cell_id(scIdx);
        const CaloDetDescrElement *dde = m_scDetMgr->get_element(scID);
        if (!dde)
        {
          ATH_MSG_ERROR("Error loading CaloDetDescrElement");
          return StatusCode::FAILURE;
        }
        if (m_scid->is_tile(scID) && m_scid->sampling(scID) != 2)
        {
          if (!m_mapTileCells)
            continue;
          if (jgTowerID->sampling(towerID) != 0 && helper.inBox(dde->eta_raw(), dde->phi_raw()))
            helper.SetTileIndices(scIdx);
        }
        else
        {
          float superCellEta = dde->eta_raw();
          // Treat forward super cells separately
          if (std::abs(superCellEta) > 3.2)
            continue;
          // Very special case being handle here. At the end of the barrel there is a constant-eta ring that has
          // eta_raw=1.4 (which is the midpoint of the scell). These will be put into the g/jTower that starts at eta=1.4
          if (std::abs(std::abs(superCellEta) - 1.4) < 0.001 &&
              m_scid->region(scID) == 0 &&
              m_scid->sampling(scID) == 2)
            superCellEta += (superCellEta > 0 ? 0.05 : -0.05);
          if (helper.inBox(superCellEta, dde->phi_raw()) &&
              jgTowerID->pos_neg(towerID) * m_scid->pos_neg(scID) > 0 &&
              ((m_scid->is_em(scID) && jgTowerID->sampling(towerID) == 0) ||
               (!m_scid->is_em(scID) && jgTowerID->sampling(towerID) == 1)))
            helper.SetSCIndices(scIdx);
        }
      } //> end loop over SC indices
    }   //> end loop over tower IDs

    std::ofstream fOut(name() + ".txt");
    fOut << "Print regions" << std::endl;
    for (const auto &p : regionParams)
      fOut << "Region #" << p.first
           << ": max phi = " << std::get<0>(p.second)
           << ", min eta = " << std::get<1>(p.second)
           << ", max eta = " << std::get<2>(p.second) << std::endl;

    fOut << "Print grid" << std::endl;
    for (auto &gridLocPair : towerGrid)
    {
      std::vector<std::size_t> &towerIndices = gridLocPair.second;
      // Figure out the relations to this square in the grid
      int pos_neg = std::get<0>(gridLocPair.first);
      int region = std::get<1>(gridLocPair.first);
      int iEta = std::get<2>(gridLocPair.first);
      int iPhi = std::get<3>(gridLocPair.first);
      int phiMax = std::get<0>(regionParams[region]);
      int etaMin = std::get<1>(regionParams[region]);
      int etaMax = std::get<2>(regionParams[region]);
      auto nextPhiItr = towerGrid.find(std::make_tuple(pos_neg, region, iEta, iPhi == phiMax ? 0 : iPhi + 1));
      auto prevPhiItr = towerGrid.find(std::make_tuple(pos_neg, region, iEta, iPhi == 0 ? phiMax : iPhi - 1));
      auto nextEtaItr = towerGrid.end();
      auto prevEtaItr = towerGrid.end();
      std::string indicesString = std::to_string(towerIndices.at(0));
      for (std::size_t idx = 1; idx < towerIndices.size(); ++idx)
        indicesString += ", " + std::to_string(towerIndices.at(idx));
      fOut << "pos_neg = "
           << pos_neg << ", region = "
           << region << ", iEta = "
           << iEta << ", iPhi = "
           << iPhi << ": "
           << indicesString << std::endl;
      // We want 'next' and 'prev' to refer to increasing eta, not increasing |eta|
      // Increasing 'region' increases |eta| so in what follows we correct for this by
      // choosing whether we set nextEtaItr or prevEtaItr based on the sign of pos_neg
      if (iEta == etaMax)
      {
        // Check if the next region a) exists and b) has the right phi granularity
        auto itr = regionParams.find(region + 1);
        if (itr != regionParams.end() && std::get<0>(itr->second) == phiMax)
          // Set the next eta index to the next region and the eta_min index of that region (probably 0)
          (pos_neg > 0 ? nextEtaItr : prevEtaItr) = towerGrid.find(std::make_tuple(pos_neg, region + 1, std::get<1>(itr->second), iPhi));
      }
      else
        (pos_neg > 0 ? nextEtaItr : prevEtaItr) = towerGrid.find(std::make_tuple(pos_neg, region, iEta + 1, iPhi));
      if (iEta == etaMin)
      {
        if (region == 0)
          (pos_neg > 0 ? prevEtaItr : nextEtaItr) = towerGrid.find(std::make_tuple(pos_neg * -1, region, iEta, iPhi));
        else
        {
          auto itr = regionParams.find(region - 1);
          if (itr != regionParams.end() && std::get<0>(itr->second) == phiMax)
            // Set the previous eta index to the previous region and the eta_max index of that region
            (pos_neg > 0 ? prevEtaItr : nextEtaItr) = towerGrid.find(std::make_tuple(pos_neg, region - 1, std::get<2>(itr->second), iPhi));
        }
      }
      else
        (pos_neg > 0 ? prevEtaItr : nextEtaItr) = towerGrid.find(std::make_tuple(pos_neg, region, iEta - 1, iPhi));

      std::vector<std::size_t> nextEtaTowers;
      std::vector<std::size_t> prevEtaTowers;
      std::vector<std::size_t> nextPhiTowers;
      std::vector<std::size_t> prevPhiTowers;
      if (nextEtaItr != towerGrid.end())
        nextEtaTowers = nextEtaItr->second;
      if (prevEtaItr != towerGrid.end())
        prevEtaTowers = prevEtaItr->second;
      if (nextPhiItr != towerGrid.end())
        nextPhiTowers = nextPhiItr->second;
      if (prevPhiItr != towerGrid.end())
        prevPhiTowers = prevPhiItr->second;
      nextEtaTowers.resize(towerIndices.size(), SIZE_MAX);
      prevEtaTowers.resize(towerIndices.size(), SIZE_MAX);
      nextPhiTowers.resize(towerIndices.size(), SIZE_MAX);
      prevPhiTowers.resize(towerIndices.size(), SIZE_MAX);

      std::size_t previousIdx = SIZE_MAX;
      for (std::size_t iIdx = 0; iIdx < towerIndices.size(); ++iIdx)
      {
        // NB: iIdx here is an index of _indices_, i.e. the position in the towerIndex
        // vector as well as the next/prev eta/phi towers vectors
        std::size_t currentIdx = towerIndices.at(iIdx);
        JGTowerHelper &helper = m_mapping.at(currentIdx);

        helper.setNextEtaIndex(nextEtaTowers.at(iIdx));
        helper.setPreviousEtaIndex(prevEtaTowers.at(iIdx));
        helper.setNextPhiIndex(nextPhiTowers.at(iIdx));
        helper.setPreviousPhiIndex(prevPhiTowers.at(iIdx));

        helper.setIndexInFront(previousIdx);
        if (previousIdx != SIZE_MAX)
          m_mapping.at(previousIdx).setIndexBehind(currentIdx);
        previousIdx = currentIdx;
      }
    }

    ATH_CHECK(buildForwardMapping());
    ATH_CHECK(loadTowerAreas());

    return StatusCode::SUCCESS;
  }
} // namespace LVL1