/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAU1P3PELEVETO_H
#define TAUREC_TAU1P3PELEVETO_H

#include "tauRecTools/TauRecToolBase.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrkCaloExtension/CaloExtensionCollection.h"

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
    virtual StatusCode execute(xAOD::TauJet& pTau) const override;
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

private:
    Gaudi::Property<bool> m_doVertexCorrection {this, "VertexCorrection", false, "switch of vertex correction"}; 
    Gaudi::Property<bool> m_useOldCalo{this,"useOldCalo",false,"If true, it uses the CaloExtensionTool for calculating track extrapolation. Otherwise, it allows the code to read from the cache created by CaloExtensionBuilderalg."};
    Gaudi::Property<bool> m_incShowerSubtr {this, "IncShowerSubtr", true, "use shower subtracted clusters in calo calculations"};
    ToolHandle<Trk::IParticleCaloExtensionTool> m_caloExtensionTool {this, 
      "ParticleCaloExtensionTool", "Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool", "Tool for calculating track extrapolation"}; 
    SG::ReadHandleKey<CaloExtensionCollection>  m_ParticleCacheKey{this,
      "tauEVParticleCache", "ParticleCaloExtension", "Name of the particle measurement extrapolation cache for TauEV Algorithm"};
};

#endif
