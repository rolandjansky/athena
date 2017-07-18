#include "JetRecTools/PuppiWeightTool.h"

#include "fastjet/PseudoJet.hh"
#include <vector>

#include "xAODCore/ShallowCopy.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODCore/ShallowAuxContainer.h"

using namespace std;
using namespace fastjet;

//------------------------------------------------------------------------------
PuppiWeightTool::PuppiWeightTool(const std::string& name) : JetConstituentModifierBase(name) {
#ifdef ASG_TOOL_ATHENA
  declareInterface<IJetConstituentModifier>(this);
#endif

  declareProperty("R0", m_R0 = 0.3);
  declareProperty("Rmin", m_Rmin = 0.001);
  declareProperty("Beta", m_beta = 1);
  declareProperty("CentralPTCutOffset", m_centralPTCutOffset = 0);
  declareProperty("CentralPTCutSlope", m_centralPTCutSlope = 0);
  declareProperty("ForwardPTCutOffset", m_forwardPTCutOffset = 0);
  declareProperty("ForwardPTCutSlope", m_forwardPTCutSlope = 0);
  declareProperty("EtaBoundary",m_etaBoundary = 2.5);

  declareProperty("ApplyWeight",m_applyWeight=true);
  declareProperty("PUPenalty", m_PUPenalty=false);
  declareProperty("IncludeCentralNeutralsInAlpha",m_includeCentralNeutralsInAlpha=false);
  declareProperty("IncludeLowPTTracks",m_includeLowPTTracks=false);
  
  m_puppi = new Puppi(m_R0, m_Rmin, m_beta, m_centralPTCutOffset, m_centralPTCutSlope, m_forwardPTCutOffset, m_forwardPTCutSlope, m_etaBoundary, m_PUPenalty);
}

//------------------------------------------------------------------------------

StatusCode PuppiWeightTool::process(xAOD::IParticleContainer* cont) const {
  xAOD::PFOContainer* pfoCont = dynamic_cast<xAOD::PFOContainer*> (cont);
  if(pfoCont) return process(pfoCont);
  else{
    ATH_MSG_ERROR("Unable to dynamic cast IParticleContainer to PFOContainer");
    return StatusCode::FAILURE;
  }
}

//------------------------------------------------------------------------------

StatusCode PuppiWeightTool::process(xAOD::PFOContainer* cont) const{

  const static SG::AuxElement::Accessor<bool> PVMatchedAcc("matchedToPV");
  const static SG::AuxElement::Accessor<double> alphaAcc("PUPPI_alpha");
  const static SG::AuxElement::Accessor<double> weightAcc("PUPPI_weight");

  std::vector<fastjet::PseudoJet> chargedHSVector;
  std::vector<fastjet::PseudoJet> chargedPUVector;
  std::vector<fastjet::PseudoJet> neutralVector;
  std::vector<fastjet::PseudoJet> forwardVector;

  // Fill input particle vectors for puppi
  for ( xAOD::PFO* ppfo : *cont ) {
    if (!PVMatchedAcc.isAvailable(*ppfo)){
      ATH_MSG_ERROR("Not known if PFO is matched to primary vertex.  Run CorrectPFOTool before ChargedHadronSubtractionTool");
      return StatusCode::FAILURE;
    }

    if (ppfo->pt()<=FLT_MIN) continue;

    fastjet::PseudoJet pj(ppfo->p4());
    //pj.set_user_info(new PuppiUserInfo({someOtherChi2,yetAnotherChi2}));  //example of how additional information could be exploited - needs to be calculated somewhere above

    float charge = ppfo->charge();
    bool isCharged = (fabs(charge) > FLT_MIN);

    if(fabs(ppfo->eta()) > m_etaBoundary) forwardVector.push_back(pj);
    else{     
      if(isCharged){
        bool matchedToPrimaryVertex=PVMatchedAcc(*ppfo);
	if(matchedToPrimaryVertex) chargedHSVector.push_back(pj);
	else chargedPUVector.push_back(pj);
      }
      else neutralVector.push_back(pj);
    }
    
    //Test inclusion of low pT tracks
    if (m_includeLowPTTracks){
      ATH_MSG_WARNING("You are doing something experimental");
      const xAOD::Vertex* vtx = nullptr; 

      const xAOD::VertexContainer* pvtxs = 0;
      ATH_CHECK(evtStore()->retrieve(pvtxs, "PrimaryVertices"));
      if ( pvtxs == 0 || pvtxs->size()==0 ) {
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

      const xAOD::TrackParticleContainer *tracks=0;
      evtStore()->retrieve(tracks, "InDetTrackParticles");
      for (auto track: *tracks){

	bool matchedToPrimaryVertex = false;

	//vtz.z() provides z of that vertex w.r.t the center of the beamspot (z = 0). Thus we corrext the track z0 to be w.r.t z = 0
	float z0 = track->z0() + track->vz() - vtx->z();
	float theta = track->theta();
	if ( fabs(z0*sin(theta)) < 2.0 ) {
	  matchedToPrimaryVertex = true;
	}
	if( track->pt()<800){  //Not sure what the threshold should be
	  if(matchedToPrimaryVertex) chargedHSVector.push_back( fastjet::PseudoJet( track->p4() ));
	  else chargedPUVector.push_back( fastjet::PseudoJet( track->p4() ));
	}
      }
    }
  }

  const xAOD::VertexContainer* pvtxs = 0;
  ATH_CHECK(evtStore()->retrieve(pvtxs, "PrimaryVertices"));
  if ( pvtxs == 0 || pvtxs->size()==0 ) {
    ATH_MSG_WARNING(" This event has no primary vertices " );
    return StatusCode::FAILURE;
  }

  int nPV=0;
  for( auto vtx_itr : *pvtxs ){
    if((int)vtx_itr->nTrackParticles() < 2 ) continue;
    ++nPV;
  }

  m_puppi->setParticles(chargedHSVector, chargedPUVector, neutralVector, forwardVector, nPV);

  for ( xAOD::PFO* ppfo : *cont ) {
    float charge = ppfo->charge();
    bool isCharged = (fabs(charge) > FLT_MIN);
    bool isForward = (fabs(ppfo->eta()) > m_etaBoundary);

    fastjet::PseudoJet pj(ppfo->p4());

    double weight = m_puppi->getWeight(pj);
    double alpha = m_puppi->getAlpha(pj);

    if ((!isCharged || isForward) && m_applyWeight) ppfo->setP4(weight*ppfo->p4());
    alphaAcc(*ppfo) = alpha;
    weightAcc(*ppfo) = weight;
  }

  ATH_MSG_DEBUG("Median: "<<m_puppi->getMedian());
  ATH_MSG_DEBUG("RMS: "<<m_puppi->getRMS());

  return StatusCode::SUCCESS;
}
