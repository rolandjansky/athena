/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUALGS_ITOOL_INPUTCONVERTER_H
#define PANTAUALGS_ITOOL_INPUTCONVERTER_H

// Gaudi
#include "AsgTools/IAsgTool.h"

#include <string>


//! xAOD EDM
#include "xAODPFlow/PFO.h"
#include "xAODTau/TauJet.h"

namespace PanTau{
    class TauConstituent2;
}

//class eflowObject;

//namespace Analysis {
//    class TauPi0Cluster;
//    class TauJet;
//}

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
            virtual StatusCode ConvertToTauConstituent2(xAOD::PFO* pfo,
                                                       PanTau::TauConstituent2* &tauConstituent,
                                                       const xAOD::TauJet* tauJet) const = 0;
            
//            virtual bool       passesPreselectionEnergy(double itsEnergy) const = 0;
            
            //cluster based converter
//             virtual StatusCode ConvertToTauConstituent2(eflowObject* efo, TauConstituent2* &tauConstituent) const;
            
    };
    

}
#endif //PANTAUALGS_ITOOL_INPUTCONVERTER_H 
