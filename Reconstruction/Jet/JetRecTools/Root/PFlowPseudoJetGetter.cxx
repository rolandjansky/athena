/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PFlowPseudoJetGetter.cxx

#include "JetRecTools/PFlowPseudoJetGetter.h"
#include "xAODTracking/VertexContainer.h" 
#include <cmath>

#include "xAODPFlow/PFO.h"

namespace PFlowPJHelper{

  // We can not use the PseudoJetGetter::append method for PFlow object because the neutral component
  // needs to be corrected.
  // We temporary use this helper object to reproduce the filling code in PseudoJetGetter::append .
  // PseudoJetGetter::append can be re-worked and the duplication in this helper class removed
  struct PseudoJetFiller {
    PseudoJetFiller(jet::PseudoJetVector& psjs, const jet::LabelIndex* pli,  const std::string &label,
                    double g, bool skipNegE=true)
    : m_pjVector(psjs), m_labelMap(pli), m_ghostscale(g>0 ? g : 1), m_skipNegE(skipNegE) {
       m_labidx = 0;
       if ( pli != 0 ) m_labidx = m_labelMap->index(label);
       if ( m_ghostscale!=1 ) m_labidx = -m_labidx;
    }


    int fill(const xAOD::PFO* pfo, const TLorentzVector& p4) const {
      if((m_skipNegE) && (p4.E()<=0) ) return 0;
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
: PseudoJetGetter(name), m_retrievePFOTool("RetrievePFOTool",this) {
  declareProperty("RetrievePFOTool", m_retrievePFOTool,  "Name of tool that builds the PFO collection.");
  declareProperty("InputIsEM",       m_inputIsEM =false, "True if neutral PFOs are EM scale clusters.");
  declareProperty("CalibratePFO",    m_calibrate =true,  "True if LC calibration should be applied to EM PFOs.");
  declareProperty("UseNeutral",      m_useneutral =true, "True to use the nuetral component of PFlow.");
  declareProperty("UseCharged",      m_usecharged =true, "True if use the charged component of PFlow.");
  declareProperty("UseVertices", m_usevertices = true, "True if we make use of the primary vertex information.");
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
    vtx = pvtxs->at(0);
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
      //vtz.z() provides z of that vertex w.r.t the center of the beamspot (z = 0). Thus we corrext the track z0 to be w.r.t z = 0
      float z0 = ptrk->z0() + ptrk->vz();
      if (m_usevertices) {
        if (vtx) z0 = z0 - vtx->z();
      }
      float theta = ptrk->theta();
      if ( fabs(z0*sin(theta)) < 2.0 ) filler.fill(pcpf, pcpf->p4() );
    }
    delete pcpfs;
  }
  
  return 0;
}

