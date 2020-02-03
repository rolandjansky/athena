/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUVERTEXVARIABLES_H
#define	TAUREC_TAUVERTEXVARIABLES_H

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "tauRecTools/TauRecToolBase.h"
#include "GaudiKernel/ToolHandle.h"

// forwards
class TauEventData;
namespace Trk {
	class ITrackToVertexIPEstimator;
    class IVertexFitter;
    class IVertexSeedFinder;
}

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
    virtual StatusCode executeVertexVariables(xAOD::TauJet& pTau, xAOD::VertexContainer& pVertexContainer) override; 
    virtual StatusCode finalize() override;
    //-------------------------------------------------------------
    //! determines the transverse flight path significance from
    //! the primary vertex and the secondary vertex of tau candidate
    //-------------------------------------------------------------
    double trFlightPathSig(const xAOD::TauJet& pTau, const xAOD::Vertex& secVertex);

private:
    ToolHandle< Trk::ITrackToVertexIPEstimator > m_trackToVertexIPEstimator;
    ToolHandle< Trk::IVertexFitter >     m_fitTool; //!< Pointer to the base class of the fit algtools
    ToolHandle< Trk::IVertexSeedFinder > m_SeedFinder;
    
    SG::ReadHandleKey<xAOD::VertexContainer> m_vertexInputContainer{this,"Key_vertexInputContainer", "PrimaryVertices", "input vertex container key"};
    SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackPartInputContainer{this,"Key_trackPartInputContainer", "InDetTrackParticles", "input track particle container key"};

};

#endif	/* TAUREC_TAUVERTEXVARIABLES_H */

