/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRecTools/TCCPseudoJetGetter.h"

TCCPseudoJetGetter::TCCPseudoJetGetter(const std::string &name) : PseudoJetGetter(name)  {
}

int TCCPseudoJetGetter::appendTo(PseudoJetVector& psjs, const LabelIndex* pli) const { 
  // temporary re-copy from PseudoJetGetter as long as tracks can not be retrieved as IParticle
  if ( evtStore()->contains<xAOD::TrackCaloClusterContainer>(m_incoll) ) {
    ATH_MSG_DEBUG("Retrieving xAOD container " << m_incoll << ", ghost scale="
                  << m_ghostscale  <<  ", isGhost=" << bool(m_ghostscale));
    const xAOD::TrackCaloClusterContainer* ppars = 0;
    ppars = evtStore()->retrieve<const xAOD::TrackCaloClusterContainer>(m_incoll);
    if ( ppars != 0 ) return append(*ppars, psjs, pli);
  }
  return 0;

  //return PseudoJetGetter::appendTo(psjs,pli);
}

jet::IConstituentUserInfo* TCCPseudoJetGetter::buildCUI(const xAOD::IParticle* p, jet::IConstituentUserInfo::Index id, const LabelIndex* labelMap) const {
  return new jet::IndexedConstituentUserInfo(*p, id, labelMap);
}

