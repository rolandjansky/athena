/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUPI0SCORECALCULATOR_H
#define	TAUREC_TAUPI0SCORECALCULATOR_H

#include <string>
#include <map>
#include "tauRecTools/TauRecToolBase.h"
#include "xAODPFlow/PFO.h"

#include "MVAUtils/BDT.h"

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

    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode execute(xAOD::TauJet&){ return StatusCode::SUCCESS; }
    virtual StatusCode executeShotFinder(xAOD::TauJet&, xAOD::CaloClusterContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0CreateROI(xAOD::TauJet&, CaloCellContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0ClusterCreator(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&, xAOD::CaloClusterContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executeVertexVariables(xAOD::TauJet&, xAOD::VertexContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0ClusterScaler(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0nPFO(xAOD::TauJet& pTau, xAOD::PFOContainer& pNeutralPFOContainer);
    virtual StatusCode executePanTau(xAOD::TauJet&, xAOD::ParticleContainer&) { return StatusCode::SUCCESS; }

    virtual StatusCode eventInitialize() { return StatusCode::SUCCESS; }
    virtual StatusCode eventFinalize() { return StatusCode::SUCCESS; }

    virtual void print() const { }

private:

    std::string m_readerOption;
    MVAUtils::BDT* m_mvaBDT;

    std::string m_weightfile;

    float m_Abs_FIRST_ETA;
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
    
    std::map<TString, float*> m_availableVars;//!< keeps track of available of availble floats

    /** @brief function used to calculate BDT score */
    float calculateScore(const xAOD::PFO* neutralPFO);

    /* /\** @brief Book TMVA methods. *\/ */
    /* StatusCode bookMethod(TMVA::Reader *reader, const std::string &methodName) const; */

};

#endif	/* TAUPI0SCORECALCULATOR_H */

