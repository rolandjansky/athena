/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUCLUSTERFINDER_H
#define TAUCLUSTERFINDER_H

#include "tauRecTools/TauRecToolBase.h"

#include "AsgTools/ToolHandle.h"

/**
 * @brief Associate the clusters used in the seed jet to the tau candidate. 
 * 
 *  The asscated clusters will always be the clusters in CaloCalTopoClusterContainer. 
 * 
 * @author Xiaozhong Huang <xiaozhong.huang@cern.ch>
 *                                                                              
 */

class TauClusterFinder : public TauRecToolBase {

  public:
    
    ASG_TOOL_CLASS2(TauClusterFinder, TauRecToolBase, ITauToolBase);
    
    /** @brief Constructor */ 
    TauClusterFinder(const std::string& name);

    /** @brief Destructor */
    ~TauClusterFinder() = default;

    /** @brief Execution of this tool */ 
    virtual StatusCode execute(xAOD::TauJet& tau) const override;

  private:
    
    std::vector<const xAOD::CaloCluster*> getClusterList(const xAOD::Jet& jet) const;
    
    Gaudi::Property<bool> m_skipNegativeEnergy {this, "SkipNegativeEnergy", true, "whether to skip negative energy clusters"};

    Gaudi::Property<bool> m_doJetVertexCorrection {this, "JetVertexCorrection", true, "switch of jet vertex correction"};
};

#endif
