/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUPI0BONNSCORECALCULATOR_H
#define	TAUREC_TAUPI0BONNSCORECALCULATOR_H

#include <string>
#include "tauRec/TauToolBase.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODPFlow/PFO.h"

namespace TMVA{
    class Reader;
}

/**
 * @brief Selectes pi0Candidates ("Bonn" Pi0 Finder).
 * 
 * @author Veit Scharf
 * @author Will Davey <will.davey@cern.ch> 
 * @author Benedict Winter <benedict.tobias.winter@cern.ch> 
 */

class TauPi0BonnScoreCalculator : public TauToolBase {
public:
    TauPi0BonnScoreCalculator(const std::string& type,
            const std::string& name,
            const IInterface *parent);
    virtual ~TauPi0BonnScoreCalculator();

    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode execute(TauCandidateData *data);

private:

    std::string m_readerOption;
    TMVA::Reader *m_tmvaReader;

    std::string m_weightfile;

    float m_FIRST_ETA;
    float m_SECOND_R;
    float m_SECOND_LAMBDA;
    float m_Abs_DELTA_PHI;
    float m_Abs_DELTA_THETA;
    float m_CENTER_LAMBDA_helped;
    float m_LATERAL;
    float m_LONGITUDINAL;
    float m_ENG_FRAC_EM;
    float m_ENG_FRAC_MAX;
    float m_ENG_FRAC_CORE;
    float m_log_SECOND_ENG_DENS;
    float m_EcoreOverEEM1;
    float m_AsymmetryWRTTrack;
    float m_NHitsInEM1;
    float m_NPosCells_PS;
    float m_NPosCells_EM1;
    float m_NPosCells_EM2;
    float m_firstEtaWRTCluster_EM1;
    float m_firstEtaWRTCluster_EM2;
    float m_secondEtaWRTCluster_EM1;
    float m_secondEtaWRTCluster_EM2;
    float m_energy_EM1;
    float m_energy_EM2;

    /** @brief function used to calculate BDT score */
    float calculateScore(const xAOD::PFO* neutralPFO);

    /** @brief Book TMVA methods. */
    StatusCode bookMethod(TMVA::Reader *reader, const std::string &methodName) const;

};

#endif	/* TAUPI0BONNSCORECALCULATOR_H */

