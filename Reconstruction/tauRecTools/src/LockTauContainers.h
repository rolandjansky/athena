/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_LOCKTAUCONTAINERS_H
#define	TAUREC_LOCKTAUCONTAINERS_H

#include "tauRecTools/TauRecToolBase.h"

/**
 * @brief  Set tau containers to const to prevent downstream modification.
 * 
 * @author Felix Friedrich
 */

class LockTauContainers : public TauRecToolBase
{
    public: 
  LockTauContainers(const std::string& name);
  
  ASG_TOOL_CLASS2(LockTauContainers, TauRecToolBase, ITauToolBase);

        ~LockTauContainers() { }

        virtual StatusCode initialize();
        virtual StatusCode finalize();  
        virtual StatusCode execute(xAOD::TauJet&)      { return StatusCode::SUCCESS; }
	virtual StatusCode executeShotFinder(xAOD::TauJet&, xAOD::CaloClusterContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
	virtual StatusCode executePi0CreateROI(xAOD::TauJet&, CaloCellContainer&) { return StatusCode::SUCCESS; }
	virtual StatusCode executePi0ClusterCreator(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&, xAOD::CaloClusterContainer&) { return StatusCode::SUCCESS; }
	virtual StatusCode executeVertexVariables(xAOD::TauJet&, xAOD::VertexContainer&) { return StatusCode::SUCCESS; }
	virtual StatusCode executePi0ClusterScaler(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
	virtual StatusCode executePanTau(xAOD::TauJet&, xAOD::ParticleContainer&) { return StatusCode::SUCCESS; }
        virtual StatusCode eventFinalize();
	virtual StatusCode eventInitialize() { return StatusCode::SUCCESS; }

	virtual void print() const {}
};

#endif	/* TAUREC_LOCKTAUCONTAINERS_H */

