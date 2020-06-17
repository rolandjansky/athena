/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PFlowAugmentationTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Fabrice Balli (fabrice.balli@cern.ch), Chris Young (christopher.young@cern.ch)
//

#include "PFlowAugmentationTool.h"

#include "StoreGate/WriteDecorHandle.h"

namespace DerivationFramework {

  PFlowAugmentationTool::PFlowAugmentationTool(const std::string& t,
                 const std::string& n,
                 const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_weightPFOTool("CP::WeightPFOTool/WeightPFOTool")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("Z0SinThetaCut", m_z0sinthcut = 2.0);
    declareProperty("UseChargedWeights",m_useChargedWeights = true, "True if we make use of weighting scheme for charged PFO");
    declareProperty("WeightPFOTool", m_weightPFOTool );
  }

  StatusCode PFlowAugmentationTool::initialize()
  {

    ATH_CHECK(m_vertexContainer_key.initialize());
    ATH_CHECK(m_pfoContainer_key.initialize());
    ATH_CHECK(m_PVmatchedKey.initialize());
    ATH_CHECK(m_corrP4_ptKey.initialize());
    ATH_CHECK(m_z0Key.initialize());
    ATH_CHECK(m_vzKey.initialize());
    ATH_CHECK(m_d0Key.initialize());
    ATH_CHECK(m_thetaKey.initialize());
    ATH_CHECK(m_envWeightKey.initialize());

    return StatusCode::SUCCESS;
  }

  StatusCode PFlowAugmentationTool::finalize()
  {
    return StatusCode::SUCCESS;
  }

  StatusCode PFlowAugmentationTool::addBranches() const
  {

    // Get the vertex.
    const xAOD::Vertex* pv(0);

    auto vertexContainer = SG::makeHandle (m_vertexContainer_key);
    if (!vertexContainer.isValid()){
      ATH_MSG_WARNING("Invalid  xAOD::VertexContainer datahandle"
		      << m_vertexContainer_key.key()); 
      return StatusCode::FAILURE;
    }
    auto pvcont = vertexContainer.cptr();
    if ( pvcont == 0 || pvcont->size()==0 ) {
      ATH_MSG_WARNING(" Failed to retrieve PrimaryVertices collection" );
      return StatusCode::FAILURE;
    }
    for (const auto& vx : *pvcont) {
      if (vx->vertexType() == xAOD::VxType::PriVtx) {
        pv = vx;
        break;
      }//If we have a vertex of type primary vertex
    }//iterate over the vertices and check their type

    // Use NoVtx as fall-back in case no PV is found, but the events should be rejected by the user
    // If there is no such then mark all CPFOs as unmatched
    if (pv == nullptr) {
      ATH_MSG_DEBUG("Could not find a primary vertex in this event" );
      for (auto theVertex : *pvcont) {
        if (xAOD::VxType::NoVtx == theVertex->vertexType() ) {
          pv = theVertex;
          break;
        }
      }
      if (nullptr == pv) {
        ATH_MSG_WARNING("Found neither PriVtx nor NoVtx in this event" );
      }
    }

    SG::WriteDecorHandle<xAOD::PFOContainer,char> dec_PVmatched(m_PVmatchedKey);
    SG::WriteDecorHandle<xAOD::PFOContainer,float> dec_corrP4_pt(m_corrP4_ptKey);
    SG::WriteDecorHandle<xAOD::PFOContainer,float> dec_z0(m_z0Key);
    SG::WriteDecorHandle<xAOD::PFOContainer,float> dec_vz(m_vzKey);
    SG::WriteDecorHandle<xAOD::PFOContainer,float> dec_d0(m_d0Key);
    SG::WriteDecorHandle<xAOD::PFOContainer,float> dec_theta(m_thetaKey);
    SG::WriteDecorHandle<xAOD::PFOContainer,float> dec_envWeight(m_envWeightKey);

    auto pfoContainer = SG::makeHandle (m_pfoContainer_key);
    if (!pfoContainer.isValid()){
      ATH_MSG_WARNING("Invalid  xAOD::PFOContainer datahandle"
                      << m_pfoContainer_key.key());
      return StatusCode::FAILURE;
    }
    auto cpfos = pfoContainer.cptr();

    for ( const xAOD::PFO* cpfo : *cpfos ) {
      if ( cpfo == 0 ) {
        ATH_MSG_WARNING("Have NULL pointer to charged PFO");
        continue;
      }
      const xAOD::TrackParticle* ptrk = cpfo->track(0);
      if ( ptrk == 0 ) {
        ATH_MSG_WARNING("Skipping charged PFO with null track pointer.");
        continue;
      }

      // decorate the track properties	
      dec_z0(*cpfo) = ptrk->z0();
      dec_vz(*cpfo) = ptrk->vz();
      dec_d0(*cpfo) = ptrk->d0();
      dec_theta(*cpfo) = ptrk->theta();

      bool matchedToPrimaryVertex = false;
      //vtz.z() provides z of that vertex w.r.t the center of the beamspot (z = 0). Thus we correct the track z0 to be w.r.t z = 0
      float z0 = ptrk->z0() + ptrk->vz();
      if (pv) {
        z0 = z0 - pv->z();
        float theta = ptrk->theta();
        if ( fabs(z0*sin(theta)) < m_z0sinthcut ) {
          matchedToPrimaryVertex = true;
        }
      }// if pv available

      //find the weights from the tool
      int isInDenseEnvironment = false;
      float weight = 1.0;
      if(cpfo->attribute(xAOD::PFODetails::PFOAttributes::eflowRec_isInDenseEnvironment,isInDenseEnvironment)){
	ATH_CHECK( m_weightPFOTool->fillWeight( *cpfo, weight ) );
      }

      // decorate the computed variables	
      dec_PVmatched(*cpfo) = matchedToPrimaryVertex;
      dec_corrP4_pt(*cpfo) = weight*cpfo->pt();
      dec_envWeight(*cpfo) = weight;
    }

    return StatusCode::SUCCESS;
  }
}
