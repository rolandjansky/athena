/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetMomentTools/JetMuonSegmentMomentsTool.h"

JetMuonSegmentMomentsTool::JetMuonSegmentMomentsTool(const std::string& name)
    : JetModifierBase(name)
    , m_assocMuonSegName("GhostMuonSegment")
{
    declareProperty("AssociatedMuonSegments", m_assocMuonSegName);
}

int JetMuonSegmentMomentsTool::modifyJet(xAOD::Jet& jet) const {
  // Get muon segments associated to the jet
  // Note that there are often no segments
  std::vector<const xAOD::IParticle*> segments;
  bool havesegs = jet.getAssociatedObjects(m_assocMuonSegName, segments);

  if ( ! havesegs ) {
    ATH_MSG_ERROR("Muon segments not found.");
    return 1;
  } else {
    ATH_MSG_DEBUG("Jet has " << segments.size() << " muon segments.");
  }

  return 0;
}


