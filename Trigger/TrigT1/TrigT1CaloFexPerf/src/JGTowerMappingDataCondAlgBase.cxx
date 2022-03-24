#include "JGTowerMappingDataCondAlgBase.h"
#include "AthenaKernel/IOVInfiniteRange.h"
#include "StoreGate/WriteCondHandle.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "TMath.h"
#include "TVector2.h"
#include <memory>
#include <map>
#include <tuple>
#include <algorithm>

namespace LVL1
{
  JGTowerMappingDataCondAlgBase::JGTowerMappingDataCondAlgBase(const std::string &name, ISvcLocator *pSvcLocator)
      : AthReentrantAlgorithm(name, pSvcLocator)
  {
  }

  JGTowerMappingDataCondAlgBase::~JGTowerMappingDataCondAlgBase() {}

  StatusCode JGTowerMappingDataCondAlgBase::initialize()
  {
    ATH_CHECK(m_outputKey.initialize());
    ATH_CHECK(detStore()->retrieve(m_ccid));
    ATH_CHECK(detStore()->retrieve(m_scid));
    return StatusCode::SUCCESS;
  }

  StatusCode JGTowerMappingDataCondAlgBase::execute(const EventContext &ctx) const
  {
    SG::WriteCondHandle<JGTowerMappingData> outputHandle(m_outputKey, ctx);
    if (outputHandle.isValid())
    {
      // In serial athena conditions algorithms are run on every event.
      // Therefore return early if the output object already exists.
      return StatusCode::SUCCESS;
    }
    const JGTowerBase_ID *jgTowerID = getJGTowerID();
    const CaloSuperCellDetDescrManager *scDetMgr{nullptr};
    ATH_CHECK(detStore()->retrieve(scDetMgr));
    // JGTowerMappingData is typedef of std::vector<JGTowerHelper>
    auto data = std::make_unique<JGTowerMappingData>();

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

      data->emplace_back(jEta, jDEta, jPhi, jDPhi);
      JGTowerHelper &helper = data->back();
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
                         [&data](std::size_t lhs, std::size_t rhs) { return data->at(lhs).sampling() < data->at(rhs).sampling(); }),
                     towerIdx);

      for (std::size_t scIdx = 0; scIdx < m_scid->calo_cell_hash_max(); ++scIdx)
      {
        Identifier scID = m_scid->cell_id(scIdx);
        const CaloDetDescrElement *dde = scDetMgr->get_element(scID);
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
        JGTowerHelper &helper = data->at(currentIdx);

        helper.setNextEtaIndex(nextEtaTowers.at(iIdx));
        helper.setPreviousEtaIndex(prevEtaTowers.at(iIdx));
        helper.setNextPhiIndex(nextPhiTowers.at(iIdx));
        helper.setPreviousPhiIndex(prevPhiTowers.at(iIdx));

        helper.setIndexInFront(previousIdx);
        if (previousIdx != SIZE_MAX)
          data->at(previousIdx).setIndexBehind(currentIdx);
        previousIdx = currentIdx;
      }
    }

    ATH_CHECK(buildForwardMapping(*data, scDetMgr));
    ATH_CHECK(loadTowerAreas(*data));

    // Set this object to be valid for every event
    outputHandle.addDependency(IOVInfiniteRange::infiniteRunLB());
    ATH_CHECK(outputHandle.record(std::move(data)));

    return StatusCode::SUCCESS;
  }
} // namespace LVL1
