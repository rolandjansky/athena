/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETRESOLUTION_IJERSMEARINGTOOL_H
#define JETRESOLUTION_IJERSMEARINGTOOL_H

// EDM includes
#include "xAODJet/Jet.h"

// Other includes
#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/ISystematicsTool.h"


/// Interface class for smearing the jet energy resolution
///
/// @author Steve Farrell <steven.farrell@cern.ch>
///
class IJERSmearingTool : public virtual CP::ISystematicsTool
{

    // Declare the interface that the class provides
    ASG_TOOL_INTERFACE( IJERSmearingTool )

  public:

    /// Virtual destructor
    virtual ~IJERSmearingTool(){};

    /// Apply the JER correction on a modifiable jet
    virtual CP::CorrectionCode applyCorrection( xAOD::Jet& jet ) = 0;

    /// Create a JER-corrected copy from a const jet
    virtual CP::CorrectionCode correctedCopy( const xAOD::Jet& input,
                                              xAOD::Jet*& output ) = 0;

    /// Optional method to set random seed. Note that the tool
    /// already has a deterministic way to do this using the jet phi.
    virtual void setRandomSeed(long int) = 0;

}; // class IJERSmearingTool

#endif
