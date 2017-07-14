// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TAUANALYSISTOOLS_TAUTRUTHMATCHINGTOOL_H
#define  TAUANALYSISTOOLS_TAUTRUTHMATCHINGTOOL_H

/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
  documentation in: ../README.rst
                    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/tags/TauAnalysisTools-<tag>/README.rst
		    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/README.rst
*/

// Local include(s):
#include "TauAnalysisTools/ITauTruthMatchingTool.h"
#include "TauAnalysisTools/BuildTruthTaus.h"

namespace TauAnalysisTools
{

class TauTruthMatchingTool
  : public virtual TauAnalysisTools::BuildTruthTaus
  , public virtual TauAnalysisTools::ITauTruthMatchingTool
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS2( TauTruthMatchingTool,
                   TauAnalysisTools::IBuildTruthTaus,
                   TauAnalysisTools::ITauTruthMatchingTool )

public:                         // Interface functions

  TauTruthMatchingTool( const std::string& name );

  virtual ~TauTruthMatchingTool();

  // initialize the tool
  virtual StatusCode initialize() override final;

  // get pointer to the truth matched particle, if no truth particle was found a null pointer is returned
  virtual const xAOD::TruthParticle* getTruth(const xAOD::TauJet& xTau) override final;

  // apply match to a single tau
  virtual const xAOD::TruthParticle* applyTruthMatch(const xAOD::TauJet& xTau) override final;

  // apply match to all taus in a vector
  virtual std::vector<const xAOD::TruthParticle*> applyTruthMatch(const std::vector<const xAOD::TauJet*>& vTaus) override final;

public:                         // Wrapper functions
  
  // wrapper function to get truth tau visible TLorentzvector
  virtual TLorentzVector getTruthTauP4Vis(const xAOD::TauJet& xTau) override final;
  virtual TLorentzVector getTruthTauP4Vis(const xAOD::TruthParticle& xTruthTau) override final;

  // wrapper function to get truth tau prompt TLorentzvector
  virtual TLorentzVector getTruthTauP4Prompt(const xAOD::TauJet& xTau) override final;
  virtual TLorentzVector getTruthTauP4Prompt(const xAOD::TruthParticle& xTruthTau) override final;

  // wrapper function to get truth tau invisible TLorentzvector
  virtual TLorentzVector getTruthTauP4Invis(const xAOD::TauJet& xTau) override final;
  virtual TLorentzVector getTruthTauP4Invis(const xAOD::TruthParticle& xTruthTau) override final;

  // get type of truth match particle (hadronic tau, leptonic tau, electron, muon, jet)
  virtual TauAnalysisTools::TruthMatchedParticleType getTruthParticleType(const xAOD::TauJet& xTau) override final;

  // wrapper function to count number of decay particles of given pdg id
  virtual int getNTauDecayParticles(const xAOD::TauJet& xTau, int iPdgId, bool bCompareAbsoluteValues = false) override final;
  virtual int getNTauDecayParticles(const xAOD::TruthParticle& xTruthTau, int iPdgId, bool bCompareAbsoluteValues = false) override final;

  // wrapper function to obtain truth verion of xAOD::TauJetParameters::DecayMode
  virtual xAOD::TauJetParameters::DecayMode getDecayMode(const xAOD::TauJet& xTau) override final;
  virtual xAOD::TauJetParameters::DecayMode getDecayMode(const xAOD::TruthParticle& xTruthTau) override final;

private:                        // private helper functions

  StatusCode findTruthTau(const xAOD::TauJet& xTau);
  StatusCode checkTruthMatch (const xAOD::TauJet& xTau, const xAOD::TruthParticleContainer& xTauContainer) const;

private:                        // steering variables

  double m_dMaxDeltaR;

private:                        // private helper variables

  SG::AuxElement::ConstAccessor<double> m_accPtVis;
  SG::AuxElement::ConstAccessor<double> m_accEtaVis;
  SG::AuxElement::ConstAccessor<double> m_accPhiVis;
  SG::AuxElement::ConstAccessor<double> m_accMVis;

}; // class TauTruthMatchingTool

}
#endif // TAUANALYSISTOOLS_TAUTRUTHMATCHINGTOOL_H
