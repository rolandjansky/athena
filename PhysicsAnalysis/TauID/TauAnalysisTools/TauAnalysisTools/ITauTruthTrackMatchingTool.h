// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_ITAUTRUTHTRACKMATCHINGTOOL_H
#define TAUANALYSISTOOLS_ITAUTRUTHTRACKMATCHINGTOOL_H

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
#include "AsgTools/IAsgTool.h"

// Core include(s):
#include "AthLinks/ElementLink.h"

// xAOD include(s):
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertex.h"

// EDM include(s):
#include "PATInterfaces/CorrectionCode.h"

// Local include(s):

namespace TauAnalysisTools
{

class ITauTruthTrackMatchingTool : public virtual asg::IAsgTool
{

  /// Declare the interface that the class provides
  ASG_TOOL_INTERFACE( TauAnalysisTools::ITauTruthTrackMatchingTool )

public:
  /// Function initialising the tool
  virtual StatusCode initialize() = 0;

  // classify vector of tracks
  virtual StatusCode classifyTracks(std::vector<const xAOD::TrackParticle*>& vTracks) = 0;

  // classify a single track
  virtual StatusCode classifyTrack(const xAOD::TrackParticle& xTrackParticle) = 0;

}; // class ITauTruthTrackMatchingTool

} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_ITAUTRUTHTRACKMATCHINGTOOL_H
