/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUPI0SELECTOR_H
#define	TAUREC_TAUPI0SELECTOR_H

#include <string>
#include "tauRec/TauToolBase.h"

/**
 * @brief Selects pi0s
 * 
 * @author Will Davey <will.davey@cern.ch> 
 * @author Benedict Winter <benedict.tobias.winter@cern.ch> 
 * @author Stephanie Yuen <stephanie.yuen@cern.ch>
 */

class TauPi0Selector : public TauToolBase {
public:
    TauPi0Selector(const std::string& type,
            const std::string& name,
            const IInterface *parent);
    virtual ~TauPi0Selector();
    virtual StatusCode initialize();
    virtual StatusCode execute(TauCandidateData *data);

private:

    std::vector<float> m_clusterEtCut;
    std::vector<float> m_clusterBDTCut_1prong;
    std::vector<float> m_clusterBDTCut_mprong;
    /** @brief function used to get eta bin of Pi0Cluster */
    int getPi0Cluster_etaBin(double Pi0Cluster_eta);
    /** @brief function used to calculate the visible tau 4 momentum */
    TLorentzVector getP4(xAOD::TauJet* tauJet);
};

#endif	/* TAUPI0SELECTOR_H */
