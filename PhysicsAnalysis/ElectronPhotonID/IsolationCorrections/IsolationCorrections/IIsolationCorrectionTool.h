/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISOLATIONCORRECTION_IISOLATIONCORRECTIONTOOL_H
#define ISOLATIONCORRECTION_IISOLATIONCORRECTIONTOOL_H

#include "AsgTools/IAsgTool.h"
//xAOD
#include "xAODEgamma/Egamma.h"

#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/ISystematicsTool.h"

// PAT includes
#include "PATCore/PATCoreEnums.h"

#include "xAODPrimitives/IsolationType.h"

namespace CP {

  class IIsolationCorrectionTool : virtual public CP::ISystematicsTool {
    // Declare the interface that the class provides
    ASG_TOOL_INTERFACE(IIsolationCorrectionTool)
      
    public:

        virtual ~IIsolationCorrectionTool() {};

        virtual StatusCode initialize() = 0;

        //Apply the correction on a modifyable egamma object (xAOD::Electron or xAOD::Photon)
        virtual CP::CorrectionCode applyCorrection(xAOD::Egamma &) = 0;

        //Create a corrected copy from a constant egamma object
        virtual CP::CorrectionCode correctedCopy(const xAOD::Egamma&, xAOD::Egamma*&) = 0;

	      // This helps to correct for the (wrong) leakage at the analysis level
	      virtual CP::CorrectionCode CorrectLeakage(xAOD::Egamma &) = 0;

	      //systematics
        // Which systematics have an effect on the tool's behaviour?
        virtual CP::SystematicSet affectingSystematics() const  = 0;
        // Is the tool affected by a specific systematic?
        virtual bool isAffectedBySystematic( const CP::SystematicVariation& systematic ) const  = 0;
        //Systematics to be used for physics analysis
        virtual CP::SystematicSet recommendedSystematics() const  = 0;
        //Use specific systematic
        virtual StatusCode applySystematicVariation ( const CP::SystematicSet& systConfig ) = 0;

        virtual float GetPtCorrectedIsolation(const xAOD::Egamma&, xAOD::Iso::IsolationType) = 0;
        virtual float GetPtCorrection(const xAOD::Egamma&, xAOD::Iso::IsolationType) const = 0;
	      virtual float GetDDCorrection(const xAOD::Egamma&, xAOD::Iso::IsolationType) = 0;

  };

}

#endif //ISOLATIONCORRECTION_IISOLATIONCORRECTIONTOOL_H
