/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _IASGFUDGEMCTOOL_H
#define _IASGFUDGEMCTOOL_H

/**
   @class AsgFudgeMCTool
   @brief Tool to fudge photon MC shower shapes.

   @author Rob Roy Fletcher (University of Pennsylvania) rob.fletcher@cern.ch
   @date   August 2014


*/

// Framework include(s):
#include "AsgTools/IAsgTool.h"

// EDM include(s):
#include "xAODEgamma/Photon.h"

// Local include(s):
#include "PATInterfaces/CorrectionCode.h"


class IAsgFudgeMCTool : public virtual asg::IAsgTool {

   /// Declare the interface that the class provides
   ASG_TOOL_INTERFACE(IAsgFudgeMCTool)

public:
   /// Apply the correction on a modifyable object
   virtual const CP::CorrectionCode applyCorrection( xAOD::Photon* gamma, int isConv, int preselection ) const = 0;

   /// Try to cast a base particle as a photon
   virtual const CP::CorrectionCode applyCorrection( xAOD::IParticle* part, int isConv, int preselection ) const = 0;

   /// Create a corrected copy from a constant photon
//   virtual CP::CorrectionCode correctedCopy( const xAOD::Photon* input,
//                                         xAOD::Photon& output ) = 0;

}; // class IAsgFudgeMCTool

#endif // _IASGFUDGEMCTOOL_H
