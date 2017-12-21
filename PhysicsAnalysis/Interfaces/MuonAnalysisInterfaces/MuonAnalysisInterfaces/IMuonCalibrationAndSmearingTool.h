/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCAST_IMUONCALIBRATIONANDSMEARINGTOOL_H
#define MCAST_IMUONCALIBRATIONANDSMEARINGTOOL_H

/// Framework include(s):
#include "AsgTools/IAsgTool.h"

/// EDM include(s):
#include "xAODMuon/Muon.h"

/// Local include(s):
#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/ISystematicsTool.h"
#include "PATInterfaces/SystematicCode.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicSet.h"
#include "PATInterfaces/SystematicVariation.h"

namespace CP {

class IMuonCalibrationAndSmearingTool : public virtual asg::IAsgTool, virtual public CP::ISystematicsTool {

  /// Declare the interface that the class provides
  ASG_TOOL_INTERFACE( CP::IMuonCalibrationAndSmearingTool )

  public:
    /// Apply the correction on a modifyable object
    virtual CorrectionCode applyCorrection( xAOD::Muon& mu ) const = 0;
    /// Create a corrected copy from a constant muon
    virtual CorrectionCode correctedCopy( const xAOD::Muon& input, xAOD::Muon*& output ) const = 0;
    /// Get the expected pT resolution
    virtual double expectedResolution( const std::string& DetType, xAOD::Muon& mu, const bool mc ) const=0;
    /// Get the expected pT resolution - int argument is more efficient
    virtual double expectedResolution( const int& DetType, xAOD::Muon& mu, const bool mc ) const=0;
    
    virtual CorrectionCode applyCorrectionTrkOnly( xAOD::TrackParticle& inTrk, const int DetType) const = 0;


}; /// class IMuonCalibrationAndSmearingTool

} /// namespace CP

#endif /// MCAST_IMUONCALIBRATIONANDSMEARINGTOOL_H
