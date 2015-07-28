/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

namespace ExpressionParsing {
    class ExpressionParser;
}
namespace DerivationFramework {

  class CellsInConeThinning:public AthAlgTool, public IAugmentationTool{
    public:
    CellsInConeThinning(const std::string& type, 
			const std::string& name, 
			const IInterface* parent);

    ~CellsInConeThinning(){}
    StatusCode initialize();
    StatusCode finalize();
    StatusCode addBranches() const;

    private:
    std::string m_SGKey;
    std::string m_InputCellsSGKey;
    std::string m_OutputClusterSGKey;
    std::string m_selectionString;
    double m_dr;
    ExpressionParsing::ExpressionParser *m_parser;

  };  
}

#endif // DERIVATIONFRAMEWORK_CALO_CELLSINCONESELECTOR_H
