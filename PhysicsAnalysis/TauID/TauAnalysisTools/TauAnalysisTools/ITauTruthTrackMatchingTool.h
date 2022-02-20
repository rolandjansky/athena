/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUANALYSISTOOLS_ITAUTRUTHTRACKMATCHINGTOOL_H
#define TAUANALYSISTOOLS_ITAUTRUTHTRACKMATCHINGTOOL_H
/*
  author: Dirk Duschinger
  mail: dirk.duschinger@cern.ch
  documentation in: ../README.rst
*/

// Framework include(s):
#include "AsgTools/IAsgTool.h"

// xAOD include(s):
#include "xAODTau/TauTrack.h"

// EDM include(s):
#include "PATInterfaces/CorrectionCode.h"

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
  virtual StatusCode classifyTracks(std::vector<const xAOD::TauTrack*>& vTracks) const = 0;

  // classify a single track
  virtual StatusCode classifyTrack(const xAOD::TauTrack& xTrackParticle) const = 0;

}; // class ITauTruthTrackMatchingTool

} // namespace TauAnalysisTools

#endif // TAUANALYSISTOOLS_ITAUTRUTHTRACKMATCHINGTOOL_H
