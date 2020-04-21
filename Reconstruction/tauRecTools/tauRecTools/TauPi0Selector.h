/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUPI0SELECTOR_H
#define	TAUREC_TAUPI0SELECTOR_H

#include <string>
#include "tauRecTools/TauRecToolBase.h"

/**
 * @brief Selects pi0s
 * 
 * @author Will Davey <will.davey@cern.ch> 
 * @author Benedict Winter <benedict.tobias.winter@cern.ch> 
 * @author Stephanie Yuen <stephanie.yuen@cern.ch>
 */

class TauPi0Selector : public TauRecToolBase {
public:
    TauPi0Selector(const std::string& name);
    ASG_TOOL_CLASS2(TauPi0Selector, TauRecToolBase, ITauToolBase)
    virtual ~TauPi0Selector();
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    virtual StatusCode executePi0nPFO(xAOD::TauJet& pTau, xAOD::PFOContainer& pNeutralPFOContainer) override;

private:

    Gaudi::Property<std::vector<float>> m_clusterEtCut {this, "ClusterEtCut"};
    Gaudi::Property<std::vector<float>> m_clusterBDTCut_1prong {this, "ClusterBDTCut_1prong"};
    Gaudi::Property<std::vector<float>> m_clusterBDTCut_mprong {this, "ClusterBDTCut_mprong"};
    /** @brief function used to get eta bin of Pi0Cluster */
    int getPi0Cluster_etaBin(double Pi0Cluster_eta);
    /** @brief function used to calculate the visible tau 4 momentum */
    TLorentzVector getP4(const xAOD::TauJet& tauJet);
};

#endif	/* TAUPI0SELECTOR_H */
