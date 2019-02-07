/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_CALOCELLCONTAINERFROMCLUSTERTOOL_H
#define CALOREC_CALOCELLCONTAINERFROMCLUSTERTOOL_H

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "Identifier/IdentifierHash.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

// Calo includes
#include "CaloInterface/ICaloConstCellMakerTool.h"

class CaloCellContainerFromClusterTool
  : public extends<AthAlgTool, ICaloConstCellMakerTool>
{

  public:

    CaloCellContainerFromClusterTool(const std::string& type, const std::string& name,
        const IInterface* parent);

    virtual StatusCode initialize() override;

    // update theCellContainer
    virtual StatusCode process (CaloConstCellContainer* theCellContainer,
                                const EventContext& ctx) const override;

  private:

    std::string m_caloClusterName; // name of input CaloClusterContainer (for backward compatibility)
    SG::ReadHandleKey<CaloCellContainer> m_cellName; // name of input CaloCellContainer

    bool m_addSamplingCells;
    std::vector<std::string> m_samplingNames;
    IdentifierHash m_hashMax;

    std::set<int> m_validSamplings;
    SG::ReadHandleKeyArray<xAOD::CaloClusterContainer> m_clusterKeys;
};

#endif

