#include "JetRecTools/ChargedHadronSubtractionTool.h"

using namespace std;

ChargedHadronSubtractionTool::ChargedHadronSubtractionTool(const std::string& name) : JetConstituentModifierBase(name)
{

  declareProperty("UseTrackToVertexTool", m_useTrackToVertexTool=false, "True if we will use the track to vertex tool");

  declareProperty("TrackVertexAssociation", 
                  m_trkVtxAssoc_key="JetTrackVtxAssoc",
                  "StoreGate key for the TrackVertexAssociation object");
  declareProperty("VertexContainerKey", 
                  m_vertexContainer_key="PrimaryVertices",
                  "StoreGate key for the primary vertex container");

}

StatusCode ChargedHadronSubtractionTool::initialize() {
  if(m_inputType!=xAOD::Type::ParticleFlow) {
    ATH_MSG_ERROR("ChargedHadronSubtractionTool requires PFO inputs. It cannot operate on objects of type "
		  << m_inputType);
  return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode ChargedHadronSubtractionTool::process_impl(xAOD::IParticleContainer* cont) const {
  // Type-checking happens in the JetConstituentModifierBase class
  // so it is safe just to static_cast
  xAOD::PFOContainer* pfoCont = static_cast<xAOD::PFOContainer*> (cont);
  return matchToPrimaryVertex(*pfoCont);
}

const xAOD::Vertex* ChargedHadronSubtractionTool::getPrimaryVertex() const {
  // Retrieve Primary Vertices
  auto handle = SG::makeHandle(m_vertexContainer_key);
  if (!handle.isValid()){
      ATH_MSG_WARNING(" This event has no primary vertices " );
      return nullptr;
  }
    
  const xAOD::VertexContainer* pvtxs = handle.cptr();
  if(pvtxs->empty()){
      ATH_MSG_WARNING(" This event has no primary vertices " );
      return nullptr;
  } 

  //Usually the 0th vertex is the primary one, but this is not always 
  // the case. So we will choose the first vertex of type PriVtx
  for (auto theVertex : *pvtxs) {
    if (theVertex->vertexType()==xAOD::VxType::PriVtx) {
      return theVertex;
    }//If we have a vertex of type primary vertex
  }//iterate over the vertices and check their type

  // If we failed to find an appropriate vertex, return the dummy vertex
  ATH_MSG_VERBOSE("Could not find a primary vertex in this event " );
  for (auto theVertex : *pvtxs) {
    if (theVertex->vertexType()==xAOD::VxType::NoVtx) {
      return theVertex;
    }
  }

  // If there is no primary vertex, then we cannot do PV matching.
  return nullptr;
}

StatusCode ChargedHadronSubtractionTool::matchToPrimaryVertex(xAOD::PFOContainer& cont) const {
  const static SG::AuxElement::Accessor<char> PVMatchedAcc("matchedToPV");

  // Use only one of TVA or PV
  const jet::TrackVertexAssociation* trkVtxAssoc = nullptr;
  const xAOD::Vertex* vtx = nullptr;
  if(m_useTrackToVertexTool) {
    auto handle = SG::makeHandle(m_trkVtxAssoc_key);
    if(!handle.isValid()){
      ATH_MSG_ERROR("Can't retrieve TrackVertexAssociation : "<< m_trkVtxAssoc_key.key()); 
      return StatusCode::FAILURE;
    }
    trkVtxAssoc = handle.cptr();
  } else {
    vtx = getPrimaryVertex();
    if(vtx==nullptr) {
      ATH_MSG_ERROR("Primary vertex container was empty or no valid vertex found!");
      return StatusCode::FAILURE;
    } else if (vtx->vertexType()==xAOD::VxType::NoVtx) {
      ATH_MSG_VERBOSE("No genuine primary vertex found. Will consider all PFOs matched.");
    }
  }

  for ( xAOD::PFO* ppfo : cont ) {
    // Ignore neutral PFOs
    if(fabs(ppfo->charge()) < FLT_MIN) continue;

    bool matchedToPrimaryVertex = false;
    const xAOD::TrackParticle* ptrk = ppfo->track(0);
    if(ptrk==nullptr) {
      ATH_MSG_WARNING("Charged PFO with index " << ppfo->index() << " has no ID track!");
      continue;
    }
    if(trkVtxAssoc) { // Use TrackVertexAssociation
      const xAOD::Vertex* thisTracksVertex = trkVtxAssoc->associatedVertex(ptrk);
      matchedToPrimaryVertex = (xAOD::VxType::PriVtx == thisTracksVertex->vertexType());
    } else { // Use Primary Vertex
      if(vtx->vertexType()==xAOD::VxType::NoVtx) { // No reconstructed vertices
	matchedToPrimaryVertex = true; // simply match all cPFOs in this case
      } else { // Had a good reconstructed vertex.
	// vtz.z() provides z of that vertex w.r.t the center of the beamspot (z = 0).
	// Thus we correct the track z0 to be w.r.t z = 0
	float z0 = ptrk->z0() + ptrk->vz() - vtx->z();
	float theta = ptrk->theta();
	matchedToPrimaryVertex = ( fabs(z0*sin(theta)) < 2.0 );
      }
    } // TVA vs PV decision
    PVMatchedAcc(*ppfo) = matchedToPrimaryVertex;
  }

  return StatusCode::SUCCESS;
}
