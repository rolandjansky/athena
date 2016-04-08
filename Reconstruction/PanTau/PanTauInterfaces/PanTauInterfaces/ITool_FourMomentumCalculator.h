/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUINTERFACES_ITOOL_FOURMOMENTUMCALCULATOR_H
#define PANTAUINTERFACES_ITOOL_FOURMOMENTUMCALCULATOR_H
// Gaudi
#include "GaudiKernel/IAlgTool.h"


#include <string>


namespace PanTau {
    class PanTauSeed;
}


namespace PanTau {



    static const InterfaceID IID_ITool_FourMomentumCalculator("PanTau::ITool_FourMomentumCalculator", 1,0);

    /** @class ITool_FourMomentumCalculator
        @brief Interface for Tool_FourMomentumCalculator
        @author Christian Limbach (limbach@physik.uni-bonn.de)
    */


    class ITool_FourMomentumCalculator : virtual public IAlgTool {
        public:
            static const InterfaceID& interfaceID();
            
            virtual StatusCode calculateFourMomentum(PanTau::PanTauSeed* inSeed) const = 0;
    };
    

    inline const InterfaceID& PanTau::ITool_FourMomentumCalculator::interfaceID() {
        return IID_ITool_FourMomentumCalculator;
    }

}
#endif //PANTAUINTERFACES_ITOOL_INFORMATIONSTORE_H

