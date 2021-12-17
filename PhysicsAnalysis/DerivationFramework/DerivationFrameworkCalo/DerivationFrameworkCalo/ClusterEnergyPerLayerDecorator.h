/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ClusterEnergyPerLayerDecorator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_ClusterEnergyPerLayerDecorator_H
#define DERIVATIONFRAMEWORK_ClusterEnergyPerLayerDecorator_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloRec/CaloClusterProcessor.h"
#include "CaloClusterCorrection/CaloFillRectangularCluster.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteDecorHandleKeyArray.h"
#include "xAODEgamma/EgammaContainer.h"

namespace DerivationFramework {

/** Decorate egamma objects with the energy per layer for a rectangular cluster
 * of size neta X nphi built on the fly
 **/
class ClusterEnergyPerLayerDecorator
  : public AthAlgTool
  , public IAugmentationTool
{
public:
  ClusterEnergyPerLayerDecorator(const std::string& t,
                                 const std::string& n,
                                 const IInterface* p);
  ~ClusterEnergyPerLayerDecorator();
  StatusCode initialize();
  StatusCode finalize();
  virtual StatusCode addBranches() const;

private:
  int m_eta_size = 0;
  int m_phi_size = 0;
  std::vector<unsigned int> m_layers;

  SG::ReadHandleKey<xAOD::EgammaContainer>
    m_SGKey_photons{ this, "SGKey_photons", "", "SG key of photon container" };

  SG::ReadHandleKey<xAOD::EgammaContainer> m_SGKey_electrons{
    this,
    "SGKey_electrons",
    "",
    "SG key of electron container"
  };

  SG::ReadHandleKey<CaloCellContainer> m_SGKey_caloCells{
    this,
    "SGKey_caloCells",
    "AllCalo",
    "SG key of the cell container"
  };

  ToolHandle<CaloClusterProcessor> m_caloFillRectangularTool{
    this,
    "CaloFillRectangularClusterTool",
    "",
    "Handle of the CaloFillRectangularClusterTool"
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

  const CaloFillRectangularCluster* m_tool = nullptr;

  std::vector<float> decorateObject(const EventContext& ctx,
                                    const xAOD::Egamma* egamma,
                                    const CaloCellContainer* cellCont) const;
};
}

#endif // DERIVATIONFRAMEWORK_ClusterEnergyPerLayerDecorator_H
