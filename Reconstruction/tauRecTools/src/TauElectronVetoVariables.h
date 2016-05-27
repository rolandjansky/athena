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
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode eventInitialize();

    virtual void print() const { }
    virtual StatusCode eventFinalize() { return StatusCode::SUCCESS; }

    bool m_doCellCorrection; //!< enable cell origin correction
    ToolHandle< Trk::IParticleCaloExtensionTool >  m_caloExtensionTool;
};

#endif
