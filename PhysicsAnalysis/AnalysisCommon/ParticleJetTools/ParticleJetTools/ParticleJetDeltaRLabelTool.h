/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLEJETDELTARLABELTOOL_H
#define PARTICLEJETDELTARLABELTOOL_H

#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetModifier.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"

#include "ParticleJetTools/ParticleJetLabelCommon.h"

class ParticleJetDeltaRLabelTool : public asg::AsgTool, public IJetModifier {
ASG_TOOL_CLASS(ParticleJetDeltaRLabelTool, IJetModifier)
public:

  /// Constructor
  ParticleJetDeltaRLabelTool(const std::string& name);

  StatusCode initialize();

  StatusCode modify(xAOD::JetContainer& jets) const;

  std::vector<std::vector<const xAOD::TruthParticle*> > match(
          const xAOD::TruthParticleContainer& parts,
          const xAOD::JetContainer& jets) const;


protected:

  /// Name of jet label attributes
  ParticleJetTools::LabelNames m_labelnames;
  std::string m_taulabelname;
  std::string m_bottomlabelname;
  std::string m_charmlabelname;

  /// Name of particle collection for labeling
  std::string m_taupartcollection;
  std::string m_bottompartcollection;
  std::string m_charmpartcollection;

  /// Read handles particle collections for labeling
  SG::ReadHandleKey<xAOD::TruthParticleContainer> m_tauPartCollectionKey{this,"TauParticleCollection","","ReadHandleKey for tauPartCollection"};
  SG::ReadHandleKey<xAOD::TruthParticleContainer> m_bottomPartCollectionKey{this,"BParticleCollection","","ReadHandleKey for bottomPartCollection"};
  SG::ReadHandleKey<xAOD::TruthParticleContainer> m_charmPartCollectionKey{this,"CParticleCollection","","ReadHandleKey for charmPartCollection"};

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
