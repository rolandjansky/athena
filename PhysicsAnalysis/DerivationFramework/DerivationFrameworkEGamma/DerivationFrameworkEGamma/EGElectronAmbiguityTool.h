/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGElectronAmbiguityTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_EGELECTRONAMBIGUITYTOOL_H
#define DERIVATIONFRAMEWORK_EGELECTRONAMBIGUITYTOOL_H

#include "GaudiKernel/ToolHandle.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

#include "GaudiKernel/EventContext.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteDecorHandle.h"

#include "AthContainers/ConstDataVector.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"

#include <string>

namespace DerivationFramework {

class EGElectronAmbiguityTool
  : public AthAlgTool
  , public IAugmentationTool
{

public:
  EGElectronAmbiguityTool(const std::string& t,
                          const std::string& n,
                          const IInterface* p);

  virtual StatusCode initialize() override final;
  virtual StatusCode addBranches() const override final;

private:
  SG::ReadHandleKey<xAOD::ElectronContainer> m_containerName{
    this,
    "ContainerName",
    "Electrons",
    "SG key of electron container"
  };
  SG::ReadHandleKey<xAOD::VertexContainer> m_VtxContainerName{
    this,
    "VtxContainerName",
    "PrimaryVertices",
    "SG key of vertex container"
  };
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_tpContainerName{
    this,
    "tpContainerName",
    "InDetTrackParticles",
    "SG key of track particles container"
  };

  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_tpCName{
    this,
    "tpCName",
    "GSFTrackParticles",
    "SG key of TrackParticleInputContainer"
  };

  // Write decoration handle keys
  SG::WriteDecorHandleKey<xAOD::ElectronContainer> m_drv{
    "Electrons.DFCommonSimpleConvRadius"
  };
  SG::WriteDecorHandleKey<xAOD::ElectronContainer> m_dphiv{
    "Electrons.DFCommonSimpleConvPhi"
  };
  SG::WriteDecorHandleKey<xAOD::ElectronContainer> m_dmee{
    "Electrons.DFCommonSimpleMee"
  };
  SG::WriteDecorHandleKey<xAOD::ElectronContainer> m_dmeeVtx{
    "Electrons.DFCommonSimpleMeeAtVtx"
  };
  SG::WriteDecorHandleKey<xAOD::ElectronContainer> m_dsep{
    "Electrons.DFCommonSimpleSeparation"
  };
  SG::WriteDecorHandleKey<xAOD::ElectronContainer> m_dambi{
    "Electrons.DFCommonAddAmbiguity"
  };
  SG::WriteDecorHandleKey<xAOD::ElectronContainer> m_dtrv{
    "Electrons.DFCommonProdTrueRadius"
  };
  SG::WriteDecorHandleKey<xAOD::ElectronContainer> m_dtpv{
    "Electrons.DFCommonProdTruePhi"
  };
  SG::WriteDecorHandleKey<xAOD::ElectronContainer> m_dtzv{
    "Electrons.DFCommonProdTrueZ"
  };

  StatusCode decorateSimple(
    const EventContext& ctx,
    std::unique_ptr<ConstDataVector<xAOD::TrackParticleContainer>>& tpC,
    const xAOD::Electron* ele,
    const xAOD::Vertex* pvtx) const;

  bool m_isMC{};

  // cuts to select the electron to run on
  double m_elepTCut;
  std::string m_idCut;

  // cuts to select the other track
  unsigned int m_nSiCut;
  double m_dctCut, m_sepCut, m_dzCut;

  // cuts to define the various types :
  // ambi = -1 : no other track, 0 : other track exists but no good gamma reco,
  // 1 : gamma*, 2 : material conversion
  double m_rvECCut, m_meeAtVtxECCut, m_meeICCut;
};
}

#endif // DERIVATIONFRAMEWORK_EGCONVERSIONINFOTOOL_H
