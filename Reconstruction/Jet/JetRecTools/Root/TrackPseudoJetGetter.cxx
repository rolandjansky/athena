/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRecTools/TrackPseudoJetGetter.h"
#include "JetEDM/VertexIndexedConstituentUserInfo.h"

#include "JetEDM/TrackVertexAssociation.h"

TrackPseudoJetGetter::TrackPseudoJetGetter(const std::string &name) : PseudoJetGetter(name) , m_trkVtxAssocName("JetTrackVtxAssoc") {
  declareProperty("TrackVertexAssociation", m_trkVtxAssocName, "SG key for the TrackVertexAssociation object");
}

int TrackPseudoJetGetter::appendTo(PseudoJetVector& psjs, const LabelIndex* pli) const { 
  StatusCode sc = evtStore()->retrieve(m_trkVtxAssoc, m_trkVtxAssocName);
  if(sc.isFailure()){ ATH_MSG_ERROR("Can't retrieve TrackVertexAssociation : "<< m_trkVtxAssocName); return 1;}

  // temporary re-copy from PseudoJetGetter as long as tracks can not be retrieved as IParticle
  if ( evtStore()->contains<xAOD::TrackParticleContainer>(m_incoll) ) {
    ATH_MSG_DEBUG("Retrieving xAOD container " << m_incoll << ", ghost scale="
                  << m_ghostscale  <<  ", isGhost=" << bool(m_ghostscale));
    const xAOD::TrackParticleContainer* ppars = 0;
    ppars = evtStore()->retrieve<const xAOD::TrackParticleContainer>(m_incoll);
    if ( ppars != 0 ) return append(*ppars, psjs, pli);
  }
  return 0;

  //return PseudoJetGetter::appendTo(psjs,pli);
}

jet::IConstituentUserInfo* TrackPseudoJetGetter::buildCUI(const xAOD::IParticle* p, jet::IConstituentUserInfo::Index id, const LabelIndex* labelMap) const {
  const xAOD::TrackParticle* trk = static_cast<const xAOD::TrackParticle*>(p);
  ATH_MSG_DEBUG( " buildCUI with index "<< id );
  return  new jet::VertexIndexedConstituentUserInfo(*p, id, labelMap, m_trkVtxAssoc->associatedVertex(trk));

}

