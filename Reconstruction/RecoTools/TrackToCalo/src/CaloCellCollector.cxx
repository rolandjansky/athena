/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackToCalo/CaloCellCollector.h"

#include <tuple>

#include "CaloConditions/CaloNoise.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloUtils/CaloCellList.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "TrkCaloExtension/CaloExtensionHelpers.h"
using CaloSample = CaloCell_ID::CaloSample;

Rec::CaloCellCollector::CaloCellCollector()
{
  // For EM, set explicit window per layer, for rest use input dR
  m_dEtadPhi.resize(CaloSampling::Unknown);
  m_dEtadPhi[CaloSampling::PreSamplerB] =
    std::pair<float, float>(0.0375, 0.15); // Ncells eta/phi: 3/3
  m_dEtadPhi[CaloSampling::PreSamplerE] =
    std::pair<float, float>(0.0375, 0.15); // Ncells eta/phi: 3/3
  m_dEtadPhi[CaloSampling::EMB1] =
    std::pair<float, float>(0.011, 0.15); // Ncells eta/phi: 7/3
  m_dEtadPhi[CaloSampling::EME1] =
    std::pair<float, float>(0.011, 0.15); // Ncells eta/phi: 7/3
  m_dEtadPhi[CaloSampling::EMB2] =
    std::pair<float, float>(0.0625, 0.0625); // Ncells eta/phi: 5/5
  m_dEtadPhi[CaloSampling::EME2] =
    std::pair<float, float>(0.0625, 0.0625); // Ncells eta/phi: 5/5
  m_dEtadPhi[CaloSampling::EMB3] =
    std::pair<float, float>(0.075, 0.0375); // Ncells eta/phi: 3/3
  m_dEtadPhi[CaloSampling::EME3] =
    std::pair<float, float>(0.075, 0.0375); // Ncells eta/phi: 3/3
  m_dEtadPhi[CaloSampling::TileBar0] =
    std::pair<float, float>(0.154, 0.151); // Ncells eta/phi: 3/3
  m_dEtadPhi[CaloSampling::TileBar1] =
    std::pair<float, float>(0.154, 0.151); // Ncells eta/phi: 3/3
  m_dEtadPhi[CaloSampling::TileBar2] =
    std::pair<float, float>(0.308, 0.151); // Ncells eta/phi: 3/3
  m_dEtadPhi[CaloSampling::TileExt0] =
    std::pair<float, float>(0.154, 0.151); // Ncells eta/phi: 3/3
  m_dEtadPhi[CaloSampling::TileExt1] =
    std::pair<float, float>(0.154, 0.151); // Ncells eta/phi: 3/3
  m_dEtadPhi[CaloSampling::TileExt2] =
    std::pair<float, float>(0.308, 0.151); // Ncells eta/phi: 3/3
  m_dEtadPhi[CaloSampling::TileGap1] =
    std::pair<float, float>(0.154, 0.151); // Ncells eta/phi: 3/3
  m_dEtadPhi[CaloSampling::TileGap2] =
    std::pair<float, float>(0.308, 0.151); // Ncells eta/phi: 3/3
  m_dEtadPhi[CaloSampling::TileGap3] =
    std::pair<float, float>(0.308, 0.151); // Ncells eta/phi: 3/3

  // EM explicit window samples
  m_samples.reserve(8);
  m_samples.push_back(CaloSampling::PreSamplerB);
  m_samples.push_back(CaloSampling::PreSamplerE);
  m_samples.push_back(CaloSampling::EMB1);
  m_samples.push_back(CaloSampling::EME1);
  m_samples.push_back(CaloSampling::EMB2);
  m_samples.push_back(CaloSampling::EME2);
  m_samples.push_back(CaloSampling::EMB3);
  m_samples.push_back(CaloSampling::EME3);
  m_samples.push_back(CaloSampling::TileBar0);
  m_samples.push_back(CaloSampling::TileBar1);
  m_samples.push_back(CaloSampling::TileBar2);
  m_samples.push_back(CaloSampling::TileExt0);
  m_samples.push_back(CaloSampling::TileExt1);
  m_samples.push_back(CaloSampling::TileExt2);
  m_samples.push_back(CaloSampling::TileGap1);
  m_samples.push_back(CaloSampling::TileGap2);
  m_samples.push_back(CaloSampling::TileGap3);
  // samples with dR
  m_samplesForDR.reserve(4);
  m_samplesForDR.push_back(CaloSampling::HEC0);
  m_samplesForDR.push_back(CaloSampling::HEC1);
  m_samplesForDR.push_back(CaloSampling::HEC2);
  m_samplesForDR.push_back(CaloSampling::HEC3);

  // From Bruno:
  // PS: 0.025x0.06 (rectangle)
  // EM1: 0.03x0.07 (rectangle)
  // EM2: 0.05 (cone)
  // EM3: 0.03 (cone)
  // Tile1: 0.08x0.06 (rectangle)
  // Tile2: 2x1 (number of cells in eta X phi)
  // Tile3: 0.15x0.06 (rectangle)
  // HEC0-3: 0.07 (cone)

  // set parameters for EtCore
  m_dEtadPhiCore.resize(CaloSampling::Unknown, std::pair<float, float>(0, 0));
  m_dEtadPhiCore[CaloSampling::PreSamplerB] =
    std::pair<float, float>(0.025, 0.06); // PS: 0.025x0.06
  m_dEtadPhiCore[CaloSampling::PreSamplerE] =
    std::pair<float, float>(0.025, 0.06); // PS: 0.025x0.06
  m_dEtadPhiCore[CaloSampling::EMB1] =
    std::pair<float, float>(0.03, 0.07); // EM1: 0.03x0.07
  m_dEtadPhiCore[CaloSampling::EME1] =
    std::pair<float, float>(0.03, 0.07); // EM1: 0.03x0.07
  m_dEtadPhiCore[CaloSampling::TileBar0] =
    std::pair<float, float>(0.08, 0.06); // Tile1: 0.08x0.06
  m_dEtadPhiCore[CaloSampling::TileBar1] =
    std::pair<float, float>(0.102, 0.101); // Tile2: 2x1
  m_dEtadPhiCore[CaloSampling::TileBar2] =
    std::pair<float, float>(0.15, 0.06); // Tile3: 0.15x0.06
  m_dEtadPhiCore[CaloSampling::TileExt0] =
    std::pair<float, float>(0.08, 0.06); // Tile1: 0.08x0.06
  m_dEtadPhiCore[CaloSampling::TileExt1] =
    std::pair<float, float>(0.102, 0.101); // Tile2: 2x1
  m_dEtadPhiCore[CaloSampling::TileExt2] =
    std::pair<float, float>(0.15, 0.06); // Tile3: 0.15x0.06
  m_dEtadPhiDRCore.resize(CaloSampling::Unknown, 0);
  m_dEtadPhiDRCore[CaloSampling::EMB2] = 0.05 * 0.05;
  m_dEtadPhiDRCore[CaloSampling::EME2] = 0.05 * 0.05;
  m_dEtadPhiDRCore[CaloSampling::EMB3] = 0.03 * 0.03;
  m_dEtadPhiDRCore[CaloSampling::EME3] = 0.03 * 0.03;
  m_dEtadPhiDRCore[CaloSampling::HEC0] = 0.07 * 0.07;
  m_dEtadPhiDRCore[CaloSampling::HEC1] = 0.07 * 0.07;
  m_dEtadPhiDRCore[CaloSampling::HEC2] = 0.07 * 0.07;
  m_dEtadPhiDRCore[CaloSampling::HEC3] = 0.07 * 0.07;
  m_selectEtCoreByEtadPhi.resize(CaloSampling::Unknown, false);
  m_selectEtCoreByEtadPhi[CaloSampling::PreSamplerB] = true;
  m_selectEtCoreByEtadPhi[CaloSampling::PreSamplerE] = true;
  m_selectEtCoreByEtadPhi[CaloSampling::EMB1] = true;
  m_selectEtCoreByEtadPhi[CaloSampling::EME1] = true;
  m_selectEtCoreByEtadPhi[CaloSampling::TileBar0] = true;
  m_selectEtCoreByEtadPhi[CaloSampling::TileBar1] = true;
  m_selectEtCoreByEtadPhi[CaloSampling::TileBar2] = true;
  m_selectEtCoreByEtadPhi[CaloSampling::TileExt0] = true;
  m_selectEtCoreByEtadPhi[CaloSampling::TileExt1] = true;
  m_selectEtCoreByEtadPhi[CaloSampling::TileExt2] = true;
}

