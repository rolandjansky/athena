/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISOLATIONCORRECTION_IISOLATIONCORRECTIONTOOL_H
#define ISOLATIONCORRECTION_IISOLATIONCORRECTIONTOOL_H

#include "AsgTools/IAsgTool.h"
//xAOD
#include "xAODEgamma/EgammaFwd.h"
#include "xAODEventInfo/EventInfo.h"

#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/ISystematicsTool.h"

#include "xAODPrimitives/IsolationType.h"

namespace CP {

class IIsolationCorrectionTool : virtual public asg::IAsgTool, virtual public CP::ISystematicsTool{
	// Declare the interface that the class provides
	ASG_TOOL_INTERFACE(IIsolationCorrectionTool)

    public:

        virtual ~IIsolationCorrectionTool() {};

        virtual StatusCode initialize() = 0;

        //Apply the correction on a modifyable egamma object (xAOD::Electron or xAOD::Photon)
        virtual CP::CorrectionCode applyCorrection(xAOD::Egamma &) = 0;

        //Create a corrected copy from a constant egamma object
        virtual CP::CorrectionCode correctedCopy( const xAOD::Egamma&, xAOD::Egamma*&) = 0;
        
        virtual float GetPtCorrectedIsolation(const xAOD::Egamma&, xAOD::Iso::IsolationType) = 0;
        virtual float GetPtCorrection(const xAOD::Egamma&, xAOD::Iso::IsolationType) = 0;

};

}

#endif //ISOLATIONCORRECTION_IISOLATIONCORRECTIONTOOL_H
