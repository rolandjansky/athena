/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITrackClassifier_H
#define ITrackClassifier_H

#include "AsgTools/IAsgTool.h"

#include <xAODJet/JetContainer.h>
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"


class ITrackClassifier : virtual public asg::IAsgTool
{
  ASG_TOOL_INTERFACE( ITrackClassifier )

  public:
  virtual double compute_HF_Score(const xAOD::TrackParticle *track, const xAOD::Jet *jet) const = 0;

};

#endif //> !ITrackClassifier_H
