/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_TAUCLUSTERFINDER_H
#define TAURECTOOLS_TAUCLUSTERFINDER_H

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

    Gaudi::Property<bool> m_useOrigCluster {this, "UseOriginalCluster", true, 
	"Use original cluster object if jet constituent is a shallow copy"};
};

#endif // TAURECTOOLS_TAUCLUSTERFINDER_H
