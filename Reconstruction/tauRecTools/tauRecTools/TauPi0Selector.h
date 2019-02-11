/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode execute(xAOD::TauJet& pTau);
    virtual StatusCode executeShotFinder(xAOD::TauJet&, xAOD::CaloClusterContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0CreateROI(xAOD::TauJet&, CaloCellContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0ClusterCreator(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&, xAOD::CaloClusterContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executeVertexVariables(xAOD::TauJet&, xAOD::VertexContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0ClusterScaler(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePanTau(xAOD::TauJet&, xAOD::ParticleContainer&) { return StatusCode::SUCCESS; }

    virtual StatusCode eventInitialize() { return StatusCode::SUCCESS; }
    virtual StatusCode eventFinalize() { return StatusCode::SUCCESS; }

    virtual void print() const { }

private:

    std::vector<float> m_clusterEtCut;
    std::vector<float> m_clusterBDTCut_1prong;
    std::vector<float> m_clusterBDTCut_mprong;
    /** @brief function used to get eta bin of Pi0Cluster */
    int getPi0Cluster_etaBin(double Pi0Cluster_eta);
    /** @brief function used to calculate the visible tau 4 momentum */
    TLorentzVector getP4(const xAOD::TauJet& tauJet);
};

#endif	/* TAUPI0SELECTOR_H */
