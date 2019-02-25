/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUMUONVETO_H
#define TAUMUONVETO_H

#include "tauRecTools/TauRecToolBase.h"
#include <string>

class TauMuonVeto: virtual public TauRecToolBase
{
  ASG_TOOL_CLASS2(TauMuonVeto, TauRecToolBase, ITauToolBase)
    public:

        TauMuonVeto(const std::string& name = "TauMuonVeto"):
            TauRecToolBase(name)
        {
	}

        virtual ~TauMuonVeto() {}

	virtual StatusCode initialize(){ return StatusCode::SUCCESS; }
	
        virtual StatusCode execute(xAOD::TauJet&);

	virtual StatusCode finalize(){ return StatusCode::SUCCESS; }


	virtual StatusCode eventInitialize(){ return StatusCode::SUCCESS; }
	virtual StatusCode executeShotFinder(xAOD::TauJet&, xAOD::CaloClusterContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
	virtual StatusCode executePi0CreateROI(xAOD::TauJet&, CaloCellContainer&) { return StatusCode::SUCCESS; }
	virtual StatusCode executePi0ClusterCreator(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&, xAOD::CaloClusterContainer&) { return StatusCode::SUCCESS; }
	virtual StatusCode executeVertexVariables(xAOD::TauJet&, xAOD::VertexContainer&) { return StatusCode::SUCCESS; }
	virtual StatusCode executePi0ClusterScaler(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
	virtual StatusCode executePi0nPFO(xAOD::TauJet&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
	virtual StatusCode executePanTau(xAOD::TauJet&, xAOD::ParticleContainer&) { return StatusCode::SUCCESS; }
	virtual StatusCode eventFinalize(){ return StatusCode::SUCCESS; }

        
};

#endif // TAUMUONVETO_H
