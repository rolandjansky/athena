// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TAUANALYSISTOOLS_TAUTRUTHTRACKMATCHINGTOOL_H
#define  TAUANALYSISTOOLS_TAUTRUTHTRACKMATCHINGTOOL_H

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
#include "AsgTools/AnaToolHandle.h"

// Local include(s)
#include "TauAnalysisTools/ITauTruthTrackMatchingTool.h"

// EDM include(s)
#include "xAODTruth/TruthVertex.h"


namespace TauAnalysisTools
{
enum TrackType_t
{
  UnclassifiedTrack     = 0, // Track is unclassified
  TauTrack              = 1, // Track is from direct tau decay
  ConversionTrack       = 2, // Track is from conversion
  UnderlyingEventTrack  = 3, // Track is from underlying event
  SecondaryTrack        = 4, // Track is a secondary track
  PileupTrack           = 5, // Track is from pileup
  FakeTrack             = 6  // Track is a fake
};


class TauTruthTrackMatchingTool
  : public TauAnalysisTools::ITauTruthTrackMatchingTool
  , public asg::AsgTool
{
  /// Create a proper constructor for Athena
  ASG_TOOL_CLASS( TauTruthTrackMatchingTool,
                  TauAnalysisTools::ITauTruthTrackMatchingTool )

public:

  TauTruthTrackMatchingTool( const std::string& name );

  virtual ~TauTruthTrackMatchingTool();

  virtual StatusCode initialize();

  // classify vector of tracks
  virtual StatusCode classifyTracks(std::vector<const TAUTRACKPARTICLE*>& vTracks);

  // classify a single track
  virtual StatusCode classifyTrack(const TAUTRACKPARTICLE& xTrackParticle);

private:

  StatusCode classifyConversion(const TAUTRACKPARTICLE& xTrackParticle, const xAOD::TruthParticle& xTruthParticle);
  StatusCode checkTrackType(const TAUTRACKPARTICLE& xTrackParticle);
  StatusCode checkTrackIsTauInheritant(const TAUTRACKPARTICLE& xTrackParticle);
  bool checkTruthParent(const xAOD::TruthParticle& xTruthParticle, int& iDepth, std::string& sHistory);
  const xAOD::TruthParticle* getTruthParticle(const TAUTRACKPARTICLE& xTrackParticle);

private:
  bool m_bIsHadronicTrackAvailable;
  bool m_bIsHadronicTrackAvailableChecked;

}; // class TauTruthTrackMatchingTool

}
#endif // TAUANALYSISTOOLS_TAUTRUTHTRACKMATCHINGTOOL_H

