/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUVERTEXVARIABLES_H
#define	TAUREC_TAUVERTEXVARIABLES_H

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "tauRec/TauToolBase.h"
#include "GaudiKernel/ToolHandle.h"

// forwards
class TauCandidateData;
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

class TauVertexVariables : public TauToolBase {
public:
    //-----------------------------------------------------------------
    // Constructor and destructor
    //-----------------------------------------------------------------
    TauVertexVariables(const std::string& type,
            const std::string& name,
            const IInterface* parent);
    ~TauVertexVariables();
    
    virtual StatusCode initialize();
    virtual StatusCode execute(TauCandidateData *data);
    virtual StatusCode eventInitialize(TauCandidateData *data);
    virtual StatusCode finalize();
    
    //-------------------------------------------------------------
    //! determines the transverse flight path significance from
    //! the primary vertex and the secondary vertex of tau candidate
    //-------------------------------------------------------------
    double trFlightPathSig(TauCandidateData *data, const xAOD::Vertex *secVertex);

private:
    std::string m_primaryVertexKey;
    std::string m_inputTrackParticleContainerName;
    bool m_useOldSeedFinderAPI;
    ToolHandle< Trk::ITrackToVertexIPEstimator > m_trackToVertexIPEstimator;
    ToolHandle< Trk::IVertexFitter >     m_fitTool; //!< Pointer to the base class of the fit algtools
    ToolHandle< Trk::IVertexSeedFinder > m_SeedFinder;
    ToolHandle< Trk::IVxCandidateXAODVertex > m_xaodConverter;  // necessary to convert VxCandidate to xAOD::Vertex in case old API is used

    xAOD::VertexContainer* m_pSecVtxContainer;
    xAOD::VertexAuxContainer* m_pSecVtxAuxContainer;
};

#endif	/* TAUREC_TAUVERTEXVARIABLES_H */

