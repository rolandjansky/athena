/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_PHOTONCONVERSIONVERTEX_H
#define TAUREC_PHOTONCONVERSIONVERTEX_H

#include "tauRecTools/TauRecToolBase.h"
#include "GaudiKernel/ToolHandle.h"

namespace Analysis {
    class TauJetContainer;
}

namespace Rec {
    class TrackParticle;
}

namespace InDet {
    class IVertexFinder;
}

/**
 * @brief Class that runs the tau conversion finding algorithm
 * 
 *  This tool identifies conversion candidates
 *  in/near (definable) a tau decay cone by reconstructing the conversion vertices 
 *  and applying a set of cuts optimized for tau conversions on the vertex parameters.
 * 
 * @author KG Tan <Kong.Guan.Tan@cern.ch>
 * 
 */

class PhotonConversionVertex : public TauRecToolBase {
public:
    //-------------------------------------------------------------
    //! Constructor and Destructor
    //-------------------------------------------------------------
    PhotonConversionVertex(const std::string& name);
    ASG_TOOL_CLASS2(PhotonConversionVertex, TauRecToolBase, ITauToolBase);
    ~PhotonConversionVertex();

    //-------------------------------------------------------------
    //! Algorithm functions
    //-------------------------------------------------------------
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode eventInitialize() { return StatusCode::SUCCESS; }
    virtual StatusCode eventFinalize();
    virtual StatusCode execute(xAOD::TauJet&) { return StatusCode::SUCCESS; }
    virtual StatusCode executeShotFinder(xAOD::TauJet&, xAOD::CaloClusterContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0CreateROI(xAOD::TauJet&, CaloCellContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0ClusterCreator(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&, xAOD::CaloClusterContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0nPFO(xAOD::TauJet&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executeVertexVariables(xAOD::TauJet&, xAOD::VertexContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0ClusterScaler(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePanTau(xAOD::TauJet&, xAOD::ParticleContainer&) { return StatusCode::SUCCESS; }

    virtual void print() const { }

private:
    //-------------------------------------------------------------
    //! Convenience functions to handle storegate objects
    //-------------------------------------------------------------
    template <class T>
    bool openContainer(T* &container, std::string containerName);

    template <class T>
    bool saveContainer(T* &container, std::string containerName);

    template <class T>
    bool retrieveTool(T &tool);

    //-------------------------------------------------------------
    //! Gets the minimum dR between a particle and a set of taus
    //-------------------------------------------------------------
    double getMinDrTauDecay(const xAOD::TauJetContainer* tauJetCont, const xAOD::TrackParticle *trackParticle);

private:
    //-------------------------------------------------------------
    //! Storegate names of input containers and output containers
    //-------------------------------------------------------------
    std::string m_inputTauJetContainerName;
    std::string m_inputTrackParticleContainerName;
    std::string m_outputConversionVertexContainerName;

    //-------------------------------------------------------------
    //! Input parameters for conversion finding
    //-------------------------------------------------------------
    double m_maxTauJetDr;

    //-------------------------------------------------------------
    //! Tool used by conversion finding, initialised in job options
    //-------------------------------------------------------------
    ToolHandle<InDet::IVertexFinder> m_vertexFinderTool;
};

#endif
