/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CorrectPFOTool.cxx

#include "JetRecTools/CorrectPFOTool.h"
#include "JetEDM/TrackVertexAssociation.h"
#include "xAODTracking/VertexContainer.h" 
#include <cmath>

CorrectPFOTool::CorrectPFOTool(const std::string &name): JetConstituentModifierBase(name), m_weightPFOTool("WeightPFOTool"), m_trkVtxAssocName("JetTrackVtxAssoc") {

#ifdef ASG_TOOL_ATHENA
  declareInterface<IJetConstituentModifier>(this);
#endif

  declareProperty("WeightPFOTool",   m_weightPFOTool,    "Name of tool that extracts the cPFO weights.");
  declareProperty("InputIsEM",       m_inputIsEM =false, "True if neutral PFOs are EM scale clusters.");
  declareProperty("CalibratePFO",    m_calibrate =true,  "True if LC calibration should be applied to EM PFOs.");
  declareProperty("CorrectNeutral",      m_correctneutral =true, "True to use the neutral component of PFlow.");
  declareProperty("CorrectCharged",      m_correctcharged =true, "True if use the charged component of PFlow.");
  declareProperty("UseVertices", m_usevertices = true, "True if we make use of the primary vertex information.");
  declareProperty("UseChargedWeights",m_useChargedWeights = true, "True if we make use of weighting scheme for charged PFO");
  declareProperty("UseTrackToVertexTool", m_useTrackToVertexTool=false, "True if we will use the track to vertex tool");
  declareProperty("TrackVertexAssociation", m_trkVtxAssocName, "SG key for the TrackVertexAssociation object");
}

StatusCode CorrectPFOTool::initialize() {
  if(m_inputType!=xAOD::Type::ParticleFlow) {
    ATH_MSG_ERROR("ChargedHadronSubtractionTool requires PFO inputs. It cannot operate on objects of type "
		  << m_inputType);
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode CorrectPFOTool::process_impl(xAOD::IParticleContainer* cont) const {
  // Type-checking happens in the JetConstituentModifierBase class
  // so it is safe just to static_cast
  xAOD::PFOContainer* pfoCont = static_cast<xAOD::PFOContainer*> (cont);
  return correctPFO(*pfoCont);
}

StatusCode CorrectPFOTool::correctPFO(xAOD::PFOContainer& cont) const { 
  // Get the vertex.
  const xAOD::VertexContainer* pvtxs = nullptr;
  const xAOD::Vertex* vtx = nullptr;
  if (m_usevertices){
    ATH_CHECK(evtStore()->retrieve(pvtxs, "PrimaryVertices"));
    if ( pvtxs == nullptr || pvtxs->size()==0 ) {
      ATH_MSG_WARNING(" This event has no primary vertices " );
      return StatusCode::FAILURE;
    }

    //Usually the 0th vertex is the primary one, but this is not always the case. So we will choose the first vertex of type PriVtx
    for (auto theVertex : *pvtxs) {
      if (xAOD::VxType::PriVtx == theVertex->vertexType() ) {
	vtx = theVertex;
	break;
      }//If we have a vertex of type primary vertex
    }//iterate over the vertices and check their type

    //If there is no primary vertex, then we cannot correct pflow inputs and hence we return (because this should not happen).
    if (nullptr == vtx) {
      ATH_MSG_VERBOSE("Could not find a primary vertex in this event " );
      for (auto theVertex : *pvtxs) {
	if (xAOD::VxType::NoVtx == theVertex->vertexType() ) {
	  vtx = theVertex;
	  break;
	}
      }
      if (nullptr == vtx) {
	ATH_MSG_WARNING("Could not find a NoVtx in this event " );
	return StatusCode::FAILURE;
      }
    }
  }

  SG::AuxElement::Accessor<bool> PVMatchedAcc("matchedToPV");

  for ( xAOD::PFO* ppfo : cont ) {
    if ( ppfo == 0 ) {
      ATH_MSG_WARNING("Have NULL pointer to neutral PFO");
      continue;
    }

    bool matchedToPrimaryVertex = false;

    if ( m_correctneutral && ppfo->charge()==0) {
      if (ppfo->e() <= 0.0) ppfo->setP4(0,0,0,0);   //This is necesarry to avoid changing sign of pT for pT<0 PFO
      else{
        if ( !m_inputIsEM || m_calibrate ) {
          if (m_usevertices) ppfo->setP4(ppfo->GetVertexCorrectedFourVec(*vtx));
        } 
        else { 
          if (m_usevertices) ppfo->setP4(ppfo->GetVertexCorrectedEMFourVec(*vtx));   //This
          else ppfo->setP4(ppfo->p4EM());
        }
      }
    }

    if ( m_correctcharged && ppfo->charge()!=0) {
      const xAOD::TrackParticle* ptrk = ppfo->track(0);
      if ( ptrk == 0 ) {
        ATH_MSG_WARNING("Skipping charged PFO with null track pointer.");
        continue;
      }

      if (true == m_useTrackToVertexTool && true == m_usevertices){
	const jet::TrackVertexAssociation* trkVtxAssoc = nullptr;

	StatusCode sc = evtStore()->retrieve(trkVtxAssoc, m_trkVtxAssocName);
	if(sc.isFailure() || nullptr == trkVtxAssoc){ ATH_MSG_ERROR("Can't retrieve TrackVertexAssociation : "<< m_trkVtxAssocName); return StatusCode::FAILURE;}
	
	const xAOD::Vertex* thisTracksVertex = trkVtxAssoc->associatedVertex(ptrk);
	
	if (xAOD::VxType::PriVtx == thisTracksVertex->vertexType()) matchedToPrimaryVertex = true;
      }
      else{
	//vtz.z() provides z of that vertex w.r.t the center of the beamspot (z = 0). Thus we corrext the track z0 to be w.r.t z = 0
	float z0 = ptrk->z0() + ptrk->vz();
	if (m_usevertices) {
	  if (vtx) z0 = z0 - vtx->z();
	}
	float theta = ptrk->theta();
	if ( fabs(z0*sin(theta)) < 2.0 ) {
	  matchedToPrimaryVertex = true;
	}
      }

      if ( true == matchedToPrimaryVertex || !m_applyCHS){
	if (true == m_useChargedWeights) {
	  float weight = 0.0;
	  ATH_CHECK( m_weightPFOTool->fillWeight( *ppfo, weight ) );
	  //if (weight>FLT_MIN){ // check against float precision
	  ATH_MSG_VERBOSE("Fill pseudojet for CPFO with weighted pt: " << ppfo->pt()*weight);
	  ppfo->setP4(ppfo->p4()*weight);
	  //} else {
	  //  ATH_MSG_VERBOSE("CPFO had a weight of 0, do not fill.");
	  //} // received a weight
	}//if should use charged PFO weighting scheme
      }
      if ( false == matchedToPrimaryVertex && m_applyCHS){
        ppfo->setP4(0,0,0,0);
      }
    }
    PVMatchedAcc(*ppfo) = matchedToPrimaryVertex;
  }
  
  return StatusCode::SUCCESS;
}

