/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CellDecorator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_CELLDECORATOR_H
#define DERIVATIONFRAMEWORK_CELLDECORATOR_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "xAODEgamma/EgammaContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "CaloIdentifier/CaloIdManager.h"

class CaloFillRectangularCluster;
class CaloClusterCollectionProcessor;
class CaloCell_ID;  // forward declaration

namespace DerivationFramework {

  class CellDecorator : public AthAlgTool, public IAugmentationTool {
    public:
      CellDecorator(const std::string& t, const std::string& n, const IInterface* p);
      ~CellDecorator();
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      std::string m_SGKey_photons;
      std::string m_SGKey_electrons;
      std::vector<int> m_eta_sizes;
      std::vector<int> m_phi_sizes;
      std::vector<std::string> m_name_sizes;

      ToolHandle<CaloClusterCollectionProcessor> m_THcaloFillRectangularTool55;
      ToolHandle<CaloClusterCollectionProcessor> m_THcaloFillRectangularTool35;
      ToolHandle<CaloClusterCollectionProcessor> m_THcaloFillRectangularTool37;
      ToolHandle<CaloClusterCollectionProcessor> m_THcaloFillRectangularTool711;
      
      std::vector< CaloFillRectangularCluster* > m_caloFillRectangularTool;

      std::string m_CellCollectionName;
      
      const DataHandle<CaloIdManager> m_caloIdMgr;
      const CaloCell_ID* m_calo_id;

      void decorateObject(const xAOD::Egamma*& egamma) const;
  };
}

#endif // DERIVATIONFRAMEWORK_CELLDECORATOR_H
