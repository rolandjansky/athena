/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUVERTEXVARIABLES_H
#define	TAUREC_TAUVERTEXVARIABLES_H

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "tauRecTools/TauRecToolBase.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkVertexFitterInterfaces/IVertexSeedFinder.h"

/**
 *  
 * @brief Class for calculating vertex variables.
 *
 * @authors Stan Lai, Felix Friedrich
 */

class TauVertexVariables : public TauRecToolBase {
public:
    //-----------------------------------------------------------------
    // Constructor and destructor
    //-----------------------------------------------------------------
    TauVertexVariables(const std::string& name);
    ASG_TOOL_CLASS2(TauVertexVariables, TauRecToolBase, ITauToolBase);
    ~TauVertexVariables();
    
    virtual StatusCode initialize() override;
    virtual StatusCode executeVertexVariables(xAOD::TauJet& pTau, xAOD::VertexContainer& pVertexContainer) const override; 

    //-------------------------------------------------------------
    //! determines the transverse flight path significance from
    //! the primary vertex and the secondary vertex of tau candidate
    //-------------------------------------------------------------
    double trFlightPathSig(const xAOD::TauJet& pTau, const xAOD::Vertex& secVertex) const;

private:
    ToolHandle<Trk::IVertexFitter> m_fitTool {this, "VertexFitter", "Trk::AdaptiveVertexFitter"};
    ToolHandle<Trk::IVertexSeedFinder> m_SeedFinder {this, "SeedFinder", "Trk::CrossDistancesSeedFinder"};
};

#endif	/* TAUREC_TAUVERTEXVARIABLES_H */
