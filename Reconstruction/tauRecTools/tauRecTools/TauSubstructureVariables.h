/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUSUBSTRUCTUREBUILDER_H
#define TAUREC_TAUSUBSTRUCTUREBUILDER_H

#include "tauRecTools/TauRecToolBase.h"

/**
 * @brief Calculate variables from the tau substructure.
 * 
 * @author M. Trottier-McDonald
 * @author Felix Friedrich
 * 
 */

class TauSubstructureVariables : public TauRecToolBase
{
    public: 
        
        static const double DEFAULT;

        TauSubstructureVariables(const std::string& name="TauSubstructureVariables");
	ASG_TOOL_CLASS2(TauSubstructureVariables, TauRecToolBase, ITauToolBase)

        ~TauSubstructureVariables();

        virtual StatusCode execute(xAOD::TauJet& pTau);
	virtual StatusCode executeShotFinder(xAOD::TauJet&, xAOD::CaloClusterContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
	virtual StatusCode executePi0CreateROI(xAOD::TauJet&, CaloCellContainer&) { return StatusCode::SUCCESS; }
	virtual StatusCode executePi0ClusterCreator(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&, xAOD::CaloClusterContainer&) { return StatusCode::SUCCESS; }
	virtual StatusCode executeVertexVariables(xAOD::TauJet&, xAOD::VertexContainer&) { return StatusCode::SUCCESS; }
	virtual StatusCode executePi0ClusterScaler(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
	virtual StatusCode executePi0nPFO(xAOD::TauJet&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
	virtual StatusCode executePanTau(xAOD::TauJet&, xAOD::ParticleContainer&) { return StatusCode::SUCCESS; }

        virtual StatusCode initialize();
        virtual StatusCode finalize();
        virtual StatusCode eventInitialize();

	virtual StatusCode eventFinalize() { return StatusCode::SUCCESS; }

	virtual void print() const { }

    private:
        /** Maximal pile up correction in GeV for a tau candidate.
         *  Used for the caloIso corrected variable.
         */
	std::string m_configPath;
        double m_maxPileUpCorrection; 
        double m_pileUpAlpha;         //!< slope of the pileup correction
        
        /** 
         * enable cell origin correction 
         * eta and phi of the cells are corrected wrt to the origin of the tau vertex
         */
        bool m_doVertexCorrection;
        bool m_inAODmode; //!< don't update everything if running on AODs
};

#endif
