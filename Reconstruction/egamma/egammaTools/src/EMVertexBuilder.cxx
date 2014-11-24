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

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

#include "InDetRecToolInterfaces/IVertexFinder.h"
#include "InDetConversionFinderTools/SingleTrackConversionTool.h"

#include "xAODEgamma/EgammaxAODHelpers.h"
#include "egammaUtils/EMConversionUtils.h"

#include "egammaInterfaces/IEMExtrapolationTools.h"

#include "EMVertexBuilder.h"

EMVertexBuilder::EMVertexBuilder(const std::string& type, const std::string& name, const IInterface* parent) :
  egammaBaseTool(type, name, parent),
  m_storeGate(0),
  m_vertexFinderTool("InDet::InDetConversionFinderTools"),
  m_singleTrkConvTool("InDet::SingleTrackConversionTool"),
  m_EMExtrapolationTool("EMExtrapolationTools")
{
  declareProperty("InputTrackParticleContainerName", m_inputTrackParticleContainerName = "GSFTrackParticles");

  declareProperty("OutputConversionContainerName",   m_outputConversionContainerName = "GSFConversionVertices");

  declareProperty("VertexFinderTool",                m_vertexFinderTool);

  // Name of the single track conversion tool
  declareProperty("SingleTrackConversionTool", m_singleTrkConvTool,
		  "Handle of the single-track conversion tool");

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
  
  // Get pointer to StoreGateSvc and cache it :
  if ( !service( "StoreGateSvc", m_storeGate ).isSuccess() ) {
    ATH_MSG_ERROR("Unable to retrieve pointer to StoreGateSvc");
    return StatusCode::FAILURE;
  }

  // Get the ID VertexFinderTool
  if ( m_vertexFinderTool.retrieve().isFailure() ) {
    ATH_MSG_ERROR("Failed to retrieve vertex finder tool " << m_vertexFinderTool);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG( "Retrieved tool " << m_vertexFinderTool);
  }

  //Retrieve Single Track Conversion Tool
  if ( m_singleTrkConvTool.retrieve().isFailure() ) {
    ATH_MSG_ERROR("Cannot retrieve single track conversion tool " << m_singleTrkConvTool);
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG("Retrieved  single track conversiontool " << m_singleTrkConvTool);
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
  const xAOD::TrackParticleContainer* TrackParticleInputContainer;
  sc = evtStore()->retrieve(TrackParticleInputContainer, m_inputTrackParticleContainerName);
  if(sc.isFailure()  ||  !TrackParticleInputContainer){
    ATH_MSG_ERROR("No TrackParticleInputContainer found in TDS");
    return sc;
  }
  
  xAOD::TrackParticleContainer * TPCol = const_cast<xAOD::TrackParticleContainer*>(TrackParticleInputContainer);

  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> vertices = m_vertexFinderTool->findVertex(TPCol);
  if (!vertices.first || !vertices.second)
  {
    ATH_MSG_ERROR("Null pointer to conversion container");
    return StatusCode::SUCCESS;
  }


  ATH_MSG_DEBUG("New conversion container size: " << vertices.first->size());

  // Remove vertices with radii above m_maxRadius   
  xAOD::VertexContainer::iterator itVtx = vertices.first->begin();
  xAOD::VertexContainer::iterator itVtxEnd = vertices.first->end();
  
  while (itVtx != itVtxEnd){

    // false is to force the extrapolation
    Amg::Vector3D momentum = m_EMExtrapolationTool->getMomentumAtVertex(**itVtx, false);
    (*itVtx)->auxdata<float>("px") = momentum.x();
    (*itVtx)->auxdata<float>("py") = momentum.y();
    (*itVtx)->auxdata<float>("pz") = momentum.z();
    
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
    vertex->auxdata<float>("etaAtCalo") = etaAtCalo;
    vertex->auxdata<float>("phiAtCalo") = phiAtCalo;
  }
  
  //put the new conversion vertex container and its aux container into StoreGate

  ATH_MSG_DEBUG("Writing container " << m_outputConversionContainerName);

  CHECK( evtStore()->record(vertices.first,  m_outputConversionContainerName) );
  CHECK( evtStore()->record(vertices.second, m_outputConversionContainerName + "Aux.") );
  
  return StatusCode::SUCCESS;
}
// ============================================================
void EMVertexBuilder::addSingleTrackVertices(const xAOD::TrackParticleContainer* tracks,
                                             xAOD::VertexContainer* vertices) const
{
  if (!tracks || !vertices) return;

  // Create a set of tracks that do belong to vertices
  std::set<const xAOD::TrackParticle*> tracksInVertices;
  xAOD::VertexContainer::const_iterator itVtx = vertices->begin();
  for ( ; itVtx != vertices->end(); ++itVtx)
    for (const auto& trackElement : (*itVtx)->trackParticleLinks() )
      tracksInVertices.insert( trackElement.cachedElement() );
  
  ATH_MSG_DEBUG("Size of set with tracks: " << tracksInVertices.size());
    
  // Loop over tracks and create vertices for those that are not in the set
  xAOD::TrackParticleContainer::const_iterator itTP = tracks->begin();
  for ( ; itTP != tracks->end(); ++itTP)
  {
    if ( tracksInVertices.find(*itTP) != tracksInVertices.end() ) continue;

    int nclus(0);
    uint8_t dummy(0); 
    
    if( (*itTP)->summaryValue(dummy,xAOD::numberOfPixelHits) )
      nclus += dummy;
    if( (*itTP)->summaryValue(dummy,xAOD::numberOfSCTHits) )
      nclus += dummy;
    
    if(nclus<4 && (*itTP)->pt() < m_minPtCut_SingleTrack )      continue;

    m_singleTrkConvTool->buildSingleTrackParticleConversion(*itTP, vertices);
  }

}

