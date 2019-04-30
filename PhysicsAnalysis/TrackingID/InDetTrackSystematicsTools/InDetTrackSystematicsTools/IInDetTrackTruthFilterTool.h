// -*- c++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETTRACKSYSTEMATICSTOOLS_IINDETTRACKTRUTHFILTERTOOL_H
#define INDETTRACKSYSTEMATICSTOOLS_IINDETTRACKTRUTHFILTERTOOL_H

#include "AsgTools/IAsgTool.h"
#include "PATInterfaces/ISystematicsTool.h"
#include "xAODTracking/TrackParticle.h"
#include <string>

namespace InDet {

  class InDetTrackTruthOriginTool;

  /// @class InDetTrackTruthFilterTool
  /// This class selects tracks based on their truth origin
  /// @author Remi Zaidan (remi.zaidan@cern.ch)
  /// @author Felix Clark (michael.ryan.clark@cern.ch)

  class IInDetTrackTruthFilterTool
    : public virtual asg::IAsgTool
    , public virtual CP::ISystematicsTool
  {
      
  public:
    ASG_TOOL_INTERFACE( InDet::IInDetTrackTruthFilterTool )
	
    virtual StatusCode initialize() = 0;
    virtual void prepare() = 0; // not sure if/why this function is necessary - felix
    virtual StatusCode finalize() = 0;

    //"standard" accept method - if appropriate systematic is activated, will call mu-dependent version (below) using mu value from EventInfo, otherwise will use truth info
    virtual bool accept(const xAOD::TrackParticle* track) const = 0;
    
    //This is a version of the accept method that takes a value of mu (i.e. mean interactions per crossing) in order to calculate a probability that a givent track at that mu is a fake, and so should be considered for being dropped for the fake systematic variation - this version does not rely on truth information
    virtual bool accept(const xAOD::TrackParticle* track, float mu) const = 0;

    // implement the CP::ISystematicTool interface
      
    /// returns: whether the tool is affected by the systematic
    virtual bool isAffectedBySystematic( const CP::SystematicVariation& ) const = 0;
    /// returns: list of systematics this tool can be affected by
    virtual CP::SystematicSet affectingSystematics() const = 0;
    /// returns: list of recommended systematics to use with this tool
    virtual CP::SystematicSet recommendedSystematics() const = 0;
    /// configure the tool to apply a given list of systematic variations
    virtual CP::SystematicCode applySystematicVariation( const CP::SystematicSet& ) = 0;
      
      
  }; // class IInDetTrackTruthFilterTool

} // namespace InDet

#endif