void
Rec::CaloCellCollector::resetCoreParameters(
  const std::vector<std::pair<float, float>>& dEtadPhiCore,
  const std::vector<float>& dEtadPhiDRCore,
  const std::vector<bool>& selectEtCoreByEtadPhi)
{
  m_dEtadPhiCore = dEtadPhiCore;
  m_dEtadPhiDRCore = dEtadPhiDRCore;
  m_selectEtCoreByEtadPhi = selectEtCoreByEtadPhi;
}

xAOD::CaloCluster*
Rec::CaloCellCollector::collectCells(
  const Trk::CaloExtension& extension,
  const CaloDetDescrManager* caloMgr,
  const CaloCellContainer& cellContainer,
  xAOD::CaloClusterContainer& clusterContainer) const
{
  // Collect cells in explicit window sizes

  std::vector<const CaloCell*> cells;

  CaloExtensionHelpers::EntryExitLayerMap entryExitLayerMap;
  CaloExtensionHelpers::entryExitLayerMap(extension, entryExitLayerMap);

  cells.reserve(100);
  CaloCellList myList(caloMgr, &cellContainer);
  // Save sampling midpoints in map
  using SampData = std::tuple<float, float, float>;
  using SampDataPair = std::pair<CaloSample, SampData>;
  std::map<CaloSample, SampData> sampleEtaPhiMap;
  float etot = 0;
  unsigned int samplingPattern = 0;
  Amg::Vector3D clusVec(0, 0, 0);

  // loop over samples using explicit sample window sizes

  for (auto samp : m_samples) {
    auto pos = entryExitLayerMap.find(samp);
    if (pos != entryExitLayerMap.end()) {
      samplingPattern |= CaloSampling::getSamplingPattern(
        samp); // add in sampling to pattern for cluster
      auto midPoint = 0.5 * (pos->second.first + pos->second.second);
      clusVec = clusVec + midPoint;
      myList.select(midPoint.eta(),
                    midPoint.phi(),
                    m_dEtadPhi[samp].first,
                    m_dEtadPhi[samp].second,
                    samp);
      cells.insert(cells.end(), myList.begin(), myList.end());
      float e = 0;
      for (const auto* cell : myList)
        e += cell->energy();
      etot += e;
      sampleEtaPhiMap.insert(
        SampDataPair(samp, std::make_tuple(e, midPoint.eta(), midPoint.phi())));
    }
  }

  // loop over samples using fabs(dR) for window

  for (auto samp : m_samplesForDR) {
    auto pos = entryExitLayerMap.find(samp);
    if (pos != entryExitLayerMap.end()) {
      samplingPattern |= CaloSampling::getSamplingPattern(
        samp); // add in sampling to pattern for cluster
      auto midPoint = 0.5 * (pos->second.first + pos->second.second);
      clusVec = clusVec + midPoint;
      myList.select(midPoint.eta(), midPoint.phi(), 0.1, samp);
      cells.insert(cells.end(), myList.begin(), myList.end());
      float e = 0;
      for (const auto* cell : myList)
        e += cell->energy();
      etot += e;
      sampleEtaPhiMap.insert(
        SampDataPair(samp, std::make_tuple(e, midPoint.eta(), midPoint.phi())));
    }
  }

  if (etot == 0) {
    return nullptr;
  }
  float clusVecEta, clusVecPhi;
  clusVecEta = clusVec.eta();
  clusVecPhi = clusVec.phi();

  // create cluster
  xAOD::CaloCluster* cluster =
    CaloClusterStoreHelper::makeCluster(&clusterContainer, &cellContainer);
  if (!cluster) {
    return nullptr;
  }

  // allocate the space for the sampling information
  cluster->setSamplingPattern(samplingPattern);

  // add  cells to cluster
  cluster->setE(etot);
  for (const auto* cell : cells) {
    if (!cell || !cell->caloDDE())
      continue;
    int index = cellContainer.findIndex(cell->caloDDE()->calo_hash());
    if (index == -1)
      continue;
    cluster->addCell(index, 1.);
  }
  // Add energy and midpoints as the cluster eta/phi sampling positions
  for (auto entry : sampleEtaPhiMap) {
    cluster->setEnergy(entry.first, std::get<0>(entry.second));
    cluster->setEta(entry.first, std::get<1>(entry.second));
    cluster->setPhi(entry.first, std::get<2>(entry.second));
  }

  cluster->setEta(clusVecEta);
  cluster->setPhi(clusVecPhi);

  return cluster;
}

