/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUINTERFACES_ITOOL_DECAYMODEDETERMINATOR_H
#define PANTAUINTERFACES_ITOOL_DECAYMODEDETERMINATOR_H
// Gaudi
#include "GaudiKernel/IAlgTool.h"


#include <string>


namespace PanTau {
    class PanTauSeed;
}


namespace PanTau {



    static const InterfaceID IID_ITool_DecayModeDeterminator("PanTau::ITool_DecayModeDeterminator", 1,0);

    /** @class ITool_DecayModeDeterminator
        @brief Interface for Tool_DecayModeDeterminator
        @author Christian Limbach (limbach@physik.uni-bonn.de)
    */


    class ITool_DecayModeDeterminator : virtual public IAlgTool {
        public:
            static const InterfaceID& interfaceID();
            
            virtual StatusCode determineDecayMode(PanTau::PanTauSeed* inSeed) const = 0;
    };
    

    inline const InterfaceID& PanTau::ITool_DecayModeDeterminator::interfaceID() {
        return IID_ITool_DecayModeDeterminator;
    }

}
#endif //PANTAUINTERFACES_ITOOL_INFORMATIONSTORE_H

