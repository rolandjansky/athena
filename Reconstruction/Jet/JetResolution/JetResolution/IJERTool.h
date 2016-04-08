/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETRESOLUTION_IJERTOOL_H
#define JETRESOLUTION_IJERTOOL_H

// Framework includes
#include "AsgTools/IAsgTool.h"
#include "PATInterfaces/CorrectionCode.h"

// EDM includes
#include "xAODJet/Jet.h"

// Local includes
#include "JetResolution/JERDefs.h"

/// Tool interface class for accessing the jet energy resolution
///
/// @author Steve Farrell <steven.farrell@cern.ch>
///
class IJERTool : public virtual asg::IAsgTool
{

    /// Declare the interface that the class provides
    ASG_TOOL_INTERFACE( IJERTool )

  public:

    /// Virtual destructor for proper memory cleanup
    virtual ~IJERTool(){};

    /// Read the resolution from the MC parameterization
    virtual double getRelResolutionMC(const xAOD::Jet* jet) = 0;

    /// Obtain the resolution for data (sigma_MC + offset)
    virtual double getRelResolutionData(const xAOD::Jet* jet) = 0;

    /// Calculate resolution uncertainty
    virtual double getUncertainty(const xAOD::Jet* jet,
                                  JER::Uncert errType = JER::JER_NP_ALL) = 0;

}; // class IJERTool

#endif // JETRESOLUTION_IJERTOOL_H
