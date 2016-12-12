/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLEJETDELTARLABELTOOL_H
#define PARTICLEJETDELTARLABELTOOL_H

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetModifier.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"


class ParticleJetDeltaRLabelTool : public asg::AsgTool, public IJetModifier {
ASG_TOOL_CLASS(ParticleJetDeltaRLabelTool, IJetModifier)
public:

  /// Constructor
  ParticleJetDeltaRLabelTool(const std::string& name);

  int modify(xAOD::JetContainer& jets) const;

  std::vector<std::vector<const xAOD::TruthParticle*> > match(
          const xAOD::TruthParticleContainer& parts,
          const xAOD::JetContainer& jets) const;


protected:

  /// Name of jet label attributes
  std::string m_labelname;
  std::string m_doublelabelname;
  std::string m_taulabelname;
  std::string m_bottomlabelname;
  std::string m_charmlabelname;

  /// Name of particle collection for labeling
  std::string m_taupartcollection;
  std::string m_bottompartcollection;
  std::string m_charmpartcollection;

  /// Minimum pT for particle selection (in MeV)
  double m_partptmin;

  /// Minimum pT for jet selection (in MeV)
  double m_jetptmin;

  /// Maximum dR for matching criterion
  double m_drmax;

  /// Matching mode: can be MinDR or MaxPt
  std::string m_matchmode;
};


#endif
