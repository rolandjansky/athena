/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EMConversionBuilder.cxx
PACKAGE:  offline/Reconstruction/egamma/egammaRec

AUTHORS:  D. Zerwas, B. Lenzi
CREATED:  Jul, 2005
CHANGES:  Mar, 2014 (BL) xAOD migration

PURPOSE:  subAlgorithm which creates an EMConversion object. 

********************************************************************/

// INCLUDE HEADER FILES:

#include "EMConversionBuilder.h"
#include "ConvVxSorter.h"
#include "egammaInterfaces/IEMExtrapolationTools.h"
#include "xAODTracking/VertexContainer.h"
#include "egammaRecEvent/egammaRecContainer.h"
#include "egammaRecEvent/egammaRec.h"
#include "FourMomUtils/P4Helpers.h"

//  END OF HEADER FILES INCLUDE

/////////////////////////////////////////////////////////////////

using CLHEP::GeV;
using namespace xAOD::EgammaParameters;

EMConversionBuilder::EMConversionBuilder(const std::string& type,
					 const std::string& name,
					 const IInterface* parent)
  : egammaBaseTool(type, name, parent),
    m_extrapolationTool("EMExtrapolationTools")
{
  
  // declare interface
  declareInterface<IEMConversionBuilder>(this);
  
  // Name of the input conversion container
  declareProperty("ConversionContainerName",                 
		  m_conversionContainerName="PhotonConversionVertices",
		  "Name of the input conversion container");
	
  // Name of the extrapolation tool
  declareProperty("ExtrapolationTool",
		  m_extrapolationTool,
		  "Handle of the extrapolation tool");
      
  declareProperty("RejectAllTRTConversions", m_rejectAllTRT = false,
		  "Ignore all conversion vertices containing exclusively TRT-only tracks");
    
  declareProperty("minTRTHits", m_minTRTHits = 0,
		  "minimum number of TRT hits for TRT-only tracks (both single and double track conversion vertices)");
  
  declareProperty("minPt_singleTrack", m_minPt_singleTrack = 0*GeV,
		  "minimum pT for single-track conversion vertices");
  
  declareProperty("minPt_singleTRT", m_minPt_singleTRT = 2*GeV,
		  "minimum pT for TRT-only single-track conversion vertices");
  
  declareProperty("minTRTonlyTrackPt", m_minTRTonlyTrackPt = 0*GeV,
		  "minimum pT for each track in TRT-only double-track conversion vertices");
        
  declareProperty("minSumPt_double", m_minSumPt_double = 0*GeV,
		  "minimum sum pT for double track conversion vertices");

  declareProperty("minSumPt_doubleTRT", m_minSumPt_doubleTRT = 2*GeV,
		  "minimum sum pT for double TRT track conversion vertices");

  declareProperty("maxEoverP_singleTrack", m_maxEoverP_singleTrack = 10.,
      "Maximum E/p for single track conversion vertices");

  declareProperty("maxEoverP_singleTrack_EtSf", m_maxEoverP_singleTrack_EtSf = 0.01,  
      "Scale maxEoverP_singleTrack by ( 1+sf*Et(cluster)/GeV ) ");
  
  declareProperty("maxTRTTubeHitFraction", m_maxTRTTubeHitFraction = 999.,
		  "Maximum fraction of tube hits for vertices with TRT tracks"); // for 21.0.X: minTRTPrecisionFraction cut applied InDetTRT_StandaloneScoringTool
  

}

// =================================================================
// DESTRUCTOR:
EMConversionBuilder::~EMConversionBuilder()
{  
}

// ==================================================================
// INITIALIZE METHOD:  
StatusCode EMConversionBuilder::initialize()
{

  ATH_MSG_DEBUG("Initializing EMConversionBuilder");

  // the extrapolation tool
  if(m_extrapolationTool.retrieve().isFailure()){
    ATH_MSG_ERROR("Cannot retrieve extrapolationTool " << m_extrapolationTool);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Retrieved extrapolationTool " << m_extrapolationTool);
  }

  return StatusCode::SUCCESS;
}

// =============================================================
StatusCode EMConversionBuilder::contExecute(EgammaRecContainer& cont) 
{

  // retrieve Conversion Container
  const xAOD::VertexContainer* conversions = 0;
  if(evtStore()->retrieve(conversions,m_conversionContainerName).isFailure()){
    ATH_MSG_WARNING("Could not retrieve Conversion container! EMConversionBuilder will stop.");
    return StatusCode::SUCCESS;
  }
  
  for (egammaRec* egRec : cont){
    ATH_CHECK(vertexExecute(egRec,conversions));
  }
  return StatusCode::SUCCESS;
}


StatusCode EMConversionBuilder::executeRec(egammaRec* egRec) {
  // retrieve Conversion Container
  const xAOD::VertexContainer* conversions = 0;
  if(evtStore()->retrieve(conversions,m_conversionContainerName).isFailure()){
    ATH_MSG_WARNING("Could not retrieve Conversion container! EMConversionBuilder will stop.");
    return StatusCode::SUCCESS;
  }
  //reset the vertices
  std::vector< ElementLink< xAOD::VertexContainer > >  vertices;
  egRec->setVertices(vertices);
  ATH_CHECK(vertexExecute(egRec,conversions));
  return StatusCode::SUCCESS;
}

