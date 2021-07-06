/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_TAUELECTRONVETOVARIABLES_H
#define TAURECTOOLS_TAUELECTRONVETOVARIABLES_H

#include "tauRecTools/TauRecToolBase.h"

#include "TrkCaloExtension/CaloExtensionCollection.h"

#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"

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
  
  ASG_TOOL_CLASS2(TauElectronVetoVariables, TauRecToolBase, ITauToolBase)

  TauElectronVetoVariables(const std::string& name);

  virtual ~TauElectronVetoVariables() = default;

  virtual StatusCode initialize() override;
  
  virtual StatusCode execute(xAOD::TauJet& pTau) const override;

private:

  Gaudi::Property<bool> m_doVertexCorrection {this, "VertexCorrection", true, "switch of vertex correction"}; 
  Gaudi::Property<bool> m_useOldCalo{this,"useOldCalo",false,"If true, it uses the CaloExtensionTool for calculating track extrapolation. Otherwise, it allows the code to read from the cache created by CaloExtensionBuilderalg."};
  ToolHandle<Trk::IParticleCaloExtensionTool> m_caloExtensionTool {this, 
    "ParticleCaloExtensionTool", "Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool", "Tool for calculating track extrapolation"}; 
  SG::ReadHandleKey<CaloExtensionCollection>  m_ParticleCacheKey{this,
    "tauEVParticleCache", "ParticleCaloExtension", "Name of the particle measurement extrapolation cache for TauEV Algorithm"};
};

#endif // TAURECTOOLS_TAUELECTRONVETOVARIABLES_H
