/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleJetTools/ParticleJetGhostLabelTool.h"
#include "ParticleJetTools/ParticleJetLabelCommon.h"
#include "xAODJet/JetContainer.h"
#include "AsgTools/Check.h"

using namespace std;
using namespace xAOD;

ParticleJetGhostLabelTool::ParticleJetGhostLabelTool(const std::string& name)
        : AsgTool(name) {
    declareProperty("LabelName", m_labelnames.singleint="HadronGhostTruthLabelID", "Name of the jet label attribute to be added.");
    declareProperty("DoubleLabelName", m_labelnames.doubleint="HadronGhostExtendedTruthLabelID", "Name of the jet label attribute to be added (with the possibility of up to 2 matched hadrons).");
    declareProperty("GhostBName", m_ghostbname="GhostBHadronsFinal", "Name of attribute for matched B hadrons.");
    declareProperty("GhostCName", m_ghostcname="GhostCHadronsFinal", "Name of attribute for matched C hadrons.");
    declareProperty("GhostTauName", m_ghosttauname="GhostTausFinal", "Name of attribute for matched Taus.");
    declareProperty("PartPtMin", m_partptmin=5000, "Minimum pT of particles for labeling (MeV)");
}


StatusCode ParticleJetGhostLabelTool::modify(JetContainer& jets) const {

  using namespace std;
  using namespace xAOD;

  ATH_MSG_VERBOSE("In " << name() << "::modify()");

  for (xAOD::Jet* jetptr: jets) {

    Jet& jet = *jetptr;
    vector<const TruthParticle*> jetlabelpartsb = match(jet, m_ghostbname);
    vector<const TruthParticle*> jetlabelpartsc = match(jet, m_ghostcname);
    vector<const TruthParticle*> jetlabelpartstau = match(jet, m_ghosttauname);

    // remove children whose parent hadrons are also in the jet.
    // don't care about double tau jets
    // so leave them for now.

    using ParticleJetTools::childrenRemoved;
    childrenRemoved(jetlabelpartsb, jetlabelpartsb);
    childrenRemoved(jetlabelpartsb, jetlabelpartsc);
    childrenRemoved(jetlabelpartsc, jetlabelpartsc);

    // set truth label for jets above pt threshold
    // hierarchy: b > c > tau > light
    ParticleJetTools::PartonCounts counts;
    counts.b = jetlabelpartsb.size();
    counts.c = jetlabelpartsc.size();
    counts.tau = jetlabelpartstau.size();
    ParticleJetTools::setJetLabels(jet, counts, m_labelnames);
  }

  return StatusCode::SUCCESS;
}


std::vector<const TruthParticle*>
ParticleJetGhostLabelTool::match(
  const xAOD::Jet& jet, const std::string& ghostname) const {

  ATH_MSG_VERBOSE("In " << name() << "::match()");

  std::vector<const xAOD::TruthParticle*> parton_links
    = jet.getAssociatedObjects<const xAOD::TruthParticle>(ghostname);

  std::vector<const xAOD::TruthParticle*> selected_partons;
  for (const xAOD::TruthParticle* part: parton_links) {
    if (part->pt() > m_partptmin) {
      selected_partons.push_back(part);
    }
  }
  return selected_partons;
}