// =============================================================
StatusCode EMConversionBuilder::hltExecute(egammaRec* egRec, const xAOD::VertexContainer* conversions){
  ATH_CHECK(vertexExecute(egRec,conversions));
  return StatusCode::SUCCESS;
}

StatusCode EMConversionBuilder::vertexExecute(egammaRec* egRec, const xAOD::VertexContainer* conversions){
  
  if (!egRec || !conversions){
    ATH_MSG_WARNING("trackExecute: NULL pointer to egammaRec or VertexContainer");
    return StatusCode::SUCCESS;
  }

  static const SG::AuxElement::Accessor<float> accetaAtCalo("etaAtCalo");
  static const SG::AuxElement::Accessor<float> accphiAtCalo("phiAtCalo");

  float etaAtCalo(0), phiAtCalo(0);
  for (unsigned int iVtx = 0; iVtx < conversions->size(); ++iVtx){
    
    const xAOD::Vertex *vertex = conversions->at(iVtx);    
    // Check if vertex was already decorated with etaAtCalo, phiAtCalo  
    if ( accetaAtCalo.isAvailable(*vertex) &&
	 accphiAtCalo.isAvailable(*vertex) ){
      etaAtCalo = accetaAtCalo(*vertex);
      phiAtCalo = accphiAtCalo(*vertex);
    }
    // check extrapolation, skip vertex in case of failure
    else if (!m_extrapolationTool->getEtaPhiAtCalo(vertex, &etaAtCalo, &phiAtCalo)){
      continue;
    }
    const xAOD::CaloCluster *cluster = egRec->caloCluster();
    if (!passPtAndEoverP(*vertex, *cluster)){
      continue;
    }
    if (!m_extrapolationTool->matchesAtCalo(cluster, vertex, etaAtCalo, phiAtCalo)){
      continue;
    }
    const ElementLink< xAOD::VertexContainer > vertexLink( *conversions, iVtx );
    
    // If this is the best (or the first) vertex, push front and keep deltaEta, deltaPhi
    if (!egRec->getNumberOfVertices() || ConvVxSorter()(*vertex, *egRec->vertex())){
      egRec->pushFrontVertex( vertexLink );
      egRec->setDeltaEtaVtx( cluster->etaBE(2) - etaAtCalo );
      egRec->setDeltaPhiVtx( P4Helpers::deltaPhi(cluster->phiBE(2), phiAtCalo) );
    }
    else {// Not the best vertex, push back
      egRec->pushBackVertex( vertexLink );       
    }
  }
  return StatusCode::SUCCESS;
}

// ==================================================================
// FINALIZE METHOD:  
StatusCode EMConversionBuilder::finalize(){
  return StatusCode::SUCCESS;
}
// ==================================================================
bool EMConversionBuilder::passPtAndEoverP(const xAOD::Vertex& vertex, const xAOD::CaloCluster& cluster) const
{
  Amg::Vector3D momentum = m_extrapolationTool->getMomentumAtVertex(vertex);
  float pt = momentum.perp();
  float EoverP = cluster.e() / momentum.mag();
  
  auto convType = xAOD::EgammaHelpers::conversionType(&vertex);
  bool isSingle = (convType == singleTRT || convType == singleSi);
  bool isTRT = (convType == singleTRT || convType == xAOD::EgammaParameters::doubleTRT);
  float EoverPcut = m_maxEoverP_singleTrack*(1+m_maxEoverP_singleTrack_EtSf*cluster.et()*1e-3);
  
  // Check TRT tube hit fraction
  float tubeHitFraction = getMaxTRTTubeHitFraction(vertex);
  if (isTRT && tubeHitFraction > m_maxTRTTubeHitFraction)
  {
    ATH_MSG_DEBUG("Conversion failed cut on TRT tube hit fraction: " 
      << tubeHitFraction << " vs. " << m_maxTRTTubeHitFraction);
    return false;
  }
  
  bool reject =  (
		  (isTRT && m_rejectAllTRT) ||
		  (isSingle && pt < m_minPt_singleTrack) ||
		  (!isSingle && pt < m_minSumPt_double) ||
		  (isSingle && EoverP > EoverPcut) ||
		  (convType == singleTRT && pt < m_minPt_singleTRT) ||
		  (convType == doubleTRT && pt < m_minSumPt_doubleTRT)
		  );
  
  if (reject) {ATH_MSG_DEBUG("Conversion failed pt or E/p cuts");}
  return !reject;
}

float EMConversionBuilder::getMaxTRTTubeHitFraction(const xAOD::Vertex& vertex) const{
  auto getTRTTubeHitFraction = [](const xAOD::TrackParticle *trk){
    uint8_t nTRT, nTRTTube;
    if (!trk || !trk->summaryValue(nTRT, xAOD::numberOfTRTHits) || !nTRT ) return 0.;
    return trk->summaryValue(nTRTTube, xAOD::numberOfTRTTubeHits) ? 1.*nTRTTube/nTRT : 0.;
  };
  
  float maxTubeHitFraction = 0.;
  for (unsigned int i=0; i < vertex.nTrackParticles(); ++i){
    if ( !vertex.trackParticle(i) ){
      ATH_MSG_WARNING("NULL pointer to track particle in conversion vertex");
    }
    else{
      float tubeHitFraction = getTRTTubeHitFraction( vertex.trackParticle(i) );
      if (tubeHitFraction > maxTubeHitFraction) {maxTubeHitFraction = tubeHitFraction;}
    }
  }  
  return maxTubeHitFraction;
}
