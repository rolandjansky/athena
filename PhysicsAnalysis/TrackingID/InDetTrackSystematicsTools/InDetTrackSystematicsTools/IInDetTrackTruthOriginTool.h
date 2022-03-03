// -*- c++ -*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETTRACKSYSTEMATICSTOOLS_IINDETTRACKTRUTHORIGINTOOL_H
#define INDETTRACKSYSTEMATICSTOOLS_IINDETTRACKTRUTHORIGINTOOL_H

#include "AsgTools/IAsgTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"

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

  /** Safely access a track's linked truth particle, if available **/
  virtual const xAOD::TruthParticle* getTruth( const xAOD::TrackParticle* track ) const = 0;

  /** Computes the truth particle origin */
  virtual int getTruthOrigin(const xAOD::TruthParticle* truth) const = 0;

  /** Computes the track origin */
  virtual int getTrackOrigin(const xAOD::TrackParticle* track) const = 0;

  /** Check if a truth particle is from the specified origin (from B or D hadron, or tau) */
  virtual bool isFrom(const xAOD::TruthParticle* truth, int flav) const = 0;

}; // class IInDetTrackTruthOriginTool

} // namespace InDet

#endif
