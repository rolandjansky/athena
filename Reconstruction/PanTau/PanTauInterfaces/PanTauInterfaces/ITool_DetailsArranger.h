/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUINTERFACES_ITOOL_DETAILSARRANGER_H
#define PANTAUINTERFACES_ITOOL_DETAILSARRANGER_H

#include "GaudiKernel/IAlgTool.h"


namespace PanTau {
    class PanTauSeed;
}

namespace Analysis {
    class TauDetailsContainer;
}


namespace PanTau {

    static const InterfaceID IID_ITool_DetailsArranger("PanTau::ITool_DetailsArranger", 1, 0);

    /** @class ITool_DetailsArranger
        @brief Interface for PID from tau seeds.

        @author Sebastian Fleischmann
        @author Christian Limbach
    */
    class ITool_DetailsArranger : virtual public IAlgTool {
        public:
            static const InterfaceID& interfaceID( ) ;

            virtual StatusCode arrangeDetails(PanTau::PanTauSeed* inSeed) = 0;//, Analysis::TauDetailsContainer* detailsCont) = 0;
            
    };

    
    inline const InterfaceID& PanTau::ITool_DetailsArranger::interfaceID() {
        return IID_ITool_DetailsArranger;
    }


}
#endif // PANTAUINTERFACES_ITAUDISCRIMINANTTOOL_H
