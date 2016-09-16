/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUALGS_ITOOL_PANTAUTOOLS_H
#define PANTAUALGS_ITOOL_PANTAUTOOLS_H

#include "AsgTools/IAsgTool.h"

#include <string>
#include <vector>

//! xAOD EDM
#include "xAODPFlow/PFO.h"
#include "xAODTau/TauJet.h"
#include "PanTauAlgs/TauConstituent.h"


namespace PanTau {
    class TauFeature2;
    class PanTauSeed2;
    class TauConstituent2;
}


namespace Rec {
    class TrackParticle;
}


namespace PanTau {

    /** @class ITool_PanTauTools
        @brief Interface for PID from tau seeds.

        @author Sebastian Fleischmann
        @author Christian Limbach
    */
    class ITool_PanTauTools : virtual public asg::IAsgTool {

    ASG_TOOL_INTERFACE(ITool_PanTauTools)

        public:

    virtual bool isInitialized() = 0;
            // used in Tool_FeatureExtractor, Tool_DecayModeDeterminator and Tool_DetailsArranger:
            virtual StatusCode execute(PanTau::PanTauSeed2* inSeed) = 0;

    };

}
#endif // PANTAUALGS_ITAUDISCRIMINANTTOOL_H
