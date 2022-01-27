/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TAUANALYSISTOOLS_TAUTRUTHTRACKMATCHINGTOOL_H
#define  TAUANALYSISTOOLS_TAUTRUTHTRACKMATCHINGTOOL_H

/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
  documentation in: ../README.rst
*/

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/AnaToolHandle.h"
#include "CxxUtils/CachedValue.h"

// Local include(s)
#include "TauAnalysisTools/ITauTruthTrackMatchingTool.h"

#include "xAODTruth/TruthParticle.h"
#include "xAODTau/TauTrack.h"

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

  virtual StatusCode initialize() override;

  // classify vector of tracks
  virtual StatusCode classifyTracks(std::vector<const xAOD::TauTrack*>& vTracks) const override;

  // classify a single track
  virtual StatusCode classifyTrack(const xAOD::TauTrack& xTrackParticle) const override;

private:

  StatusCode classifyConversion(const xAOD::TauTrack& xTrackParticle, const xAOD::TruthParticle& xTruthParticle) const;
  StatusCode checkTrackType(const xAOD::TauTrack& xTrackParticle) const;
  StatusCode checkTrackIsTauInheritant(const xAOD::TauTrack& xTrackParticle) const;
  bool checkTruthParent(const xAOD::TruthParticle& xTruthParticle, int& iDepth, std::string& sHistory) const;
  const xAOD::TruthParticle* getTruthParticle(const xAOD::TauTrack& xTrackParticle) const;

private:
  CxxUtils::CachedValue<bool> m_bIsHadronicTrackAvailable;

}; // class TauTruthTrackMatchingTool

}
#endif // TAUANALYSISTOOLS_TAUTRUTHTRACKMATCHINGTOOL_H