void
Rec::CaloCellCollector::collectEtCore(const xAOD::CaloCluster& clus,
                                      std::vector<float>& etcore,
                                      const CaloNoise* caloNoise,
                                      float sigmaNoiseCut) const
{
  // Collect the cells in the core for a muon

  // Collect etCore for the different samples
  float etCore = 0;
  float etCoreEM = 0;
  float etCoreTile = 0;
  float etCoreHEC = 0;
  std::vector<float> sampEt(CaloSampling::Unknown, 0);

  for (const auto* cell : clus) {
    CaloSample samp = cell->caloDDE()->getSampling();
    float clusEta = clus.etaSample(samp);
    float clusPhi = clus.phiSample(samp);
    float deta = clusEta - cell->eta();
    float dphi = CaloPhiRange::diff(clusPhi, cell->phi());
    bool addCell = false;

    if (m_selectEtCoreByEtadPhi[samp]) {
      if (fabs(deta) < m_dEtadPhiCore[samp].first &&
          fabs(dphi) < m_dEtadPhiCore[samp].second) {
        addCell = true;
      }
    } else {
      if ((deta * deta + dphi * dphi) < m_dEtadPhiDRCore[samp]) {
        addCell = true;
      }
    }
    // Check if cell passes the noise threshold of 3.4sigma
    if (caloNoise && addCell &&
        cell->energy() <
          sigmaNoiseCut * caloNoise->getNoise(cell->ID(), cell->gain())) {
      addCell = false;
    }
    // sum of et, defined by cell E, and muon track eta
    if (addCell) {
      sampEt[samp] += cell->energy() / cosh(clusEta);
    }
  }
  // Set the core ET
  etCoreEM += sampEt[CaloSampling::PreSamplerB];
  etCoreEM += sampEt[CaloSampling::PreSamplerE];
  etCoreEM += sampEt[CaloSampling::EMB1];
  etCoreEM += sampEt[CaloSampling::EME1];
  etCoreEM += sampEt[CaloSampling::EMB2];
  etCoreEM += sampEt[CaloSampling::EME2];
  etCoreEM += sampEt[CaloSampling::EMB3];
  etCoreEM += sampEt[CaloSampling::EME3];
  etCoreTile += sampEt[CaloSampling::TileBar0];
  etCoreTile += sampEt[CaloSampling::TileBar1];
  etCoreTile += sampEt[CaloSampling::TileBar2];
  etCoreTile += sampEt[CaloSampling::TileExt0];
  etCoreTile += sampEt[CaloSampling::TileExt1];
  etCoreTile += sampEt[CaloSampling::TileExt2];
  etCoreHEC += sampEt[CaloSampling::HEC0];
  etCoreHEC += sampEt[CaloSampling::HEC1];
  etCoreHEC += sampEt[CaloSampling::HEC2];
  etCoreHEC += sampEt[CaloSampling::HEC3];
  etCore = etCoreEM + etCoreTile + etCoreHEC;

  etcore[Rec::CaloCellCollector::ET_Core] = etCore;
  etcore[Rec::CaloCellCollector::ET_EMCore] = etCoreEM;
  etcore[Rec::CaloCellCollector::ET_TileCore] = etCoreTile;
  etcore[Rec::CaloCellCollector::ET_HECCore] = etCoreHEC;
}
