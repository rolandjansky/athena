/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUALGS_ITOOL_TAUCONSTITUENTSELECTOR_H
#define PANTAUALGS_ITOOL_TAUCONSTITUENTSELECTOR_H

// Gaudi
#include "AsgTools/IAsgTool.h"

#include <vector>

#include "PanTauAlgs/TauConstituent.h"

namespace PanTau {

    /** @class ITool_TauConstituentSelector
        @brief Interface for Tool_TauConstituentSelector
        @author Christian Limbach (limbach@physik.uni-bonn.de)
    */


    class ITool_TauConstituentSelector : virtual public asg::IAsgTool {

    ASG_TOOL_INTERFACE(ITool_TauConstituentSelector)

        public:
    virtual bool isInitialized() = 0;
            virtual StatusCode SelectTauConstituents(std::vector<TauConstituent2*> inputList,
                                                     std::vector<TauConstituent2*>& outputList) const = 0;
        protected:
//            virtual bool    passesSelection_NeutralConstituent(TauConstituent2* tauConstituent) const = 0;
//            virtual bool    passesSelection_Pi0NeutConstituent(TauConstituent2* tauConstituent) const = 0;
//            virtual bool    passesSelection_ChargedConstituent(TauConstituent2* tauConstituent) const = 0;
//            virtual bool    passesSelection_OutNeutConstituent(TauConstituent2* TauConstituent) const = 0;
//            virtual bool    passesSelection_OutChrgConstituent(TauConstituent2* TauConstituent) const = 0;
//            virtual bool    passesSelection_NeutLowAConstituent(TauConstituent2* TauConstituent) const = 0;
//            virtual bool    passesSelection_NeutLowBConstituent(TauConstituent2* TauConstituent) const = 0;
            
//            virtual double  getEtCut(double eta, PanTau::TauConstituent2::Type constituentType) const = 0;
    };
    
}
#endif //PANTAUALGS_ITOOL_TAUCONSTITUENTSELECTOR_H 
