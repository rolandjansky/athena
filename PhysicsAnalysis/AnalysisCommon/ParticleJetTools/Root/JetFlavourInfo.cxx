/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleJetTools/JetFlavourInfo.h"

#include <vector>
#include <string>

using xAOD::IParticle;
using xAOD::JetFlavourLabelType;

int
xAOD::GAFinalHadronFlavourLabel (const xAOD::Jet* jet) {

  const std::string labelB = "GhostBHadronsFinal";
  const std::string labelC = "GhostCHadronsFinal";
  const std::string labelTau = "GhostTausFinal";

  std::vector<const IParticle*> ghostB;
  if (jet->getAssociatedObjects<IParticle>(labelB, ghostB) && ghostB.size() > 0) return 5;
  std::vector<const IParticle*> ghostC;
  if (jet->getAssociatedObjects<IParticle>(labelC, ghostC) && ghostC.size() > 0) return 4;
  std::vector<const IParticle*> ghostTau;
  if (jet->getAssociatedObjects<IParticle>(labelTau, ghostTau) && ghostTau.size() > 0) return 15;
  return 0;
}

int
xAOD::GAInitialHadronFlavourLabel (const xAOD::Jet* jet) {

  const std::string labelB = "GhostBHadronsInitial";
  const std::string labelC = "GhostCHadronsInitial";
  const std::string labelTau = "GhostTausFinal";

  std::vector<const IParticle*> ghostB;
  if (jet->getAssociatedObjects<IParticle>(labelB, ghostB) && ghostB.size() > 0) return 5;
  std::vector<const IParticle*> ghostC;
  if (jet->getAssociatedObjects<IParticle>(labelC, ghostC) && ghostC.size() > 0) return 4;
  std::vector<const IParticle*> ghostTau;
  if (jet->getAssociatedObjects<IParticle>(labelTau, ghostTau) && ghostTau.size() > 0) return 15;
  return 0;
}

int
xAOD::GAFinalPartonFlavourLabel (const xAOD::Jet* jet) {

  const std::string labelB = "GhostBQuarksFinal";
  const std::string labelC = "GhostCQuarksFinal";
  const std::string labelTau = "GhostTausFinal";

  std::vector<const IParticle*> ghostB;
  if (jet->getAssociatedObjects<IParticle>(labelB, ghostB) && ghostB.size() > 0) return 5;
  std::vector<const IParticle*> ghostC;
  if (jet->getAssociatedObjects<IParticle>(labelC, ghostC) && ghostC.size() > 0) return 4;
  std::vector<const IParticle*> ghostTau;
  if (jet->getAssociatedObjects<IParticle>(labelTau, ghostTau) && ghostTau.size() > 0) return 15;
  return 0;
}

int
xAOD::ConeFinalPartonFlavourLabel (const xAOD::Jet* jet) {
  // default label means "invalid"
  int label = -1;

  // First try the new naming scheme
  if (jet->getAttribute("ConeTruthLabelID",label)) return label;
  // If that fails, revert to the old scheme. In this case, further testing is not very useful
  jet->getAttribute("TruthLabelID", label);
  return label;
}

int
xAOD::ExclusiveConeHadronFlavourLabel (const xAOD::Jet* jet) {
  // default label means "invalid"
  int label = -1;

  // We don't check the return value, as we would not be able to handle it gracefully anyway
  jet->getAttribute("HadronConeExclTruthLabelID",label);
  return label;
}

int
xAOD::ExclusiveConeDoubleHadronFlavourLabel (const xAOD::Jet* jet) {
  // default label means "invalid"
  int label = -1;

  // We don't check the return value, as we would not be able to handle it gracefully anyway
  jet->getAttribute("HadronConeExclExtendedTruthLabelID",label);
  return label;
}

int xAOD::jetFlavourLabel (const xAOD::Jet* jet, JetFlavourLabelType t) {

  switch (t) {
  case GAFinalHadron:   return GAFinalHadronFlavourLabel (jet);   break;
  case GAInitialHadron: return GAInitialHadronFlavourLabel (jet); break;
  case GAFinalParton:   return GAFinalPartonFlavourLabel (jet);   break;
  case ExclConeHadron:  return ExclusiveConeHadronFlavourLabel (jet);
  case ConeFinalParton: default: return ConeFinalPartonFlavourLabel (jet);
  }
}
