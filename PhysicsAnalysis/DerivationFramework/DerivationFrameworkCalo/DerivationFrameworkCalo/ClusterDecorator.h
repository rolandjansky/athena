/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ClusterDecorator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_ClusterDecorator_H
#define DERIVATIONFRAMEWORK_ClusterDecorator_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "xAODEgamma/EgammaFwd.h"

#include "GaudiKernel/ToolHandle.h"

class IegammaSwTool;
class CaloClusterCollectionProcessor;
class CaloCellDetPos;

namespace DerivationFramework {
  
  /** Decorate egamma objects with the Sliding-Windows cluster
    **/
  class ClusterDecorator : public AthAlgTool, public IAugmentationTool {
    public:
      ClusterDecorator(const std::string& t, const std::string& n, const IInterface* p);
      ~ClusterDecorator();
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;
      void fillPositionsInCalo(xAOD::CaloCluster* cluster) const ;

    private:
      std::string m_SGKey_photons;
      std::string m_SGKey_electrons;
      std::string m_OutputClusterSGKey;
      std::vector<unsigned int> m_layers;
      
      /** @brief Tool to handle cluster corrections */
      ToolHandle<IegammaSwTool>   m_clusterCorrectionTool;
      /** @brief Name of tool for cluster corrections */
      std::string            m_ClusterCorrectionToolName;
      /** @brief Name of cell collection  */
      std::string m_CellCollectionName;
      /** @brief Position in Calo frame**/  
      std::unique_ptr<CaloCellDetPos> m_caloCellDetPos;      
      StatusCode decorateObject(const xAOD::Egamma* egamma, xAOD::CaloClusterContainer *outputClusterContainer) const;
  };
}

#endif // DERIVATIONFRAMEWORK_ClusterDecorator_H
