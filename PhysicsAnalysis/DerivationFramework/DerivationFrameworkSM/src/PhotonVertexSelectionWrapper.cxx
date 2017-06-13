/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// PhotonVertexSelectionWrapper.cxx
///////////////////////////////////////////////////////////////////
// Author: Matthias Saimpert (matthias.saimpert@cern.ch)
//

// local includes
#include "DerivationFrameworkSM/PhotonVertexSelectionWrapper.h"

// EDM includes
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "PhotonVertexSelection/IPhotonPointingTool.h"
#include "PhotonVertexSelection/PhotonVertexHelpers.h"
#include <vector>
#include <string>

namespace DerivationFramework {

  PhotonVertexSelectionWrapper::PhotonVertexSelectionWrapper(const std::string& t,
      const std::string& n,
      const IInterface* p) :
    AthAlgTool(t,n,p),
    m_tool("CP::PhotonPointingTool"),
    m_sgName(""),
    m_photonContainerName(""),
    m_vertexContainerName("")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("PointingTool", m_tool);
    declareProperty("DecorationPrefix", m_sgName);
    declareProperty("PhotonContainerName", m_photonContainerName);
    declareProperty("VertexContainerName", m_vertexContainerName);
  }

  StatusCode PhotonVertexSelectionWrapper::initialize()
  {
    if (m_sgName=="") {
      ATH_MSG_ERROR("No decoration prefix name provided for the output of PhotonVertexSelectionWrapper!");
      return StatusCode::FAILURE;
    }
    if (m_photonContainerName=="") {
      ATH_MSG_ERROR("No Photons collection provided for PhotonVertexSelectionWrapper!");
      return StatusCode::FAILURE;
    }
    if (m_vertexContainerName=="") {
      ATH_MSG_ERROR("No PrimaryVertices collection provided for PhotonVertexSelectionWrapper!");
      return StatusCode::FAILURE;
    }
    ATH_CHECK(m_tool.retrieve());
    return StatusCode::SUCCESS;
  }

  StatusCode PhotonVertexSelectionWrapper::finalize()
  {
    return StatusCode::SUCCESS;
  }

 
  StatusCode PhotonVertexSelectionWrapper::addBranches() const
  {
    // Retrieve Photons from TEvent
    const xAOD::PhotonContainer* photons = 0;
    if (evtStore()->retrieve(photons, m_photonContainerName).isFailure()) {
         ATH_MSG_WARNING("Couldn't retrieve " << m_photonContainerName << " from TEvent");
         return StatusCode::FAILURE;
    }

    // Retrieve PrimaryVertices from TEvent
    const xAOD::VertexContainer* vertices = 0;
    if (evtStore()->retrieve(vertices, m_vertexContainerName).isFailure()) {
         ATH_MSG_WARNING("Couldn't retrieve " << m_vertexContainerName << " from TEvent");
         return StatusCode::FAILURE;
    }

    // Update calo pointing auxdata for photons
    if(m_tool->updatePointingAuxdata(*photons).isFailure()) {
      ATH_MSG_WARNING("Couldn't update photon calo pointing auxdata");
      return StatusCode::FAILURE;
    }

    // Loop over vertices and update auxdata
    for (xAOD::VertexContainer::const_iterator vtxItr = vertices->begin(); vtxItr!=vertices->end(); ++vtxItr) {

      const xAOD::Vertex *vertex = *vtxItr;

      // Skip dummy vertices
      if (vertex->vertexType() != xAOD::VxType::VertexType::PriVtx and
          vertex->vertexType() != xAOD::VxType::VertexType::PileUp) continue;

      // Get momentum vector of vertex and add it as a decoration
      TLorentzVector vmom = xAOD::PVHelpers::getVertexMomentum(vertex);
      float pt = sqrt( vmom.Px()*vmom.Px() + vmom.Py()*vmom.Py() );

      float eta = 0;
      float phi = 0;

      if(pt==0)
      {
        eta = -999;
        phi = -999;
      }
      else
      {
        eta = asinh( vmom.Pz()/pt );
        phi = acos(  vmom.Px()/pt );  // in [0,Pi]
      }

      SG::AuxElement::Decorator< float > decnPT(m_sgName+"_pt");
      SG::AuxElement::Decorator< float > decnETA(m_sgName+"_eta");
      SG::AuxElement::Decorator< float > decnPHI(m_sgName+"_phi");
      decnPT( *vertex ) = pt; 
      decnETA( *vertex ) = eta; 
      decnPHI( *vertex ) = phi; 

      // Compute the other variables required for the MVA and add them as decoration
      float sumPt = xAOD::PVHelpers::getVertexSumPt(vertex);
      float sumPt2 = xAOD::PVHelpers::getVertexSumPt(vertex, 2);

      SG::AuxElement::Decorator< float > decnSUMPT(m_sgName+"_sumPt");
      SG::AuxElement::Decorator< float > decnSUMPT2(m_sgName+"_sumPt2");

      decnSUMPT( *vertex ) = sumPt;
      decnSUMPT2( *vertex ) = sumPt2;

    } // end loop of vertices

    return StatusCode::SUCCESS;

  }

}
