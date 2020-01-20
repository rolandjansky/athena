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
    virtual StatusCode execute(xAOD::TauJet& pTau) override;
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    virtual StatusCode eventInitialize() override;
    virtual StatusCode eventFinalize() override{ return StatusCode::SUCCESS; }


    bool m_doCellCorrection; //!< enable cell origin correction
    ToolHandle< Trk::IParticleCaloExtensionTool >  m_caloExtensionTool;
    Gaudi::Property<bool> m_useOldCalo{this,"useOldCalo",false,"If true, it uses the CaloExtensionTool for calculating track extrapolation. Otherwise, it allows the code to read from the cache created by CaloExtensionBuilderalg."};

    //output particle calo extension collection
    SG::ReadHandleKey<CaloExtensionCollection>  m_ParticleCacheKey{this,
      "tauEVParticleCache", "ParticleCaloExtension", "Name of the particle measurement extrapolation cache for TauEV Algorithm"};
};

#endif
