/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUSUBSTRUCTUREBUILDER_H
#define TAUREC_TAUSUBSTRUCTUREBUILDER_H

#include "tauRecTools/TauRecToolBase.h"
#include "tauRecTools/ITauVertexCorrection.h"

#include "AsgTools/ToolHandle.h"

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
	    ASG_TOOL_CLASS2(TauSubstructureVariables, TauRecToolBase, ITauToolBase)
        
        static const float DEFAULT;

        TauSubstructureVariables(const std::string& name="TauSubstructureVariables");

        ~TauSubstructureVariables();

        virtual StatusCode initialize() override;
        virtual StatusCode execute(xAOD::TauJet& pTau) const override;

    private:
	// use shower subtracted clusters with PFlow jet seeds
	bool m_useSubtractedCluster;
  
    ToolHandle<ITauVertexCorrection> m_tauVertexCorrection { this, 
      "TauVertexCorrection", "TauVertexCorrection", "Tool to perform the vertex correction"};
};

#endif
