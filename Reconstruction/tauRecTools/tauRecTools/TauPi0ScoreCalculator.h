/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUPI0SCORECALCULATOR_H
#define	TAUREC_TAUPI0SCORECALCULATOR_H

#include <string>
#include <map>
#include "tauRecTools/TauRecToolBase.h"
#include "xAODPFlow/PFO.h"
#include "tauRecTools/BDTHelper.h"

/**
 * @brief Selectes pi0Candidates (Pi0 Finder).
 * 
 * @author Veit Scharf
 * @author Will Davey <will.davey@cern.ch> 
 * @author Benedict Winter <benedict.tobias.winter@cern.ch> 
 * @author Stephanie Yuen <stephanie.yuen@cern.ch>
 */

class TauPi0ScoreCalculator : public TauRecToolBase {
public:
    TauPi0ScoreCalculator(const std::string& name);
    ASG_TOOL_CLASS2(TauPi0ScoreCalculator, TauRecToolBase, ITauToolBase)
    virtual ~TauPi0ScoreCalculator();

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    virtual StatusCode executePi0nPFO(xAOD::TauJet& pTau, xAOD::PFOContainer& pNeutralPFOContainer) const override;

private:
    /** @brief function used to calculate BDT score */
    float calculateScore(const xAOD::PFO* neutralPFO) const;

    std::string m_weightfile;    
    std::unique_ptr<tauRecTools::BDTHelper> m_mvaBDT;
};

#endif	/* TAUPI0SCORECALCULATOR_H */

