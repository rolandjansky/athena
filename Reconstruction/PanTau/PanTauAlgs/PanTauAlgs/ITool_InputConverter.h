/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUALGS_ITOOL_INPUTCONVERTER_H
#define PANTAUALGS_ITOOL_INPUTCONVERTER_H

#include "AsgTools/IAsgTool.h"

#include <string>

#include "xAODPFlow/PFO.h"
#include "xAODTau/TauJet.h"

namespace PanTau{
    class TauConstituent;
}

namespace Rec {
    class TrackParticle;
}

namespace PanTau {

    /** @class ITool_InputConverter
        @brief Interface for Tool_InputConverter
        @author Christian Limbach (limbach@physik.uni-bonn.de)
    */


    class ITool_InputConverter : virtual public asg::IAsgTool {

    ASG_TOOL_INTERFACE(ITool_InputConverter)

        public:

    virtual bool isInitialized() = 0;
            
    //PFO Converter (r19+)
    virtual StatusCode ConvertToTauConstituent(xAOD::PFO* pfo,
						PanTau::TauConstituent* &tauConstituent,
						const xAOD::TauJet* tauJet) const = 0;
            
    };
    

}
#endif //PANTAUALGS_ITOOL_INPUTCONVERTER_H 
