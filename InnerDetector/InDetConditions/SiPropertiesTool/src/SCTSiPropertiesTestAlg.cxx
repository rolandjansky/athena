/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCTSiPropertiesTestAlg.h"

#include "InDetIdentifier/SCT_ID.h"

SCTSiPropertiesTestAlg::SCTSiPropertiesTestAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : ::AthReentrantAlgorithm(name, pSvcLocator)
  , m_id{nullptr}
{
}

StatusCode SCTSiPropertiesTestAlg::initialize() {
  // SCT silicon properties tool
  ATH_CHECK(m_tool.retrieve());

  // SCT ID helper
  ATH_CHECK(detStore()->retrieve(m_id, "SCT_ID"));

  return StatusCode::SUCCESS;
}

StatusCode SCTSiPropertiesTestAlg::execute(const EventContext& /*ctx*/) const {
  unsigned int maxHash{static_cast<unsigned int>(m_id->wafer_hash_max())};
  for (unsigned int hash{0}; hash<maxHash; hash++) {
    const IdentifierHash elementHash{hash};
    const InDet::SiliconProperties& siProperties{m_tool->getSiProperties(elementHash)};
    const double electronDriftMobility{siProperties.electronDriftMobility()};
    const double holeDriftMobility{siProperties.holeDriftMobility()};
    const double electronHallMobility{siProperties.electronHallMobility()};
    const double holeHallMobility{siProperties.holeHallMobility()};
    const double electronDiffusionConstant{siProperties.electronDiffusionConstant()};
    const double holeDiffusionConstant{siProperties.holeDiffusionConstant()};
    const double electronSaturationVelocity{siProperties.electronSaturationVelocity()};
    const double holeSaturationVelocity{siProperties.holeSaturationVelocity()};
    const double electronHolePairsPerEnergy{siProperties.electronHolePairsPerEnergy()};
    ATH_MSG_INFO("hash " << hash <<
                 " electronDriftMobility " << electronDriftMobility <<
                 " holeDriftMobility " << holeDriftMobility <<
                 " electronHallMobility " << electronHallMobility <<
                 " holeHallMobility " << holeHallMobility <<
                 " electronDiffusionConstant " << electronDiffusionConstant <<
                 " holeDiffusionConstant " << holeDiffusionConstant <<
                 " electronSaturationVelocity " << electronSaturationVelocity <<
                 " holeSaturationVelocity " << holeSaturationVelocity <<
                 " electronHolePairsPerEnergy " << electronHolePairsPerEnergy);
  }

  return StatusCode::SUCCESS;
}

StatusCode SCTSiPropertiesTestAlg::finalize()
{
  return StatusCode::SUCCESS;
}
