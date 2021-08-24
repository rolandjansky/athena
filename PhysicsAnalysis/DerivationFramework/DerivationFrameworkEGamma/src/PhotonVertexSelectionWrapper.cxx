/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkEGamma/PhotonVertexSelectionWrapper.h"
#include "PhotonVertexSelection/PhotonVertexHelpers.h"
#include <vector>

namespace DerivationFramework {

PhotonVertexSelectionWrapper::PhotonVertexSelectionWrapper(const std::string& t,
                                                           const std::string& n,
                                                           const IInterface* p)
  : AthAlgTool(t, n, p)
  , m_decPrefix("")
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty(
    "DecorationPrefix", m_decPrefix, "Prefix for the decoration name");
}

StatusCode
PhotonVertexSelectionWrapper::initialize()
{
  ATH_MSG_DEBUG("Initializing " << name() << "...");
  ATH_CHECK(m_photonPointingTool.retrieve());
  ATH_MSG_DEBUG("Retrieved tool " << m_photonPointingTool);

  ATH_CHECK(m_photonContainer.initialize());
  ATH_CHECK(m_vertexContainer.initialize());

  if (m_decPrefix.empty()) {
    ATH_MSG_ERROR("No decoration prefix name provided for the output of "
                  "PhotonVertexSelectionWrapper!");
    return StatusCode::FAILURE;
  }

  const std::string baseName = m_vertexContainer.key();
  m_vtxPt = baseName + "." + m_decPrefix + "_" + "pt";
  m_vtxEta = baseName + "." + m_decPrefix + "_" + "eta";
  m_vtxPhi = baseName + "." + m_decPrefix + "_" + "phi";
  m_vtxSumPt = baseName + "." + m_decPrefix + "_" + "sumPt";
  m_vtxSumPt2 = baseName + "." + m_decPrefix + "_" + "sumPt2";

  ATH_CHECK(m_vtxPt.initialize());
  ATH_CHECK(m_vtxEta.initialize());
  ATH_CHECK(m_vtxPhi.initialize());
  ATH_CHECK(m_vtxSumPt.initialize());
  ATH_CHECK(m_vtxSumPt2.initialize());

  ATH_MSG_DEBUG("Initialization successful");

  return StatusCode::SUCCESS;
}

StatusCode
PhotonVertexSelectionWrapper::addBranches() const
{
  // retrieve the input containers
  const EventContext& ctx = Gaudi::Hive::currentContext();
  SG::ReadHandle<xAOD::PhotonContainer> photons{ m_photonContainer, ctx };
  SG::ReadHandle<xAOD::VertexContainer> vertices{ m_vertexContainer, ctx };

  // Update calo pointing auxdata for photons
  if (m_photonPointingTool->updatePointingAuxdata(*photons).isFailure()) {
    ATH_MSG_ERROR("Couldn't update photon calo pointing auxdata");
    return StatusCode::FAILURE;
  }

  // create the decorators
  SG::WriteDecorHandle<xAOD::VertexContainer, float> vtxPt(m_vtxPt, ctx);
  SG::WriteDecorHandle<xAOD::VertexContainer, float> vtxEta(m_vtxEta, ctx);
  SG::WriteDecorHandle<xAOD::VertexContainer, float> vtxPhi(m_vtxPhi, ctx);
  SG::WriteDecorHandle<xAOD::VertexContainer, float> vtxSumPt(m_vtxSumPt, ctx);
  SG::WriteDecorHandle<xAOD::VertexContainer, float> vtxSumPt2(m_vtxSumPt2,
                                                               ctx);

  // Loop over vertices and update auxdata
  for (xAOD::VertexContainer::const_iterator vtxItr = vertices->begin();
       vtxItr != vertices->end();
       ++vtxItr) {

    const xAOD::Vertex* vertex = *vtxItr;
    // initialize decorations to some default value
    vtxPt(*vertex) = -999.;
    vtxEta(*vertex) = -999.;
    vtxPhi(*vertex) = -999.;
    vtxSumPt(*vertex) = -999.;
    vtxSumPt2(*vertex) = -999.;

    // Skip dummy vertices
    if (vertex->vertexType() != xAOD::VxType::VertexType::PriVtx and
        vertex->vertexType() != xAOD::VxType::VertexType::PileUp)
      continue;

    // Get momentum vector of vertex and add it as a decoration
    TLorentzVector vmom = xAOD::PVHelpers::getVertexMomentum(vertex);
    float pt = sqrt(vmom.Px() * vmom.Px() + vmom.Py() * vmom.Py());
    float eta = 0;
    float phi = 0;
    if (pt == 0.) {
      eta = -999.;
      phi = -999.;
    } else {
      eta = asinh(vmom.Pz() / pt);
      phi = acos(vmom.Px() / pt); // in [0,Pi]
    }
    // Calculate additional quantities
    float sumPt = xAOD::PVHelpers::getVertexSumPt(vertex);
    float sumPt2 = xAOD::PVHelpers::getVertexSumPt(vertex, 2);

    // write decorations
    vtxPt(*vertex) = pt;
    vtxEta(*vertex) = eta;
    vtxPhi(*vertex) = phi;
    vtxSumPt(*vertex) = sumPt;
    vtxSumPt2(*vertex) = sumPt2;

  } // end loop o vertices

  return StatusCode::SUCCESS;
}

}
