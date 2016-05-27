/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
    class IVxCandidateXAODVertex;
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
    
    virtual StatusCode initialize();
    virtual StatusCode execute(xAOD::TauJet&);
    virtual StatusCode eventInitialize();
    virtual StatusCode finalize();
    virtual StatusCode eventFinalize() { return StatusCode::SUCCESS; }

    virtual void print() const { }
    
    //-------------------------------------------------------------
    //! determines the transverse flight path significance from
    //! the primary vertex and the secondary vertex of tau candidate
    //-------------------------------------------------------------
    double trFlightPathSig(const xAOD::TauJet& pTau, const xAOD::Vertex& secVertex);

private:
    std::string m_primaryVertexKey;
    std::string m_inputTrackParticleContainerName;
    bool m_useOldSeedFinderAPI;
    bool m_AODmode;
    ToolHandle< Trk::ITrackToVertexIPEstimator > m_trackToVertexIPEstimator;
    ToolHandle< Trk::IVertexFitter >     m_fitTool; //!< Pointer to the base class of the fit algtools
    ToolHandle< Trk::IVertexSeedFinder > m_SeedFinder;
    ToolHandle< Trk::IVxCandidateXAODVertex > m_xaodConverter;  // necessary to convert VxCandidate to xAOD::Vertex in case old API is used

    xAOD::VertexContainer* m_pSecVtxContainer;
    xAOD::VertexAuxContainer* m_pSecVtxAuxContainer;
};

#endif	/* TAUREC_TAUVERTEXVARIABLES_H */

