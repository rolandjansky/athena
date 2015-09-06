/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_CALOCELLCONTAINERFROMCLUSTERTOOL_H
#define CALOREC_CALOCELLCONTAINERFROMCLUSTERTOOL_H

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "Identifier/IdentifierHash.h"

// Calo includes
#include "CaloInterface/ICaloConstCellMakerTool.h"

class CaloCellContainerFromClusterTool: public AthAlgTool
                                      , virtual public ICaloConstCellMakerTool

{

  public:

    CaloCellContainerFromClusterTool(const std::string& type, const std::string& name,
        const IInterface* parent);

    virtual StatusCode initialize();

    // update theCellContainer
    virtual StatusCode process(CaloConstCellContainer* theCellContainer);

  private:

    std::string m_caloClusterName; // name of input CaloClusterContainer (for backward compatibility)
    std::string m_cellName;  // name of input CaloCellContainer

    std::vector<std::string> m_caloClusterNames;
    bool m_addSamplingCells;
    std::vector<std::string> m_samplingNames;
    IdentifierHash m_hashMax;

    std::set<int> m_validSamplings;
    std::set<std::string> m_caloClusterNameSet;
    bool m_firstEvent;
};

#endif

