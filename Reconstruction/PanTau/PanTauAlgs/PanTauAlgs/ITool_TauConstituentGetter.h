/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUALGS_ITOOL_TAUCONSTITUENTGETTER_H
#define PANTAUALGS_ITOOL_TAUCONSTITUENTGETTER_H

// Gaudi
#include "AsgTools/IAsgTool.h"

#include <string>

namespace PanTau{
    class TauConstituent2;
}

//namespace Analysis {
//    class TauJet;
//}
#include "xAODTau/TauJet.h"

namespace PanTau {


    /** @class ITool_TauConstituentGetter
        @brief Interface for Tool_TauConstituentGetter
        @author Christian Limbach (limbach@physik.uni-bonn.de)
    */


    class ITool_TauConstituentGetter : virtual public asg::IAsgTool {

    ASG_TOOL_INTERFACE(ITool_TauConstituentGetter)

        public:
            
    virtual bool isInitialized() = 0;
            virtual StatusCode GetTauConstituents(//const Analysis::TauJet*,
                                                  const xAOD::TauJet* tauJet,
                                                  std::vector<TauConstituent2*>& outputList,
                                                  std::string algName) const = 0;
            
    };
    
}
#endif //PANTAUALGS_ITOOL_TAUCONSTITUENTGETTER_H 
