/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKTOCALO_CALOCELLCOLLECTOR_H
#define TRACKTOCALO_CALOCELLCOLLECTOR_H


#include "CaloGeoHelpers/CaloSampling.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
//
#include <vector>
// forward declarations
namespace Trk {
class CaloExtension;
}
class CaloCellContainer;
class CaloNoise;
class CaloDetDescrManager;

namespace Rec {

class CaloCellCollector
{
public:
  enum CoreEt
  {
    ET_Core,
    ET_EMCore,
    ET_TileCore,
    ET_HECCore,
    N_ET_Core
  };

  CaloCellCollector();

  xAOD::CaloCluster* collectCells(
    const Trk::CaloExtension& extension,
    const CaloDetDescrManager* caloMgr,
    const CaloCellContainer& cellContainer,
    xAOD::CaloClusterContainer& clusterContainer) const;

  void collectEtCore(const xAOD::CaloCluster& cluster,
                     std::vector<float>& et_core,
                     const CaloNoise* caloNoise,
                     float sigmaNoiseCut = 3.4) const;

  void resetCoreParameters(
    const std::vector<std::pair<float, float>>& dEtadPhiCore,
    const std::vector<float>& dEtadPhiDRCore,
    const std::vector<bool>& selectEtCoreByEtadPhi);

private:
  // parameters for overall cell collection
  std::vector<std::pair<float, float>> m_dEtadPhi;
  std::vector<CaloSampling::CaloSample> m_samples;
  std::vector<CaloSampling::CaloSample> m_samplesForDR;
  // parameters for overall EtCore energy collection
  std::vector<std::pair<float, float>> m_dEtadPhiCore;
  std::vector<float> m_dEtadPhiDRCore;
  std::vector<bool> m_selectEtCoreByEtadPhi;
};
} // namespace Rec

#endif
