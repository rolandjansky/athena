/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_VERSIONS_GBLOCKAUXCONTAINER_V1_H
#define XAODTRIGL1CALO_VERSIONS_GBLOCKAUXCONTAINER_V1_H

#include <vector>
#include "AthLinks/ElementLink.h"
#include "../JGTowerContainer.h"
#include "xAODCore/AuxContainerBase.h"

namespace xAOD
{
  class GBlockAuxContainer_v1 : public AuxContainerBase
  {
  public:
    GBlockAuxContainer_v1();

  private:
    std::vector<float> eta;
    std::vector<float> phi;
    std::vector<float> deta;
    std::vector<float> dphi;
    std::vector<float> et;
    std::vector<float> area;
    std::vector<ElementLink<JGTowerContainer>> seedJGTower;
    std::vector<std::vector<ElementLink<JGTowerContainer>>> JGTowers;
  }; //> end class GBlockAuxContainer_v1
} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE(xAOD::GBlockAuxContainer_v1, xAOD::AuxContainerBase);

#endif //> XAODTRIGL1CALO_VERSIONS_GBLOCKAUXCONTAINER_V1_H