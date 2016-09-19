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


#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "InDetRecToolInterfaces/IVertexFinder.h"
#include "InDetConversionFinderTools/SingleTrackConversionTool.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "egammaInterfaces/IEMExtrapolationTools.h"
#include "EMVertexBuilder.h"

EMVertexBuilder::EMVertexBuilder(const std::string& type, const std::string& name, const IInterface* parent) :
  egammaBaseTool(type, name, parent),
  m_vertexFinderTool("InDet::InDetConversionFinderTools"),
  m_EMExtrapolationTool("EMExtrapolationTools")
{
  declareProperty("InputTrackParticleContainerName", m_inputTrackParticleContainerName = "GSFTrackParticles");

  declareProperty("OutputConversionContainerName",   m_outputConversionContainerName = "GSFConversionVertices");

  declareProperty("VertexFinderTool",                m_vertexFinderTool);

  // Name of the extrapolation tool
  declareProperty("ExtrapolationTool",
		  m_EMExtrapolationTool,
		  "Handle of the extrapolation tool");


  declareProperty("MaxRadius", m_maxRadius = 800., 
      "Maximum radius accepted for conversion vertices");

  declareProperty("minPCutDoubleTrackConversion",      m_minPtCut_DoubleTrack = 2000 ,  "Minimum Pt, less than that TRT tracks pileup for double-track conversion");  

  declareProperty("minPCutSingleTrackConversion",      m_minPtCut_SingleTrack = 2000 ,  "Minimum Pt, less than that TRT track pileup for single-track conversion");  

  declareInterface<IEMVertexBuilder>(this);
}

// ============================================================
EMVertexBuilder::~EMVertexBuilder() {}

// ============================================================
StatusCode EMVertexBuilder::initialize() {

  ATH_MSG_DEBUG( "Initializing " << name() << "...");
  

  // Get the ID VertexFinderTool
  if ( m_vertexFinderTool.retrieve().isFailure() ) {
    ATH_MSG_ERROR("Failed to retrieve vertex finder tool " << m_vertexFinderTool);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG( "Retrieved tool " << m_vertexFinderTool);
  }

  // Retrieve EMExtrapolationTool
  if(m_EMExtrapolationTool.retrieve().isFailure()){
    ATH_MSG_ERROR("Cannot retrieve extrapolationTool " << m_EMExtrapolationTool);
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
StatusCode EMVertexBuilder::contExecute()
{  	
  StatusCode sc;

  //retrieve TrackParticleContainer and cast into non-const
  const xAOD::TrackParticleContainer* TPCol;
  sc = evtStore()->retrieve(TPCol, m_inputTrackParticleContainerName);
  if(sc.isFailure()  ||  !TPCol){
    ATH_MSG_ERROR("No TrackParticleInputContainer found in TDS");
    return sc;
  }

  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> vertices = m_vertexFinderTool->findVertex(TPCol);
  if (!vertices.first || !vertices.second){
    ATH_MSG_ERROR("Null pointer to conversion container");
    return StatusCode::SUCCESS;
  }
  CHECK( evtStore()->record(vertices.first,  m_outputConversionContainerName) );
  CHECK( evtStore()->record(vertices.second, m_outputConversionContainerName + "Aux.") );

  ATH_MSG_DEBUG("New conversion container size: " << vertices.first->size());

  // Remove vertices with radii above m_maxRadius   
  xAOD::VertexContainer::iterator itVtx = vertices.first->begin();
  xAOD::VertexContainer::iterator itVtxEnd = vertices.first->end();
  
  while (itVtx != itVtxEnd){
    xAOD::Vertex& vertex = **itVtx;
    
    Amg::Vector3D momentum(0., 0., 0.);
    for (unsigned int i = 0; i < vertex.nTrackParticles(); ++i){
      momentum += m_EMExtrapolationTool->getMomentumAtVertex(vertex, i);
    }

    const static SG::AuxElement::Accessor<float> accPx("px");
    const static SG::AuxElement::Accessor<float> accPy("py");
    const static SG::AuxElement::Accessor<float> accPz("pz");
    accPx(vertex) = momentum.x();
    accPy(vertex) = momentum.y();
    accPz(vertex) = momentum.z();
    
    xAOD::EgammaParameters::ConversionType convType(xAOD::EgammaHelpers::conversionType((*itVtx)));
    bool vxDoubleTRT = (convType == xAOD::EgammaParameters::doubleTRT);
    bool vxSingleTRT = (convType == xAOD::EgammaParameters::singleTRT);

    if ((vxDoubleTRT && momentum.perp() < m_minPtCut_DoubleTrack) ||
	(vxSingleTRT &&  momentum.perp()< m_minPtCut_SingleTrack) || 
	((*itVtx)->position().perp() > m_maxRadius)){
      
      itVtx = vertices.first->erase(itVtx);
      itVtxEnd = vertices.first->end();
    }
    else{
      ++itVtx;
    }
  }
  
  // Decorate the vertices with the momentum at the conversion point and 
  // etaAtCalo, phiAtCalo (extrapolate each vertex)
  float etaAtCalo = -9999., phiAtCalo = -9999.;
  for (itVtx = vertices.first->begin(); itVtx != vertices.first->end(); ++itVtx ){
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
  
  //put the new conversion vertex container and its aux container into StoreGate

  ATH_MSG_DEBUG("Writing container " << m_outputConversionContainerName);

  
  return StatusCode::SUCCESS;
}
