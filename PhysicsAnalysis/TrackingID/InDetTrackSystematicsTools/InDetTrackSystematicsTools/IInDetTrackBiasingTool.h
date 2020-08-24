// -*- c++ -*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETTRACKSYSTEMATICSTOOLS_IINDETTRACKBIASINGTOOL_H
#define INDETTRACKSYSTEMATICSTOOLS_IINDETTRACKBIASINGTOOL_H

#include "AsgTools/IAsgTool.h"
#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/ISystematicsTool.h"
#include "xAODTracking/TrackParticleContainer.h"

namespace InDet {

  /// @class InDetTrackBiasingTool
  /// This class biases tracks to emulate systematic distortions of the tracking geometry
  /// @author Pawel Bruckman (pawel.bruckman.de.renstrom@cern.ch)
  /// @author Felix Clark (michael.ryan.clark@cern.ch)

  class IInDetTrackBiasingTool
    : public virtual asg::IAsgTool
    , public virtual CP::ISystematicsTool
  {
    
    ASG_TOOL_INTERFACE( InDet::IInDetTrackBiasingTool )
    
  public:
    virtual StatusCode initialize() = 0;
    virtual void prepare() = 0;
    
    /** Computes the tracks origin */
    virtual CP::CorrectionCode applyCorrection(xAOD::TrackParticle& track) = 0;
    virtual CP::CorrectionCode correctedCopy( const xAOD::TrackParticle& in,
					      xAOD::TrackParticle*& out ) = 0;
    virtual CP::CorrectionCode applyContainerCorrection( xAOD::TrackParticleContainer& cont ) = 0;
    
    // implement the CP::ISystematicTool interface      
    /// returns: whether the tool is affected by the systematic
    virtual bool isAffectedBySystematic( const CP::SystematicVariation& ) const = 0;
    /// returns: list of systematics this tool can be affected by
    virtual CP::SystematicSet affectingSystematics() const = 0;
    /// returns: list of recommended systematics to use with this tool
    virtual CP::SystematicSet recommendedSystematics() const = 0;
    /// configure the tool to apply a given list of systematic variations
    virtual CP::SystematicCode applySystematicVariation( const CP::SystematicSet& ) = 0;


  }; // class IInDetTrackBiasingTool
  
} // namespace InDet

#endif
