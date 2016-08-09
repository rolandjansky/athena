/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISOLATIONSELECTION_IISOLATIONCLOSEBYCORRECTIONTOOL_H
#define ISOLATIONSELECTION_IISOLATIONCLOSEBYCORRECTIONTOOL_H

#include "PATInterfaces/CorrectionCode.h"
#include <xAODBase/IParticle.h>
#include "xAODPrimitives/IsolationType.h"
#include "PATCore/TAccept.h"
#include "IsolationSelection/IsolationSelectionTool.h"

namespace CP {

    class IIsolationCloseByCorrectionTool : public virtual asg::IAsgTool {
   
        ASG_TOOL_INTERFACE(CP::IIsolationCloseByCorrectionTool)
        
        public: 

	        // This function calculates and applies (the particle is not redecorated) the corrections for close-by objects for each isolation variables and tests whether the particle passes the isolation working point after the corrections.
	        // Note that to use this functionality, a IsolationSelectionTool must have been passed to the tool (which must have been intialised indicating which isolation working point to use).
	        // The result returned is a TAccept object which is the decision made by the tool with respect to the particle passing the working point.
            virtual Root::TAccept& acceptCorrected(const xAOD::IParticle& x, const std::vector<const xAOD::IParticle*>& closePar, int topoetconeModel = -1) = 0; 

	        // This function calculates the values of the corrections for close-by objects to be applied to the isolation variables (without redecorating the particles).
	        // The corrections are returned in a vector (one correction per isolation type provided).
	        // This function is intended for experts only who want to check the effects of the corrections.
            virtual CP::CorrectionCode getCloseByCorrection(std::vector<float>& corrections, const xAOD::IParticle& par, const std::vector<xAOD::Iso::IsolationType>& types, const std::vector<const xAOD::IParticle*>& closePar, int topoetconeModel = -1) const = 0; 

    };
  
}
#endif
