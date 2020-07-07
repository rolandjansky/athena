// -*- c++ -*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETTRACKSYSTEMATICSTOOLS_IJETTRACKFILTERTOOL_H
#define INDETTRACKSYSTEMATICSTOOLS_IJETTRACKFILTERTOOL_H

#include "AsgTools/IAsgTool.h"
#include "PATInterfaces/ISystematicsTool.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODJet/JetContainer.h"
#include <string>

namespace InDet {

  /// @class JetTrackFilterTool
  /// This class randomly rejects tracks in the core of a jet
  /// @author Felix Clark (michael.ryan.clark@cern.ch)

  class IJetTrackFilterTool
    : public virtual asg::IAsgTool
    , public virtual CP::ISystematicsTool
  {
      
  public:
    ASG_TOOL_INTERFACE( InDet::IJetTrackFilterTool )
	
    virtual StatusCode initialize() = 0;
    virtual void prepare() = 0; // not sure if/why this function is necessary - felix

    virtual bool accept( const xAOD::TrackParticle*, const xAOD::Jet* ) const = 0;
    virtual bool accept( const xAOD::TrackParticle*, const xAOD::JetContainer* ) const = 0;
      
    // implement the CP::ISystematicTool interface
      
    /// returns: whether the tool is affected by the systematic
    virtual bool isAffectedBySystematic( const CP::SystematicVariation& ) const = 0;
    /// returns: list of systematics this tool can be affected by
    virtual CP::SystematicSet affectingSystematics() const = 0;
    /// returns: list of recommended systematics to use with this tool
    virtual CP::SystematicSet recommendedSystematics() const = 0;
    /// configure the tool to apply a given list of systematic variations
    virtual CP::SystematicCode applySystematicVariation( const CP::SystematicSet& ) = 0;
      
      
  }; // class IJetTrackFilterTool

} // namespace InDet

#endif
