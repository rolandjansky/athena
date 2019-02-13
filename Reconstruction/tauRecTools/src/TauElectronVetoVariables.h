/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAU1P3PELEVETO_H
#define TAUREC_TAU1P3PELEVETO_H

#include "tauRecTools/TauRecToolBase.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk {
  class IParticleCaloExtensionTool;
}

/**
 * @brief Calculate variables sensitive on electrons.
 * 
 *  The variables are mainly used by the electron veto in the TauDiscriminant package.
 * 
 * @author Zofia Czyczula
 */

class TauElectronVetoVariables : public TauRecToolBase {
public:

    TauElectronVetoVariables(const std::string& name);
    ASG_TOOL_CLASS2(TauElectronVetoVariables, TauRecToolBase, ITauToolBase);

    virtual ~TauElectronVetoVariables();
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

    virtual void print() const { }
    virtual StatusCode eventFinalize() { return StatusCode::SUCCESS; }

    bool m_doCellCorrection; //!< enable cell origin correction
    ToolHandle< Trk::IParticleCaloExtensionTool >  m_caloExtensionTool;
};

#endif
