/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PFlowPseudoJetGetter.cxx

#include "JetRecTools/PFlowPseudoJetGetter.h"
#include "JetEDM/TrackVertexAssociation.h"
#include "xAODTracking/VertexContainer.h" 
#include <cmath>

namespace PFlowPJHelper{

  // We can not use the PseudoJetGetter::append method for PFlow object because the neutral component
  // needs to be corrected.
  // We temporary use this helper object to reproduce the filling code in PseudoJetGetter::append .
  // PseudoJetGetter::append can be re-worked and the duplication in this helper class removed
  class PseudoJetFiller {
  public:
    PseudoJetFiller(jet::PseudoJetVector& psjs, const jet::LabelIndex* pli,  const std::string &label,
                    double g, bool skipNegE=true)
    : m_pjVector(psjs), m_labelMap(pli), m_ghostscale(g>0 ? g : 1), m_skipNegE(skipNegE) {
       m_labidx = 0;
       if ( pli != 0 ) m_labidx = m_labelMap->index(label);
       if ( m_ghostscale!=1 ) m_labidx = -m_labidx;
    }


    int fill(const xAOD::PFO* pfo, const TLorentzVector& p4) const {
      //This check MUST be on pfo, not p4 - p4 cannot return -ve energies, pfo can.
      if((m_skipNegE) && (pfo->e()<-1*FLT_MIN) ) return 0;
      if(std::isnan(p4.E()) ) return 0;

      fastjet::PseudoJet psj(p4);
      psj*=m_ghostscale;

      if(m_labelMap){ 
        jet::IConstituentUserInfo* pcui = m_getter->buildCUI(pfo, m_labidx, m_labelMap);
        psj.set_user_info(pcui);
      }
      m_pjVector.push_back(psj);
      return 1;
    }

    const PFlowPseudoJetGetter* m_getter;
    jet::PseudoJetVector &m_pjVector;
    const jet::LabelIndex* m_labelMap;
    jet::IConstituentUserInfo::Index m_labidx;

    const double m_ghostscale;
    const bool m_skipNegE;
  };
}

PFlowPseudoJetGetter::PFlowPseudoJetGetter(const std::string &name)
  : PseudoJetGetter(name), m_retrievePFOTool("RetrievePFOTool"), m_weightPFOTool("WeightPFOTool"), m_trkVtxAssocName("JetTrackVtxAssoc")  {
  declareProperty("RetrievePFOTool", m_retrievePFOTool,  "Name of tool that builds the PFO collection.");
  declareProperty("WeightPFOTool",   m_weightPFOTool,    "Name of tool that extracts the cPFO weights.");
  declareProperty("InputIsEM",       m_inputIsEM =false, "True if neutral PFOs are EM scale clusters.");
  declareProperty("CalibratePFO",    m_calibrate =true,  "True if LC calibration should be applied to EM PFOs.");
  declareProperty("UseNeutral",      m_useneutral =true, "True to use the neutral component of PFlow.");
  declareProperty("UseCharged",      m_usecharged =true, "True if use the charged component of PFlow.");
  declareProperty("UseVertices", m_usevertices = true, "True if we make use of the primary vertex information.");
  declareProperty("UseChargedWeights",m_useChargedWeights = true, "True if we make use of weighting scheme for charged PFO");
  declareProperty("UseTrackToVertexTool", m_useTrackToVertexTool=false, "True if we will use the track to vertex tool");
  declareProperty("TrackVertexAssociation", m_trkVtxAssocName, "SG key for the TrackVertexAssociation object");
  declareProperty("ApplyChargedHadronSubtraction",m_applyChargedHadronSubtraction=true, "True to remove charged hadrons not matched to the PV");
}

int PFlowPseudoJetGetter::appendTo(PseudoJetVector& psjs, const LabelIndex* pli) const { 
  PFlowPJHelper::PseudoJetFiller filler(psjs, pli,  m_label,m_ghostscale, m_skipNegativeEnergy);
  filler.m_getter = this;

  // Get the vertex.
  const xAOD::VertexContainer* pvtxs = 0;
  const xAOD::Vertex* vtx = nullptr;
  if (m_usevertices){
    ATH_CHECK(evtStore()->retrieve(pvtxs, "PrimaryVertices"));
    if ( pvtxs == 0 || pvtxs->size()==0 ) {
      ATH_MSG_WARNING(" This event has no primary vertices " );
      return 1;
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
	return 1;
      }
    }
  }

  // Get the neutral pflow.
  if ( m_useneutral ) {
    CP::PFO_JetMETConfig_inputScale inscale = m_inputIsEM ? CP::EM : CP::LC;
    const xAOD::PFOContainer* pnpfs = m_retrievePFOTool->retrievePFO(inscale, CP::neutral);
    if ( pnpfs == 0 ) {
      std::string sscale = m_inputIsEM ? "EM" : "LC";
      ATH_MSG_WARNING(sscale << " neutral PFOs not found.");
      return 1;
    }
    for ( const xAOD::PFO* ppfo : *pnpfs ) {
      if ( ppfo == 0 ) {
        ATH_MSG_WARNING("Have NULL pointer to neutral PFO");
        continue;
      }
      if ( !m_inputIsEM || m_calibrate ) {
	if (m_usevertices) filler.fill(ppfo, ppfo->GetVertexCorrectedFourVec(*vtx));
        else filler.fill(ppfo, ppfo->p4());
      } else { 
	if (m_usevertices) filler.fill(ppfo, ppfo->GetVertexCorrectedEMFourVec(*vtx));
        else filler.fill(ppfo, ppfo->p4EM());
      }
    }
    delete pnpfs;
  }

  // Get the charged pflow.
  if ( m_usecharged ) {
    const xAOD::PFOContainer* pcpfs = m_retrievePFOTool->retrievePFO(CP::EM,CP::charged);
    TLorentzVector cpfo_p4;
    for ( const xAOD::PFO* pcpf : *pcpfs ) {
      if ( pcpf == 0 ) {
        ATH_MSG_WARNING("Have NULL pointer to charged PFO");
        continue;
      }
      const xAOD::TrackParticle* ptrk = pcpf->track(0);
      if ( ptrk == 0 ) {
        ATH_MSG_WARNING("Skipping charged PFO with null track pointer.");
        continue;
      }
      cpfo_p4.Clear();

      bool matchedToPrimaryVertex = false;
      if (true == m_useTrackToVertexTool && true == m_usevertices){
	const jet::TrackVertexAssociation* trkVtxAssoc = nullptr;

	StatusCode sc = evtStore()->retrieve(trkVtxAssoc, m_trkVtxAssocName);
	if(sc.isFailure() || nullptr == trkVtxAssoc){ ATH_MSG_ERROR("Can't retrieve TrackVertexAssociation : "<< m_trkVtxAssocName); return 1;}
	
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

      if ( true == matchedToPrimaryVertex || !m_applyChargedHadronSubtraction){
	if (true == m_useChargedWeights) {
	  float weight = 0.0;
	  ATH_CHECK( m_weightPFOTool->fillWeight( *pcpf, weight ) );
	  //if (weight>FLT_MIN){ // check against float precision
	  ATH_MSG_VERBOSE("Fill pseudojet for CPFO with weighted pt: " << pcpf->pt()*weight);
	  filler.fill(pcpf, pcpf->p4()*weight);
	  //} else {
	  //  ATH_MSG_VERBOSE("CPFO had a weight of 0, do not fill.");
	  //} // received a weight
	}//if should use charged PFO weighting scheme
	else  filler.fill(pcpf, pcpf->p4());
      }
    }
    delete pcpfs;
  }
  
  return 0;
}

