/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CellsInConeThinning.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_CALO_CELLSINTHINNING_H
#define DERIVATIONFRAMEWORK_CALO_CELLSINTHINNING_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODEgamma/EgammaFwd.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODEgamma/EgammaContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "xAODCaloEvent/CaloCluster.h"

#include "ExpressionEvaluation/ExpressionParserUser.h"

namespace DerivationFramework {

  class CellsInConeThinning:public ExpressionParserUser<AthAlgTool>, public IAugmentationTool{
    public:
    CellsInConeThinning(const std::string& type, 
			const std::string& name, 
			const IInterface* parent);

    ~CellsInConeThinning(){}
    StatusCode initialize();
    StatusCode finalize();
    StatusCode addBranches() const;

    private:
    SG::ReadHandleKey<xAOD::EgammaContainer> m_SGKey{this, "InputSGKey", "Electrons", "SG key for input container"};
    SG::ReadHandleKey<CaloCellContainer>  m_InputCellsSGKey{this, "InputCellsSGKey", "AllCalo", "SG key for input cells container"};
    SG::WriteHandleKey<xAOD::CaloClusterContainer> m_OutputClusterSGKey{this, "OutputClusterSGKey", "EgammaDummyClusters", "SG key for output"};
    SG::WriteHandleKey<CaloClusterCellLinkContainer> m_OutputCellLinkSGKey{this, "OutputCellLinksSGKey", "EgammaDummyCellLink", "SG key for output cell links"};

    std::string m_selectionString;
    double m_dr;

  };  
}

#endif // DERIVATIONFRAMEWORK_CALO_CELLSINCONESELECTOR_H
