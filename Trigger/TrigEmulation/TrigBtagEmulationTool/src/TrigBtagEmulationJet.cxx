/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration 
*/

#include "src/TrigBtagEmulationJet.h"
#include "xAODBTagging/BTaggingUtilities.h"

//**********************************************************************

namespace Trig {

TrigBtagEmulationJet::TrigBtagEmulationJet(const xAOD::Jet& jet,
					   const std::string& btagLink)
  :  m_jet(&jet),
     m_btag(xAOD::BTaggingUtilities::getBTagging(jet, btagLink)),
     m_pt(jet.pt()),
     m_eta(jet.eta()),
     m_phi(jet.phi()),
     m_p4(jet.p4())
{}

bool TrigBtagEmulationJet::satisfy(const std::string& tagger_name,
				   double workingPoint) const 
{
  if (not m_btag) return false;
  if (tagger_name == "offperf") return true;

  double pu = -1;
  double pb = -1;
  double pc = -1;

  m_btag->pu(tagger_name, pu); 
  m_btag->pb(tagger_name, pb);
  m_btag->pc(tagger_name, pc);

  double tagger_weight = dl1r_weight(pu, pb, pc);
  return tagger_weight > workingPoint;
}
 
}

//**********************************************************************

