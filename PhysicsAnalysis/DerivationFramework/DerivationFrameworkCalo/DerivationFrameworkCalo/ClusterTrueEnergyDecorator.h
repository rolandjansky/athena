/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ClusterTrueEnergyDecorator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_ClusterTrueEnergyDecorator_H
#define DERIVATIONFRAMEWORK_ClusterTrueEnergyDecorator_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

#include "xAODCaloEvent/CaloClusterFwd.h"

#include "GaudiKernel/ToolHandle.h"

#include <string>

class CalibHitToCaloCellTool;
class CaloCellContainer;

namespace DerivationFramework {
  
  /** Decorate egamma clustrers with the true energy contained within the cluster.
      Obtained from true EM,visible(EM+hadronic),total(visible+invisible) energy
      deposits using calibration hits.
    **/
  class ClusterTrueEnergyDecorator : public AthAlgTool, public IAugmentationTool {
    public:
      ClusterTrueEnergyDecorator(const std::string& t, const std::string& n, const IInterface* p);
      ~ClusterTrueEnergyDecorator();
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

  private:
      std::string m_egammaClustersContName;
      ToolHandle<CalibHitToCaloCellTool> m_calibhitToCaloCellTool; 
      float getTrueEnergyInCluster(const xAOD::CaloCluster* cluster,const CaloCellContainer* truthCells) const;
  };
}

#endif // DERIVATIONFRAMEWORK_ClusterTrueEnergyDecorator_H
