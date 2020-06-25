/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ClusterEnergyPerLayerDecorator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_ClusterEnergyPerLayerDecorator_H
#define DERIVATIONFRAMEWORK_ClusterEnergyPerLayerDecorator_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "xAODEgamma/EgammaFwd.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ToolHandle.h"

class CaloFillRectangularCluster;
class CaloClusterCollectionProcessor;

namespace DerivationFramework {
  
  /** Decorate egamma objects with the energy per layer for a rectangular cluster
    * of size neta X nphi built on the fly
    **/
  class ClusterEnergyPerLayerDecorator : public AthAlgTool, public IAugmentationTool {
    public:
      ClusterEnergyPerLayerDecorator(const std::string& t, const std::string& n, const IInterface* p);
      ~ClusterEnergyPerLayerDecorator();
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      std::string m_SGKey_photons;
      std::string m_SGKey_electrons;
      int m_eta_size;
      int m_phi_size;
      std::vector<unsigned int> m_layers;

      ToolHandle<CaloClusterCollectionProcessor> m_caloFillRectangularTool;
      CaloFillRectangularCluster *m_tool;
      std::string m_CellCollectionName;

      void decorateObject(const EventContext& ctx,
                          const xAOD::Egamma* egamma) const;
  };
}

#endif // DERIVATIONFRAMEWORK_ClusterEnergyPerLayerDecorator_H
