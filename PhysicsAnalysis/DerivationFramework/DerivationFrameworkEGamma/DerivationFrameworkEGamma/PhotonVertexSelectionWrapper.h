/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DERIVATIONFRAMEWORK_PHOTONVERTEXSELECTIONWRAPPER_H
#define DERIVATIONFRAMEWORK_PHOTONVERTEXSELECTIONWRAPPER_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "PhotonVertexSelection/IPhotonPointingTool.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteDecorHandle.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTracking/VertexContainer.h"

// namespace CP
//{
//  class IPhotonPointingTool;
//}

namespace DerivationFramework {

class PhotonVertexSelectionWrapper
  : public AthAlgTool
  , public IAugmentationTool
{
public:
  PhotonVertexSelectionWrapper(const std::string& t,
                               const std::string& n,
                               const IInterface* p);

  StatusCode initialize() override final;
  virtual StatusCode addBranches() const override final;

private:
  /** @brief PhotonPointingTool **/
  ToolHandle<CP::IPhotonPointingTool> m_photonPointingTool{
    this,
    "PhotonPointingTool",
    "",
    "Handle to the photon pointing tool"
  };

  /** @brief Prefix for the decorations **/
  std::string m_decPrefix;

  /** @brief Input photon container **/
  SG::ReadHandleKey<xAOD::PhotonContainer> m_photonContainer{
    this,
    "PhotonContainer",
    "Photons",
    "SG key of input photon container"
  };

  /** @brief Input primary vertex container **/
  SG::ReadHandleKey<xAOD::VertexContainer> m_vertexContainer{
    this,
    "VertexContainer",
    "PrimaryVertices",
    "SG key of input vertex container"
  };

  // Write decoration handle keys
  // these are not really configurable
  SG::WriteDecorHandleKey<xAOD::VertexContainer> m_vtxPt{ this, "pt", "", "" };
  SG::WriteDecorHandleKey<xAOD::VertexContainer> m_vtxEta{ this,
                                                           "eta",
                                                           "",
                                                           "" };
  SG::WriteDecorHandleKey<xAOD::VertexContainer> m_vtxPhi{ this,
                                                           "phi",
                                                           "",
                                                           "" };
  SG::WriteDecorHandleKey<xAOD::VertexContainer> m_vtxSumPt{ this,
                                                             "sumPt",
                                                             "",
                                                             "" };
  SG::WriteDecorHandleKey<xAOD::VertexContainer> m_vtxSumPt2{ this,
                                                              "sumPt2",
                                                              "",
                                                              "" };
};
}

#endif // DERIVATIONFRAMEWORK_PHOTONVERTEXSELECTIONWRAPPER_H
