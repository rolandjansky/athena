#pragma once

#include "AsgTools/IAsgTool.h"
#include "xAODTracking/TrackParticle.h"

namespace InDet {

  /**
  @class IInDetTrackTruthOriginTool
  This class determines the truth origin of a track
  @author Remi Zaidan (remi.zaidan@cern.ch)
  @author Felix Clark (michael.ryan.clark@cern.ch)
  */

  class IInDetTrackTruthOriginTool : public virtual asg::IAsgTool {

  ASG_TOOL_INTERFACE( InDet::IInDetTrackTruthOriginTool )
  public:
    
  virtual StatusCode initialize() = 0;
  virtual void prepare() = 0;
  virtual StatusCode finalize() = 0;

  /** Computes the tracks origin */
  virtual int getTrackOrigin(const xAOD::TrackParticle* track) const = 0;
  virtual int getTrackOrigin(const xAOD::TrackParticle* track, int &parentID) const = 0;


}; // class IInDetTrackTruthOriginTool

} // namespace InDet
