/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUPI0CREATERCHOOSER_H
#define	TAUREC_TAUPI0CREATERCHOOSER_H

#include "GaudiKernel/ToolHandle.h"
#include "tauRec/TauToolBase.h"
#include <vector>
#include <string>

namespace Analysis {
    class TauCommonDetails;
}

/**
 * @brief Tool to select the correct Pi0 Finder.
 * 
 *  Part of the "Bonn" Pi0 Finder algorithm.
 *
 * @author  Veit Scharf
 */

class TauPi0CreatorChooser : public TauToolBase {
public:
    TauPi0CreatorChooser(const std::string& type,
            const std::string& name,
            const IInterface *parent);
    virtual ~TauPi0CreatorChooser();

    virtual StatusCode initialize();
    virtual StatusCode eventInitialize(TauCandidateData *data);
    virtual StatusCode execute(TauCandidateData *data);
    virtual StatusCode eventFinalize(TauCandidateData *data);
    virtual void cleanup(TauCandidateData *data);

private:

    ToolHandle<TauToolBase> m_tau1p3pCreatePi0ClusTool;
    ToolHandle<TauToolBase> m_tauCommonCreatePi0ClusTool;

    std::vector<Analysis::TauCommonDetails*> m_tauDetails;
    std::string m_pi0ClusterContainerName;
};


#endif	/* TAUREC_TAUPI0CREATERCHOOSER_H */

