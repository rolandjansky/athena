// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TAUANALYSISTOOLS_DITAUTRUTHMATCHINGTOOL_H
#define  TAUANALYSISTOOLS_DITAUTRUTHMATCHINGTOOL_H

/*
  author: David Kirchmeier
  mail: david.kirchmeier@cern.ch
  implementation close to TauTruthMatchingTool.h
  documentation in: 
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/tags/TauAnalysisTools-<tag>/doc/README-DiTauTruthMatchingTool.rst
        or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/doc/README-DiTauTruthMatchingTool.rst
*/

// Local include(s):
#include "TauAnalysisTools/IDiTauTruthMatchingTool.h"
#include "tauRecTools/BuildTruthTaus.h"

namespace TauAnalysisTools
{

class DiTauTruthMatchingTool
  : public virtual tauRecTools::BuildTruthTaus
  , public virtual TauAnalysisTools::IDiTauTruthMatchingTool
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS2( DiTauTruthMatchingTool,
                   tauRecTools::IBuildTruthTaus,
                   TauAnalysisTools::IDiTauTruthMatchingTool )

public:                         // Interface functions

  DiTauTruthMatchingTool( const std::string& name );

  virtual ~DiTauTruthMatchingTool();

  // initialize the tool
  virtual StatusCode initialize();

  // get pointer to the truth matched particle, if no truth particle was found a null pointer is returned
  virtual void getTruth(const xAOD::DiTauJet& xDiTau);

  // apply match to a single tau
  virtual void applyTruthMatch(const xAOD::DiTauJet& xDiTau);

  // apply match to all taus in a vector
  virtual void applyTruthMatch(const std::vector<const xAOD::DiTauJet*>& vDiTaus);

private:                        // private helper functions

  StatusCode findTruthTau(const xAOD::DiTauJet& xDiTau);
  StatusCode checkTruthMatch (const xAOD::DiTauJet& xDiTau, const xAOD::TruthParticleContainer& xTauContainer) const;
  StatusCode truthMatch (const TLorentzVector& vSubjetTLV,
                         const xAOD::TruthParticleContainer& xTruthTauContainer, 
                         const xAOD::TruthParticle* &xTruthMatch,
                         TruthMatchedParticleType &eTruthMatchedParticleType) const;

private:                        // steering variables

  double m_dMaxDeltaR;

private:                        // private helper variables

  SG::AuxElement::ConstAccessor<double> m_accPtVis;
  SG::AuxElement::ConstAccessor<double> m_accEtaVis;
  SG::AuxElement::ConstAccessor<double> m_accPhiVis;
  SG::AuxElement::ConstAccessor<double> m_accMVis;

}; // class DiTauTruthMatchingTool

}
#endif // TAUANALYSISTOOLS_DITAUTRUTHMATCHINGTOOL_H
