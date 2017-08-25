/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAANALYSISINTERFACES_IELECTRONPHOTONSHOWERSHAPEFUDGETOOL_H
#define EGAMMAANALYSISINTERFACES_IELECTRONPHOTONSHOWERSHAPEFUDGETOOL_H

/**
   @class AsgFudgeMCTool
   @brief Tool to fudge photon MC shower shapes.

   @author Rob Roy Fletcher (University of Pennsylvania) rob.fletcher@cern.ch
   @date   August 2014


*/

// Framework include(s):
#include "AsgTools/IAsgTool.h"

// EDM include(s):
#include "xAODEgamma/EgammaFwd.h"
#include "xAODEgamma/PhotonFwd.h"
#include "xAODEgamma/ElectronFwd.h"

// Local include(s):
#include "PATInterfaces/CorrectionCode.h"


class IElectronPhotonShowerShapeFudgeTool : public virtual asg::IAsgTool {

   /// Declare the interface that the class provides
   ASG_TOOL_INTERFACE(IElectronPhotonShowerShapeFudgeTool)

public:
   /// Apply the correction on a modifyable object
   virtual const CP::CorrectionCode applyCorrection(xAOD::Photon& ph ) const = 0;

   virtual const CP::CorrectionCode applyCorrection(xAOD::Electron& el ) const = 0;

   virtual const CP::CorrectionCode correctedCopy( const xAOD::Photon& ph, xAOD::Photon*& output ) const = 0;

   virtual const CP::CorrectionCode correctedCopy( const xAOD::Electron& el, xAOD::Electron*& output ) const = 0;



}; // class IElectronPhotonShowerShapeFudgeTool

#endif // _IASGFUDGEMCTOOL_H
