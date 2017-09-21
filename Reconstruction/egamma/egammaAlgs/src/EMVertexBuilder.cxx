/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************

EMVertexBuilder.cxx  -  Description
-------------------
begin   : 11-03-2011
authors : Kerstin Tackmann, Bruno Lenzi
email   : kerstin.tackmann@cern.ch, Bruno.Lenzi@cern.ch
changes :
          Nov 2011 (KT) Initial version
          Mar 2014 (BL) xAOD migration, creates both double and single track vertices

***************************************************************************/

#include "EMVertexBuilder.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

EMVertexBuilder::EMVertexBuilder(const std::string& name, 
				 ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator)
{
}

// ============================================================
StatusCode EMVertexBuilder::initialize() {

  ATH_MSG_DEBUG( "Initializing " << name() << "...");
  
  ATH_CHECK(m_inputTrackParticleContainerKey.initialize());
  ATH_CHECK(m_outputConversionContainerKey.initialize());

  // Get the ID VertexFinderTool
  if ( m_vertexFinderTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL("Failed to retrieve vertex finder tool " << m_vertexFinderTool);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG( "Retrieved tool " << m_vertexFinderTool);
  }

  // Retrieve EMExtrapolationTool
  if(m_EMExtrapolationTool.retrieve().isFailure()){
    ATH_MSG_FATAL("Cannot retrieve extrapolationTool " << m_EMExtrapolationTool);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Retrieved extrapolationTool " << m_EMExtrapolationTool);
  }


  ATH_MSG_DEBUG( "Initialization successful");

  return StatusCode::SUCCESS;
}

// ============================================================
StatusCode EMVertexBuilder::finalize() {
  return StatusCode::SUCCESS;
}

// ============================================================
StatusCode EMVertexBuilder::execute()
{  	
  //retrieve TrackParticleContainer
  SG::ReadHandle<xAOD::TrackParticleContainer> TPCol(m_inputTrackParticleContainerKey);

  // check for serial running only, remove in MT
  if(!TPCol.isValid()){
    ATH_MSG_FATAL("No TrackParticleInputContainer found in TDS");
    return StatusCode::FAILURE;
  }

  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> verticesPair = m_vertexFinderTool->findVertex(TPCol.cptr());
  if (!verticesPair.first || !verticesPair.second){
    ATH_MSG_ERROR("Null pointer to conversion container");
    return StatusCode::SUCCESS;
  }

  SG::WriteHandle<xAOD::VertexContainer>vertices(m_outputConversionContainerKey);
  ATH_CHECK(vertices.record(std::unique_ptr<xAOD::VertexContainer>(verticesPair.first),
			    std::unique_ptr<xAOD::VertexAuxContainer>(verticesPair.second)));

  ATH_MSG_DEBUG("New conversion container size: " << vertices->size());

  // Remove vertices with radii above m_maxRadius   
  xAOD::VertexContainer::iterator itVtx = vertices->begin();
  xAOD::VertexContainer::iterator itVtxEnd = vertices->end();
  
  while (itVtx != itVtxEnd){
    xAOD::Vertex& vertex = **itVtx;
    
    Amg::Vector3D momentum(0., 0., 0.);
    for (unsigned int i = 0; i < vertex.nTrackParticles(); ++i){
      momentum += m_EMExtrapolationTool->getMomentumAtVertex(vertex, i);
    }

    static const SG::AuxElement::Accessor<float> accPx("px");
    static const SG::AuxElement::Accessor<float> accPy("py");
    static const SG::AuxElement::Accessor<float> accPz("pz");
    accPx(vertex) = momentum.x();
    accPy(vertex) = momentum.y();
    accPz(vertex) = momentum.z();
    
    xAOD::EgammaParameters::ConversionType convType(xAOD::EgammaHelpers::conversionType((*itVtx)));
    bool vxDoubleTRT = (convType == xAOD::EgammaParameters::doubleTRT);
    bool vxSingleTRT = (convType == xAOD::EgammaParameters::singleTRT);

    if ((vxDoubleTRT && momentum.perp() < m_minPtCut_DoubleTrack) ||
	(vxSingleTRT &&  momentum.perp()< m_minPtCut_SingleTrack) || 
	((*itVtx)->position().perp() > m_maxRadius)){
      
      itVtx = vertices->erase(itVtx);
      itVtxEnd = vertices->end();
    }
    else{
      ++itVtx;
    }
  }
  
  // Decorate the vertices with the momentum at the conversion point and 
  // etaAtCalo, phiAtCalo (extrapolate each vertex)
  float etaAtCalo = -9999., phiAtCalo = -9999.;
  for (itVtx = vertices->begin(); itVtx != vertices->end(); ++itVtx ){
    xAOD::Vertex *vertex = *itVtx;
    
    
    if (!m_EMExtrapolationTool->getEtaPhiAtCalo(vertex, &etaAtCalo, &phiAtCalo)){
      ATH_MSG_DEBUG("getEtaPhiAtCalo failed!");
    }
    
    // Decorate vertex with etaAtCalo, phiAtCalo
    static const SG::AuxElement::Accessor<float> accetaAtCalo("etaAtCalo");
    static const SG::AuxElement::Accessor<float> accphiAtCalo("phiAtCalo");
    accetaAtCalo(*vertex) = etaAtCalo;
    accphiAtCalo(*vertex) = phiAtCalo;
  }
  
  return StatusCode::SUCCESS;
}
