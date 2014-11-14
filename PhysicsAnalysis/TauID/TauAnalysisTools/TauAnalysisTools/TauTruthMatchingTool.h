// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TAUTRUTHMATCHINGTOOL_H
#define  TAUTRUTHMATCHINGTOOL_H

/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
  documentation in: ../README.rst
                    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/tags/TauAnalysisTools-<tag>/README.rst
		    or
                    https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TauID/TauAnalysisTools/trunk/README.rst
*/

// Framework include(s):
#include "AsgTools/AsgTool.h"

// Core include(s):
#include "AthLinks/ElementLink.h"

#ifndef __CINT__
// EDM include(s):
#include "xAODTau/TauJet.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTruth/TruthVertex.h"
#endif // not __MAKECINT__

// Local include(s):
#include "TauAnalysisTools/ITauTruthMatchingTool.h"

namespace TauAnalysisTools
{

enum eSampleType
{
  OTHER            = 0,
  PYTHIA           = 1,
  SHERPA           = 2,
};


class TauTruthMatchingTool
  : public TauAnalysisTools::ITauTruthMatchingTool
  , public asg::AsgTool
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS( TauTruthMatchingTool,
                  TauAnalysisTools::ITauTruthMatchingTool )

public:

  TauTruthMatchingTool( const std::string& name );

  virtual ~TauTruthMatchingTool();

  // initialize the tool
  virtual StatusCode initialize();

#ifndef __MAKECINT__
  // create TruthTauContainer
  virtual StatusCode createTruthTauContainer();

  // get TruthTauContainer
  virtual xAOD::TruthParticleContainer* getTruthTauContainer()
  {
    return m_xTruthTauContainer;
  };

  // get TruthTauAuxContainer
  virtual xAOD::TruthParticleAuxContainer* getTruthTauAuxContainer()
  {
    return m_xTruthTauAuxContainer;
  };

  // apply match to a single tau
  virtual const xAOD::TruthParticle* applyTruthMatch(const xAOD::TauJet& xTau);

  // apply match to all taus in a vector
  virtual std::vector<const xAOD::TruthParticle*> applyTruthMatch(const std::vector<const xAOD::TauJet*>& vTaus);
#endif // not __MAKECINT__

  // set pointer to truth particle container
  virtual StatusCode setTruthParticleContainer(const xAOD::TruthParticleContainer* xTruthParticleContainer);

private:
  void examineTruthTau (const xAOD::TruthParticle& xTruthParticle) const;
  bool isGoodDaughter (int iStatus,
                       int iBarcode,
                       int iVtxBarcode) const;
  const xAOD::TruthParticle* checkTruthMatch (const xAOD::TauJet& xTau) const;
  void findTruthTaus();

#ifndef __MAKECINT__
  const xAOD::TruthParticleContainer* m_xTruthParticleContainer;
  const xAOD::TruthParticleAuxContainer* m_xTruthParticleAuxContainer;
  xAOD::TruthParticleContainer* m_xTruthTauContainer;
  xAOD::TruthParticleAuxContainer* m_xTruthTauAuxContainer;
  std::vector<const xAOD::TruthParticle* > m_vTruthTaus;
#endif // not __MAKECINT__
  std::vector<size_t > m_vTruthTausProng;

  int m_iSampleType;
  double m_dMaxDeltaR;

}; // class TauTruthMatchingTool

}
#endif // TAUTRUTHMATCHINGTOOL_H
