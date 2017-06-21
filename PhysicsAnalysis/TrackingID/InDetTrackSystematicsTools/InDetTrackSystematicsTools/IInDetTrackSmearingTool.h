// -*- c++ -*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETTRACKSYSTEMATICSTOOLS_IINDETTRACKSMEARINGTOOL_H
#define INDETTRACKSYSTEMATICSTOOLS_IINDETTRACKSMEARINGTOOL_H

// Framework include(s):
#include "AsgTools/IAsgTool.h"
#include "PATInterfaces/ISystematicsTool.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/CorrectionCode.h"

// EDM include(s):
#include "xAODTracking/TrackParticleContainer.h"

namespace InDet {

  /// Simple interface for calibrating/correcting tracks (inner-detector particles)
  ///
  /// Following the design principles outlined in the TF3 recommendations.
  ///
  /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
  /// @author Michael Clark <michael.ryan.clark@cern.ch>
  ///
  /// $Revision: 299883 $
  /// $Date: 2014-03-28 18:34:16 +0100 (Fri, 28 Mar 2014) $
  ///  
  class IInDetTrackSmearingTool
    : public virtual asg::IAsgTool
    , public virtual CP::ISystematicsTool
  {
    
    /// Declare the interface that the class provides
    ASG_TOOL_INTERFACE( InDet::IInDetTrackSmearingTool )

    public:
    // implement the CP::CorrectionTool interface

    /// Smearing method 
    virtual CP::CorrectionCode applyCorrection( xAOD::TrackParticle& ID ) = 0; 
    virtual CP::CorrectionCode correctedCopy( const xAOD::TrackParticle&, xAOD::TrackParticle*& ) = 0; 
    virtual CP::CorrectionCode applyContainerCorrection( xAOD::TrackParticleContainer& ) = 0;

    // implement the CP::ISystematicTool interface

    /// returns: whether the tool is affected by the systematic
    virtual bool isAffectedBySystematic( const CP::SystematicVariation& ) const = 0;
    /// returns: list of systematics this tool can be affected by
    virtual CP::SystematicSet affectingSystematics() const = 0;
    /// returns: list of recommended systematics to use with this tool
    virtual CP::SystematicSet recommendedSystematics() const = 0;
    
    /// configure the tool to apply a given list of systematic variations
    virtual CP::SystematicCode applySystematicVariation( const CP::SystematicSet& ) = 0;
    

  };/// class IInDetTrackSmearingTool

} /// namespace InDet

#endif
