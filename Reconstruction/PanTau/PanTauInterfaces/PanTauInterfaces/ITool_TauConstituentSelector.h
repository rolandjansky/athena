/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUINTERFACES_ITOOL_TAUCONSTITUENTSELECTOR_H
#define PANTAUINTERFACES_ITOOL_TAUCONSTITUENTSELECTOR_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

#include <vector>

#include "PanTauEvent/TauConstituent.h"

namespace PanTau {



    static const InterfaceID IID_ITool_TauConstituentSelector("PanTau::ITool_TauConstituentSelector", 1,0);

    /** @class ITool_TauConstituentSelector
        @brief Interface for Tool_TauConstituentSelector
        @author Christian Limbach (limbach@physik.uni-bonn.de)
    */


    class ITool_TauConstituentSelector : virtual public IAlgTool {
        public:
            static const InterfaceID& interfaceID();
            
            virtual StatusCode SelectTauConstituents(std::vector<TauConstituent*> inputList,
                                                     std::vector<TauConstituent*>& outputList) const = 0;
        protected:
            virtual bool    passesSelection_NeutralConstituent(TauConstituent* tauConstituent) const = 0;
            virtual bool    passesSelection_Pi0NeutConstituent(TauConstituent* tauConstituent) const = 0;
            virtual bool    passesSelection_ChargedConstituent(TauConstituent* tauConstituent) const = 0;
            virtual bool    passesSelection_OutNeutConstituent(TauConstituent* TauConstituent) const = 0;
            virtual bool    passesSelection_OutChrgConstituent(TauConstituent* TauConstituent) const = 0;
            virtual bool    passesSelection_NeutLowAConstituent(TauConstituent* TauConstituent) const = 0;
            virtual bool    passesSelection_NeutLowBConstituent(TauConstituent* TauConstituent) const = 0;
            
            virtual double  getEtCut(double eta, PanTau::TauConstituent::Type constituentType) const = 0;
    };
    

    inline const InterfaceID& PanTau::ITool_TauConstituentSelector::interfaceID() {
        return IID_ITool_TauConstituentSelector;
    }

}
#endif //PANTAUINTERFACES_ITOOL_TAUCONSTITUENTSELECTOR_H 
