/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaxCellDecorator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_MAXCELLDECORATOR_H
#define DERIVATIONFRAMEWORK_MAXCELLDECORATOR_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
//
#include "GaudiKernel/EventContext.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteDecorHandleKeyArray.h"
#include "xAODEgamma/EgammaContainer.h"

namespace DerivationFramework {

class MaxCellDecorator
  : public AthAlgTool
  , public IAugmentationTool
{
public:
  MaxCellDecorator(const std::string& t,
                   const std::string& n,
                   const IInterface* p);
  ~MaxCellDecorator();
  StatusCode initialize();
  StatusCode finalize();
  virtual StatusCode addBranches() const;

  struct calculation
  {
    float maxEcell_time = -9999.9;
    float maxEcell_energy = -9999.9;
    int maxEcell_gain = -1;
    uint64_t maxEcell_onlId = 0;
    float maxEcell_x = -9999.9;
    float maxEcell_y = -9999.9;
    float maxEcell_z = -9999.9;
  };

private:
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{
    this,
    "CablingKey",
    "LArOnOffIdMap",
    "SG Key of LArOnOffIdMapping object"
  };

  SG::ReadHandleKey<xAOD::EgammaContainer>
    m_SGKey_photons{ this, "SGKey_photons", "", "SG key of photon container" };

  SG::ReadHandleKey<xAOD::EgammaContainer> m_SGKey_electrons{
    this,
    "SGKey_electrons",
    "",
    "SG key of electron container"
  };

  SG::WriteDecorHandleKeyArray<xAOD::EgammaContainer>
    m_SGKey_photons_decorations{
      this,
      "SGKey_photons_decorations_noConf",
      {},
      "SG keys for photon decorations not really configurable"
    };

  SG::WriteDecorHandleKeyArray<xAOD::EgammaContainer>
    m_SGKey_electrons_decorations{
      this,
      "SGKey_electrons_decorations_noConf",
      {},
      "SG keys for electrons decorations not really configurable"
    };

  calculation decorateObject(const xAOD::Egamma* egamma,
                             const EventContext& ctx) const;
};
}

#endif // DERIVATIONFRAMEWORK_MAXCELLDECORATOR_H
